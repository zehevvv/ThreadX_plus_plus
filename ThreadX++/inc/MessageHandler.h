/*
 * MessageHandler.h
 *
 *  Created on: 16 במרץ 2023
 *      Author: zehevvv
 */

#ifndef INC_MESSAGEHANDLER_H_
#define INC_MESSAGEHANDLER_H_

#include "stdint.h"
#include "tx_api.h"
#include "Queue.h"


typedef enum MSG_TYPE : uint8_t
{
	MSG_TYPE_NORMAL = 0,
	MSG_TYPE_EVENT,
} MSG_TYPE;

#pragma pack(1)
typedef struct MESSAGE
{
	void* 		pointer;
	MSG_TYPE	type;
} MESSAGE;
#pragma pack()

class MessageHandler
{
public:
	bool NotifyMessage(void* pointer)
	{
		MESSAGE msg;
		msg.type = MSG_TYPE_NORMAL;
		msg.pointer = pointer;
		return m_queue.Push(msg);
	}

	bool NotifyMessage(MESSAGE& msg)
	{
		return m_queue.Push(msg);
	}

protected:
	MessageHandler(uint32_t num_message) :
		m_queue(num_message)
	{
	}

	bool Pull(MESSAGE& msg, uint64_t time)
	{
		return m_queue.Pull(msg, time);
	}

	virtual void ReceiveMsg(void* pointer) = 0;

private:

	Queue<MESSAGE>	m_queue;
};


#endif /* INC_MESSAGEHANDLER_H_ */
