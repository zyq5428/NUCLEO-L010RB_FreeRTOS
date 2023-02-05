/*
 * SI2C_Device.c
 *
 *  Created on: 2022年12月26日
 *
 */

#include <Drivers/Simulation/SI2C/SI2C_Device.h>
#include "delay.h"

void IIC_Delay(SI2C_Handle handle)
{
	SI2C_Device_Object            *object;
	object = handle->object;

	delay_us(SI2C_Delay[object->bitRate]);	//10KHz speed
}

void SDA_IN(SI2C_Handle handle)
{
	SI2C_Device_Object            *object;
	object = handle->object;

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/*Configure GPIO pin : SDA_Pin */
	GPIO_InitStruct.Pin = object->si2cPins.pinSDA;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(object->si2cPins.portSDA, &GPIO_InitStruct);
}

SI2C_PinState READ_SDA(SI2C_Handle handle)
{
	SI2C_Device_Object            *object;
	object = handle->object;

	/*Reads the specified input port pin */
	return(HAL_GPIO_ReadPin(object->si2cPins.portSDA, object->si2cPins.pinSDA));
}

void SDA_OUT(SI2C_Handle handle)
{
	SI2C_Device_Object            *object;
	object = handle->object;

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/*Configure GPIO pin : SDA_Pin */
	GPIO_InitStruct.Pin = object->si2cPins.pinSDA;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(object->si2cPins.portSDA, &GPIO_InitStruct);
}

void IIC_SDA(SI2C_Handle handle, SI2C_PinState PinState)
{
	SI2C_Device_Object            *object;
	object = handle->object;

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(object->si2cPins.portSDA, object->si2cPins.pinSDA, PinState);
}

void IIC_SCL(SI2C_Handle handle, SI2C_PinState PinState)
{
	SI2C_Device_Object            *object;
	object = handle->object;

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(object->si2cPins.portSCL, object->si2cPins.pinSCL, PinState);
}

void IIC_Start(SI2C_Handle handle)
{
	SDA_OUT(handle);
	IIC_SDA(handle, IIC_HIGH);
	IIC_SCL(handle, IIC_HIGH);
	IIC_Delay(handle);
	IIC_SDA(handle, IIC_LOW);	//START:when CLK is high,DATA change form high to low
	IIC_Delay(handle);
	IIC_SCL(handle, IIC_LOW);	//钳住I2C总线，准备发送或接收数据
	IIC_Delay(handle);
}

void IIC_Stop(SI2C_Handle handle)
{
	SDA_OUT(handle);
	IIC_SDA(handle, IIC_LOW);
	IIC_SCL(handle, IIC_HIGH);
	IIC_Delay(handle);
	IIC_SDA(handle, IIC_HIGH);	//STOP:when CLK is high DATA change form low to high
	IIC_Delay(handle);
}

/**
 * @brief	等待应答信号到来
 *
 * @param   void
 *
 * @return  uint8_t		1，接收应答失败
 *					0，接收应答成功
 */
uint8_t IIC_Wait_Ack(SI2C_Handle handle)
{
	uint16_t ucErrTime = 0;
	SDA_IN(handle);
//	IIC_SDA(handle, IIC_HIGH);
	IIC_Delay(handle);
	IIC_SCL(handle, IIC_HIGH);
	IIC_Delay(handle);

	while(READ_SDA(handle))
	{
		ucErrTime++;
		if(ucErrTime > 1000)
		{
			IIC_SCL(handle, IIC_LOW);
			return 1;
		}
	}

	IIC_SCL(handle, IIC_LOW);
	return 0;
}

void IIC_Ack(SI2C_Handle handle)
{
	IIC_SCL(handle, IIC_LOW);
	SDA_OUT(handle);
	IIC_SDA(handle, IIC_LOW);
	IIC_Delay(handle);
	IIC_SCL(handle, IIC_HIGH);
	IIC_Delay(handle);
	IIC_SCL(handle, IIC_LOW);
}

void IIC_NAck(SI2C_Handle handle)
{
	IIC_SCL(handle, IIC_LOW);
	SDA_OUT(handle);
	IIC_SDA(handle, IIC_HIGH);
	IIC_Delay(handle);
	IIC_SCL(handle, IIC_HIGH);
	IIC_Delay(handle);
	IIC_SCL(handle, IIC_LOW);
}

void IIC_Send_Byte(SI2C_Handle handle, uint8_t tx_data)
{
	uint8_t t;

	SDA_OUT(handle);
	IIC_SDA(handle, IIC_LOW);

	for(t = 0; t < 8; t++)
	{
		IIC_SDA(handle, (tx_data & 0x80) >> 7);
		tx_data = tx_data << 1;
		IIC_SCL(handle, IIC_HIGH);
		IIC_Delay(handle);
		IIC_SCL(handle, IIC_LOW);
		IIC_Delay(handle);
	}
}

