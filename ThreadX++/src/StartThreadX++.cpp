/*
 * StartThreadX++.cpp
 *
 *  Created on: 1 באפר׳ 2023
 *      Author: zehevvv
 */
#include "StartThreadX++.h"
#include "MemoryManager.h"
#include "TasksManager.h"


void StartThreadXPlusPlus(void *first_unused_memory)
{
	Memory_manager(first_unused_memory);
	Tasks_manager_start();
}



