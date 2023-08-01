#include <stdio.h>
#include <unistd.h>
#include "motor_module.h"
#include "hi_timer.h"
#include "hi_gpio.h"
#include "hi_pwm.h"
#include "dbg.h"
#include "hi_spi.h"
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_pwm.h"
#include "iot_errno.h"
#include "iot_gpio.h"
#include "hi_errno.h"
#include "hi_io.h"
#include "hi_task.h"
#include <string.h>
#include <stdlib.h>
#include "wifi_connect.h"
#include "lwip/sockets.h"
#include "oc_mqtt.h"
#include "E53_IA1.h"
#include <cJSON.h>
#include "wifi_device.h"
#include "wifi_hotspot.h"
#include "wifi_error_code.h"
#include "lwip/netifapi.h"
#define WIFI_SSID   "woyaofangjia"
#define WIFI_PASSWORD "20020617"
#define CLIENT_ID "64a74dbd5c51f150f4d3e2c8_123456789_0_0_2023070702"
#define USERNAME "64a74dbd5c51f150f4d3e2c8_123456789"
#define PASSWORD "2824511944a1554958760d8cfcdb20505074a2de54a644ed017b601a47c87320"
#define MSGQUEUE_OBJECTS 16 // number of Message Queue Objects
#define motorio HI_IO_NAME_GPIO_2
#define pwm_motor_bridge HI_IO_FUNC_GPIO_2_PWM2_OUT
#define motor_pwm_controller HI_PWM_PORT_PWM2
#define PWM_CLK_FREQ 160000000 
#define freq 15000  //min
#define FAN_LED HI_IO_NAME_GPIO_6
#define FAN_LED_GPIO HI_IO_FUNC_GPIO_6_GPIO
extern hi_gpio_value Infrared_info;
extern float Temperature; //环境温度，状态
extern float Humidity; //环境湿度，状态
char out_name[50] = WIFI_SSID;
int year=8888;
int month=8;
int day=8;
int is_connected = 0;

typedef struct
{ // object data type
    char *Buf;
    uint8_t Idx;
} MSGQUEUE_OBJ_t;

MSGQUEUE_OBJ_t msg;
osMessageQueueId_t mid_MsgQueue; // message queue id

typedef enum
{
    en_msg_cmd = 0,
    en_msg_report,
} en_msg_type_t;

typedef struct
{
    char *request_id;
    char *payload;
} cmd_t;

typedef struct
{
    int lum;
    int temp;
    int hum;
} report_t;

typedef struct
{
    en_msg_type_t msg_type;
    union
    {
        cmd_t cmd;
        report_t report;
    } msg;
} app_msg_t;

typedef struct
{
    int connected;
    int led;
    int motor;
} app_cb_t;
static app_cb_t g_app_cb;



//--------------------------------------huawei_data--------------------------
extern int RPM;
int mode = 0;
int auto_fan_strength = 50; //触发时的风扇强度
int temp_threshold = 30; //温度触发阈值
int humi_threshold = 80; //湿度触发阈值
//-----------------------------------------------------------------------------
int locker_lan_led = -1; //LED强制锁
int fan_led_stat = 0; //LED状态
static unsigned int g_MonitorTask;
const hi_task_attr MonitorTaskAttr = {
    .task_prio = 20,
    .stack_size = 4096,
    .task_name = "BuggyNetworkMonitorTask",
};

void *MonitorOledTask(void * para) /* OLEDtask处理函数 */
{
    while(1){
        test_led_screen();
    }
    return NULL;
}

void *MonitorMotorTask(void * para) /* 电机task处理函数 */
{
    while(1){
        manage_motor();
    }
    return NULL;
}

void *MonitorShtTask(void * para) /* 温湿度传感器处理函数 */
{
    while(1){
        sleep(2);
        SHT3X_ReadMeasurementVal(0);
    }
    return NULL;
}

