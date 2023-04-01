/*
 * FlashDriver.h
 *
 *  Created on: 21 במרץ 2023
 *      Author: zehevvv
 */

#ifndef INC_INTERNALFLASHDRIVER_H_
#define INC_INTERNALFLASHDRIVER_H_

#include <stdint.h>
#include "Types.h"

class InternalFlashDriver {
public:
	InternalFlashDriver();

	bool Read(uint32_t* address_start, uint32_t* buffer, uint32_t num_words);
	bool Write(uint32_t* address_start, uint32_t* buffer, uint32_t num_words);
	bool Earse(uint32_t address);
	uint32_t GetSectorSize();

private:
	uint16_t GetSecotr(uint32_t Address);
};


#endif /* INC_INTERNALFLASHDRIVER_H_ */
