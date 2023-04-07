/*
 * DWT.cpp
 *
 *  Created on: Mar 11, 2023
 *      Author: zehevvv
 */

#include <DWT_Utility.h>
#include "tx_api.h"
#include "HW.h"

#ifdef DWT_ENABLE

#include "stm32h7xx_hal.h"


/// @brief C'tor, enable the clock cycle
DWT_Utility::DWT_Utility()
{
	InitClockCycle();
}

/// @brief Delay in microseconds, when it delay it will move to other tasks that have same priority.
/// @param time_to_delay:	The time the thread will delaying.
void DWT_Utility::Delay_us(uint32_t time_to_delay)
{
	uint32_t au32_initial_ticks = DWT->CYCCNT;
	uint32_t au32_ticks = (HAL_RCC_GetHCLKFreq() / 1000000);
	time_to_delay *= au32_ticks;
	while ((DWT->CYCCNT - au32_initial_ticks) < time_to_delay - au32_ticks)
		tx_thread_relinquish();
}

/// @brief Get current ticks of the cycle clock counter.
/// @return current ticks of the cycle clock counter.
uint32_t DWT_Utility::GetCurrentTicks()
{
	return DWT->CYCCNT;
}

/// @brief Convert ticks of cycle counter to microseconds
///
/// @param ticks	- The number of ticks that need to convert to microseconds.
///
/// @return Microseconds.
uint32_t DWT_Utility::ConvrtTiksToUs(uint32_t ticks)
{
	return ticks / (HAL_RCC_GetHCLKFreq() / 1000000);
}

/// @brief Enable the cycle clock.
/// @return Boolean, True - init success.
bool DWT_Utility::InitClockCycle()
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

#endif
