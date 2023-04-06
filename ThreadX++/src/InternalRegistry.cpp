/*
 * InternalRegistery.cpp
 *
 *  Created on: 22 במרץ 2023
 *      Author: zehevvv
 */

#include <stdio.h>
#include <InternalRegistry.h>
#include "InternalFlashDriver.h"
#include "TimeEvent.h"

#ifdef INTERNAL_REGISTRY_ENABLE

typedef struct RegisteryObject
{
	uint8_t		name_size;
	char*		name;
	uint8_t		value_size;
	void*		value;
} RegisteryObject;


/// @brief C'tor, Read the stored data from flash.
InternalRegistry::InternalRegistry() : Task("InternalRegistery", TASK_PRIORITY, STACK_SIZE),
	m_flash(INTERNAL_REGISTRY_START_ADDRESS, INTERNAL_REGISTRY_NUM_SECTOR, INTERNAL_REGISTRY_LOGICAL_BLOCK_SIZE),
	m_need_update_flash(false),
	m_event_flahs_update(&InternalRegistry::EventCheckFlash, this, this, 1000, true)
{
	m_flash_buffer = new uint8_t[m_flash.GetMaxFlashSize()];
	m_shadow_flash_buffer = new uint8_t[m_flash.GetMaxFlashSize()];

	m_flash_buffer_size = m_flash.GetSize();
	m_flash.Read(m_flash_buffer, m_flash_buffer_size);
	UpdateListObject();
}

/// @brief D'tor, delete caching buffer.
InternalRegistry::~InternalRegistry()
{
	delete[] m_flash_buffer;
	delete[] m_shadow_flash_buffer;
}

/// @brief 	This is a callback of event that jump every second and check if buffer of the cache is change (new value is add
/// 		Or old value is changed) and if yes, update the flash.
///
/// @param pointer	- Ignore
void InternalRegistry::EventCheckFlash(void* pointer)
{
	if (m_need_update_flash)
	{
		UpdateFlash();
	}
}

/// @brief Write registry value of buffer to cache buffer,
///
/// @param name			- The name of the value (key).
/// @param name_size	- The size of the name (bytes).
/// @param buffer		- Pointer to buffer of values that need to store.
/// @param buffer_size	- The size of the buffer (bytes).
void InternalRegistry::WriteBuffer(char* name, uint8_t name_size, uint8_t* buffer, uint8_t buffer_size)
{
	Write(name, name_size, (void*)buffer, buffer_size);
}

/// @brief 	Write registry value to cache buffer, this function just check if the value is already stored in the cache
/// 		if yes edit the stored value, else write a new value to cache.
///
/// @param name			- The name of the value (key).
/// @param name_size	- The size of the name (bytes).
/// @param buffer		- Pointer to value that need to store.
/// @param buffer_size	- The size of the value type (bytes).
void InternalRegistry::Write(char* name, uint8_t name_size, void* value, uint8_t value_size)
{
	m_mutex.Lock();

	// Check if the object already exist
	RegisteryObject* object = GetObject(name, name_size);
	if (object != NULL)
	{
		if (object->value_size != value_size)
		{
			printf("Error: registry failed to write new value because the old size is %u and the new size is %u", object->value_size, value_size);
			return;
		}

		memcpy(object->value, value, value_size);
	}
	else
	{
		if (value_size + m_flash_buffer_size > m_flash.GetMaxFlashSize())
		{
			printf("Error: registry failed to write new value because don't have enough space");
			return;
		}

		WriteNewObjectToBuffer(name, name_size, value, value_size);
		m_flash_buffer_size = AddObjectToList(m_flash_buffer_size);
	}

	m_need_update_flash = true;
	m_mutex.Unlock();
}

/// @brief Write registry value to cache buffer, write the value in the next order: name size - name - value size - value.
///
/// @param name			- The name of the value (key).
/// @param name_size	- The size of the name (bytes).
/// @param buffer		- Pointer to value that need to store.
/// @param buffer_size	- The size of the value type (bytes).
void InternalRegistry::WriteNewObjectToBuffer(char* name, uint8_t name_size, void* value, uint8_t value_size)
{
	uint32_t index = m_flash_buffer_size;

	memcpy(&m_flash_buffer[index], &name_size, 1);
	index++;

	memcpy(&m_flash_buffer[index], name, name_size);
	index += name_size;

	memcpy(&m_flash_buffer[index], &value_size, 1);
	index++;

	memcpy(&m_flash_buffer[index], value, value_size);
	index += value_size;
}

