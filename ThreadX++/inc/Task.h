#pragma once

#include "MessageHandler.h"


class Task : public MessageHandler
{
public:
	void StartTask();

protected:
	Task(const char* name, uint32_t priority, uint32_t stack_size, uint32_t num_message = 10);
	~Task();

	void Sleep_ms(uint64_t time = TX_WAIT_FOREVER);
	virtual void ReceiveMsg(void* pointer);
	virtual void Main_loop();

private:
	static void Thread_entry(ULONG initial_input);

	TX_THREAD 		m_thread_ptr;
	uint8_t* 		m_thread_stack;
	bool			m_start;
};
