/*
 * DWT.h
 *
 *  Created on: Mar 11, 2023
 *      Author: zehevvv
 */

#ifndef INC_DWT_UTILITY_H_
#define INC_DWT_UTILITY_H_

#include <stdint.h>
#include "Singleton.h"

class DWT_Utility : public Singleton<DWT_Utility>
{
friend Singleton<DWT_Utility>;

public:
	void Delay_us(uint32_t time_to_sleep);
	uint32_t GetCurrentTicks();
	uint32_t ConvrtTiksToUs(uint32_t ticks);

private:
	DWT_Utility();
	bool Init_clock_cycle();
};

#endif /* INC_DWT_UTILITY_H_ */
