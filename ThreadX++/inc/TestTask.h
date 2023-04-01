/*
 * TestTask.h
 *
 *  Created on: 14 במרץ 2023
 *      Author: zehevvv
 */

#ifndef INC_TESTTASK_H_
#define INC_TESTTASK_H_

#include "Task.h"
#include "Singleton.h"
#include "Event.h"


class TestTask : public Task, public Singleton<TestTask>
{
friend Singleton<TestTask>;
private:
	TestTask();
	void Main_loop();
	void TestEvent(void* param);
	void TestTimeEvent(void* param);

	static const uint16_t 	TASK_PRIORITY 	= NORMAL_PRIORITY;
	static const uint16_t 	STACK_SIZE 		= 1024;
	Event event;
};


#endif /* INC_TESTTASK_H_ */
