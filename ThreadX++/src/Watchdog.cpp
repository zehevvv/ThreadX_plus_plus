/*
 * Watchdog.cpp
 *
 *  Created on: Mar 31, 2023
 *      Author: zehevvv
 */

#include <Watchdog.h>
#include "HW.h"

#define IWDG_KEY_RELOAD                 0x0000AAAAu  /*!< IWDG Reload Counter Enable   */
#define IWDG_KEY_ENABLE                 0x0000CCCCu  /*!< IWDG Peripheral Enable       */
#define IWDG_KEY_WRITE_ACCESS_ENABLE    0x00005555u  /*!< IWDG KR Write Access Enable  */
#define IWDG_KEY_WRITE_ACCESS_DISABLE   0x00000000u  /*!< IWDG KR Write Access Disable */

#define IWDG_KERNEL_UPDATE_FLAGS        (IWDG_SR_WVU | IWDG_SR_RVU | IWDG_SR_PVU)

#define IWDG_PRESCALER_4                0x00000000u                                     /*!< IWDG prescaler set to 4   */
#define IWDG_PRESCALER_8                IWDG_PR_PR_0                                    /*!< IWDG prescaler set to 8   */
#define IWDG_PRESCALER_16               IWDG_PR_PR_1                                    /*!< IWDG prescaler set to 16  */
#define IWDG_PRESCALER_32               (IWDG_PR_PR_1 | IWDG_PR_PR_0)                   /*!< IWDG prescaler set to 32  */
#define IWDG_PRESCALER_64               IWDG_PR_PR_2                                    /*!< IWDG prescaler set to 64  */
#define IWDG_PRESCALER_128              (IWDG_PR_PR_2 | IWDG_PR_PR_0)                   /*!< IWDG prescaler set to 128 */
#define IWDG_PRESCALER_256              (IWDG_PR_PR_2 | IWDG_PR_PR_1)                   /*!< IWDG prescaler set to 256 */


/* Status register needs up to 5 LSI clock periods divided by the clock
   prescaler to be updated. The number of LSI clock periods is upper-rounded to
   6 for the timeout value calculation.
   The timeout value is calculated using the highest prescaler (256) and
   the LSI_VALUE constant. The value of this constant can be changed by the user
   to take into account possible LSI clock period variations.
   The timeout value is multiplied by 1000 to be converted in milliseconds.
   LSI startup time is also considered here by adding LSI_STARTUP_TIME
   converted in milliseconds. */
#define HAL_IWDG_DEFAULT_TIMEOUT        (((6UL * 256UL * 1000UL) / LSI_VALUE) + ((LSI_STARTUP_TIME / 1000UL) + 1UL))


Watchdog::Watchdog() : Task("WatchdogTask", TASK_PRIORITY, STACK_SIZE),
	m_event_petdog(&Watchdog::Refresh, this, this, WATCHDOG_PET_MS, true)
{
	/* Enable IWDG. LSI is turned on automatically */
	WRITE_REG(IWDG1->KR, IWDG_KEY_ENABLE);

	/* Enable write access to IWDG_PR, IWDG_RLR and IWDG_WINR registers by writing
	 0x5555 in KR */
	WRITE_REG(IWDG1->KR, IWDG_KEY_WRITE_ACCESS_DISABLE);

	/* Write to IWDG registers the Prescaler & Reload values to work with */
	IWDG1->PR = IWDG_PRESCALER_32;
	IWDG1->RLR = WATCHDOG_TIMEOUT_MS;

	/* Check pending flag, if previous update not done, return timeout */
	uint32_t tick_start = HAL_GetTick();

	/* Wait for register to be updated */
	while ((IWDG1->SR & IWDG_KERNEL_UPDATE_FLAGS) != 0x00u)
	{
		if ((HAL_GetTick() - tick_start) > HAL_IWDG_DEFAULT_TIMEOUT)
		{
			if ((IWDG1->SR & IWDG_KERNEL_UPDATE_FLAGS) != 0x00u)
			{
				printf("Watchdog init failed\n");
				while (1);
			}
		}
	}

	/* Reload IWDG counter with value defined in the reload register */
	WRITE_REG(IWDG1->KR, IWDG_KEY_RELOAD);
}

Watchdog::~Watchdog()
{
}

void Watchdog::Refresh(void* pointer)
{
	/* Reload IWDG counter with value defined in the reload register */
	WRITE_REG(IWDG1->KR, IWDG_KEY_RELOAD);
}
