/*
 * MessageHandler.cpp
 *
 *  Created on: 7 באפר׳ 2023
 *      Author: zehevvv
 */

#include "MessageHandler.h"


/// @brief C'tor.
///
/// @param num_message	- The number of messages that the internal queue can hold
MessageHandler::MessageHandler(uint32_t num_message) :
	m_queue(num_message)
{
}

/// @brief Push message to queue.
///
/// @param value	- value that send as parameter in the message that push to queue.
///
/// @return True - Send message success.
bool MessageHandler::NotifyMessage(void* value)
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
bool MessageHandler::NotifyMessage(MESSAGE& msg)
{
	return m_queue.Push(msg);
}

/// @brief Pull message from queue, the function will wait until get new message from the queue, you can set timeout.
///
/// @param msg		- [Out] Message that pull from the queue.
/// @param timeout	- Timeout for waiting to new message to push to the queue.
bool MessageHandler::Pull(MESSAGE& msg, uint64_t timeout)
{
	return m_queue.Pull(msg, timeout);
}