/// @brief Read registry value of buffer from cache buffer,
///
/// @param name			- The name of the value (key).
/// @param name_size	- The size of the name (bytes).
/// @param buffer		- Pointer to buffer of values that need to read.
/// @param buffer_size	- The size of the buffer (bytes).
///
/// @return True - read success.
bool InternalRegistry::ReadBuffer(char* name, uint8_t name_size,  uint8_t* buffer, uint8_t buffer_size)
{
	return Read(name, name_size, (void*)buffer, buffer_size);
}

/// @brief 	Read registry value to cache buffer, this function just check if the value is already stored in the cache
/// 		if yes read the stored value, else the read failed.
///
/// @param name			- The name of the value (key).
/// @param name_size	- The size of the name (bytes).
/// @param buffer		- Pointer to value that need to store.
/// @param buffer_size	- The size of the value type (bytes).
///
/// @return True - read success.
bool InternalRegistry::Read(char* name, uint8_t name_size, void* value, uint8_t value_size)
{
	m_mutex.Lock();
	bool result = true;

	RegisteryObject* object = GetObject(name, name_size);
	if ((object != NULL) &&
		(object->value_size == value_size))
	{
		memcpy(value, object->value, value_size);
	}
	else
	{
		printf("Registry not found the object - %s \n", name);
		result = false;
	}

	m_mutex.Unlock();

	return result;
}

/// @brief Check if the value is exist in the cache.
///
/// @param name			- The name of the value (key).
/// @param name_size	- The size of the name (bytes)
///
/// @return True - the value exist in the cache.
bool InternalRegistry::IsExist(char* name, uint8_t name_size)
{
	m_mutex.Lock();

	bool result = (GetObject(name, name_size) != NULL);

	m_mutex.Unlock();

	return result;
}

/// @brief Get the value from the cache.
///
/// @param name			- The name of the value (key).
/// @param name_size	- The size of the name (bytes)
///
/// @return Pointer to object, if not found return NULL.
RegisteryObject* InternalRegistry::GetObject(char* name, uint8_t name_size)
{
    for (RegisteryObject& object : m_list_objects)
    {
    	if ((name_size == object.name_size) &&
    		(memcmp(name, object.name, name_size) == 0))
    	{
    		return &object;
    	}
	}

	return NULL;
}

/// @brief  Update the list object of the flash.
/// 		After read the values from the flash call this function that run on the cache buffer and create the list of
/// 		the objects
void InternalRegistry::UpdateListObject()
{
	if (m_flash_buffer_size == 0)
		return;

	uint32_t index = 0;

    while (index < m_flash_buffer_size)
    {
    	index = AddObjectToList(index);
	}
}

/// @brief  Create object that hold the pointers of the name and the value of registry object from provided index of
/// 		the cache
///
/// @param	index	- Index in the cache buffer where we want to read the index.
///
/// @return The end index of the object (the index of the next object).
uint32_t InternalRegistry::AddObjectToList(uint32_t index)
{
	RegisteryObject object;
	memcpy(&object.name_size, &m_flash_buffer[index], 1);
	index++;

	object.name = (char*)&m_flash_buffer[index];
	index += object.name_size;

	memcpy(&object.value_size, &m_flash_buffer[index], 1);
	index++;

	object.value = &m_flash_buffer[index];
	index += object.value_size;

	m_list_objects.push_back(object);
	return index;
}

/// @brief	Copy the flash to shadow buffer than copy the shadow to flash
void InternalRegistry::UpdateFlash()
{
	m_mutex.Lock();

    m_need_update_flash = false;
	memcpy(m_shadow_flash_buffer, m_flash_buffer, m_flash_buffer_size);

	m_mutex.Unlock();

	m_flash.Write(m_shadow_flash_buffer, m_flash_buffer_size);
}

/// @brief Print all the register values.
void InternalRegistry::PrintAll()
{
	m_mutex.Lock();

	printf("\n\\\\\\\\\\ Registry \\\\\\\\\\\n\n");

    for (RegisteryObject& object : m_list_objects)
    {
    	printf("Name - ");
    	for (uint8_t i = 0; i < object.name_size; i++)
    		printf("%c", object.name[i]);

    	printf(",	name size - %u,	value - ", object.name_size);
    	for (uint8_t i = 0; i < object.value_size; i++)
    		printf("0x%X, ", ((uint8_t*)object.value)[i]);

    	printf(",	value size %u \n", object.value_size);
	}

    printf("\n\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n\n");

	m_mutex.Unlock();
}

#endif
