/*
 * device_tree.h
 *
 *  Created on: 2022年12月26日
 *
 */

#ifndef SYSTEM_DEVICE_TREE_H_
#define SYSTEM_DEVICE_TREE_H_

typedef enum DEVICE_SI2CName {
	DEVICE_SI2C0 = 0,
	DEVICE_SI2C1 = 1,

	DEVICE_SI2CCOUNT
} DEVICE_SI2CName;

typedef enum TASK_Name {
	TASK_LED		= 0,
	TASK_GY30		= 1,
	TASK_OLED,
	TASK_RTC,

	TASK_CCOUNT
} TASK_Name;

#endif /* SYSTEM_DEVICE_TREE_H_ */
