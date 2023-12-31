#include "includes.h"
#define  TASK_STK_SIZE   512                //任务堆栈长度
char *s1="学号";
char *s2="姓名拼音";
OS_EVENT *OSSemCreate(INT16U cnt);
OS_EVENT *OSSemDel(OS_EVENT *pevent,INT8U opt, INT8U *err);

OS_STK   StartTaskStk[TASK_STK_SIZE];           //定义任务堆栈区
OS_STK   MyTaskStk[TASK_STK_SIZE];          //定义任务堆栈区
OS_STK   YouTaskStk[TASK_STK_SIZE];         //定义任务堆栈区
INT16S   key;                       //用于退出的键
INT8U OSSemPost(OS_EVENT *pevent);
INT8U err;                          
INT8U y=0;                      //字符显示位置
INT8U count;
INT16U OSSemAccept(OS_EVENT *pevent);
INT8U OSSemQuery(OS_EVENT *pevent,OS_SEM_DATA *pdata);

void  Fun(INT8U x,INT8U y);
void  StartTask(void *data);                //声明起始任务
void  MyTask(void *data);               //声明任务
void  YouTask(void *data);              //声明任务
void OSSemPend(OS_EVENT *pevent,INT16U timeout,INT8U *err);

    


/************************主函数*********************************/
OS_EVENT *Fun_Semp;
void  main (void)
{
    OSInit( );                      //初始化µC/OS-II_II
    PC_DOSSaveReturn( );                //保存Dos环境
    PC_VectSet(uCOS, OSCtxSw);              //安装µC/OS-II_II中断
    Fun_Semp=OSSemCreate(0);
    OSTaskCreate(StartTask,             //创建任务StartTask
    (void*)0,                   //给任务传递参数
    &StartTaskStk[TASK_STK_SIZE - 1],       //设置任务堆栈栈顶
    0);                     //使任务的优先级别为0
    OSStart( );                     //启动多任务管理
}
/***********************任务StartTask*******************************/
void  StartTask (void *pdata)
{
#if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr;
#endif
    pdata = pdata; 
    OS_ENTER_CRITICAL( );
    PC_VectSet(0x08, OSTickISR);        //安装时钟中断向量
    PC_SetTickRate(OS_TICKS_PER_SEC);       //设置µC/OS-II_II时钟频率
    OS_EXIT_CRITICAL( );

    OSStatInit( );              //初始化统计任务
    OSTaskCreate(MyTask,            //创建任务MyTask
        (void*)0,           //给任务传递参数
        &MyTaskStk[TASK_STK_SIZE - 1],  //设置任务堆栈栈顶
        1);             //使任务的优先级别为1
    OSTaskCreate(YouTask,           //创建任务YouTask
        (void*)0,           //给任务传递参数
        &YouTaskStk[TASK_STK_SIZE - 1], //设置任务堆栈栈顶
        2);             //使任务的优先级别为2

    for (;;) 
    {
    //如果按下Esc键则退出µC/OS-II_II
    if (PC_GetKey(&key) == TRUE) 
    {
            if (key == 0x1B) 
        {
                PC_DOSReturn();
            }
        }
        OSTimeDlyHMSM(0, 0, 3, 0);  //等待3秒
    }
}
/************************任务MyTask*******************************/
void  MyTask (void *pdata)
{
#if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr;
#endif
    pdata = pdata;
  for (;;) 
    {
    OSSemPend(Fun_Semp,0,&err);
       PC_DispStr(0,++y,
        s1,//学号
        DISP_BGND_BLACK+DISP_FGND_WHITE );
     Fun(12,y);                           //调用函数fun
     OSTimeDlyHMSM(0, 0, 1, 0);  //等待1秒
    }
}
/************************任务YouTask******************************/
void  YouTask (void *pdata)
{
#if OS_CRITICAL_METHOD == 3
    OS_CPU_SR  cpu_sr;
#endif
    pdata = pdata;     
 for (;;)
 {
  PC_DispStr(0,++y,
        s2,
        DISP_BGND_BLACK+DISP_FGND_//颜色 );
  
        OSTimeDlyHMSM(0, 0, 1, 0);  //等待1秒
        count++;
        if(count==6)
        {
            count=0;
            OSSemPost(Fun_Semp);
        }
//计数
  }
}
void Fun(INT8U x,INT8U y)
{
 PC_DispStr(x,y,"invoked Fun()!",
  DISP_BGND_BLACK+DISP_FGND_//颜色 );
}
/************************End**************************************/
