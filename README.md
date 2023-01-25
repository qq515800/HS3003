# HS3003温湿度模块驱动

# 简介
该模块由IIC驱动，所以需要有系统支持IIC接口。模块可以在带操作系统下使用，也可以在裸机下使用。

# 例子
以下例子是在瑞萨开发板上截取的
```c
#include "HS3003_Task.h"

#include "HS3003_Hw.h"
#include "hal_data.h"

struct tagHS3003Task
{
    HS3003_Declare(HS3003CB);
    int32_t RH;
    int32_t Temp;
};

struct tagHS3003Task HumTemp;


uint8_t IIC_Init(void *cb)
{
    const i2c_master_instance_t *hs3003 = (const i2c_master_instance_t *)cb;
    
    return (uint8_t)R_SCI_I2C_Open(hs3003->p_ctrl, hs3003->p_cfg);
}

void IIC_Read(void *cb, uint8_t *buf, uint32_t size)
{
    const i2c_master_instance_t *hs3003 = (const i2c_master_instance_t *)cb;
    
    R_SCI_I2C_Read(hs3003->p_ctrl, buf, size, 0);
}

void IIC_Write(void *cb, uint8_t *buf, uint32_t size)
{
    const i2c_master_instance_t *hs3003 = (const i2c_master_instance_t *)cb;
    
    R_SCI_I2C_Write(hs3003->p_ctrl, buf, size, 0);
}

void hs3003_i2c3_callback(i2c_master_callback_args_t * p_args)
{
    //if (HS3003_CALLBACK_STATUS_SUCCESS == p_args->event)
    {
        //hs3003_callback_status = HS3003_CALLBACK_STATUS_SUCCESS;
    }
    //else
    {
        //hs3003_callback_status = HS3003_CALLBACK_STATUS_REPEAT;
    }
}

                /* HS3003_Task entry function */
                /* pvParameters contains TaskHandle_t */
                void HS3003_Task_entry(void * pvParameters)
                {
                    FSP_PARAMETER_NOT_USED(pvParameters);
                    HS3003_DeInit(HumTemp.HS3003CB);
                    HS3003_InitFunc(HumTemp.HS3003CB,
                                    IIC_Init,
                                    IIC_Read,
                                    IIC_Write,
                                    (void *)&HS3003_I2C3);
                    HS3003_Init(HumTemp.HS3003CB);

                    /* TODO: add your own code here */
                    while(1)
                    {
                        HS3003_Calc(HumTemp.HS3003CB);

                        HumTemp.RH = HS3003_GetRH(HumTemp.HS3003CB);
                        HumTemp.Temp = HS3003_GetTemp(HumTemp.HS3003CB);
                        
                        /* 1ms调用一次 */
                        vTaskDelay(1);
                    }
                }

```