/* Check device */
uint8_t IIC_Check_Device(SI2C_Handle handle, uint8_t Trials)
{
	uint8_t status = 1;
	SI2C_Device_Object            *object;
	object = handle->object;

	for(uint8_t i = 0; i < Trials; i++)
	{
		IIC_Start(handle);
		IIC_Send_Byte(handle, (object->slaveAddress << 1) | 0);

		if(IIC_Wait_Ack(handle))
		{
			IIC_Stop(handle);
			status = 1;
		}
		else
		{
			status = 0;
			break;
		}
	}

	IIC_Stop(handle);
	return status;
}

/* Write Multi data */
uint8_t IIC_Send_Multi_Byte(SI2C_Handle handle, uint8_t *data, uint8_t len)
{
	uint8_t i;
	SI2C_Device_Object            *object;
	object = handle->object;

	IIC_Start(handle);
	IIC_Send_Byte(handle, (object->slaveAddress << 1) | 0);

	if(IIC_Wait_Ack(handle))
	{
		IIC_Stop(handle);
		return 1;
	}

	for(i = 0; i < len; i++)
	{
		IIC_Send_Byte(handle, data[i]);
		IIC_Wait_Ack(handle);
	}

	IIC_Stop(handle);
	return 0;
}

/* Write Multi data */
uint8_t IIC_Mem_Write(SI2C_Handle handle, uint8_t MemAddress, uint8_t *data, uint16_t len)
{
	uint16_t i;
	SI2C_Device_Object            *object;
	object = handle->object;

	IIC_Start(handle);
	IIC_Send_Byte(handle, (object->slaveAddress << 1) | 0);

	if(IIC_Wait_Ack(handle))
	{
		IIC_Stop(handle);
		return 1;
	}

	IIC_Send_Byte(handle, MemAddress);
	IIC_Wait_Ack(handle);

	for(i = 0; i < len; i++)
	{
		IIC_Send_Byte(handle, data[i]);
		IIC_Wait_Ack(handle);
	}

	IIC_Stop(handle);
	return 0;
}

uint8_t IIC_Read_Byte(SI2C_Handle handle, uint8_t ack)
{
	uint8_t i, rx_data =0;

	SDA_IN(handle);

	for(i = 0; i < 8; i++)
	{
		IIC_SCL(handle, IIC_LOW);
		IIC_Delay(handle);
		IIC_SCL(handle, IIC_HIGH);
		rx_data = rx_data << 1;
		if(READ_SDA(handle))
		{
			rx_data++;
		}
		IIC_Delay(handle);
	}

	if(ack)
	{
		IIC_Ack(handle);
	} else
	{
		IIC_NAck(handle);
	}

	return rx_data;
}

/* Read Multi data */
uint8_t IIC_Read_Multi_Byte(SI2C_Handle handle, uint8_t *data, uint8_t len)
{
	uint8_t i, ack;
	SI2C_Device_Object            *object;
	object = handle->object;

	IIC_Start(handle);
	IIC_Send_Byte(handle, (object->slaveAddress << 1) | 1);

	if(IIC_Wait_Ack(handle))
	{
		IIC_Stop(handle);
		return 1;
	}

	for(i = 0; i < (len - 1); i++)
	{
		ack = 1;
		data[i] = IIC_Read_Byte(handle, ack);
	}

	ack = 0;
	data[len - 1] = IIC_Read_Byte(handle, ack);

	IIC_Stop(handle);

	return 0;
}

/*
 *  =============================== Prototypes =================================
 */
void         SI2C_Device_init(SI2C_Handle handle);
SI2C_Handle   SI2C_Device_open(SI2C_Handle handle, SI2C_Params *params);
bool_t         SI2C_Device_transfer(SI2C_Handle handle, SI2C_Transaction *transaction);
void         SI2C_Device_cancel(SI2C_Handle handle);
void         SI2C_Device_close(SI2C_Handle handle);
int_fast16_t SI2C_Device_control(SI2C_Handle handle, uint_fast16_t cmd, void *arg);

/*
 *  ========================== Local Prototypes ================================
 */
static int      SI2C_Device_initIO(SI2C_Handle handle, void *pinCfg);


/*
 *  ============================== Constants ===================================
 */
/* I2C function table for I2CCC26XX implementation */
const SI2C_FxnTable SI2C_Device_fxnTable = {
    SI2C_Device_cancel,
    SI2C_Device_close,
    SI2C_Device_control,
    SI2C_Device_init,
    SI2C_Device_open,
    SI2C_Device_transfer
};

