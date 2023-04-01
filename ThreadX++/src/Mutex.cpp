/*
 * Mutex.cpp
 *
 *  Created on: 18 במרץ 2023
 *      Author: zehevvv
 */

#include <Mutex.h>
#include <stdio.h>
#include "Utility.h"

Mutex::Mutex() {

	UINT status = tx_mutex_create(&m_mutex, (char*)" ", TX_INHERIT);
	if (status != TX_SUCCESS)
	{
		printf("Error: failed to create mutex, error - %d \n", status);
	}
}


void Mutex::Lock(uint32_t timeout_MS)
{
	if (timeout_MS != MUTEX_WAIT_FOREVER)
		timeout_MS = CONVERT_MS_TO_TICKS(timeout_MS);

	UINT status = tx_mutex_get(&m_mutex, TX_WAIT_FOREVER);
	if (status != TX_SUCCESS)
	{
		printf("Error: failed to lock mutex, error - %d \n", status);
	}
}

void Mutex::Unlock()
{
	UINT status = tx_mutex_put(&m_mutex);
	if (status != TX_SUCCESS)
	{
		printf("Error: failed to unlock mutex, error - %d \n", status);
	}
}
