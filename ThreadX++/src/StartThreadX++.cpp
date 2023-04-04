/*
 * StartThreadX++.cpp
 *
 *  Created on: 1 באפר׳ 2023
 *      Author: zehevvv
 */
#include "StartThreadX++.h"
#include "MemoryManager.h"
#include "TasksManager.h"
#include "HW.h"


void StartThreadXPlusPlus(void *first_unused_memory)
{
#ifdef MEMORY_MANAGER_ENABLE
	Memory_manager(first_unused_memory);
#endif
	Tasks_manager_start();
}



