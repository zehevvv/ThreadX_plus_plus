/*
 * MeasureTime.h
 *
 *  Created on: 18 במרץ 2023
 *      Author: zehevvv
 */

#ifndef INC_DEBUG_H_
#define INC_DEBUG_H_

#include "Types.h"

namespace Debug {

class MeasureTime {
public:
	MeasureTime();
	void Start();
	uint32_t End();
private:
	uint32_t m_start_time;
};

} /* namespace Debug */

#endif /* INC_DEBUG_H_ */
