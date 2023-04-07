#include "Task.h"
#include "tx_api.h"
#include "Queue.h"
#include "Event.h"
#include "Macros.h"


Task::Task(const char* name, uint32_t priority, uint32_t stack_size, uint32_t num_message) :
		MessageHandler(num_message),
		m_start(false)
{
	m_thread_stack = new uint8_t[stack_size];
	UINT status = tx_thread_create(&m_thread_ptr,
					 	 	 	   (char*)name,
								   Thread_entry,
								   (ULONG)this,
								   m_thread_stack,
								   stack_size,
								   priority,
								   priority,
								   1,
								   TX_AUTO_START);
	if (status != TX_SUCCESS)
	{
		printf("Error: failed to create thread, error - %d \n", status);
		while(1);
	}

	status = tx_thread_suspend(&m_thread_ptr);
	if (status != TX_SUCCESS)
	{
		printf("Error: failed to suspend thread, error - %d \n", status);
		while(1);
	}
}

Task::~Task()
{
	UINT status = tx_thread_delete(&m_thread_ptr);
	if (status != TX_SUCCESS)
	{
		printf("Error: failed to delete thread, error - %d \n", status);
		while(1);
	}

	delete[] m_thread_stack;
}

void Task::StartTask()
{
	UINT status = tx_thread_resume(&m_thread_ptr);
	if (status != TX_SUCCESS)
	{
		printf("Error: failed to resume thread, error - %d \n", status);
		while(1);
	}

	m_start = true;
}

void Task::Sleep_ms(uint64_t time)
{
	MESSAGE msg;
	ULONG end_sleep_time = tx_time_get() + CONVERT_MS_TO_TICKS(time);
	ULONG current_time = tx_time_get();

	while (current_time < end_sleep_time || time == TX_WAIT_FOREVER)
	{
		ULONG time_to_sleep = end_sleep_time - current_time;

		if (Pull(msg, CONVERT_TICKS_TO_MS(time_to_sleep)))
		{
			if (msg.type == MSG_TYPE_NORMAL)
			{
				ReceiveMsg(msg.value);
			}
			else
			{
				((Class_invoker_base*)(msg.value))->Invoke();
			}
		}

		current_time = tx_time_get();
	}
}

void Task::ReceiveMsg(void* pointer)
{
}

void Task::Main_loop()
{
	while(1)
	{
		Sleep_ms();
	}
}

void Task::Thread_entry(ULONG initial_input)
{
	Task* ThisTask = (Task*)initial_input;
	while(!ThisTask->m_start)
	{
		ThisTask->Sleep_ms(1);
	}

	ThisTask->Main_loop();
}
