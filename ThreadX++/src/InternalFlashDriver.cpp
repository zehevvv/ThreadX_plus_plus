/*
 * InternalFlashDriver.c
 *
 *  Created on: 21 במרץ 2023
 *      Author: zehevvv
 */
#include <stdio.h>
#include <cstring>
#include "InternalFlashDriver.h"
#include "HW.h"

#ifdef INTERNAL_REGISTRY_ENABLE

#include "stm32h7xx_hal.h"


//#define DEBUG_INTERNAL_FLASH

/// @brief C'tor
InternalFlashDriver::InternalFlashDriver() {}

/// @brief 	Read from flash,
/// 		Check if address is align to 4 (32 bit processor)
/// @note	Be ware that the function read 32bits and not bytes.
///
/// @param address_start	- Address to start to read
/// @param buffer			- [out] Pointer to buffer that hold the data that read from flash
/// @param num_words		- The number of words (32bits) that need to read from flash
///
/// @return true - read success.
bool InternalFlashDriver::Read(uint32_t* address_start, uint32_t* buffer, uint32_t num_words)
{
#ifdef DEBUG_INTERNAL_FLASH
	printf("Internal flash read: address - 0x%lX, num words - %lu \n", (uint32_t)address_start, num_words);
#endif

	if (((uint32_t)address_start % 4) != 0)
	{
		printf("Can't read to flash address that don't align to 4 /n");
		return false;
	}

	for (uint32_t i = 0; i < num_words; i++)
	{
		buffer[i] = *(__IO uint32_t*)(address_start + i);
	}

	return true;
}

/// @brief 	Write to flash
/// 		Because of the internal flash is must write flash words (8 bytes), we need to check if the start
/// 		and the end of the buffer is align to flash words size (8 bytes), if not create temp buffer and
/// 		then write the data.
/// @note	Be ware that the function write 32bits and not bytes.
///
/// @param address_start 	- The first address that the data should store (must align by 4).
/// @param buffer			- Pointer to buffet that need to copy to flash.
/// @param num_words		- The number of words (32bits) that need to store on the flash.
///
/// @return true - write success.
bool InternalFlashDriver::Write(uint32_t* address_start, uint32_t* buffer, uint32_t num_words)
{
#ifdef DEBUG_INTERNAL_FLASH
	printf("Internal flash write: address - 0x%lX, num words - %lu \n", (uint32_t)address_start, num_words);
#endif

	static uint32_t s_buffer[INTERNAL_FLASH_FLASHWORD];

	if (((uint32_t)address_start) % 4 != 0)
	{
		printf("Failed to write sector, the address not align to 4 \n");
		return false;
	}

	HAL_FLASH_Unlock();

	// Write the not align word in the start of the buffer
	uint32_t num_word_not_align = (((uint32_t)address_start) % (INTERNAL_FLASH_FLASHWORD * 4)) / 4;
	if (num_word_not_align != 0)
	{
		uint32_t address_start_align = (uint32_t)(address_start - num_word_not_align);
		Read((uint32_t*)address_start_align, s_buffer, INTERNAL_FLASH_FLASHWORD);

		uint32_t num_word_need_shift = INTERNAL_FLASH_FLASHWORD - num_word_not_align;
		if (num_word_need_shift > num_words)
			num_word_need_shift = num_words;

		for (uint32_t i = 0; i < num_word_need_shift; ++i)
		{
			s_buffer[num_word_not_align + i] &= buffer[i];
		}

		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, address_start_align, (uint32_t)s_buffer) != HAL_OK)
		{
			printf("Failed to write sector, get error %lu \n", HAL_FLASH_GetError());
			HAL_FLASH_Lock();
			return false;
		}

		num_words -= num_word_need_shift;
		address_start += num_word_need_shift;
		buffer += num_word_need_shift;
	}

	// Write the buffer that align.
	for (uint32_t i = 0; i < num_words / INTERNAL_FLASH_FLASHWORD; ++i)
	{
		uint32_t next_address = (uint32_t)(address_start + INTERNAL_FLASH_FLASHWORD * i);
		uint32_t next_buffer = (uint32_t)(buffer + INTERNAL_FLASH_FLASHWORD * i);
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, next_address, next_buffer) != HAL_OK)
		{
			printf("Failed to write sector, get error %lu \n", HAL_FLASH_GetError());
			HAL_FLASH_Lock();
			return false;
		}
	}

	// Write the end of the buffer that not aligned.
	num_word_not_align = (num_words % INTERNAL_FLASH_FLASHWORD);
	if (num_word_not_align != 0)
	{
		memset(s_buffer, 0xff, sizeof(s_buffer));

		uint32_t index_not_align_word = num_words - num_word_not_align;

		for (uint32_t i = 0; i < num_word_not_align; ++i)
		{
			s_buffer[i] = buffer[index_not_align_word + i];
		}

		uint32_t address_start_not_align = (uint32_t)(address_start + index_not_align_word);
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, address_start_not_align, (uint32_t)s_buffer) != HAL_OK)
		{
			printf("Failed to write sector, get error %lu \n", HAL_FLASH_GetError());
			HAL_FLASH_Lock();
			return false;
		}
	}

	HAL_FLASH_Lock();
	return true;
}

