/*
 * Tasksmanager.cpp
 *
 *  Created on: 11 במרץ 2023
 *      Author: zehevvv
 */

#include <stdio.h>
#include "TasksManager.h"
#include "LedTask.h"
#include "TestTask.h"
#include "InternalRegistry.h"
#include "Watchdog.h"


void Tasks_manager_start()
{
	printf("ThreadX++ start run \n");

//	Watchdog::Instance()->Instance();
	InternalRegistry::Instance()->Instance();

	LedTask::Instance()->StartTask();
	TestTask::Instance()->StartTask();
}