void control_fan_led(int stat){
    if(locker_lan_led != -1){
        return;
    }
    fan_led_stat = stat;
    (void)hi_gpio_set_ouput_val(FAN_LED,fan_led_stat == 0?HI_GPIO_VALUE0:HI_GPIO_VALUE1);
}

static void deal_cmd_msg(cmd_t *cmd)
{
    // {"paras":{"LED_Satus":"ON"},"service_id":"SmartFan_Service","command_name":"LED_Status_Control"}
    cJSON *obj_root;
    cJSON *obj_cmdname;
    cJSON *obj_paras;
    cJSON *obj_para;
    int cmdret = 1;
    oc_mqtt_profile_cmdresp_t cmdresp;
    obj_root = cJSON_Parse(cmd->payload);
    if(NULL == obj_root){
        printf("OBJ_ROOT_NULL\r\n");
        return;
    }
    obj_cmdname = cJSON_GetObjectItem(obj_root,"command_name");
    printf("got %s \r\n", cJSON_GetStringValue(obj_cmdname));
    obj_paras = cJSON_GetObjectItem(obj_root,"paras");
    if(NULL == obj_cmdname){
        cmdresp.paras = 1;
        cmdresp.request_id = cmd->request_id;
        cmdresp.ret_code = cmdret;
        cmdresp.ret_name = "success";
        (void)oc_mqtt_profile_cmdresp("64a74dbd5c51f150f4d3e2c8_123456789",&cmdresp);
        printf("cmdname_null\r\n");
        return;
    }

    if(0 == strcmp(cJSON_GetStringValue(obj_cmdname),"Fan_Status_Control")){
        printf("fun1\r\n");
        obj_para = cJSON_GetObjectItem(obj_paras,"Fan_Status");
        mode = 2;
        if(NULL == obj_para){
            printf("OBJ_PARA_NULL\r\n");
            return;
        }
        else if(0 == strcmp(cJSON_GetStringValue(obj_para),"LEVEL01")){
            RPM = 1;
            IoTPwmStart(motor_pwm_controller, 1, freq);
            printf("LEVEL01\r\n");
        }
        else if(0 == strcmp(cJSON_GetStringValue(obj_para),"LEVEL02")){
            RPM = 20;
            IoTPwmStart(motor_pwm_controller, 10, freq);
            printf("LEVEL02\r\n");
        }
        else if(0 == strcmp(cJSON_GetStringValue(obj_para),"LEVEL03")){
            RPM = 40;
            IoTPwmStart(motor_pwm_controller, 40, freq);
            printf("LEVEL03\r\n");
        }
        else if(0 == strcmp(cJSON_GetStringValue(obj_para),"LEVEL04")){
            RPM = 70;
            IoTPwmStart(motor_pwm_controller, 70, freq);
            printf("LEVEL04\r\n");
        }
        else if(0 == strcmp(cJSON_GetStringValue(obj_para),"OFF")){
            RPM = 0;
            IoTPwmStop(motor_pwm_controller);
            printf("OFF\r\n");
        }
        else{
            printf("MODE_UNKNOWN %s\r\n", cJSON_GetStringValue(obj_para));
        }
        cmdret = 0;
    }
    else if(0 == strcmp(cJSON_GetStringValue(obj_cmdname),"LED_Status_Control")){
        printf("fun2\r\n");
        obj_para = cJSON_GetObjectItem(obj_paras,"LED_Satus");
        if(NULL == obj_para){
            printf("OBJ_PARA_NULL\r\n");
            return;
        }
        else if(0 == strcmp(cJSON_GetStringValue(obj_para),"ON")){
            locker_lan_led = -1;
            control_fan_led(1);
            locker_lan_led = 1;
            printf("LED ON\r\n");
        }
        else if(0 == strcmp(cJSON_GetStringValue(obj_para),"OFF")){
            locker_lan_led = -1;
            control_fan_led(0);
            locker_lan_led = 0;
            printf("LED OFF\r\n");
        }
        else if(0 == strcmp(cJSON_GetStringValue(obj_para),"DEFAULT")){
            locker_lan_led = -1;
            control_fan_led(0);
            printf("DEFAULT\r\n");
        }
        else{
            printf("MODE_UNKNOWN %s\r\n", cJSON_GetStringValue(obj_para));
        }
        cmdret = 0;
    }
    else if(0 == strcmp(cJSON_GetStringValue(obj_cmdname),"Temperature_Threshold_Control")){
        printf("fun3\r\n");
        obj_para = cJSON_GetObjectItem(obj_paras,"Temperature_Threshold");
        if(NULL == obj_para){
            printf("OBJ_PARA_NULL\r\n");
            return;
        }
        // temp_threshold = atoi(cJSON_GetStringValue(obj_para));
        temp_threshold = (int)cJSON_GetNumberValue(obj_para);
        if(temp_threshold > 100 || temp_threshold <-100){
            printf("temp_threshold illegal %s\r\n", cJSON_GetStringValue(obj_para));
            temp_threshold = 30;
        }
        printf("New Temperature_Threshold %d\r\n", temp_threshold);
        cmdret = 0;
    }
    else if(0 == strcmp(cJSON_GetStringValue(obj_cmdname),"Humidity_Threshold_Control")){
        printf("fun4\r\n");
        obj_para = cJSON_GetObjectItem(obj_paras,"Humidity_Threshold");
        if(NULL == obj_para){
            printf("OBJ_PARA_NULL\r\n");
            return;
        }
        else{
            humi_threshold = (int)cJSON_GetNumberValue(obj_para);
            if(humi_threshold > 200 || humi_threshold <-100){
                printf("humi_threshold illegal %s\r\n", cJSON_GetStringValue(obj_para));
                humi_threshold = 80;
            }
            printf("New humi_threshold %d\r\n", humi_threshold);
            cmdret = 0;
        }
    }
    else if(0 == strcmp(cJSON_GetStringValue(obj_cmdname),"Defaultl_Fan_Status_Control")){
        printf("fun5\r\n");
        obj_para = cJSON_GetObjectItem(obj_paras,"Defaultl_Fan_Status");
        if(NULL == obj_para){
            printf("OBJ_PARA_NULL\r\n");
            return;
        }
        else{
            auto_fan_strength = (int)cJSON_GetNumberValue(obj_para);
            if(auto_fan_strength > 200 || auto_fan_strength <-100){
                printf("auto_fan_strength illegal %s\r\n", cJSON_GetStringValue(obj_para));
                auto_fan_strength = 80;
            }
        }
        printf("New auto_fan_strength %d\r\n", auto_fan_strength);
        cmdret = 0;
    }
    else if(0 == strcmp(cJSON_GetStringValue(obj_cmdname),"System_Status_Control")){
        printf("fun6\r\n");
        obj_para = cJSON_GetObjectItem(obj_paras,"System_Status");
        if(NULL == obj_para){
            printf("OBJ_PARA_NULL\r\n");
            return;
        }
        else if(0 == strcmp(cJSON_GetStringValue(obj_para),"AUTO")){
            mode = 1;
            printf("MODE AUTO\r\n");
        }
        else if(0 == strcmp(cJSON_GetStringValue(obj_para),"ADJUST")){
            mode = 2;
            printf("MODE ADJUST\r\n");
        }
        else if(0 == strcmp(cJSON_GetStringValue(obj_para),"CLOSE")){
            mode = 0;
            printf("MODE CLOSER\r\n");
        }
        else{
            printf("MODE_UNKNOWN %s\r\n", cJSON_GetStringValue(obj_para));
        }
        cmdret = 0;
    }
    else{
        printf("REQUEST_UNKNOWN %s\r\n", cJSON_GetStringValue(obj_cmdname));
    }
    cJSON_Delete(obj_root);
    cmdresp.paras = 1;
    cmdresp.request_id = cmd->request_id;
    cmdresp.ret_code = cmdret;
    cmdresp.ret_name = "success";
    (void)oc_mqtt_profile_cmdresp("64a74dbd5c51f150f4d3e2c8_123456789",&cmdresp);
    printf("exec json over %s\r\n", cJSON_GetStringValue(obj_cmdname));
    return;
}


