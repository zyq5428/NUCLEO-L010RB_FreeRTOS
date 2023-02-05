/*
 * easy_log.h
 *
 *  Created on: Nov 19, 2022
 *  
 */

#ifndef SYSTEM_EASY_LOG_H_
#define SYSTEM_EASY_LOG_H_

#include "printf-stdarg.h"
#include "system.h"

#if defined(__ARMCC_VERSION) || defined(__ICCARM__)
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#elif defined(__GNUC__)
/* With GCC, small printf (option LD Linker->Libraries->Small printf
 *    set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#endif

#endif /* SYSTEM_EASY_LOG_H_ */
