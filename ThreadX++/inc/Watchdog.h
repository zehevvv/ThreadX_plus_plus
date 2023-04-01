/*
 * Watchdog.h
 *
 *  Created on: Mar 31, 2023
 *      Author: zehevvv
 */

#ifndef INC_WATCHDOG_H_
#define INC_WATCHDOG_H_

#include "Task.h"
#include "Singleton.h"
#include "stm32h7xx_hal.h"
#include "TimeEvent.h"

class Watchdog : public Singleton<Watchdog>, public Task
{
friend Singleton<Watchdog>;
public:
	void Refresh(void* pointer);

private:
	Watchdog();
	virtual ~Watchdog();

	TimeEvent 			m_event_petdog;

	static const uint16_t 	TASK_PRIORITY 		= WATCHDOG_PRIORITY;
	static const uint16_t 	STACK_SIZE 			= 256;
};

#endif /* INC_WATCHDOG_H_ */
