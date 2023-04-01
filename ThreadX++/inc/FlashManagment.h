/*
 * FlashManagment.h
 *
 *  Created on: Mar 25, 2023
 *      Author: zehevvv
 */

#ifndef INC_FLASHMANAGMENT_H_
#define INC_FLASHMANAGMENT_H_

#include "Types.h"
#include "HW.h"
#include "InternalFlashDriver.h"

class FlashManagment
{
public:
	FlashManagment(uint32_t start_address, uint32_t num_sector, uint32_t logical_block_size);
	~FlashManagment();

	void Write(uint8_t* buffer, uint32_t buffer_size);
	uint32_t Read(uint8_t* buffer, uint32_t num_byte_to_read);
	uint32_t GetSize() { return m_size; }
	uint32_t GetMaxFlashSize();

private:
	void FindLastLogicalBlock();
	bool IsAdressInSameSector(uint32_t address_1, uint32_t address_2);
	void EraseSector(uint32_t address);
	void RestartFlash();
	void WriteToFlash(uint8_t* buffer, uint32_t buffer_size);
	bool FindGoodBlock();

	static const uint64_t m_MAGIC_USED_BLOCK = 0x74BDC5A50FEFA13EULL;

	uint32_t 			m_counter;
	uint32_t 			m_current_address;
	uint32_t 			m_size;
	uint8_t*	 		m_cache_buffer;
	bool	 			m_is_first_block;
	InternalFlashDriver	m_flash_driver;
	uint32_t 			m_start_address;
	uint32_t 			m_num_sector;
	uint32_t 			m_logical_block_size;
	uint32_t			m_flash_total_size;
};

#endif /* INC_FLASHMANAGMENT_H_ */
