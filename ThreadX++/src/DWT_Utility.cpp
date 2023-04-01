/*
 * DWT.cpp
 *
 *  Created on: Mar 11, 2023
 *      Author: zehevvv
 */

#include <DWT_Utility.h>
#include "stm32h7xx_hal.h"
#include "tx_api.h"


DWT_Utility::DWT_Utility()
{
	Init_clock_cycle();
}

void DWT_Utility::Delay_us(uint32_t time_to_sleep)
{
	uint32_t au32_initial_ticks = DWT->CYCCNT;
	uint32_t au32_ticks = (HAL_RCC_GetHCLKFreq() / 1000000);
	time_to_sleep *= au32_ticks;
	while ((DWT->CYCCNT - au32_initial_ticks) < time_to_sleep - au32_ticks)
		tx_thread_relinquish();
}

uint32_t DWT_Utility::GetCurrentTicks()
{
	return DWT->CYCCNT;
}

uint32_t DWT_Utility::ConvrtTiksToUs(uint32_t ticks)
{
	return ticks / (HAL_RCC_GetHCLKFreq() / 1000000);
}

bool DWT_Utility::Init_clock_cycle()
{
    /* Disable TRC */
    CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk; // ~0x01000000;
    /* Enable TRC */
    CoreDebug->DEMCR |=  CoreDebug_DEMCR_TRCENA_Msk; // 0x01000000;

    /* Disable clock cycle counter */
    DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk; //~0x00000001;
    /* Enable  clock cycle counter */
    DWT->CTRL |=  DWT_CTRL_CYCCNTENA_Msk; //0x00000001;

    /* Reset the clock cycle counter value */
    DWT->CYCCNT = 0;

    /* 3 NO OPERATION instructions */
    __ASM volatile ("NOP");
    __ASM volatile ("NOP");
    __ASM volatile ("NOP");

    /* Check if clock cycle counter has started */
    if(DWT->CYCCNT)
    {
       return false; /*clock cycle counter started*/
    }
    else
    {
      return true; /*clock cycle counter not started*/
    }
}
