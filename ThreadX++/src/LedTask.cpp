#include "LedTask.h"
#include "stm32h7xx_hal.h"
#include "main.h"
#include "stdio.h"


LedTask::LedTask() : Task("LedTask", TASK_PRIORITY, STACK_SIZE)
{
}

void LedTask::Main_loop()
{
////////////////////////////////////////////////////////////////
///////////////////// Test blink ///////////////////////////////

//	while(1)
//	{
//		ToggleLed();
//		printf("blink \n");
//		Sleep_ms(1000);
//	}
////////////////////////////////////////////////////////\////////

////////////////////////////////////////////////////////////////
///////////////////// Test Event by other task ///////////////////////////////

	int counter = 0;
	while(1)
	{
		counter++;
		event.Invoke((void*)&counter);
		Sleep_ms(1000);
	}
////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
///////////////////// Test mutex ///////////////////////////////

//	while(1)
//	{
//		m_mutex.Lock();
//		Sleep_ms(1000);
//		m_mutex.Unlock();
//	}

////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////
///////////////////// Test Semaphore ///////////////////////////////

//	while(1)
//	{
//		m_semahore.Put();
//		Sleep_ms(1000);
//	}

////////////////////////////////////////////////////////////////

}

void LedTask::ReceiveMsg(void* pointer)
{
	printf("Led task counter %d \n", *(int*)pointer);
}

void LedTask::ToggleLed()
{
//	HAL_GPIO_TogglePin(LD1_GPIO_Port , LD1_Pin);
}
