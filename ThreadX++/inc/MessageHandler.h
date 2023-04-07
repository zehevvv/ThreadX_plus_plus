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
	void* 		value;
	MSG_TYPE	type;
} MESSAGE;
#pragma pack()

class MessageHandler
{
public:
	/// @brief Push message to queue.
	///
	/// @param value	- value that send as parameter in the message that push to queue.
	///
	/// @return True - Send message success.
	bool NotifyMessage(void* value = NULL)
	{
		MESSAGE msg;
		msg.type = MSG_TYPE_NORMAL;
		msg.value = value;
		return m_queue.Push(msg);
	}

	/// @brief Push message to queue.
	///
	/// @param msg	- Message that will push to queue.
	///
	/// @return True - Send message success.
	bool NotifyMessage(MESSAGE& msg)
	{
		return m_queue.Push(msg);
	}

protected:
	/// @brief C'tor.
	///
	/// @param num_message	- The number of messages that the internal queue can hold
	MessageHandler(uint32_t num_message) :
		m_queue(num_message)
	{
	}

	/// @brief Pull message from queue, the function will wait until get new message from the queue, you can set timeout.
	///
	/// @param msg		- [Out] Message that pull from the queue.
	/// @param timeout	- Timeout for waiting to new message to push to the queue.
	bool Pull(MESSAGE& msg, uint64_t timeout)
	{
		return m_queue.Pull(msg, timeout);
	}

	/// @brief	The function that need called when pull new message.
	///
	/// @param value	- The parameter that send over the message.
	virtual void ReceiveMsg(void* value) = 0;

private:

	Queue<MESSAGE>	m_queue;
};


#endif /* INC_MESSAGEHANDLER_H_ */
