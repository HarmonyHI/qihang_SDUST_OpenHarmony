#include <stdio.h>
#include <unistd.h>
#include <dbg.h>
#include "sht3x_i2c.h"
#include "hi_timer.h"
#include "hi_gpio.h"
#include "hi_pwm.h"
#include "hi_spi.h"
#include "ohos_init.h"
#include "cmsis_os2.h"
#include <hi_types_base.h>
#include <hi_io.h>
#include <hi_early_debug.h>
#include <hi_gpio.h>
#include <hi_task.h>
#include <hi_i2c.h>

float Temperature = 0;
float Humidity = 0;
int RPM = 0;
hi_gpio_value Infrared_info = 0;
int judge_temp;
int norm_temp = 512;
int norm_hp = 512;
void dump_buf(unsigned char * buf,unsigned int len)
{
    if(buf == NULL)
        return ;

    DBG("in buf : \r\n");
    for(int i = 0; i < len; i++){
        printf("%#x ",*buf++); 
    }
    printf(" \r\n");
}

static unsigned char SHT3X_CalcCrc(unsigned char data[], unsigned int nbrOfBytes) 
{
    unsigned char bit; // bit mask
    unsigned char crc = 0xFF; // calculated checksum
    unsigned char byteCtr; // byte counter

    // calculates 8-Bit checksum with given polynomial
    for(byteCtr = 0; byteCtr < nbrOfBytes; byteCtr++)
    {
        crc ^= (data[byteCtr]);
        for(bit = 8; bit > 0; --bit)
        {
            if(crc & 0x80) 
                crc = (crc << 1) ^ POLYNOMIAL;
            else 
                crc = (crc << 1);
        }
    }

    return crc; 
}

static etError SHT3X_CheckCrc(unsigned char data[],\
    unsigned int nbrOfBytes, unsigned char checksum) 
{
    unsigned char crc; // calculated checksum

    // calculates 8-Bit checksum
    crc = SHT3X_CalcCrc(data, nbrOfBytes);

    // verify checksum
    if(crc != checksum) 
        return CHECKSUM_ERROR;
    else 
        return NO_ERROR; 
}

int SHT3x_ReadSerialNumber(unsigned int *serialNumber)
{
    int ret = -1;
    unsigned char serialNumWords[6] = {0};
    unsigned char cmd[2] = {0x37,0x80,};

    hi_i2c_data sht3x_i2c_data = { 0 };
    sht3x_i2c_data.send_buf = cmd;
    sht3x_i2c_data.send_len = 2;
    sht3x_i2c_data.receive_buf = serialNumWords;
    sht3x_i2c_data.receive_len = sizeof(serialNumWords);
    
    ret = hi_i2c_write(0, ((unsigned char)0x44) << 1, &sht3x_i2c_data);
    if(ret != 0){
        DBG("hi_i2c_write failed ret :%#x \r\n",ret);
        return ret;
    }
    //dump_buf(serialNumWords,sizeof(serialNumWords));
    
    ret = hi_i2c_read(0, ((unsigned char)0x44) << 1 | 0x01, &sht3x_i2c_data);
    if(ret != 0){
        DBG("hi_i2c_read failed ret :%#x \r\n",ret);
        return ret;
    }
    //dump_buf(serialNumWords,sizeof(serialNumWords));

    ret = SHT3X_CheckCrc(serialNumWords,2,serialNumWords[2]);
    if(ret != NO_ERROR){
        DBG("read serial number crc check failed \r\n"); 
        return ret;
    }

    ret = SHT3X_CheckCrc(&serialNumWords[3],2,serialNumWords[5]);
    if(ret != NO_ERROR){
        DBG("read serial number crc check failed \r\n"); 
        return ret;
    }
    return ret;
}

int SHT3x_WriteCMD(unsigned short cmd)
{
    int ret = -1;
    unsigned char sendbuf[2] = {0};
    unsigned char rcvbuf[2] = {0};
    
    hi_i2c_data sht3x_i2c_data = { 0 };
    sht3x_i2c_data.send_buf = sendbuf;
    sht3x_i2c_data.send_len = sizeof(sendbuf);
    sht3x_i2c_data.receive_buf = rcvbuf;
    sht3x_i2c_data.receive_len = sizeof(rcvbuf);
    
    sendbuf[0] = (cmd & 0xff00) >> 8;
    sendbuf[1] = cmd & 0xff;
    //dump_buf(sendbuf,2); 
    ret = hi_i2c_write(0, ((unsigned char)0x44) << 1, &sht3x_i2c_data);
    if(ret != 0){
        DBG("hi_i2c_write failed ret :%#x \r\n",ret);
        return ret;
    }
    return 0;        
}

int SHT3x_Read4BytesDataAndCrc(unsigned short *data)
{
    int ret = -1;
    unsigned char sendbuf[2] = {0};
    unsigned char rcvbuf[6] = {0};
    
    hi_i2c_data sht3x_i2c_data = { 0 };
    sht3x_i2c_data.send_buf = sendbuf;
    sht3x_i2c_data.send_len = sizeof(sendbuf);
    sht3x_i2c_data.receive_buf = rcvbuf;
    sht3x_i2c_data.receive_len = sizeof(rcvbuf);

    if(data == NULL){
        DBG("invalid para \r\n");
        return ret;
    }

    ret = hi_i2c_read(0, ((unsigned char)0x44) << 1 | 0x01, &sht3x_i2c_data);
    if(ret != 0){
        DBG("hi_i2c_read failed ret :%#x \r\n",ret);
        return ret;
    }
    
    ret = SHT3X_CheckCrc(rcvbuf,2,rcvbuf[2]);
    if(ret != NO_ERROR){
        DBG("read serial number crc check failed \r\n"); 
        return ret;
    }
    
    ret = SHT3X_CheckCrc(&rcvbuf[3],2,rcvbuf[5]);
    if(ret != NO_ERROR){
        DBG("read serial number crc check failed \r\n"); 
        return ret;
    }

    data[0] = rcvbuf[0] << 8 | rcvbuf[1];
    data[1] = rcvbuf[3] << 8 | rcvbuf[4];

    return 0;        
}

