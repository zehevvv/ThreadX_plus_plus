/*
 * Macros.h
 *
 *  Created on: Mar 25, 2023
 *      Author: zehevvv
 */

#ifndef INC_MACROS_H_
#define INC_MACROS_H_


#define MIN(a,b)	((a < b) ? a : b)
#define MAX(a,b)    ((a > b) ? a : b)
#define ALIGN_TO_4(a)	(((a % 4) == 0) ? a : (a + 4 - (a % 4)))


#endif /* INC_MACROS_H_ */
