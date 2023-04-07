/*
 * Event.h
 *
 *  Created on: 14 במרץ 2023
 *      Author: zehevvv
 */

#ifndef INC_EVENT_H_
#define INC_EVENT_H_

#include "MessageHandler.h"
#include "list"

class Event;

class Class_invoker_base
{
public:
	/// @brief Set the parameter that should send to callback that will invoke.
	void SetParam(void* param)
	{
		m_param = param;
	}

	virtual void Invoke() = 0;
	virtual void SendMsg() = 0;
	virtual ~Class_invoker_base(){}

protected:
	void* m_param;
};

template <class T>
class Task_invoker : public Class_invoker_base
{
friend Event;
public:
	/// @brief C'tor, set the pointers so the callback function can invoke
	///
	/// @param pfunction		- Pointer to function that will calls when the event invokes.
	/// @param class_pointer 	- Class pointer that have the "pfunction" parameter as member function.
	/// @param msg_handler		- Pointer to class of "MessageHandler" (most of the time it will be pointer to Task class), this
	/// 						  the thread that will be run the callback "pfunction"
	Task_invoker(void (T::*pfunction)(void* param), T* class_pointer, MessageHandler* msgHandler) :
		m_functionn(pfunction),
		m_class_pointer(class_pointer),
		m_msgHandler(msgHandler)
	{}

	/// @brief invoke the callback with the parameter.
	void Invoke()
	{
		(*m_class_pointer.*m_functionn)(m_param);
	}

	/// @brief 	Send message of type "event" that have the pointer of the invoker, the message send to message handler that should
	/// 		be task, the message push to queue of the task, when the task pull the message it will invoke the callback of the
	/// 		invokers
	void SendMsg()
	{
		MESSAGE msg;
		msg.value = (void*)this;
		msg.type = MSG_TYPE_EVENT;
		m_msgHandler->NotifyMessage(msg);
	}

private:
	void (T::*		m_functionn)(void* param);
	T* 				m_class_pointer;
	MessageHandler* m_msgHandler;
};

class Event
{
public:
	Event()
	{
	}

	/// @brief C'tor, Create the invokers object and push it to queue of invokers.
	///
	/// @param pfunction		- Pointer to function that will calls when the event invokes.
	/// @param class_pointer 	- Class pointer that have the "pfunction" parameter as member function.
	/// @param msg_handler		- Pointer to class of "MessageHandler" (most of the time it will be pointer to Task class), this
	/// 						  the thread that will be run the callback "pfunction"
	template <class T>
	void Register(void (T::*pfunction)(void* param), T* class_pointer, MessageHandler* msg_handler)
	{
		Task_invoker<T>* invoker = new Task_invoker<T>(pfunction, class_pointer, msg_handler);
		m_invokeres.push_back(invoker);
	}

	/// @brief C'tor, Remove the invokers from the queue and delete him.
	///
	/// @param pfunction		- Pointer to function that will calls when the event invokes.
	/// @param class_pointer 	- Class pointer that have the "pfunction" parameter as member function.
	template <class T>
	void Unregister(void (T::*pfunction)(void* param), T* class_pointer)
	{
	    for (Class_invoker_base* invoker : m_invokeres)
	    {
	    	Task_invoker<T>* object = (Task_invoker<T>*)invoker;
	    	if (object->m_class_pointer == class_pointer &&
				object->m_functionn == pfunction)
	    	{
	    		m_invokeres.remove(invoker);
	    		delete invoker;
	    		break;
	    	}
		}
	}

	/// @brief Invoke the event, run on all the invoker and active them.
	///
	/// @brief param	- The parameter that send to all of the callback of the invokers
	void Invoke(void* param = NULL)
	{
	    for (Class_invoker_base* invoker : m_invokeres)
	    {
	    	invoker->SetParam(param);
	    	invoker->SendMsg();
		}
	}

private:
	std::list<Class_invoker_base*> m_invokeres;
};


#endif /* INC_EVENT_H_ */
