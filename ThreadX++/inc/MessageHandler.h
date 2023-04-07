/*
 * MessageHandler.h
 *
 *  Created on: 16 במרץ 2023
 *      Author: zehevvv
 */

#ifndef INC_MESSAGEHANDLER_H_
#define INC_MESSAGEHANDLER_H_

#include "stdint.h"
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
	bool NotifyMessage(void* value = NULL);
	bool NotifyMessage(MESSAGE& msg);

protected:
	MessageHandler(uint32_t num_message);
	bool Pull(MESSAGE& msg, uint64_t timeout);

	/// @brief	The function that need called when pull new message.
	///
	/// @param value	- The parameter that send over the message.
	virtual void ReceiveMsg(void* value) = 0;

private:

	Queue<MESSAGE>	m_queue;
};


#endif /* INC_MESSAGEHANDLER_H_ */
