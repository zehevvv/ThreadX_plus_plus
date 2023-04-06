/*
 * Semaphore.cpp
 *
 *  Created on: 18 במרץ 2023
 *      Author: zehevvv
 */

#include <Semaphore.h>
#include <stdio.h>
#include "Macros.h"

/// @brief C'tor, create the semaphore.
///
/// @param initial_count	- The number of time need to put before the get release.
Semaphore::Semaphore(uint32_t initial_count)
{
	UINT status = tx_semaphore_create(&m_semaphore, (char*) " ", initial_count);
	if (status != TX_SUCCESS)
	{
		printf("Error: failed to create semaphore, error - %d \n", status);
	}
}

/// @brief Get the semaphore
///
/// @param timeout_MS	- The max time try to get semaphore (milliseconds).
///
/// @return True - get success.
bool Semaphore::Get(uint32_t timeout_MS)
{
	if (timeout_MS != SEMAPHORE_WAIT_FOREVER)
		timeout_MS = CONVERT_MS_TO_TICKS(timeout_MS);

	UINT status = tx_semaphore_get(&m_semaphore, timeout_MS);
	if (status != TX_SUCCESS)
	{
		printf("Error: failed to get semaphore, error - %d \n", status);
		return false;
	}
	else
	{
		return  true;
	}
}

/// @brief Put the semaphore
void Semaphore::Put()
{
	UINT status = tx_semaphore_put(&m_semaphore);
	if (status != TX_SUCCESS)
	{
		printf("Error: failed to put semaphore, error - %d \n", status);
	}
}

