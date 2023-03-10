#ifndef _HS3003_HW_H_
#define _HS3003_HW_H_

#include <stdint.h>


/* 模块结构块大小 */
#define HS3003_MODULE_SIZE          (32)

/* 模块实体定义 */
#define HS3003_Declare(hscb)        uint32_t hscb[(HS3003_MODULE_SIZE + 3) / 4]

/*******************************************************************************
*  函数名称      :   uint8_t HS3003_DeInit(uint32_t *hscb)
*  描     述   :   结构块重置初始化
*  输     入   :   模块结构块
*  返     回   :   1-成功    0-失败
*  作     者   :   cyc
*  时     间   :   2023.01.21  14:49:53
*******************************************************************************/
uint8_t HS3003_DeInit(uint32_t *hscb);

/*******************************************************************************
*  函数名称      :   void HS3003_Init(void)
*  描     述   :   传感器初始化
*  输     入   :   模块结构块
*  返     回   :   1-成功    0-失败
*  作     者   :   cyc
*  时     间   :   2023.01.21  14:51:25
*******************************************************************************/
uint8_t HS3003_Init(uint32_t *hscb);

/*******************************************************************************
*  函数名称      :   uint8_t HS3003_InitFunc(uint32_t *hscb,
*  描     述   :   方法初始化
*  输     入   :   
*  返     回   :   1-成功    0-失败
*  作     者   :   cyc
*  时     间   :   2023.01.21  15:05:15
*******************************************************************************/
uint8_t HS3003_InitFunc(uint32_t *hscb,
                                uint8_t (*init)(void *),
                                void (*read)(void *, uint8_t *, uint32_t),
                                void (*write)(void *, uint8_t *, uint32_t),
                                void *pt);

/*******************************************************************************
*  函数名称      :   uint8_t HS3003_Calc(uint32_t *hscb)
*  描     述   :   数据获取--1ms周期调用
*  输     入   :   模块结构块
*  返     回   :   1-成功    0-失败
*  作     者   :   cyc
*  时     间   :   2023.01.21  14:51:43
*******************************************************************************/
uint8_t HS3003_Calc(uint32_t *hscb);

/*******************************************************************************
*  函数名称      :   int32_t HS3003_GetRH(uint32_t *hscb)
*  描     述   :   获取湿度
*  输     入   :   模块结构块
*  返     回   :   模块湿度
*  作     者   :   cyc
*  时     间   :   2023.01.23  15:10:16
*******************************************************************************/
int32_t HS3003_GetRH(uint32_t *hscb);

/*******************************************************************************
*  函数名称      :   int32_t HS3003_GetTemp(uint32_t *hscb)
*  描     述   :   获取温度
*  输     入   :   模块结构块
*  返     回   :   模块温度
*  作     者   :   cyc
*  时     间   :   2023.01.23  15:10:33
*******************************************************************************/
int32_t HS3003_GetTemp(uint32_t *hscb);





#endif

