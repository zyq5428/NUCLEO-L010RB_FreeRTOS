/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PRINTF_STDARG_H
#define __PRINTF_STDARG_H

/* Includes ------------------------------------------------------------------*/
#include <stdarg.h>

extern int printf(const char *format, ...);
extern int sprintf(char *out, const char *format, ...);
extern int snprintf( char *buf, unsigned int count, const char *format, ... );

#endif /* __PRINTF_STDARG_H */
