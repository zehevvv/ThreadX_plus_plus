/*
 * Semaphore.h
 *
 *  Created on: 18 במרץ 2023
 *      Author: zehevvv
 */

#ifndef INC_SEMAPHORE_H_
#define INC_SEMAPHORE_H_

#include "tx_api.h"
#include "Types.h"

#define SEMAPHORE_WAIT_FOREVER	(TX_WAIT_FOREVER)
#define SEMAPHORE_NO_WAIT		(TX_NO_WAIT)

class Semaphore {
public:
	Semaphore(uint32_t initial_count = 0);
	bool Get(uint32_t timeout_MS = SEMAPHORE_WAIT_FOREVER);
	void Put();

private:
	TX_SEMAPHORE m_semaphore;
};


#endif /* INC_SEMAPHORE_H_ */
