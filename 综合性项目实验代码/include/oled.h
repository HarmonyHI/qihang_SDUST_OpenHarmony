#ifndef __OLED_H
#define __OLED_H			  	 
#include "base_type.h"

#include <hi_types_base.h>
#include <hi_io.h>
#include <hi_early_debug.h>
#include <hi_gpio.h>
#include "hi_spi.h"
//--------------OLED��������---------------------
#define PAGE_SIZE    8
#define XLevelL		0x02 //1.3��XLevelLΪ0x02 0.9��XLevelLΪ0x00
#define XLevelH		0x10
#define YLevel       0xB0
#define	Brightness	 0xFF 
#define WIDTH 	     128
#define HEIGHT 	     64	

//-------------д��������ݶ���-------------------
#define OLED_CMD     0	//д����
#define OLED_DATA    1	//д���� 
   						  
//-----------------OLED�˿ڶ���----------------
#define OLED_CS   GPIO_Pin_11   //Ƭѡ�ź�           PB11
#define OLED_DC   GPIO_Pin_10   //����/��������ź�  PB10
#define OLED_RST  GPIO_Pin_12   //��λ�ź�           PB12

//-----------------OLED�˿ڲ�������---------------- 
/*
#define OLED_CS_Clr()  GPIO_ResetBits(GPIOB,OLED_CS)
#define OLED_CS_Set()  GPIO_SetBits(GPIOB,OLED_CS)

#define OLED_DC_Clr()  GPIO_ResetBits(GPIOB,OLED_DC)
#define OLED_DC_Set()  GPIO_SetBits(GPIOB,OLED_DC)
 					   
#define OLED_RST_Clr()  GPIO_ResetBits(GPIOB,OLED_RST)
#define OLED_RST_Set()  GPIO_SetBits(GPIOB,OLED_RST)
*/

#define OLED_CS_Clr() hi_gpio_set_ouput_val(HI_GPIO_IDX_12,HI_GPIO_VALUE0);
#define OLED_CS_Set() hi_gpio_set_ouput_val(HI_GPIO_IDX_12,HI_GPIO_VALUE1);

#define OLED_DC_Clr()  hi_gpio_set_ouput_val(HI_GPIO_IDX_11,HI_GPIO_VALUE0);
#define OLED_DC_Set()  hi_gpio_set_ouput_val(HI_GPIO_IDX_11,HI_GPIO_VALUE1);
 					  
#define OLED_RST_Clr()  hi_gpio_set_ouput_val(HI_GPIO_IDX_8,HI_GPIO_VALUE0);
#define OLED_RST_Set()  hi_gpio_set_ouput_val(HI_GPIO_IDX_8,HI_GPIO_VALUE1);

#define SPI_WriteByte(SPI2,dat) spi_write_data(0,dat,1);

//OLED�����ú���
void OLED_WR_Byte(unsigned dat,unsigned cmd);     							   		    
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_Reset(void);
void OLED_Init_GPIO(void);	   							   		    
void OLED_Init(void);
void OLED_Set_Pixel(unsigned char x, unsigned char y,unsigned char color);
void OLED_Display(void);
void OLED_Clear(unsigned dat);  
#endif
