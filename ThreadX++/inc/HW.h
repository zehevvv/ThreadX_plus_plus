/*
 * HW.h
 *
 *  Created on: Mar 31, 2023
 *      Author: zehevvv
 */

#ifndef HW_H_
#define HW_H_


#define INTERNAL_FLASH_FLASHWORD 		(8)
#define INTERNAL_FLASH_SECTOR_SIZE		(0x20000)


#define INTERNAL_REGISTRY_START_ADDRESS			(0x081C0000)
#define INTERNAL_REGISTRY_NUM_SECTOR			(2)
#define INTERNAL_REGISTRY_LOGICAL_BLOCK_SIZE	(512)


//#define WATCHDOG_ENABLE
#define WATCHDOG_TIMEOUT_MS		(100)
#define WATCHDOG_PET_MS 		(90)


//#define MEMORY_MANAGER_ENABLE
#define MEMORY_MANAGER_POOL_SIZE		(64000)


//#define DWT_ENABLE

#endif /* HW_H_ */
