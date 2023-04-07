
#pragma once

#include <stdio.h>
#include "tx_api.h"
#include "Macros.h"


#define QUEUE_WAIT_FOREVER 	(TX_WAIT_FOREVER)
#define QUEUE_NO_WAIT 		(TX_NO_WAIT)

template <typename T>
class Queue {
public:
	/// @brief C'tor, create a memory pool for the values and create the queue.
	///
	/// @param num_values	- The number values that the queue hold.
	Queue(ULONG num_values)
	{
		UINT msg_size = sizeof(T) / 4;
		if ((sizeof(T) % 4) != 0)
			msg_size += 1;

		m_memory = new T[num_values * msg_size * 4];
		UINT status = tx_queue_create(&m_queue, (char*)"queue", msg_size, (VOID*)m_memory , num_values * msg_size * 4);

		if (status != TX_SUCCESS)
			printf("Error: failed to create queue, error - %d \n", status);
	}

	/// @brief D'tor, delete the memory pool of the messages.
	~Queue()
	{
		delete[] m_memory;
	}

	/// @brief Push new value to the queue.
	///
	/// @param value		- The value that will push to the queue
	/// @param timeout_MS	- The max time that threadX will try to push the value to queue.
	///
	/// @return True - the push success.
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

	/// @brief Pull value from the queue.
	///
	/// @param value		- [out] The value that will pull from to the queue
	/// @param timeout_MS	- The max time that threadX will wait until new value will push to queue.
	///
	/// @return True - the pull success.
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
