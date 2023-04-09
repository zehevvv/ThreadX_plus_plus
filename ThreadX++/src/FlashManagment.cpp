/*
 * FlashManagment.cpp
 *
 *  Created on: Mar 25, 2023
 *      Author: zehevvv
 */

#include <stdio.h>
#include <cstring>
#include <FlashManagment.h>
#include "InternalFlashDriver.h"
#include "Macros.h"
#include "HW.h"
#include "Watchdog.h"

#ifdef INTERNAL_REGISTRY_ENABLE


typedef struct BlockHeaderStart
{
	uint64_t magic_used_block;
} BlockHeaderStart;

typedef struct BlockHeaderEnd
{
	uint32_t size;
	uint32_t counter;
} BlockHeaderEnd;


/// @brief C'tor, Read the keys name and the values from the flash and create the cache buffer.
///
/// @param start_address		- The start address of the flash where the data store.
/// @param num_sector			- The number of sectors of the registry.
/// @param logical_block_size 	- The size of the logical blocks (bytes).
FlashManagment::FlashManagment(uint32_t start_address, uint32_t num_sector, uint32_t logical_block_size) :
	m_start_address(start_address),
	m_num_sector(num_sector),
	m_logical_block_size(logical_block_size)
{
	m_flash_total_size = num_sector * m_flash_driver.GetSectorSize();
	m_cache_buffer = new uint8_t[m_logical_block_size];

//	RestartFlash();
	FindLastLogicalBlock();
}

/// @brief D'tor, remove the buffer
FlashManagment::~FlashManagment()
{
	delete[] m_cache_buffer;
}

/// @brief Find the last block that store the data
void FlashManagment::FindLastLogicalBlock()
{
	uint32_t num_logical_blocks = m_flash_total_size / m_logical_block_size;
	uint32_t* address_to_read = (uint32_t*)m_start_address;
	m_counter = UINT32_MAX;
	BlockHeaderStart start_header;
	BlockHeaderEnd end_header;

	// Run on all logic block and check where is the higher counter.
	for (uint32_t i = 0; i < num_logical_blocks; i++)
	{
		m_flash_driver.Read(address_to_read, (uint32_t*)&start_header, sizeof(start_header) / 4);

		// Check if the block is used
		if (start_header.magic_used_block == m_MAGIC_USED_BLOCK)
		{
			// Read the header in the end of the block
			uint32_t* address_of_end_header = address_to_read + ((m_logical_block_size - sizeof(BlockHeaderEnd)) / 4);
			m_flash_driver.Read(address_of_end_header, (uint32_t*)&end_header, sizeof(end_header) / 4);

			// Check if current block is valid
			if (end_header.counter !=  UINT32_MAX)
			{
				// Check if this the first counter and if the counter is bigger than last counter
				if ((m_counter == UINT32_MAX) || (end_header.counter > m_counter))
				{
					m_counter = end_header.counter;
					m_current_address = (uint32_t)address_to_read;
					m_size = end_header.size;
				}
			}
		}

		// Prepare next cycle
		address_to_read += (m_logical_block_size / 4);
	}

	// Check if not found any block
	if (m_counter == UINT32_MAX)
	{
		printf("Internal flash management: not found any block \n");
		RestartFlash();
	}
	else
	{
		printf("Internal flash management: find block at address - 0x%lX, counter - %lu \n", m_current_address, m_counter);
		m_counter++;
	}
}

