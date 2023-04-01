/*
 * Tasksmanager.cpp
 *
 *  Created on: 11 במרץ 2023
 *      Author: zehevvv
 */

#include "TasksManager.h"
#include "LedTask.h"
#include "TestTask.h"
#include "Watchdog.h"
#include "InternalRegistry.h"


void Tasks_manager_start()
{
	printf("ThreadX++ start run \n");

	// Start the watchdog
	Watchdog::Instance();
	Watchdog::Instance()->StartTask();

	// Init the tasks
	InternalRegistry::Instance();
	LedTask::Instance();
	TestTask::Instance();

	// Start the task
	InternalRegistry::Instance()->StartTask();
	LedTask::Instance()->StartTask();
	TestTask::Instance()->StartTask();
}