static void deal_report_msg(report_t *report)
{
    // oc_mqtt_profile_service_t    service;
    // oc_mqtt_profile_kv_t         Fan_Status;
    // oc_mqtt_profile_kv_t         System_Status;
    // oc_mqtt_profile_kv_t         LED_Status;


    // service.event_time = NULL;
    // service.service_id = "SmartFan_Service";
    // service.service_property = &Fan_Status;
    // service.nxt = NULL;

    // Fan_Status.key = "Temperature";
    // Fan_Status.value = &report->temp;
    // Fan_Status.type = EN_OC_MQTT_PROFILE_VALUE_INT;
    // Fan_Status.nxt = &humidity;

    // humidity.key = "Humidity";
    // humidity.value = &report->hum;
    // humidity.type = EN_OC_MQTT_PROFILE_VALUE_INT;
    // humidity.nxt = &led;

    // led.key = "LightStatus";
    // led.value = g_app_cb.led?"ON":"OFF";
    // led.type = EN_OC_MQTT_PROFILE_VALUE_STRING;
    // led.nxt = &motor;

    // motor.key = "MotorStatus";
    // motor.value = g_app_cb.motor?"ON":"OFF";
    // motor.type = EN_OC_MQTT_PROFILE_VALUE_STRING;
    // motor.nxt = NULL;

    // oc_mqtt_profile_propertyreport(USERNAME,&service);
    // return;
}
app_msg_t *app_msg;
void oc_cmd_rsp_cb(uint8_t *recv_data, size_t recv_size, uint8_t **resp_data, size_t *resp_size)
{
	int ret = 0;
	app_msg = malloc(sizeof(app_msg_t));
	app_msg->msg_type = en_msg_cmd;
	app_msg->msg.cmd.payload = (char *)recv_data;
    printf("recv data is %.*s\n", recv_size, recv_data);
    ret = osMessageQueuePut(mid_MsgQueue,&app_msg,0U, 0U);
    if(ret != 0){
        free(recv_data);
    }
    *resp_data = NULL;
    *resp_size = 0;
}

