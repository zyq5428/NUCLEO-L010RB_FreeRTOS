/*
 * device_tree.c
 *
 *  Created on: 2022年12月26日
 *
 */
#include "device_tree.h"
#include "system.h"

#include <Drivers/Simulation/SI2C.h>
#include <Drivers/Simulation/SI2C/SI2C_Device.h>


SI2C_Device_Object si2c_DeviceObjects[DEVICE_SI2CCOUNT] = {0};

const SI2C_Device_HWAttrsV1 si2c_DeviceHWAttrs[DEVICE_SI2CCOUNT] = {
    {
        .name		 = DEVICE_SI2C0,
		.sdaPort	 = GPIOB,
        .sclPort	 = GPIOB,
        .sdaPin      = GPIO_PIN_9,
        .sclPin      = GPIO_PIN_8,
    },
    {
        .name		 = DEVICE_SI2C1,
		.sdaPort	 = GPIOC,
        .sclPort	 = GPIOC,
        .sdaPin      = GPIO_PIN_6,
        .sclPin      = GPIO_PIN_5,
    }
};

const SI2C_Config SI2C_config[DEVICE_SI2CCOUNT] = {
    {
        .fxnTablePtr = &SI2C_Device_fxnTable,
        .object      = &si2c_DeviceObjects[DEVICE_SI2C0],
        .hwAttrs     = &si2c_DeviceHWAttrs[DEVICE_SI2C0]
    },
    {
        .fxnTablePtr = &SI2C_Device_fxnTable,
        .object      = &si2c_DeviceObjects[DEVICE_SI2C1],
        .hwAttrs     = &si2c_DeviceHWAttrs[DEVICE_SI2C1]
    }
};

const uint_least8_t SI2C_count = DEVICE_SI2CCOUNT;
