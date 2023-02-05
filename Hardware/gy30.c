/*
 * gy30.c
 *
 *  Created on: Jan 4, 2023
 *
 */
#include <easy_log.h>
#include "gy30.h"

/* Write command */
HAL_StatusTypeDef GY30_WriteCommand(SI2C_Handle handle, uint8_t command)
{
	if( IIC_Send_Multi_Byte(handle, &command, 1) )
		return HAL_ERROR;
	else
		return HAL_OK;
}

/* Continuous measurement mode  */
HAL_StatusTypeDef GY30_ContinuouslyMode(SI2C_Handle handle, uint8_t *data, uint8_t command)
{
	if( IIC_Send_Multi_Byte(handle, &command, 1) )
		return HAL_ERROR;

	HAL_Delay(180);	/* H-resolution mode measurement.( max. 180ms. ) */

	if( IIC_Read_Multi_Byte(handle, data, 2) )
		return HAL_ERROR;
	else
		return HAL_OK;
}

/* One time measurement mode  */
HAL_StatusTypeDef GY30_OnetimeMode(SI2C_Handle handle, uint8_t *data, uint8_t command)
{
	if( IIC_Send_Multi_Byte(handle, &command, 1) )
		return HAL_ERROR;
	else
		return HAL_OK;

	HAL_Delay(24);	/* L-resolution mode measurement.( max. 24ms. ) */

	if( IIC_Read_Multi_Byte(handle, data, 2) )
		return HAL_ERROR;
	else
		return HAL_OK;
}

/* GY-30 Init */
uint16_t GY30_Measurement(SI2C_Handle handle)
{
	uint8_t data[2] = {0x00, 0x00};
	uint16_t lux;
    if(HAL_OK == GY30_ContinuouslyMode(handle, data, GY30_Continuously_H_mode))
    		lux = (uint16_t)( ((data[0] << 8) + data[1]) / 1.2 );	/* Limited minimum precision 1 lux */
    else {
    	printf("Measurement Fail\r\n");
    	return 0xFFFF;	/* The measurement result cannot be greater than 0xFFFF (result/1.2) */
    }

    return lux;
}

/* GY-30 Init */
uint8_t GY30_Init(SI2C_Handle handle)
{
	/* Check if GY-30 connected to I2C */
	if(!(IIC_Check_Device(handle, 3)))
	{
		printf("GY-30 is ready\r\n");
	} else {
		printf("GY-30 is error\r\n");
	}
	/* A little delay */
	HAL_Delay(100);

	/* Init GY-30 */
    GY30_WriteCommand(handle, GY30_Power_On);

    /* Return OK */
    return HAL_OK;
}