static int network_main_entry( void )
{
	WifiConnect(WIFI_SSID,WIFI_PASSWORD);
	device_info_init(CLIENT_ID,USERNAME,PASSWORD);
	oc_mqtt_init();
	oc_set_cmd_rsp_cb(oc_cmd_rsp_cb);

    while(1){
        if(is_connected){
            update_date();
        }
        app_msg = NULL;
        (void)osMessageQueueGet(mid_MsgQueue,(void **)&app_msg,NULL, 0U);
        if(NULL != app_msg){
            switch(app_msg->msg_type){
                case en_msg_cmd:
                    deal_cmd_msg(&app_msg->msg.cmd);
                    break;
                case en_msg_report:
                    deal_report_msg(&app_msg->msg.report);
                    break;
                default:
                    break;
            }
            free(app_msg);
        }
    }
    return 0;
}

void *NetworkMainTask(void * para)
{
    network_main_entry();
    return NULL;
}
void parse_timet(int *ay,int *am,int *ad,char* time,struct tm* timeP){
    
}
hi_void motor_gpio_io_init(void)
{
    hi_u32 ret;

    /*gpio5按键控制电机速度*/
    ret = hi_io_set_func(HI_IO_NAME_GPIO_5, HI_IO_FUNC_GPIO_5_GPIO);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_io_set_func ret:%d\r\n", ret);
        return;
    }
    printf("----- gpio5 fan set func success-----\r\n");

    ret = hi_gpio_set_dir(HI_GPIO_IDX_5, HI_GPIO_DIR_IN);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_gpio_set_dir1 ret:%d\r\n", ret);
        return;
    }
    printf("----- gpio set dir success! -----\r\n");

    /*gpio6电机模块led*/
    ret = hi_io_set_func(HI_IO_NAME_GPIO_6, HI_IO_FUNC_GPIO_6_GPIO);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_io_set_func ret:%d\r\n", ret);
        return;
    }
    printf("----- io set func success-----\r\n");

    ret = hi_gpio_set_dir(HI_GPIO_IDX_6, HI_GPIO_DIR_OUT);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_gpio_set_dir1 ret:%d\r\n", ret);
        return;
    }
    
    /*gpio7 电机模块红外传感*/
    ret = hi_io_set_func(HI_IO_NAME_GPIO_7, HI_IO_FUNC_GPIO_7_GPIO);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_io_set_func ret:%d\r\n", ret);
        return;
    }
    printf("----- io set func success-----\r\n");

    ret = hi_gpio_set_dir(HI_GPIO_IDX_7, HI_GPIO_DIR_IN);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_gpio_set_dir1 ret:%d\r\n", ret);
        return;
    }
    printf("----- gpio set dir success! -----\r\n");

    ret = hi_io_set_func(HI_IO_NAME_GPIO_8, HI_IO_FUNC_GPIO_8_GPIO);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_io_set_func ret:%d\r\n", ret);
        return;
    }
    printf("----- io set func success-----\r\n");

    ret = hi_gpio_set_dir(HI_GPIO_IDX_8, HI_GPIO_DIR_OUT);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_gpio_set_dir1 ret:%d\r\n", ret);
        return;
    }
    printf("----- gpio set dir success! -----\r\n");

    ret = hi_io_set_func(HI_IO_NAME_GPIO_11, HI_IO_FUNC_GPIO_11_GPIO);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_io_set_func ret:%d\r\n", ret);
        return;
    }
    printf("----- io set func success-----\r\n");

    ret = hi_gpio_set_dir(HI_GPIO_IDX_11, HI_GPIO_DIR_OUT);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_gpio_set_dir1 ret:%d\r\n", ret);
        return;
    }
    printf("----- gpio set dir success! -----\r\n");

    ret = hi_io_set_func(HI_IO_NAME_GPIO_12, HI_IO_FUNC_GPIO_12_GPIO);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_io_set_func ret:%d\r\n", ret);
        return;
    }
    printf("----- io set func success-----\r\n");

    ret = hi_gpio_set_dir(HI_GPIO_IDX_12, HI_GPIO_DIR_OUT);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_gpio_set_dir1 ret:%d\r\n", ret);
        return;
    }
    printf("----- gpio set dir success! -----\r\n");
}

