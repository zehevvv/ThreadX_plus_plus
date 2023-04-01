
#pragma once

#include "tx_api.h"
#include "Utility.h"
#include <stdio.h>

#define QUEUE_WAIT_FOREVER 	(TX_WAIT_FOREVER)
#define QUEUE_NO_WAIT 		(TX_NO_WAIT)

template <typename T>
class Queue {
public:
	Queue(ULONG num_messages)
	{
		UINT msg_size = sizeof(T) / 4;
		if ((sizeof(T) % 4) != 0)
			msg_size += 1;

		m_memory = new T[num_messages * msg_size * 4];
		UINT status = tx_queue_create(&m_queue, (char*)"queue", msg_size, (VOID*)m_memory , num_messages * msg_size * 4);

		if (status != TX_SUCCESS)
			printf("Error: failed to create queue, error - %d \n", status);
	}

	~Queue()
	{
		delete[] m_memory;
	}

	bool Push(T& value, uint32_t timeout_MS = QUEUE_NO_WAIT)
	{
		if (timeout_MS != QUEUE_WAIT_FOREVER)
			timeout_MS = CONVERT_MS_TO_TICKS(timeout_MS);
		UINT status = tx_queue_send(&m_queue, (VOID*)&value, timeout_MS);
		if (status != TX_SUCCESS)
		{
			printf("queue push error %d\n", status);
			return  false;
		}
		else
		{
			return true;
		}
	}

	bool Pull(T& value, uint32_t timeout_MS = 10)
	{
		if (timeout_MS != QUEUE_WAIT_FOREVER)
			timeout_MS = CONVERT_MS_TO_TICKS(timeout_MS);

		UINT status = tx_queue_receive(&m_queue, &value, timeout_MS);
		if (status == TX_SUCCESS)
		{
			return true;
		}
		else
		{
			if (status != TX_QUEUE_EMPTY)
			{
				printf("queue pull error %d\n", status);
			}
			return false;
		}
	}

private:
	TX_QUEUE	m_queue;
	T* 			m_memory;
};
