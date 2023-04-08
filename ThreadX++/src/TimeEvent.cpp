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

/// @brief D'tor, delete the timer
TimeEvent::~TimeEvent()
{
	UINT status = tx_timer_delete(&m_timer);
	if (status != TX_SUCCESS)
	{
		printf("Error: failed to remove timer, error - %d \n", status);
	}
	delete m_invoker;
}

/// @brief 	Register to the event so the event invoke the callback every amount of time and the
/// 		callback get the invoker object as parameter.
///
/// @param invoker			- Pointer to task invoker
/// @param time_to_execute	- The amount of time between event invokers (milliseconds).
/// @param is_periodic		- Flag if the event will invoke once or periodic, true - periodic, false - once.
void TimeEvent::Register(ULONG invoker, uint32_t time_to_execute, bool is_periodic)
{
	uint32_t time_to_execute_ticks = CONVERT_MS_TO_TICKS(time_to_execute);
	if (time_to_execute_ticks == 0)
		time_to_execute_ticks = 1;

	uint32_t reschedule_ticks = 0;

	if (is_periodic)
		reschedule_ticks = time_to_execute_ticks;

	UINT status = tx_timer_create(&m_timer, (char*)" ", TimerExpired, invoker, time_to_execute_ticks, reschedule_ticks, TX_AUTO_ACTIVATE);
	if (status != TX_SUCCESS)
	{
		printf("Error: failed to create timer, error - %d \n", status);
	}
}

/// @brief The callback that the timer of the ThreadX will invoke every time the event need to invoke.
///
/// @param param	- The pointer to invoker object of the register event.
void TimeEvent::TimerExpired(ULONG param)
{
	Class_invoker_base* invoker = (Class_invoker_base*)param;
	invoker->SendMsg();
}

