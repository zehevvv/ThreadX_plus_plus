/*
 * TimeEvent.h
 *
 *  Created on: 19 במרץ 2023
 *      Author: zehevvv
 */

#ifndef INC_TIMEEVENT_H_
#define INC_TIMEEVENT_H_

#include "Event.h"
#include "Macros.h"


class TimeEvent
{
public:

	/// @brief 	C'tor, create the task invoker than register to the event so the event invoke the callback every
	/// 		amount of time and the callback get the invoker object as parameter.
	///
	/// @param pfunction		- Pointer to function that will calls when the event invokes.
	/// @param class_pointer 	- Class pointer that have the "pfunction" parameter as member function.
	/// @param msg_handler		- Pointer to class of "MessageHandler" (most of the time it will be pointer to Task class), this
	/// 						  the thread that will be run the callback "pfunction"
	/// @param time_to_execute	- The amount of time between event invokers (milliseconds).
	/// @param is_periodic		- Flag if the event will invoke once or periodic, true - periodic, false - once.
	template <class T>
	TimeEvent(void (T::*pfunction)(void* param), T* class_pointer, MessageHandler* msg_handler, uint32_t time_to_execute, bool is_periodic)
	{
		m_invoker = new Task_invoker<T>(pfunction, class_pointer, msg_handler);
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
