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


typedef struct BlockHeaderStart
{
	uint64_t magic_used_block;
} BlockHeaderStart;

typedef struct BlockHeaderEnd
{
	uint32_t size;
	uint32_t counter;
} BlockHeaderEnd;


FlashManagment::FlashManagment(uint32_t start_address, uint32_t num_sector, uint32_t logical_block_size) :
	m_is_first_block(false),
	m_start_address(start_address),
	m_num_sector(num_sector),
	m_logical_block_size(logical_block_size)
{
	m_flash_total_size = num_sector * m_flash_driver.GetSectorSize();
	m_cache_buffer = new uint8_t[m_logical_block_size];

//	RestartFlash();
	FindLastLogicalBlock();
}

FlashManagment::~FlashManagment()
{
	delete[] m_cache_buffer;
}

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

void FlashManagment::Write(uint8_t* buffer, uint32_t buffer_size)
{
	if (buffer_size == 0)
	{
		printf("Can't write logical logs, get buffer size = 0\n");
		return;
	}

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
		last_address = m_start_address;
	}

	// Write the new data to flash.
	WriteToFlash(buffer, buffer_size);

	// Check if we move the other sector and we can erase the used sector.
	if (!IsAdressInSameSector(last_address, m_current_address))
	{
		EraseSector(last_address);
	}
}

bool FlashManagment::FindGoodBlock()
{
	uint32_t first_address = m_current_address;

	// Run on every block and check if the magic is still not write
	do
	{
		// Check if it is not the first time we write to flash.
		if (!m_is_first_block)
		{
			// Set the address to next physical block.
			m_current_address += m_logical_block_size;

			// Check if we to do overlap.
			uint32_t end_address = m_start_address + m_flash_total_size;
			if (m_current_address == end_address)
			{
				m_current_address = m_start_address;
			}
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

void FlashManagment::WriteToFlash(uint8_t* buffer, uint32_t buffer_size)
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

	// Write the buffer to flash
	m_flash_driver.Write((uint32_t*)m_current_address, (uint32_t*)&m_cache_buffer, m_logical_block_size / 4);

	m_counter++;
	m_size = buffer_size;
	m_is_first_block = false;
}

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

void FlashManagment::EraseSector(uint32_t address)
{
	m_flash_driver.Earse(address);
}

void FlashManagment::RestartFlash()
{
	uint32_t sector_size = m_flash_driver.GetSectorSize();
	for (uint32_t i = 0; i < m_num_sector; ++i)
	{
		m_flash_driver.Earse(m_start_address + i * sector_size);
	}

	m_counter = 0;
	m_current_address = m_start_address;
	m_size = 0;
	m_is_first_block = true;
}

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