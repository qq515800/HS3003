#include "HS3003_Dr.h"


/* 传感器通信缓存大小 */
#define HS3003_COMM_SIZE        (4)

/* 请求延时 */
#define HS3003_REQ_DELAY        (10)

/* 数据获取延时 */
#define HS3003_DATA_GET_DELAY   (100)


/* 通信状态机 */
enum emHS3003CommSta
{
    HS3003COMM_STA_idle = 0,
    HS3003COMM_STA_write = 1,
    HS3003COMM_STA_read = 2,
    HS3003COMM_STA_getdata = 3,
};

/* 输入方法 */
struct tagHS3003InFunc
{
    uint8_t (*Init)(void *);        /* IIC初始化 */
    void (*Read)(void *,            /* IIC读 */
                uint8_t *,          /* --读取的数据 */
                uint32_t);          /* --读取的数据长度 */
    void (*Write)(void *,           /* IIC写 */
                uint8_t *,          /* --写入的数据 */
                uint32_t);          /* --写入的数据长度 */
    void *Pt;                       /* IIC结构块 */
};

/* 输出 */
struct tagHS3003Output
{
    int32_t RH;                     /* 湿度 */
    int32_t TEMP;                   /* 温度 */
};

/* 私有变量 */
struct tagHS3003Prv
{
    uint8_t Buf[HS3003_COMM_SIZE];  /* 通信缓存 */
    uint32_t Cnt:30;                /* 延时计数 */
    uint32_t Sta:2;                 /* 状态机 */
};

/* 模块结构体 */
struct tagHS3003CB
{
    struct tagHS3003InFunc InFunc;
    struct tagHS3003Output Output;
    struct tagHS3003Prv Prv;
};

/* 模块实体 */
struct tagHS3003CB HS3003CB =
{
    .Output =
    {
        .RH = 0.0,
        .TEMP = 0.0,
    },
    .Prv =
    {
        .Buf = {0, 0, 0, 0},
    },
};

/*******************************************************************************
*  函数名称      :   uint8_t HS3003_DeInit(uint32_t *hscb)
*  描     述   :   结构块重置初始化
*  输     入   :   模块结构块
*  返     回   :   1-成功    0-失败
*  作     者   :   cyc
*  时     间   :   2023.01.21  14:49:53
*******************************************************************************/
uint8_t HS3003_DeInit(uint32_t *hscb)
{
    struct tagHS3003CB *cb = (struct tagHS3003CB *)hscb;

    /* 习惯性保护一下*/
    if (!cb)
    {
        return 0;
    }

    /* 实体数据清0 */
    for (uint8_t i = 0; i < HS3003_COMM_SIZE; i++)
    {
        cb->Prv.Buf[i] = 0;
    }
    cb->Output.RH = 0;
    cb->Output.TEMP = 0;

    cb->Prv.Cnt = 0;
    cb->Prv.Sta = HS3003COMM_STA_idle;

    cb->InFunc.Init = (void *)0;
    cb->InFunc.Read = (void *)0;
    
    cb->InFunc.Write = (void *)0;
    cb->InFunc.Pt = (void *)0;
    

    return 1;
}

/*******************************************************************************
*  函数名称      :   void HS3003_Init(void)
*  描     述   :   传感器初始化
*  输     入   :   模块结构块
*  返     回   :   1-成功    0-失败
*  作     者   :   cyc
*  时     间   :   2023.01.21  14:51:25
*******************************************************************************/
uint8_t HS3003_Init(uint32_t *hscb)
{
    struct tagHS3003CB *cb = (struct tagHS3003CB *)hscb;

    /* 习惯性保护一下*/
    if (!cb)
    {
        return 0;
    }

    
    cb->Prv.Sta = HS3003COMM_STA_write;
    /* IIC初始化 */
    return cb->InFunc.Init(cb->InFunc.Pt);
}

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
                                void *pt)
{
    struct tagHS3003CB *cb = (struct tagHS3003CB *)hscb;

    /* 习惯性保护一下*/
    if (!cb)
    {
        return 0;
    }

    cb->InFunc.Init = init;
    cb->InFunc.Read = read;
    cb->InFunc.Write = write;
    cb->InFunc.Pt = pt;
    
    return 1;
}


