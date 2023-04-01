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
	Task_invoker(void (T::*pfunction)(void* param), T* class_pointer, MessageHandler* msgHandler) :
		m_functionn(pfunction),
		m_class_pointer(class_pointer),
		m_msgHandler(msgHandler)
	{}

	void Invoke()
	{
		(*m_class_pointer.*m_functionn)(m_param);
	}

	void SendMsg()
	{
		MESSAGE msg;
		msg.pointer = (void*)this;
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

	template <class T>
	void Register(void (T::*pfunction)(void* param), T* class_pointer, MessageHandler* msg_handler)
	{
		Task_invoker<T>* invoker = new Task_invoker<T>(pfunction, class_pointer, msg_handler);
		m_invokeres.push_back(invoker);
	}

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
	    		break;
	    	}
		}
	}

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
