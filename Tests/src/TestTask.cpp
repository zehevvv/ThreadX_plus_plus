/*
 * TestTask.cpp
 *
 *  Created on: 14 במרץ 2023
 *      Author: zehevvv
 */

#include "TestTask.h"

#include <InternalRegistry.h>
#include "TimeEvent.h"
#include "Debug.h"
#include "tx_api.h"
#include "LedTask.h"

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


uint8_t m_data[] = {0x1,0x2,0x3,0x4,0x5,0x6,0x7,0x8};
uint8_t m_rx_Data[12];

void FlashTest()
{
//	Flash_earse(0x081E0000, 20);
//	Flash_write(0x081E0000, m_data, sizeof(m_data));
//	Flash_read(0x081E0000, m_rx_Data, sizeof(m_rx_Data));
//	InternalRegistry::Instance();
}

void TestInternalRegistry()
{
//	InternalRegistry::Instance();
//	uint32_t val = 0x12345678;
//	InternalRegistry::Instance()->Write((char*)"Test", 4, val);
//
//	uint16_t val1 = 0x9abc;
//	InternalRegistry::Instance()->Write((char*)"Test1", 5, val1);
//
//	uint32_t val2 = 0;
//	InternalRegistry::Instance()->Read((char*)"Test", (uint8_t)4, val2);
//	printf("val2 = 0x%lX \n", val2);
//
//	uint16_t val3 = 0;
//	InternalRegistry::Instance()->Read((char*)"Test1", (uint8_t)5, val3);
//	printf("val3 = 0x%X \n", val3);




//	uint8_t buff1[] = "Hello to me";
//	uint8_t buff2[] = "Who are you";

//	InternalRegistry::Instance()->WriteBuffer("Buff1", 5, buff1, sizeof(buff1));
//
//	InternalRegistry::Instance()->WriteBuffer("Buff2", 5, buff2, sizeof(buff2));

//	uint8_t read_buff1[sizeof(buff1)];
//	uint8_t read_buff2[sizeof(buff2)];
//	memset(read_buff1, 0, sizeof(read_buff1));
//	memset(read_buff2, 0, sizeof(read_buff2));
//
//	InternalRegistry::Instance()->ReadBuffer("Buff2", 5, read_buff2, sizeof(read_buff2));
//	InternalRegistry::Instance()->ReadBuffer("Buff1", 5, read_buff1, sizeof(read_buff1));
//
//	printf("buffer1 %s \n", read_buff1);
//	printf("buffer2 %s \n", read_buff2);


	InternalRegistry::Instance()->PrintAll();
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


TestTask::TestTask() : Task("TestTask", TASK_PRIORITY, STACK_SIZE)
{
}

void TestTask::Main_loop()
{
	TestInternalRegistry();
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////// Test event by other task //////////////////////////////////

	LedTask::Instance()->event.Register(&TestTask::TestEvent, this, this);
	while(1)
	{
//		Debug::MeasureTime measureTime;
		printf("time - %lu \n",tx_time_get());
//		measureTime.Start();
		Sleep_ms(1000);
//		measureTime.End();
//		Sleep();
	}

//////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// Test Message to other thread ////////////////

//	int counter = 0;
//	while(1)
//	{
//		counter++;
//		LedTask::Instance()->NotifyMessage((void*)&counter);
//		Sleep_ms(1000);
//	}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// Test mutex //////////////////////////////////

//	int counter = 0;
//	while(1)
//	{
//		Debug::MeasureTime measure;
//		Sleep_ms(75);
//		measure.Start();
//		LedTask::Instance()->m_mutex.Lock();
//		LedTask::Instance()->m_mutex.Unlock();
//		uint32_t time = measure.End();
//		if (time > 1000)
//		{
//			printf("time take %u, counter %d \n", time, counter);
//			counter = 0;
//		}
//		else
//		{
//			counter++;
//		}
//	}

////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// Test semaphore //////////////////////////////////
//	while(1)
//	{
//		LedTask::Instance()->m_semahore.Get();
//		printf("Get semaphore\n");
//	}
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////// Test semaphore //////////////////////////////////

//	TimeEvent time_event(&TestTask::TestTimeEvent, this, this, 1000, true);
//	while(1)
//	{
//		ReadNewMessage(1000);
//	}
////////////////////////////////////////////////////////////////////////////////

}

void TestTask::TestEvent(void* param)
{
	printf("Test event success, counter %d \n", *(int*)param);
//	LedTask::Instance()->event.Unregister(&TestTask::TestEvent, this);
}

void TestTask::TestTimeEvent(void* param)
{
	printf("Test time event success\n");
}