/*******************************************************************************
*  函数名称      :   uint8_t HS3003_Calc(uint32_t *hscb)
*  描     述   :   数据获取--1ms周期调用
*  输     入   :   模块结构块
*  返     回   :   1-成功    0-失败
*  作     者   :   cyc
*  时     间   :   2023.01.21  14:51:43
*******************************************************************************/
uint8_t HS3003_Calc(uint32_t *hscb)
{
    struct tagHS3003CB *cb = (struct tagHS3003CB *)hscb;

    /* 习惯性保护一下*/
    if (!cb)
    {
        return 0;
    }

    cb->Prv.Cnt++;

    switch (cb->Prv.Sta)
    {
        /* 空闲阶段，未初始化时就一直在这 */
        case HS3003COMM_STA_idle:
        {
            cb->Prv.Cnt = 0;
            break;
        }
        /* 先发送写指令，请求数据检测 */
        case HS3003COMM_STA_write:
        {
            cb->InFunc.Write(cb->InFunc.Pt, cb->Prv.Buf, 4);
            cb->Prv.Cnt = 0;
            cb->Prv.Sta = HS3003COMM_STA_read;
            break;
        }
        /* 再发读指令获取数据 */
        case HS3003COMM_STA_read:
        {
            /* 请求完延时一会再读 */
            if (cb->Prv.Cnt < HS3003_REQ_DELAY)
            {
                break;
            }
            cb->InFunc.Read(cb->InFunc.Pt, cb->Prv.Buf, 4);
            cb->Prv.Cnt = 0;
            cb->Prv.Sta = HS3003COMM_STA_getdata;
            break;
        }
        /* 获取数据 */
        case HS3003COMM_STA_getdata:
        {
            if (cb->Prv.Cnt < HS3003_DATA_GET_DELAY)
            {
                break;
            }
            cb->Output.RH =(int32_t)(((uint32_t)(cb->Prv.Buf[0] & 0x3f)<< 8)+ cb->Prv.Buf[1]) * 10000 / 16383; //湿度变换为浮点
            cb->Output.TEMP =(int32_t)((((uint32_t)cb->Prv.Buf[2]<<8)+cb->Prv.Buf[3])>> 2) * 16500 / 16383 - 4000;//温度变换浮点
            cb->Prv.Cnt = 0;
            cb->Prv.Sta = HS3003COMM_STA_write;
            break;
        }
        /* 异常处理 */
        default:
        {
            cb->Prv.Cnt = 0;
            cb->Prv.Sta = HS3003COMM_STA_idle;
            break;
        }
    }

    return 1;
}


/*******************************************************************************
*  函数名称      :   int32_t HS3003_GetRH(uint32_t *hscb)
*  描     述   :   获取湿度
*  输     入   :   模块结构块
*  返     回   :   模块湿度
*  作     者   :   cyc
*  时     间   :   2023.01.23  15:10:16
*******************************************************************************/
int32_t HS3003_GetRH(uint32_t *hscb)
{
    struct tagHS3003CB *cb = (struct tagHS3003CB *)hscb;

    if (!cb)
    {
        return 0;
    }
    
    return cb->Output.RH;
}

/*******************************************************************************
*  函数名称      :   int32_t HS3003_GetTemp(uint32_t *hscb)
*  描     述   :   获取温度
*  输     入   :   模块结构块
*  返     回   :   模块温度
*  作     者   :   cyc
*  时     间   :   2023.01.23  15:10:33
*******************************************************************************/
int32_t HS3003_GetTemp(uint32_t *hscb)
{
    struct tagHS3003CB *cb = (struct tagHS3003CB *)hscb;

    if (!cb)
    {
        return 0;
    }
    return cb->Output.TEMP;
}



