/*
 * InternalRegistery.h
 *
 *  Created on: 22 במרץ 2023
 *      Author: zehevvv
 */

#ifndef INC_INTERNALREGISTRY_H_
#define INC_INTERNALREGISTRY_H_

#include "Singleton.h"
#include "FlashManagment.h"
#include "Mutex.h"
#include "Task.h"
#include "TimeEvent.h"
#include "HW.h"

struct RegisteryObject;

class InternalRegistry : public Task, public Singleton<InternalRegistry>
{
friend Singleton<InternalRegistry>;
public:
	/// @brief 	Write registry value to cache buffer, this function just check if the value is already stored in the cache
	/// 		if yes edit the stored value, else write a new value to cache.
	///
	/// @param name			- The name of the value (key).
	/// @param name_size	- The size of the name (bytes).
	/// @param value		- The value that will store in the flash.
	template <typename T>
	void Write(char* name, uint8_t name_size, T value)
	{
		Write(name, name_size, (void*)&value, (uint8_t)sizeof(T));
	}

	/// @brief Read registry value of buffer from cache buffer,
	///
	/// @param name			- The name of the value (key).
	/// @param name_size	- The size of the name (bytes).
	/// @param buffer		- [out] The value that store in the cache.
	template <typename T>
	bool Read(char* name, uint8_t name_size, T& value)
	{
		return Read(name, name_size, (void*)&value, (uint8_t)sizeof(T));
	}

	void WriteBuffer(char* name, uint8_t name_size, uint8_t* buffer, uint8_t buffer_size);
	bool ReadBuffer(char* name, uint8_t name_size,  uint8_t* buffer, uint8_t buffer_size);
	bool IsExist(char* name, uint8_t name_size);
	void PrintAll();

private:
	InternalRegistry();
	virtual ~InternalRegistry();

	bool FindObject(char* name, uint8_t name_size, RegisteryObject& object);
	uint32_t GetObjectByIndex(uint32_t index, RegisteryObject& object);
	void Write(char* name, uint8_t name_size, void* value, uint8_t value_size);
	void WriteNewObjectToBuffer(char* name, uint8_t name_size, void* value, uint8_t value_size);
	void EventCheckFlash(void* pointer);
	bool Read(char* name, uint8_t name_size, void* value, uint8_t value_size);

	FlashManagment 				m_flash;
	Mutex						m_mutex;
	bool						m_need_update_flash;
	uint8_t*					m_flash_buffer;
	uint8_t*					m_shadow_flash_buffer;
	uint32_t					m_flash_buffer_size;
	TimeEvent 					m_event_flahs_update;

	static const uint16_t 	TASK_PRIORITY 	= INTERNAL_REGISTRY_TASK_PRIORITY;
	static const uint16_t 	STACK_SIZE 		= 1024;
};



#endif /* INC_INTERNALREGISTRY_H_ */
