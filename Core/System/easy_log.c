/*
 * easy_log.c
 *
 *  Created on: Nov 19, 2022
 *  
 */

#include "easy_log.h"

extern UART_HandleTypeDef huart2;

/**
* @brief  Retargets the C library printf function to the USART.
* @param  None
* @retval None
*/
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART3 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}