int SHT3x_Read2BytesDataAndCrc(unsigned short *data)
{
    int ret = -1;
    unsigned char sendbuf[2] = {0};
    unsigned char rcvbuf[3] = {0};
    
    hi_i2c_data sht3x_i2c_data = { 0 };
    sht3x_i2c_data.send_buf = sendbuf;
    sht3x_i2c_data.send_len = sizeof(sendbuf);
    sht3x_i2c_data.receive_buf = rcvbuf;
    sht3x_i2c_data.receive_len = sizeof(rcvbuf);
    
    if(data == NULL){
        DBG("invalid para \r\n");
        return ret;
    }
   
    ret = hi_i2c_read(0, ((unsigned char)0x44) << 1 | 0x01, &sht3x_i2c_data);
    if(ret != 0){
        DBG("hi_i2c_read failed ret :%#x \r\n",ret);
        return ret;
    }
    
    ret = SHT3X_CheckCrc(rcvbuf,2,rcvbuf[2]);
    if(ret != NO_ERROR){
        DBG("read serial number crc check failed \r\n"); 
        return ret;
    }

    data[0] = rcvbuf[0] << 8 | rcvbuf[1];
    return 0;        
}

void SHT3X_SoftReset(void)
{
    SHT3x_WriteCMD(CMD_SOFT_RESET);
}

void SHT3X_ReadSerialNumber(void) 
{
    unsigned short data[2] = {0};
    SHT3X_SoftReset();
    SHT3x_WriteCMD(CMD_READ_SERIALNBR);

    SHT3x_Read4BytesDataAndCrc(data);
    printf("--------------------------------\r\n");
    //dump_buf((unsigned char *)data,sizeof(data));
}

void SHT3X_ReadStatus(unsigned short* status) 
{
    SHT3x_WriteCMD(CMD_READ_STATUS);
    SHT3x_Read2BytesDataAndCrc(status);
}

void SHT3X_ClearAllAlertFlags(void)
{
    SHT3x_WriteCMD(CMD_CLEAR_STATUS);
}

void SHT3X_StartPeriodicMeasurment(void)
{
    // medium repeatability, 2.0 Hz
    SHT3x_WriteCMD(CMD_MEAS_PERI_2_M);
}

static float SHT3X_CalcTemperature(unsigned short rawValue) 
{
    return 175.0f * (float)rawValue / 65535.0f - 45.0f; 
}

static float SHT3X_CalcHumidity(unsigned short rawValue) 
{
    return 100.0f * (float)rawValue / 65535.0f; 
}

void SHT3X_init(void)
{
    int ret = 0;
    unsigned short data[2] = {0};
    SHT3X_SoftReset();
    SHT3x_WriteCMD(CMD_READ_SERIALNBR);
    
    SHT3x_WriteCMD(CMD_MEAS_PERI_2_M);
    //SHT3X_StartPeriodicMeasurment();

}

void SHT3X_ReadMeasurementBuffer(float* temperature, float* humidity) 
{

    unsigned int rawValueTemp = 0; 
    
    SHT3x_WriteCMD(CMD_FETCH_DATA);
    SHT3x_Read4BytesDataAndCrc((unsigned short *)&rawValueTemp);

    //dump_buf((unsigned char *)&rawValueTemp,sizeof(rawValueTemp));    

    *temperature = SHT3X_CalcTemperature(rawValueTemp);
    *humidity = SHT3X_CalcHumidity(*((unsigned short *)(&rawValueTemp)+1));
    Temperature = *temperature;
    Humidity = *humidity;
    if(norm_temp == 512){
        if(Temperature != 0){
            norm_temp = Temperature;
        }
    }
    else{
        judge_temp = norm_temp - Temperature;
        if(judge_temp>=20||judge_temp<=-20){
            Temperature = norm_temp;
        }
        else{
            norm_temp = Temperature;
        }
    }
    if(norm_hp == 512){
        if(Humidity != 0){
            norm_hp = Humidity;
        }
    }
    else{
        if(Humidity == 0){
            Humidity = norm_hp;
        }
        else{
            norm_hp = Humidity;
        }
    }
    // printf("temp :%.2f,hum :%.2f \r\n",Temperature,Humidity);
}
hi_gpio_value get_infrared(){
    hi_u32 ret;
    hi_gpio_value val = HI_GPIO_VALUE0;
    ret = hi_gpio_get_input_val(HI_GPIO_IDX_7, &val);
    if (ret != HI_ERR_SUCCESS) {
        printf("===== ERROR ===== gpio -> hi_gpio_get_input_val ret:%d\r\n", ret);
        return -1;
    }
    return val;
}
void SHT3X_ReadMeasurementVal(unsigned int para)
{
    (void) para;
    static int cunt = 0;
    static float humidity = 0.0; 
    static float temperature = 0.0; 
    Infrared_info = get_infrared();
    SHT3X_ReadMeasurementBuffer(&temperature,&humidity);
    
}
