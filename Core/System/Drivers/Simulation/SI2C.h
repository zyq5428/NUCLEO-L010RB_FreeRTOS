/*
 * SI2C.h
 *
 *  Created on: 2022年12月26日
 *
 */

#ifndef SYSTEM_DRIVERS_SIMULATION_SI2C_H_
#define SYSTEM_DRIVERS_SIMULATION_SI2C_H_

#include "system.h"

#define I2C_CMD_RESERVED           (32)

/*!
 * @private
 * Common I2C_control status code reservation offset.
 * I2C driver implementations should offset status codes with
 * #I2C_STATUS_RESERVED growing negatively.
 *
 * Example implementation specific status codes:
 * @code
 * #define I2CXYZ_STATUS_ERROR0     I2C_STATUS_RESERVED - 0
 * #define I2CXYZ_STATUS_ERROR1     I2C_STATUS_RESERVED - 1
 * #define I2CXYZ_STATUS_ERROR2     I2C_STATUS_RESERVED - 2
 * @endcode
 */
#define SI2C_STATUS_RESERVED        (-32)
#define SI2C_STATUS_SUCCESS         (0)
#define SI2C_STATUS_ERROR           (-1)
#define SI2C_STATUS_UNDEFINEDCMD    (-2)

typedef struct SI2C_Config_ *SI2C_Handle;

typedef struct {
    void         *writeBuf;
    size_t        writeCount;
    void         *readBuf;
    size_t        readCount;
    uint_least8_t slaveAddress;
    void         *arg;
    void         *nextPtr;
} SI2C_Transaction;

typedef enum {
	SI2C_10kHz      = 0,
	SI2C_50kHz      = 1,
    SI2C_100kHz     = 2,    /*!< I2C Standard-mode. Up to 100 kbit/s. */
    SI2C_400kHz     = 3,    /*!< I2C Fast-mode. Up to 400 kbit/s. */
    SI2C_1000kHz    = 4,    /*!< I2C Fast-mode Plus. Up to 1Mbit/s. */
    SI2C_3330kHz    = 5,    /*!< I2C High-speed mode. Up to 3.4Mbit/s. */
    SI2C_3400kHz    = 5,    /*!< I2C High-speed mode. Up to 3.4Mbit/s. */
} SI2C_BitRate;

extern uint32_t SI2C_Delay[];

typedef struct {
    SI2C_BitRate bitRate;
    uint8_t		slaveAddress;
    /*! Pointer to a device specific extension of the #I2C_Params */
    void *custom;
} SI2C_Params;

typedef void (*SI2C_CancelFxn) (SI2C_Handle handle);

typedef void (*SI2C_CloseFxn) (SI2C_Handle handle);

typedef int_fast16_t (*SI2C_ControlFxn) (SI2C_Handle handle, uint_fast16_t cmd,
    void *controlArg);

typedef void (*SI2C_InitFxn) (SI2C_Handle handle);

typedef SI2C_Handle (*SI2C_OpenFxn) (SI2C_Handle handle, SI2C_Params *params);

typedef bool_t (*SI2C_TransferFxn) (SI2C_Handle handle,
    SI2C_Transaction *transaction);

typedef struct SI2C_FxnTable_ {
    SI2C_CancelFxn   cancelFxn;
    SI2C_CloseFxn    closeFxn;
    SI2C_ControlFxn  controlFxn;
    SI2C_InitFxn     initFxn;
    SI2C_OpenFxn     openFxn;
    SI2C_TransferFxn transferFxn;
} SI2C_FxnTable;

typedef struct SI2C_Config_ {
    SI2C_FxnTable const *fxnTablePtr;
    void               *object;
    void         const *hwAttrs;
} SI2C_Config;


extern void SI2C_cancel(SI2C_Handle handle);

extern void SI2C_close(SI2C_Handle handle);

extern int_fast16_t SI2C_control(SI2C_Handle handle, uint_fast16_t cmd,
    void *controlArg);

extern void SI2C_init(void);

extern SI2C_Handle SI2C_open(uint_least8_t index, SI2C_Params *params);

extern void SI2C_Params_init(SI2C_Params *params);

extern bool_t SI2C_transfer(SI2C_Handle handle, SI2C_Transaction *transaction);


#endif /* SYSTEM_DRIVERS_SIMULATION_SI2C_H_ */