/// @brief Erase one sector
///
/// @param address	- The address that include in the range addresses of the flash sector that need to erase.
///
/// @return True - erase success.
bool InternalFlashDriver::Earse(uint32_t address)
{
	/* Get the number of sector to erase from 1st sector */
	uint32_t start_number = GetSecotr(address);
	if (start_number == HAL_ERROR)
		return false;

	FLASH_EraseInitTypeDef erase_init_struct;

	// The the proper BANK to erase the Sector
	if (address < 0x08100000)
		erase_init_struct.Banks = FLASH_BANK_1;
	else
		erase_init_struct.Banks = FLASH_BANK_2;

	/* Fill EraseInit structure*/
	erase_init_struct.TypeErase = FLASH_TYPEERASE_SECTORS;
	erase_init_struct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	erase_init_struct.Sector = start_number;
	erase_init_struct.NbSectors = 1;

	/* Unlock the Flash to enable the flash control register access *************/
	HAL_FLASH_Unlock();
	uint32_t sector_error;
	if (HAL_FLASHEx_Erase(&erase_init_struct, &sector_error) != HAL_OK)
	{
		printf("Failed o erase sector, get error %lu \n", HAL_FLASH_GetError());

		// Lock the flash
		HAL_FLASH_Lock();
		return false;
	}

	// Lock the flash
	HAL_FLASH_Lock();

	return true;
}

/// @brief The size of sector in bytes units.
///
/// @return The size of sector in bytes units.
uint32_t InternalFlashDriver::GetSectorSize()
{
	return INTERNAL_FLASH_SECTOR_SIZE;
}

/// @brief Get address and return the logical flash sector value that it belong.
///
/// @param Address	- Address in the flash.
///
/// @return The logical flash sector value.
uint16_t InternalFlashDriver::GetSecotr(uint32_t Address)
{
	uint32_t sector = HAL_ERROR;

	if ((Address >= 0x08000000) && (Address < 0x08020000))
		sector = FLASH_SECTOR_0;
	else if ((Address >= 0x08020000) && (Address < 0x08040000))
		sector = FLASH_SECTOR_1;
	else if ((Address >= 0x08040000) && (Address < 0x08060000))
		sector = FLASH_SECTOR_2;
	else if ((Address >= 0x08060000) && (Address < 0x08080000))
		sector = FLASH_SECTOR_3;
	else if ((Address >= 0x08080000) && (Address < 0x080A0000))
		sector = FLASH_SECTOR_4;
	else if ((Address >= 0x080A0000) && (Address < 0x080C0000))
		sector = FLASH_SECTOR_5;
	else if ((Address >= 0x080C0000) && (Address < 0x080E0000))
		sector = FLASH_SECTOR_6;
	else if ((Address >= 0x080E0000) && (Address < 0x08100000))
		sector = FLASH_SECTOR_7;
	else if ((Address >= 0x08100000) && (Address < 0x08120000))
		sector = FLASH_SECTOR_0;
	else if ((Address >= 0x08120000) && (Address < 0x08140000))
		sector = FLASH_SECTOR_1;
	else if ((Address >= 0x08140000) && (Address < 0x08160000))
		sector = FLASH_SECTOR_2;
	else if ((Address >= 0x08160000) && (Address < 0x08180000))
		sector = FLASH_SECTOR_3;
	else if ((Address >= 0x08180000) && (Address < 0x081A0000))
		sector = FLASH_SECTOR_4;
	else if ((Address >= 0x081A0000) && (Address < 0x081C0000))
		sector = FLASH_SECTOR_5;
	else if ((Address >= 0x081C0000) && (Address < 0x081E0000))
		sector = FLASH_SECTOR_6;
	else if ((Address >= 0x081E0000) && (Address < 0x08200000))
		sector = FLASH_SECTOR_7;

	return sector;
}

#endif
