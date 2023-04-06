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

/// @brief 	This function should call from function "tx_application_define" and also need pass from this function the
/// 		parameter "first_unused_memory".
///
/// @param first_unused_memory	- The address of the start of unused memory that needed for the allocate memory.
void StartThreadXPlusPlus(void *first_unused_memory)
{
#ifdef MEMORY_MANAGER_ENABLE
	Memory_manager(first_unused_memory);
#endif
	Tasks_manager_start();
}



