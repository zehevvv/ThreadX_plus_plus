/*
 * TimeEvent.h
 *
 *  Created on: 19 במרץ 2023
 *      Author: zehevvv
 */

#ifndef INC_TIMEEVENT_H_
#define INC_TIMEEVENT_H_

#include "Event.h"
#include "Utility.h"


class TimeEvent
{
public:
	template <class a>
	TimeEvent(void (a::*pfunction)(void* param), a* class_pointer, MessageHandler* msg_handler, uint32_t time_to_execute, bool is_periodic)
	{
		m_invoker = new Task_invoker<a>(pfunction, class_pointer, msg_handler);
		Register((ULONG)m_invoker, time_to_execute, is_periodic);
	}

	~TimeEvent();

private:
	static void TimerExpired(ULONG param);

	void Register(ULONG invoker, uint32_t time_to_execute, bool is_periodic);

	TX_TIMER 			m_timer;
	Class_invoker_base* m_invoker;
};

#endif /* INC_TIMEEVENT_H_ */
