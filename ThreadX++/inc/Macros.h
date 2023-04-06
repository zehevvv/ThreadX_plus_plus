/*
 * Macros.h
 *
 *  Created on: Mar 25, 2023
 *      Author: zehevvv
 */

#ifndef INC_MACROS_H_
#define INC_MACROS_H_
#include "tx_user.h"


#define MIN(a,b)	((a < b) ? a : b)
#define MAX(a,b)    ((a > b) ? a : b)

#define ALIGN_TO_4(a)	(((a % 4) == 0) ? a : (a + 4 - (a % 4)))

#define CONVERT_MS_TO_TICKS(ms)			(ms / (1000 / TX_TIMER_TICKS_PER_SECOND))
#define CONVERT_TICKS_TO_MS(ticks)		(ticks * (1000 / TX_TIMER_TICKS_PER_SECOND))


#endif /* INC_MACROS_H_ */
