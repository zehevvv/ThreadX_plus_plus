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
#include "list"
#include "Mutex.h"
#include "Task.h"
#include "TimeEvent.h"

struct RegisteryObject;

class InternalRegistry : public Task, public Singleton<InternalRegistry>
{
friend Singleton<InternalRegistry>;
public:
	template <typename T>
	void Write(char* name, uint8_t name_size, T value)
	{
		Write(name, name_size, (void*)&value, (uint8_t)sizeof(T));
	}

	template <typename T>
	bool Read(char* name, uint8_t name_size, T& value)
	{
		return Read(name, name_size, (void*)&value, (uint8_t)sizeof(T));
	}

	void WriteBuffer(char* name, uint8_t name_size, uint8_t* buffer, uint8_t buffer_size);
	bool ReadBuffer(char* name, uint8_t name_size,  uint8_t* buffer, uint8_t buffer_size);

	bool IsExist(char* name, uint8_t name_size);

	void PrintAll();
protected:
	void OnStart();

private:
	InternalRegistry();
	virtual ~InternalRegistry();

	void Write(char* name, uint8_t name_size, void* value, uint8_t value_size);
	bool Read(char* name, uint8_t name_size, void* value, uint8_t value_size);
	RegisteryObject* GetObject(char* name, uint8_t name_size);
	void UpdateListObject();
	void UpdateFlash();
	void WriteNewObjectToBuffer(char* name, uint8_t name_size, void* value, uint8_t value_size);
	uint32_t AddObjectToList(uint32_t index);
	void EventCheckFlash(void* pointer);

	FlashManagment 				m_flash;
	std::list<RegisteryObject>	m_list_objects;
	Mutex						m_mutex;
	bool						m_need_update_flash;
	uint8_t*					m_flash_buffer;
	uint8_t*					m_shadow_flash_buffer;
	uint32_t					m_flash_buffer_size;
	TimeEvent 					m_event_flahs_update;

	static const uint16_t 	TASK_PRIORITY 	= FLASH_PRIORITY;
	static const uint16_t 	STACK_SIZE 		= 1024;
};



#endif /* INC_INTERNALREGISTRY_H_ */
