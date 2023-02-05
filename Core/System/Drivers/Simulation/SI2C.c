/*
 * SI2C.c
 *
 *  Created on: 2022年12月26日
 *
 */
#include <Drivers/Simulation/SI2C.h>

extern const SI2C_Config SI2C_config[];
extern const uint_least8_t SI2C_count;

/* Default I2C parameters structure */
const SI2C_Params SI2C_defaultParams = {
    SI2C_100kHz,         /* bitRate */
	0x00,
    NULL                /* custom */
};

static bool_t isInitialized = false;

uint32_t SI2C_Delay[7] = {50, 10, 5, 1, 1, 1, 1};

void SI2C_cancel(SI2C_Handle handle)
{
    handle->fxnTablePtr->cancelFxn(handle);
}

void SI2C_close(SI2C_Handle handle)
{
    handle->fxnTablePtr->closeFxn(handle);
}

void SI2C_init(void)
{
    uint_least8_t i;

    if (!isInitialized) {
        isInitialized = (uint8_t) true;

        /* Call each driver's init function */
        for (i = 0; i < SI2C_count; i++) {
            SI2C_config[i].fxnTablePtr->initFxn((SI2C_Handle)&(SI2C_config[i]));
        }
    }
}

SI2C_Handle SI2C_open(uint_least8_t index, SI2C_Params *params)
{
    SI2C_Handle handle = NULL;

    if (isInitialized && (index < SI2C_count)) {
        /* If params are NULL use defaults. */
        if (params == NULL) {
            params = (SI2C_Params *) &SI2C_defaultParams;
        }

        /* Get handle for this driver instance */
        handle = (SI2C_Handle)&(SI2C_config[index]);
        handle = handle->fxnTablePtr->openFxn(handle, params);
    }

    return (handle);
}

void SI2C_Params_init(SI2C_Params *params)
{
    *params = SI2C_defaultParams;
}

bool_t SI2C_transfer(SI2C_Handle handle, SI2C_Transaction *transaction)
{
    return (handle->fxnTablePtr->transferFxn(handle, transaction));
}
