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


///////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief C'tor, Read the stored data from flash.
///////////////////////////////////////////////////////////////////////////////////////////////////
InternalRegistry::InternalRegistry() : Task("InternalRegistery", TASK_PRIORITY, STACK_SIZE),
	m_flash(INTERNAL_REGISTRY_START_ADDRESS, INTERNAL_REGISTRY_NUM_SECTOR, INTERNAL_REGISTRY_LOGICAL_BLOCK_SIZE),
	m_need_update_flash(false),
	m_event_flahs_update(&InternalRegistry::EventCheckFlash, this, this, 1000, true)
{
	m_flash_buffer = new uint8_t[m_flash.GetMaxFlashSize()];

	m_flash_buffer_size = m_flash.GetSize();
	m_flash.Read(m_flash_buffer, m_flash_buffer_size);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief D'tor, delete caching buffer.
///////////////////////////////////////////////////////////////////////////////////////////////////
InternalRegistry::~InternalRegistry()
{
	delete[] m_flash_buffer;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief 	This is a callback of event that jump every second and check if buffer of the cache is change (new value is add
/// 		Or old value is changed) and if yes, update the flash.
///
/// @param pointer	- Ignore
///////////////////////////////////////////////////////////////////////////////////////////////////
void InternalRegistry::EventCheckFlash(void* pointer)
{
	if (m_need_update_flash)
	{
		m_mutex.Lock();

	    m_need_update_flash = false;
		m_flash.Write(m_flash_buffer, m_flash_buffer_size);

		m_mutex.Unlock();

		m_flash.WriteToFlash();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Write registry value of buffer to cache buffer,
///
/// @param name			- The name of the value (key).
/// @param name_size	- The size of the name (bytes).
/// @param buffer		- Pointer to buffer of values that need to store.
/// @param buffer_size	- The size of the buffer (bytes).
///////////////////////////////////////////////////////////////////////////////////////////////////
void InternalRegistry::WriteBuffer(char* name, uint8_t name_size, uint8_t* buffer, uint8_t buffer_size)
{
	Write(name, name_size, (void*)buffer, buffer_size);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief 	Write registry value to cache buffer, this function just check if the value is already stored in the cache
/// 		if yes edit the stored value, else write a new value to cache.
///
/// @param name			- The name of the value (key).
/// @param name_size	- The size of the name (bytes).
/// @param buffer		- Pointer to value that need to store.
/// @param buffer_size	- The size of the value type (bytes).
///////////////////////////////////////////////////////////////////////////////////////////////////
void InternalRegistry::Write(char* name, uint8_t name_size, void* value, uint8_t value_size)
{
	m_mutex.Lock();

	RegisteryObject object;

	// Check if the object already exist
	bool is_object_exist = FindObject(name, name_size, object);
	if (is_object_exist)
	{
		if (object.value_size != value_size)
		{
			printf("Error: registry failed to write new value because the old size is %u and the new size is %u", object.value_size, value_size);
			return;
		}

		memcpy(object.value, value, value_size);
	}
	else
	{
		if (value_size + m_flash_buffer_size > m_flash.GetMaxFlashSize())
		{
			printf("Error: registry failed to write new value because don't have enough space");
			return;
		}

		WriteNewObjectToBuffer(name, name_size, value, value_size);
	}

	m_need_update_flash = true;
	m_mutex.Unlock();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Write registry value to cache buffer, write the value in the next order: name size - name - value size - value.
///
/// @param name			- The name of the value (key).
/// @param name_size	- The size of the name (bytes).
/// @param buffer		- Pointer to value that need to store.
/// @param buffer_size	- The size of the value type (bytes).
///////////////////////////////////////////////////////////////////////////////////////////////////
void InternalRegistry::WriteNewObjectToBuffer(char* name, uint8_t name_size, void* value, uint8_t value_size)
{
	memcpy(&m_flash_buffer[m_flash_buffer_size], &name_size, 1);
	m_flash_buffer_size++;

	memcpy(&m_flash_buffer[m_flash_buffer_size], name, name_size);
	m_flash_buffer_size += name_size;

	memcpy(&m_flash_buffer[m_flash_buffer_size], &value_size, 1);
	m_flash_buffer_size++;

	memcpy(&m_flash_buffer[m_flash_buffer_size], value, value_size);
	m_flash_buffer_size += value_size;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Read registry value of buffer from cache buffer,
///
/// @param name			- The name of the value (key).
/// @param name_size	- The size of the name (bytes).
/// @param buffer		- [out] Pointer to buffer of values that need to read.
/// @param buffer_size	- The size of the buffer (bytes).
///
/// @return True - read success.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool InternalRegistry::ReadBuffer(char* name, uint8_t name_size,  uint8_t* buffer, uint8_t buffer_size)
{
	return Read(name, name_size, (void*)buffer, buffer_size);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief 	Read registry value to cache buffer, this function just check if the value is already stored in the cache
/// 		if yes read the stored value, else the read failed.
///
/// @param name			- The name of the value (key).
/// @param name_size	- The size of the name (bytes).
/// @param buffer		- [out] Pointer to value that need to store.
/// @param buffer_size	- The size of the value type (bytes).
///
/// @return True - read success.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool InternalRegistry::Read(char* name, uint8_t name_size, void* value, uint8_t value_size)
{
	m_mutex.Lock();

	RegisteryObject object;
	bool result = FindObject(name, name_size, object);

	if ((result) &&
		(object.value_size == value_size))
	{
		memcpy(value, object.value, value_size);
	}
	else
	{
		printf("Registry not found the object - %s \n", name);
	}

	m_mutex.Unlock();

	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief 	Remove the value from the cache buffer and set the flag of need to update the flash.
///
/// @param name			- The name of the value (key).
/// @param name_size	- The size of the name (bytes).
///////////////////////////////////////////////////////////////////////////////////////////////////
void InternalRegistry::Delete(char* name, uint8_t name_size)
{
	m_mutex.Lock();

	RegisteryObject object;
	uint32_t start_index = 0;
	uint32_t next_object_index = GetObjectByIndex(start_index, object);
	while (next_object_index != 0)
	{
		if ((name_size == object.name_size) &&
			(memcmp(name, object.name, name_size) == 0))
		{
			for (uint32_t i = 0; i < m_flash_buffer_size - next_object_index; ++i)
			{
				m_flash_buffer[start_index + i] = m_flash_buffer[next_object_index + i];
			}

			m_flash_buffer_size -= next_object_index - start_index;
			m_need_update_flash = true;
			break;
		}

		start_index = next_object_index;
		next_object_index = GetObjectByIndex(next_object_index, object);
	}

	m_mutex.Unlock();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Check if the value is exist in the cache.
///
/// @param name			- The name of the value (key).
/// @param name_size	- The size of the name (bytes)
///
/// @return True - the value exist in the cache.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool InternalRegistry::IsExist(char* name, uint8_t name_size)
{
	m_mutex.Lock();

	RegisteryObject object;
	bool result = FindObject(name, name_size, object);

	m_mutex.Unlock();

	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief 	Get the value from the cache, run on the cache and check if one of the object have the
/// 		Same key..
///
/// @param name			- The name of the value (key).
/// @param name_size	- The size of the name (bytes)
/// @param object		- [out] The object.
///
/// @return True - found the object
///////////////////////////////////////////////////////////////////////////////////////////////////
bool InternalRegistry::FindObject(char* name, uint8_t name_size, RegisteryObject& object)
{
	uint32_t index = GetObjectByIndex(0, object);
	while (index != 0)
	{
    	if ((name_size == object.name_size) &&
    		(memcmp(name, object.name, name_size) == 0))
    	{
    		return true;
    	}
    	index = GetObjectByIndex(index, object);
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief 	Read object from the cache at index that provided as parameter and return the index of
/// 		next object
///
/// @param index		- The index in the internal buffer of the flash (cache) where the object.
/// @param object		- [out] The object.
///
/// @return The index of the next object, if reach to end of the buffer return 0.
///////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t InternalRegistry::GetObjectByIndex(uint32_t index, RegisteryObject& object)
{
	if (index >= m_flash_buffer_size)
		return 0;

	uint32_t update_index = index;

	object.name_size = m_flash_buffer[update_index];
	update_index += 1;

	object.name = (char*)&m_flash_buffer[update_index];
	update_index += object.name_size;

	object.value_size = m_flash_buffer[update_index];
	update_index += 1;

	object.value = (void*)&m_flash_buffer[update_index];
	update_index += object.value_size;

	return update_index;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Print all the register values.
///////////////////////////////////////////////////////////////////////////////////////////////////
void InternalRegistry::PrintAll()
{
	m_mutex.Lock();

	printf("\n\\\\\\\\\\ Registry \\\\\\\\\\\n\n");

	RegisteryObject object;
	uint32_t index = GetObjectByIndex(0, object);
	while (index != 0)
	{
    	printf("Name - ");
    	for (uint8_t i = 0; i < object.name_size; i++)
    		printf("%c", object.name[i]);

    	printf(",	name size - %u,	value - ", object.name_size);
    	for (uint8_t i = 0; i < object.value_size; i++)
    		printf("0x%X, ", ((uint8_t*)object.value)[i]);

    	printf("	value size %u \n", object.value_size);

    	index = GetObjectByIndex(index, object);
	}

    printf("\n\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\n\n");

	m_mutex.Unlock();
}

#endif