int get_info(int from, int deflt){
    hi_u32 ret;
    hi_gpio_value val = deflt;
    ret = hi_gpio_get_input_val(from, &val);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_gpio_get_input_val ret:%d\r\n", ret);
        return -1;
    }
    return val;
}
hi_void exec_fan(int mode){
    if(mode == 0){
        if(Infrared_info){ //距离
            RPM = auto_fan_strength;
            IoTPwmStart(motor_pwm_controller, auto_fan_strength, freq);
            control_fan_led(1);
        }
        else{
            RPM = 0;
            IoTPwmStop(motor_pwm_controller);
            control_fan_led(0);
        }
    }
    else if(mode == 1){
        if(Temperature > temp_threshold || Humidity > humi_threshold){//温湿度
            RPM = auto_fan_strength;
            IoTPwmStart(motor_pwm_controller, auto_fan_strength, freq);
            control_fan_led(1);
        }
        else{
            RPM = 0;
            IoTPwmStop(motor_pwm_controller);
            control_fan_led(0);
        }
        
    }
    else if(mode == 2){ 
        if(RPM != 0){ //手动
            IoTPwmStart(motor_pwm_controller, RPM, freq);
            control_fan_led(1);
        }
        else{
            RPM = 0;
            IoTPwmStop(motor_pwm_controller);
            control_fan_led(0);
        }
    }
    else{
        printf("mode error %d\r\n",mode);
    }
}
char* exec_udp(char* message){
    //1 马达
    //2 LED
    //3 temp thr
    //4 humi thr
    //5 auto strg
    //6 mode
    int length = strlen(message);
    int whichdata = message[0] - '0';
    int value;
    if(message[3]>='0'&&message[3]<='9'){
        value = (message[2]-'0')*10+(message[3]-'0');
    }
    else{
        value = message[2]-'0';
    }
    printf("%d %d udp \r\n",whichdata, value);
    if(whichdata == 1){
        RPM = value;
        mode = 2;
        if(value > 0){
            IoTPwmStart(motor_pwm_controller, value, freq);
            printf("MADAR START\r\n");
            return "MADAR START\r\n";
        }
        else{
            IoTPwmStop(motor_pwm_controller);
            printf("MADAR STOP\r\n");
            return "MADAR STOP\r\n";
        }
    }
    else if(whichdata == 2){
        if(value == 1){
            locker_lan_led = -1;
            control_fan_led(1);
            locker_lan_led = 1;
            printf("LED ON\r\n");
            return "LED ON\r\n";
        }
        else if(value == 0){
            locker_lan_led = -1;
            control_fan_led(0);
            locker_lan_led = 0;
            printf("LED OFF\r\n");
            return "LED OFF\r\n";
        }
        else if(value == 2){
            locker_lan_led = -1;
            control_fan_led(0);
            printf("DEFAULT\r\n");
            return "DEFAULT\r\n";
        }
        else{
            printf("MODE UNKNOWN\r\n");
            return "MODE UNKNOWN\r\n";
        }
    }
    else if(whichdata == 3){
        temp_threshold = value;
        printf("New temp_threshold %d\r\n", temp_threshold);
        return "New temp_threshold setting over\r\n";
    }
    else if(whichdata == 4){
        humi_threshold = value;
        printf("New humi_threshold %d\r\n", humi_threshold);
        return "New temp_threshold setting over\r\n";
    }
    else if(whichdata == 5){
        auto_fan_strength = value;
        printf("New auto_fan_strength %d\r\n", auto_fan_strength);
        return "New temp_threshold setting over\r\n";
    }
    else if(whichdata == 6){
        mode = value;
        printf("New mode %d\r\n", mode);
        return "New mode setting over\r\n";
    }
}
int rpmi = 1;
hi_void manage_motor(hi_void){
    exec_fan(mode);
    if(mode == 2){
        if(Infrared_info&&RPM!=0){ 
            rpmi++;
            if(rpmi % 10000 == 0){
                RPM = RPM + 1;
                rpmi ++;
            }
            if(RPM >= 99){
                RPM = 0;
            }
        }
    }
    if(get_info(HI_GPIO_IDX_5,HI_GPIO_VALUE1) == 0){
        if(mode == 0){
            mode = 1;
            RPM = 0;
        }
        else if(mode == 1){
            mode = 2;
            RPM = 0;
        }
        else{
            if(RPM == 0){
                RPM = 1;
            }
            else if(RPM >= 80){
                mode = 0;
            }
            else{
                RPM = RPM + 20;
            }
        }
        exec_fan(mode);
        sleep(1);
    }
}
void update_date(char* time){
    int ay=2023,am=7,ad=8;
    time_t PTime = 0;
    struct tm* timeP;
    PTime += (8*60*60);
    parse_timet(&ay,&am,&ad,time,timeP);
    year = ay;
    month = am;
    day = ad;
}
void *UDPTask(void* para){
	//在sock_fd 进行监听
    int sock_fd;
    //服务端地址信息
	struct sockaddr_in server_sock;

	if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{
		perror("socket is error.\r\n");
	}
	bzero(&server_sock, sizeof(server_sock));
	server_sock.sin_family = AF_INET;
	server_sock.sin_addr.s_addr = htonl(INADDR_ANY);
	server_sock.sin_port = htons(8888);
	if (bind(sock_fd, (struct sockaddr *)&server_sock, sizeof(struct sockaddr)) == -1)
	{
		perror("bind is error.\r\n");
	}
    int ret;
    char recvBuf[512] = {0};
    char sendBuf[512] = "\x1b\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
    struct sockaddr_in client_addr;
    int size_client_addr= sizeof(struct sockaddr_in);
    while (1)
    {
        printf("Waiting to receive data...\r\n");
        memset(recvBuf, 0, sizeof(recvBuf));
        ret = recvfrom(sock_fd, recvBuf, sizeof(recvBuf), 0, (struct sockaddr*)&client_addr,(socklen_t*)&size_client_addr);
        if(ret < 0)
        {
            printf("UDP server receive failed!\r\n");
        }
        printf("receive %d bytes of data from ipaddr = %s, port = %d.\r\n", ret, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        printf("data is %s\r\n",recvBuf);
        //发回==================
        ret = sendto(sock_fd, exec_udp(recvBuf), strlen(recvBuf), 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
        if (ret < 0)
        {
            printf("UDP server send failed!\r\n");
        }
        //=====================




        // =======================
        memset(recvBuf, 0, sizeof(recvBuf));
        client_addr.sin_port = htons(123);
        client_addr.sin_family = AF_INET;
        client_addr.sin_addr.s_addr = inet_addr("210.72.145.44");
        ret = sendto(sock_fd, sendBuf, strlen(sendBuf), 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
        if (ret < 0){
            printf("Time server send failed!\r\n");
        }
        memset(recvBuf, 0, sizeof(recvBuf));
        ret = recvfrom(sock_fd, recvBuf, sizeof(recvBuf), 0, (struct sockaddr*)&client_addr,(socklen_t*)&size_client_addr);
    }
    /*********************END********************/
}


hi_void motor_demo(hi_void)
{
    int ret;
    motor_gpio_io_init();
    SHT3X_init();
    // motor_pwm_init();
    IoTGpioInit(motorio);
    (void)hi_io_set_func(motorio,pwm_motor_bridge);
    (void)hi_gpio_set_dir(motorio,HI_GPIO_DIR_OUT);
    IoTPwmInit(HI_PWM_PORT_PWM2);
    (void)hi_io_set_func(FAN_LED,FAN_LED_GPIO);
    (void)hi_gpio_set_dir(FAN_LED,HI_GPIO_DIR_OUT);
    hi_spi_deinit(HI_SPI_ID_0);
    screen_spi_master_init(0);

    ret = hi_task_create(&g_MonitorTask, // task标识 //
        &MonitorTaskAttr,
        MonitorOledTask, // task处理函数 //
        NULL); // task处理函数参数 //

    if (ret < 0) {
        printf("Create monitor oled task failed [%d]\r\n", ret);
        return;
    }
   
    ret = hi_task_create(&g_MonitorTask, // task标识 //
        &MonitorTaskAttr,
        MonitorMotorTask, // task处理函数 //
        NULL); // task处理函数参数 //

    if (ret < 0) {
        printf("Create monitor motor task failed [%d]\r\n", ret);
        return;
    }   

    ret = hi_task_create(&g_MonitorTask, // task标识 //
        &MonitorTaskAttr,
        MonitorShtTask, // task处理函数 //
        NULL); // task处理函数参数 //

    if (ret < 0) {
        printf("Create monitor motor task failed [%d]\r\n", ret);
        return;
    }

    ret = hi_task_create(&g_MonitorTask, // task标识 //
        &MonitorTaskAttr,
        NetworkMainTask, // task处理函数 //
        NULL); // task处理函数参数 //

    if (ret < 0) {
        printf("Create network main task failed [%d]\r\n", ret);
        return;
    }     

    ret = hi_task_create(&g_MonitorTask, // task标识 //
        &MonitorTaskAttr,
        UDPTask, // task处理函数 //
        NULL); // task处理函数参数 //

    if (ret < 0) {
        printf("Create socket main task failed [%d]\r\n", ret);
        return;
    }     
    
    return;
}
APP_FEATURE_INIT(motor_demo);
