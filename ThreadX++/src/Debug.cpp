/*
 * MeasureTime.cpp
 *
 *  Created on: 18 במרץ 2023
 *      Author: zehevvv
 */

#include <Debug.h>
#include <DWT_Utility.h>
#include "HW.h"

#ifdef DWT_ENABLE

using namespace Debug;

MeasureTime::MeasureTime() {
}

void MeasureTime::Start()
{
	m_start_time = DWT_Utility::Instance()->GetCurrentTicks();
}

uint32_t MeasureTime::End()
{
	uint32_t end_time = DWT_Utility::Instance()->GetCurrentTicks();
	return (DWT_Utility::Instance()->ConvrtTiksToUs(end_time -  m_start_time));
}

#endif
