/*
 * TimeEvent.cpp
 *
 *  Created on: 31 במרץ 2023
 *      Author: zehevvv
 */




/*
 * TimeEvent.h
 *
 *  Created on: 19 במרץ 2023
 *      Author: zehevvv
 */

#include "TimeEvent.h"

TimeEvent::~TimeEvent()
{
	UINT status = tx_timer_delete(&m_timer);
	if (status != TX_SUCCESS)
	{
		printf("Error: failed to remove timer, error - %d \n", status);
	}
	delete m_invoker;
}

void TimeEvent::Register(ULONG invoker, uint32_t time_to_execute, bool is_periodic)
{
	uint32_t time_to_execute_ticks = CONVERT_MS_TO_TICKS(time_to_execute);
	uint32_t reschedule_ticks = 0;

	if (is_periodic)
		reschedule_ticks = time_to_execute_ticks;

	UINT status = tx_timer_create(&m_timer, (char*)" ", TimerExpired, invoker, time_to_execute, reschedule_ticks, TX_AUTO_ACTIVATE);
	if (status != TX_SUCCESS)
	{
		printf("Error: failed to create timer, error - %d \n", status);
	}
}

void TimeEvent::TimerExpired(ULONG param)
{
	Class_invoker_base* invoker = (Class_invoker_base*)param;
	invoker->SendMsg();
}

