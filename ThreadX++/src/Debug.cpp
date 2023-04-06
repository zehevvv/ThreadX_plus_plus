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


/// @brief C'tor
MeasureTime::MeasureTime()
{
}

/// @brief Starts measure time
void MeasureTime::Start()
{
	m_start_time = DWT_Utility::Instance()->GetCurrentTicks();
}

/// @brief End of the measure time and return the measured time
/// @return The measured time (delta)
uint32_t MeasureTime::End()
{
	uint32_t end_time = DWT_Utility::Instance()->GetCurrentTicks();
	return (DWT_Utility::Instance()->ConvrtTiksToUs(end_time -  m_start_time));
}

#endif
