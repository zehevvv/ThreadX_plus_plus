/*
 * Mutex.h
 *
 *  Created on: 18 במרץ 2023
 *      Author: zehevvv
 */

#ifndef INC_MUTEX_H_
#define INC_MUTEX_H_

#include "tx_api.h"
#include "Types.h"

#define MUTEX_WAIT_FOREVER	(TX_WAIT_FOREVER)
#define MUTEX_NO_WAIT		(TX_NO_WAIT)

class Mutex
{
public:
	Mutex();
	bool Lock(uint32_t timeout_MS = TX_WAIT_FOREVER);
	void Unlock();
private:
	TX_MUTEX m_mutex;
};

#endif /* INC_MUTEX_H_ */