/// @brief 	Read logical block from the flash, the function check if the size is valid convert the size from bytes to word
/// 		(32bits), then read from flash and copy to buffer.
///
/// @param buffer			- Pointer to buffer that will store the data from the flash.
/// @param num_byte_to_read	- The number of byte that need to read from the flash.
///
/// @return the number of bytes that read from the flash
uint32_t FlashManagment::Read(uint8_t* buffer, uint32_t num_byte_to_read)
{
	// Check if no data saved on the flash.
	if (m_size == 0)
		return 0;

	if (num_byte_to_read > m_size)
	{
		printf("Can't read, try to read more bytes can have in block, bytes to read = %lu, bytes in block = %lu \n",
				num_byte_to_read, m_size);
		return 0;
	}

	uint32_t size_to_read = ALIGN_TO_4(num_byte_to_read) / 4;
	uint32_t* address_to_read = (uint32_t*)(m_current_address + sizeof(BlockHeaderStart));

	// Read the flash
	m_flash_driver.Read(address_to_read, (uint32_t*)m_cache_buffer, size_to_read);

	// Copy data to buffer.
	memcpy(buffer, m_cache_buffer, num_byte_to_read);

	return num_byte_to_read;
}

/// @brief  Write the logical block to flash, this function check if size if fine and find new block and
/// 		call to function to write to cache buffer and check if need to erase sector.
/// @note	To write to flash you need to call to function "WriteToFlash()".
///
/// @param buffer			- Pointer to buffer that will store the data from the flash.
/// @param num_byte_to_read	- The number of byte that need to read from the flash.
void FlashManagment::Write(uint8_t* buffer, uint32_t buffer_size)
{
	// Check if have enough space in the block without the header.
	if (buffer_size > (m_logical_block_size - sizeof(BlockHeaderStart) - sizeof(BlockHeaderEnd)))
	{
		printf("Can't write buffer is too much big, buffer size = %lu", buffer_size);
	}

	uint32_t last_address = m_current_address;

	if (!FindGoodBlock())
	{
		printf("Corrupted magic!, reset the flash!");
		RestartFlash();
		m_current_address += m_logical_block_size;
		last_address = m_start_address;
	}

	// Write the new data to flash.
	WriteToCache(buffer, buffer_size);

	// Check if we move the other sector and we can erase the used sector.
	if (!IsAdressInSameSector(last_address, m_current_address))
	{
		EraseSector(last_address);
	}
}

/// @brief Find empty block that can store new data.
///
/// @return True - find empty block.
bool FlashManagment::FindGoodBlock()
{
	uint32_t first_address = m_current_address;

	// Run on every block and check if the magic is still not write
	do
	{
		// Set the address to next physical block.
		m_current_address += m_logical_block_size;

		// Check if we to do overlap.
		uint32_t end_address = m_start_address + m_flash_total_size;
		if (m_current_address == end_address)
		{
			m_current_address = m_start_address;
		}

		// Read the head of the current block.
		BlockHeaderStart header;
		m_flash_driver.Read((uint32_t*)m_current_address, (uint32_t*)&header, sizeof(header) / 4);

		// Check if the flash is corrupted.
		if (header.magic_used_block == UINT64_MAX)
		{
			return true;
		}
	}
	while (first_address != m_current_address);

	return false;
}

/// @brief Write to cache buffer, prepare the buffer in the next order: magic - data - size - counter.
///
/// @param buffer		- Pointer to buffer with data that need to store on the flash.
/// @param buffer_size	- The size of the buffer that need to write to flash in bytes units.
void FlashManagment::WriteToCache(uint8_t* buffer, uint32_t buffer_size)
{
	// Mark the block "used"
	static const uint64_t MAGIC_USED_BLOCK = m_MAGIC_USED_BLOCK; // Need this line because the compiler is idiot
	memcpy(m_cache_buffer, (uint8_t*)&MAGIC_USED_BLOCK, sizeof(MAGIC_USED_BLOCK));

	// Write the data after the "Magic"
	uint32_t index_to_copy = sizeof(MAGIC_USED_BLOCK);
	memcpy(&m_cache_buffer[index_to_copy], buffer, buffer_size);

	// Write the size and the counter in the end of the block
	BlockHeaderEnd end_header;
	end_header.size = buffer_size;
	end_header.counter = m_counter;
	index_to_copy = m_logical_block_size - sizeof(BlockHeaderEnd);
	memcpy(&m_cache_buffer[index_to_copy], (uint8_t*)&end_header, sizeof(end_header));

	m_counter++;
	m_size = buffer_size;
}

