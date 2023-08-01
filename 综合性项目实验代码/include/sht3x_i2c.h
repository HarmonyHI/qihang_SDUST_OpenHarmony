#ifndef __SHT3X_I2C_H__
#define __SHT3X_I2C_H__

#include <hi_types_base.h>
#include <hi_i2c.h>
#include <hi_early_debug.h>
#include <hi_stdlib.h>

#define I2C_REG_ARRAY_LEN 64
#define SHT3X_DEV_ADDR 0x80          /* 11000 0 */
#define ES8311_REG_ADDR 0x10
#define I2C_SEND_LEN_2  2
#define I2C_RECV_LEN_1  1

#define POLYNOMIAL 0x131 // P(x) = x^8 + x^5 + x^4 + 1 = 100110001

typedef enum{
 NO_ERROR = 0x00, // no error
 ACK_ERROR = 0x01, // no acknowledgment error
 CHECKSUM_ERROR = 0x02, // checksum mismatch error
 TIMEOUT_ERROR = 0x04, // timeout error
 PARM_ERROR = 0x80, // parameter out of range error
}etError;

typedef enum{
    CMD_READ_SERIALNBR = 0x3780, // read serial number
    CMD_READ_STATUS = 0xF32D, // read status register
    CMD_CLEAR_STATUS = 0x3041, // clear status register
    CMD_HEATER_ENABLE = 0x306D, // enabled heater
    CMD_HEATER_DISABLE = 0x3066, // disable heater
    CMD_SOFT_RESET = 0x30A2, // soft reset
    CMD_MEAS_CLOCKSTR_H = 0x2C06, // measurement: clock stretching, high repeatability
    CMD_MEAS_CLOCKSTR_M = 0x2C0D, // measurement: clock stretching, medium repeatability
    CMD_MEAS_CLOCKSTR_L = 0x2C10, // measurement: clock stretching, low repeatability
    CMD_MEAS_POLLING_H = 0x2400, // measurement: polling, high repeatability
    CMD_MEAS_POLLING_M = 0x240B, // measurement: polling, medium repeatability
    CMD_MEAS_POLLING_L = 0x2416, // measurement: polling, low repeatability
    CMD_MEAS_PERI_05_H = 0x2032, // measurement: periodic 0.5 mps, high repeatability
    CMD_MEAS_PERI_05_M = 0x2024, // measurement: periodic 0.5 mps, medium repeatability
    CMD_MEAS_PERI_05_L = 0x202F, // measurement: periodic 0.5 mps, low repeatability
    CMD_MEAS_PERI_1_H = 0x2130, // measurement: periodic 1 mps, high repeatability
    CMD_MEAS_PERI_1_M = 0x2126, // measurement: periodic 1 mps, medium repeatability
    CMD_MEAS_PERI_1_L = 0x212D, // measurement: periodic 1 mps, low repeatability
    CMD_MEAS_PERI_2_H = 0x2236, // measurement: periodic 2 mps, high repeatability
    CMD_MEAS_PERI_2_M = 0x2220, // measurement: periodic 2 mps, medium repeatability
    CMD_MEAS_PERI_2_L = 0x222B, // measurement: periodic 2 mps, low repeatability
    CMD_MEAS_PERI_4_H = 0x2334, // measurement: periodic 4 mps, high repeatability
    CMD_MEAS_PERI_4_M = 0x2322, // measurement: periodic 4 mps, medium repeatability
    CMD_MEAS_PERI_4_L = 0x2329, // measurement: periodic 4 mps, low repeatability
    CMD_MEAS_PERI_10_H = 0x2737, // measurement: periodic 10 mps, high repeatability
    CMD_MEAS_PERI_10_M = 0x2721, // measurement: periodic 10 mps, medium repeatability
    CMD_MEAS_PERI_10_L = 0x272A, // measurement: periodic 10 mps, low repeatability
    CMD_FETCH_DATA = 0xE000, // readout measurements for periodic mode
    CMD_R_AL_LIM_LS = 0xE102, // read alert limits, low set
    CMD_R_AL_LIM_LC = 0xE109, // read alert limits, low clear
    CMD_R_AL_LIM_HS = 0xE11F, // read alert limits, high set
    CMD_R_AL_LIM_HC = 0xE114, // read alert limits, high clear
    CMD_W_AL_LIM_HS = 0x611D, // write alert limits, high set
    CMD_W_AL_LIM_HC = 0x6116, // write alert limits, high clear
    CMD_W_AL_LIM_LC = 0x610B, // write alert limits, low clear
    CMD_W_AL_LIM_LS = 0x6100, // write alert limits, low set
    CMD_NO_SLEEP = 0x303E, 
}etCommands;

#define LITTLE_ENDIAN 

typedef union {
    unsigned short u16;
    struct{
#ifdef LITTLE_ENDIAN // bit-order is little endian
        unsigned short CrcStatus : 1; // write data checksum status
        unsigned short CmdStatus : 1; // command status
        unsigned short Reserve0 : 2; // reserved
        unsigned short ResetDetected : 1; // system reset detected
        unsigned short Reserve1 : 5; // reserved
        unsigned short T_Alert : 1; // temperature tracking alert
        unsigned short RH_Alert : 1; // humidity tracking alert
        unsigned short Reserve2 : 1; // reserved
        unsigned short HeaterStatus : 1; // heater status
        unsigned short Reserve3 : 1; // reserved
        unsigned short AlertPending : 1; // alert pending status 
#else // bit-order is big endian
        unsigned short AlertPending : 1;
        unsigned short Reserve3 : 1;
        unsigned short HeaterStatus : 1;
        unsigned short Reserve2 : 1;
        unsigned short RH_Alert : 1;
        unsigned short T_Alert : 1;
        unsigned short Reserve1 : 5;
        unsigned short ResetDetected : 1;
        unsigned short Reserve0 : 2;
        unsigned short CmdStatus : 1;
        unsigned short CrcStatus : 1;
#endif
    }bit; 
} regStatus;
void SHT3X_SoftReset(void);
void SHT3X_init(void);
void SHT3X_ReadSerialNumber(void);
void SHT3X_StartPeriodicMeasurment(void);
void SHT3X_ReadMeasurementBuffer(float* temperature, float* humidity);
#endif
