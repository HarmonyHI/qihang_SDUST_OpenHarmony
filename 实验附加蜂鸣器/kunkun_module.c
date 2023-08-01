#include <stdio.h>
#include "hi_task.h"
#include "ohos_init.h"
#include <stdio.h>
#include <unistd.h>
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "hi_pwm.h"
#include "hi_errno.h"
#include "hi_io.h"
#include "hi_task.h"
#include "hi_gpio.h"
#define ALARM HI_IO_NAME_GPIO_2
#define PWM_POWER HI_PWM_PORT_PWM2
#define PWM_ALARM_BRIDGE HI_IO_FUNC_GPIO_2_PWM2_OUT
#define DOL 1908	
#define REL 1701
#define MIL 1515
#define FAL 1449
#define SOL 1275
#define LAL 1136
#define SIL 1012
#define DOM 956
#define REM 852
#define MIM 759
#define FAM 716
#define SOM 638
#define LAM 568
#define SIM 506
#define DOH 478
#define REH 426
#define MIH 379
#define FAH 358
#define SOH 319
#define LAH 284
#define SIH 254
#define STOP 65535


hi_void kunkun_pwm_init(hi_void)
{
    int ret = -1;
    ret = hi_pwm_deinit(PWM_POWER);
    if(ret != 0){ 
        printf("hi_pwm_deinit failed :%#x \r\n",ret); 
    }
    ret = hi_pwm_init(PWM_POWER);
    if(ret != 0){ 
        printf("hi_pwm_init failed :%#x \r\n",ret); 
    }
    ret = hi_pwm_set_clock(PWM_CLK_160M);
    if(ret != 0){ 
        printf("hi_pwm_set_clock failed ret : %#x \r\n",ret); 
    }
}
hi_void kunkun_alarm_init(hi_void)
{
    IoTGpioInit(ALARM);
    (void)hi_io_set_func(ALARM, PWM_ALARM_BRIDGE);
    IoTGpioSetDir(ALARM, HI_GPIO_DIR_OUT);
    kunkun_pwm_init();
}

static unsigned int g_MonitorTask;
static const hi_task_attr MonitorTaskAttr = {
    .task_prio = 20,
    .stack_size = 4096,
    .task_name = "CAIXUKUN",
};

void *MonitorTask_kunkun(void * para)
{
    printf("stg1\n");
    printf("stg2\n");
    int music[]=  
    { 
        LAM,1,LAM,1,LAM,1,LAM,DOH,REH,MIH,
        LAM,1,LAM,1,LAM,SOM,SOM,LAM,
        LAM,1,LAM,1,LAM,1,LAM,DOH,REH,MIH,
        LAM,1,LAM,1,LAM,FAH,FAH,MIH,
        LAM,1,LAM,1,LAM,1,LAM,DOH,REH,MIH,
        LAM,1,LAM,1,LAM,1,LAM,SOM,LAM,
        LAM,1,LAM,1,LAM,1,LAM,DOH,REH,MIH,
        LAM,SOM,SOM,LAM,1,LAM,
        LAM,1,LAM,SOM,SOM,SOM,LAM,
        SOM,MIM,MIM,SOM,MIM,
        LAM,1,LAM,SOM,LAM,SIM,
        DOH,SIM,LAM,SIM,LAM,SOM,0,0
    };
    for(int i=0;music[i]>0;i=i+1){
        music[i] = (int)(2000000/music[i]);
    }
    for(int i=0;music[i]>0;i=i+1){
        IoTPwmStart(PWM_POWER,40,music[i]);
        usleep(200*1000);
        IoTPwmStop(PWM_POWER);
    }
    printf("stg3\n");
    return NULL;
}

 
hi_void kunkun_adc_demo(hi_void)
{   
    kunkun_alarm_init();
    (void)hi_task_create(&g_MonitorTask,
        &MonitorTaskAttr,
        MonitorTask_kunkun,
        NULL);
}

APP_FEATURE_INIT(kunkun_adc_demo);