static void SI2C_GPIO_Init(SI2C_Handle handle)
{
	SI2C_Device_Object            *object;
	object = handle->object;

	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(object->si2cPins.portSDA, object->si2cPins.pinSDA, GPIO_PIN_SET);
	HAL_GPIO_WritePin(object->si2cPins.portSCL, object->si2cPins.pinSCL, GPIO_PIN_SET);

	/*Configure GPIO pin : SDA_Pin */
	GPIO_InitStruct.Pin = object->si2cPins.pinSDA;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(object->si2cPins.portSDA, &GPIO_InitStruct);

	/*Configure GPIO pin : SCL_Pin */
	GPIO_InitStruct.Pin = object->si2cPins.pinSCL;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(object->si2cPins.portSCL, &GPIO_InitStruct);

}

void SI2C_Device_cancel(SI2C_Handle handle)
{
    SI2C_Device_Object *object = handle->object;

    /* clean up object */
    object->mode = SI2C_Device_IDLE_MODE;
    object->currentTransaction = NULL;
}

void SI2C_Device_close(SI2C_Handle handle)
{
    SI2C_Device_Object            *object;

    /* Get the pointer to the object and hwAttrs */
    object = handle->object;

    /* Mark the module as available */
    object->isOpen = false;

    return;
}

int_fast16_t SI2C_Device_control(SI2C_Handle handle, uint_fast16_t cmd, void *arg)
{
    /* No implementation */
    return (SI2C_STATUS_UNDEFINEDCMD);
}

void SI2C_Device_init(SI2C_Handle handle)
{
    SI2C_Device_Object           *object;

    /* Get the pointer to the object */
    object = handle->object;

    /* Initially the drivers is not open */
    object->isOpen = false;

}

SI2C_Handle SI2C_Device_open(SI2C_Handle handle, SI2C_Params *params)
{
    SI2C_Device_Object               *object;

    /* Get the pointer to the object and hwAttrs */
    object = handle->object;

    if(object->isOpen == true){
        return (NULL);
    }

    /* Mark the handle as being used */
    object->isOpen = true;

    /* Configure the IOs.*/
    if (SI2C_Device_initIO(handle, params->custom)) {
      object->isOpen = false;
      return (NULL);
    }

    /* Save parameters */
    object->bitRate = params->bitRate;
    object->slaveAddress = params->slaveAddress;

    /* Specify the idle state for this I2C peripheral */
    object->mode = SI2C_Device_IDLE_MODE;

    /* Clear the head pointer */
    object->headPtr = NULL;
    object->tailPtr = NULL;

    return (handle);
}

bool_t SI2C_Device_transfer(SI2C_Handle handle,
                        SI2C_Transaction *transaction)
{
    bool_t                        ret = false;
    SI2C_Device_Object           *object;

    /* Get the pointer to the object and hwAttrs */
    object = handle->object;

    /* Check if anything needs to be written or read */
    if ((!transaction->writeCount) && (!transaction->readCount)) {
        /* Nothing to write or read */
        return (ret);
    }

    /* Store the new internal counters and pointers */
    object->currentTransaction = transaction;

    object->writeBufIdx = transaction->writeBuf;
    object->writeCountIdx = transaction->writeCount;

    object->readBufIdx = transaction->readBuf;
    object->readCountIdx = transaction->readCount;

    /* Start transfer in Transmit mode */
    if (object->writeCountIdx) {
    	if(!IIC_Send_Multi_Byte(handle, object->writeBufIdx, object->writeCountIdx))
    		return (true);
    }
    /* Start transfer in Receive mode */
    else {
    	if(!IIC_Read_Multi_Byte(handle, object->readBufIdx, object->readCountIdx))
    		return (true);
    }
    return (SI2C_STATUS_SUCCESS);
}


static int SI2C_Device_initIO(SI2C_Handle handle, void *pinCfg) {
    SI2C_Device_Object            *object;
    SI2C_Device_HWAttrsV1 const  *hwAttrs;

    /* Get the pointer to the object and hwAttrs */
    object = handle->object;
    hwAttrs = handle->hwAttrs;

    /* If the pinCfg pointer is NULL, use hwAttrs pins */
    if (pinCfg == NULL) {
		object->si2cPins.portSDA = hwAttrs->sdaPort;
    	object->si2cPins.portSCL = hwAttrs->sclPort;
    	object->si2cPins.pinSDA = hwAttrs->sdaPin;
    	object->si2cPins.pinSCL = hwAttrs->sclPin;
    } else {
    	object->si2cPins.portSDA = ((SI2C_Device_I2CPinCfg *)pinCfg)->portSDA;
    	object->si2cPins.portSCL = ((SI2C_Device_I2CPinCfg *)pinCfg)->portSCL;
    	object->si2cPins.pinSDA = ((SI2C_Device_I2CPinCfg *)pinCfg)->pinSDA;
    	object->si2cPins.pinSCL = ((SI2C_Device_I2CPinCfg *)pinCfg)->pinSCL;
    }

    /* Allocate pins*/
    SI2C_GPIO_Init(handle);

    return SI2C_STATUS_SUCCESS;
}

