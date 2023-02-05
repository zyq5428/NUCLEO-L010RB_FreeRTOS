#include "delay.h"

/**
 * @brief	延时微秒(us)函数
 *
 * @remark	nus:0~190887435(最大值即2^32/fac_us@fac_us=22.5)
 *
 * @param   nus		需要延时多少微秒
 *
 * @return  void
 */
void delay_us(uint32_t nus)
{
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;
    uint32_t reload = SysTick->LOAD;				//LOAD的值
    ticks = nus * (SysTick->LOAD + 1UL) / 1000;		//需要的节拍数
    told = SysTick->VAL;					//刚进入时的计数器值

    while(1)
    {
        tnow = SysTick->VAL;

        if(tnow != told)
        {
            if(tnow < told)
            	tcnt += told - tnow;	//这里注意一下SYSTICK是一个递减的计数器就可以了.
            else
            	tcnt += reload - tnow + told;
			
            told = tnow;
            if(tcnt >= ticks)
            	break;			//时间超过/等于要延迟的时间,则退出.
        }
    }
}


/**
 * @brief	延时毫秒(ms)函数
 *
 * @param   nms		需要延时多少毫秒
 *
 * @return  void
 */
void delay_ms(uint32_t nms)
{
    uint32_t i;

    for(i = 0; i < nms; i++)
    	delay_us(1000);
}







