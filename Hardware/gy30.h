/*
 * gy30.h
 *
 *  Created on: Jan 4, 2023
 *
 */

#ifndef GY30_H_
#define GY30_H_

#include "system.h"
#include <Drivers/Simulation/SI2C/SI2C_Device.h>

/* I2C address */
#define GY30_I2C_ADDR		0x23 /* ADDR=L */
//#define GY30_I2C_ADDR		0x5C /* ADDR=H */

/* operation command */
#define GY30_Continuously_H_mode		0x10 /* Continuously H-resolution mode : 1 lx*/
#define GY30_Continuously_H_mode2		0x11 /* Continuously H-resolution mode2 : 0.5 lx*/
#define GY30_Continuously_L_mode		0x13 /* Continuously L-resolution mode : 4 lx*/
#define GY30_One_Time_H_mode			0x20 /* One time H-resolution mode */
#define GY30_One_Time_H_mode2			0x21 /* One time H-resolution mode2 */
#define GY30_One_Time_L_mode			0x23 /* One time L-resolution mode */
#define GY30_Power_Down					0x00 /* No active state */
#define GY30_Power_On					0x01 /* Waiting for measurement command */
#define GY30_Reset						0x07 /* Reset Data register value */

extern uint8_t GY30_Init(SI2C_Handle handle);
extern uint16_t GY30_Measurement(SI2C_Handle handle);

#endif /* GY30_H_ */