/// @brief Write the cache buffer to flash.
void FlashManagment::WriteToFlash()
{
	// Write the buffer to flash
	m_flash_driver.Write((uint32_t*)m_current_address, (uint32_t*)m_cache_buffer, m_logical_block_size / 4);
}

/// @brief Check if the 2 address (of block logic) are in the same sector of the flash.
/// @param address_1	- Address one.
/// @param address_2	- Address two.
///
/// @return true - the addresses are from the same sector.
bool FlashManagment::IsAdressInSameSector(uint32_t address_1, uint32_t address_2)
{
	uint32_t sector_size = m_flash_driver.GetSectorSize();

	for (uint32_t i = 0; i < m_num_sector; ++i)
	{
		uint32_t current_address = m_start_address + i * sector_size;

		// Check if address 1 is in current sector
		if (address_1 >= current_address && address_1 < current_address + sector_size)
		{
			// Check if the address 2 is also in the current sector
			if (address_2 >= current_address && address_2 < current_address + sector_size)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	printf("Not found address 1 in the flash, address 1 - 0x%lX \n", address_1);
	RestartFlash();
	return true;
}

/// @brief Erase sector of flash
///
/// @address	- Address that inside the range of sector addresses.
void FlashManagment::EraseSector(uint32_t address)
{
	m_flash_driver.Earse(address);
}

/// @brief Erase all the flash sectors of the registry and set the internal variable to defaults values.
void FlashManagment::RestartFlash()
{
	uint32_t sector_size = m_flash_driver.GetSectorSize();
	for (uint32_t i = 0; i < m_num_sector; ++i)
	{
#ifdef WATCHDOG_ENABLE
	Watchdog::Instance()->Refresh(NULL);
#endif
		m_flash_driver.Earse(m_start_address + i * sector_size);
#ifdef WATCHDOG_ENABLE
	Watchdog::Instance()->Refresh(NULL);
#endif
	}

	m_counter = 0;
	m_current_address = m_start_address;
	m_size = 0;
	WriteToCache(NULL, 0);
}

/// @brief Get the size of the max data that can write to logical block.
///
/// @return The size of the max data that can write to logical block.
uint32_t FlashManagment::GetMaxFlashSize()
{
	return m_logical_block_size - sizeof(BlockHeaderStart) - sizeof(BlockHeaderEnd);
}


///////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////	Tests	///////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

void TestFlashManagmentWrite(FlashManagment& flash)
{
//	const uint32_t SIZE = 20;
//	uint8_t* buffer = new uint8_t[SIZE];
//	for (uint32_t i = 0; i < SIZE; ++i)
//		buffer[i] = i + 1;
//	flash.Write(buffer, SIZE);
//	flash.Write(buffer, SIZE);

//	const uint32_t SIZE = 17;
//	uint8_t* buffer = new uint8_t[SIZE];
//	for (uint32_t i = 0; i < SIZE; ++i)
//		buffer[i] = i + 1;
//	flash.Write(buffer, SIZE);

//	const uint32_t SIZE = 17;
//	uint8_t* buffer = new uint8_t[SIZE];
//	for (uint32_t i = 0; i < SIZE; ++i)
//		buffer[i] = i + 1;
//	for (int j = 0; j < 260; ++j)
//		flash.Write(buffer, SIZE);
}

void TestFlashManagmentRead(FlashManagment& flash)
{
//	uint8_t* buffer = new uint8_t[flash.GetSize()];
//	flash.Read(buffer, flash.GetSize());
//	for (uint32_t i = 0; i < flash.GetSize() - 1; ++i)
//		printf("%d, ", buffer[i]);
//	printf("\n");
}

void TestFlashManagment()
{
//	Internal_flash_earse((uint32_t)Internal_flash_start_address());
//	FlashManagment flash;
//
//	TestFlashManagmentWrite(flash);
//	TestFlashManagmentRead(flash);
}

#endif
