/*
 * SI2C_STM32.h
 *
 *  Created on: 2022年12月26日
 *
 */

#ifndef SYSTEM_DRIVERS_SIMULATION_SI2C_SI2C_DEVICE_H_
#define SYSTEM_DRIVERS_SIMULATION_SI2C_SI2C_DEVICE_H_

#include <Drivers/Simulation/SI2C.h>

extern const SI2C_FxnTable SI2C_Device_fxnTable;

typedef enum
{
	IIC_LOW = 0U,
	IIC_HIGH
} SI2C_PinState;

typedef struct SI2C_Device_I2CPinCfg {
	GPIO_TypeDef	*portSDA;
	GPIO_TypeDef	*portSCL;
    uint32_t 		pinSDA;
    uint32_t 		pinSCL;
} SI2C_Device_I2CPinCfg;

typedef enum SI2C_Device_Mode {
    SI2C_Device_IDLE_MODE = 0,  /* I2C is not performing a transaction */
    SI2C_Device_WRITE_MODE,     /* I2C is currently performing write operations */
    SI2C_Device_READ_MODE,      /* I2C is currently performing read operations */
    SI2C_Device_BUSBUSY_MODE,   /* I2C Bus is currently busy */
    SI2C_Device_ERROR = 0xFF    /* I2C error has occurred, exit gracefully */
} SI2C_Device_Mode;

typedef struct SI2C_Device_HWAttrsV1 {
    /*! I2C peripheral's name */
	uint8_t				name;
    /*! I2C SDA port mapping */
    GPIO_TypeDef 		*sdaPort;
    /*! I2C SCL port mapping */
    GPIO_TypeDef 		*sclPort;
    /*! I2C SDA pin mapping */
    uint32_t             sdaPin;
    /*! I2C SCL pin mapping */
    uint32_t             sclPin;
} SI2C_Device_HWAttrsV1;

typedef struct SI2C_Device_Object {
    volatile SI2C_Device_Mode mode;            /*!< Stores the I2C state */
    uint32_t            bitRate;             /*!< Bitrate of the I2C module */
    uint8_t				slaveAddress;

    /* I2C current transaction */
    SI2C_Transaction     *currentTransaction; /*!< Ptr to current I2C transaction */
    uint8_t             *writeBufIdx;        /*!< Internal inc. writeBuf index */
    unsigned int        writeCountIdx;       /*!< Internal dec. writeCounter */
    uint8_t             *readBufIdx;         /*!< Internal inc. readBuf index */
    unsigned int        readCountIdx;        /*!< Internal dec. readCounter */

    SI2C_Device_I2CPinCfg	si2cPins;

    /* I2C transaction pointers for I2C_MODE_CALLBACK */
    SI2C_Transaction     *headPtr;            /*!< Head ptr for queued transactions */
    SI2C_Transaction     *tailPtr;            /*!< Tail ptr for queued transactions */

    bool_t					isOpen;             /*!< flag to indicate module is open */
} SI2C_Device_Object;

extern void IIC_Send_Byte(SI2C_Handle handle, uint8_t tx_data);
extern uint8_t IIC_Check_Device(SI2C_Handle handle, uint8_t Trials);
extern uint8_t IIC_Send_Multi_Byte(SI2C_Handle handle, uint8_t *data, uint8_t len);
extern uint8_t IIC_Mem_Write(SI2C_Handle handle, uint8_t MemAddress, uint8_t *data, uint16_t len);
extern uint8_t IIC_Read_Byte(SI2C_Handle handle, uint8_t ack);
extern uint8_t IIC_Read_Multi_Byte(SI2C_Handle handle, uint8_t *data, uint8_t len);

#endif /* SYSTEM_DRIVERS_SIMULATION_SI2C_SI2C_STM32_H_ */
