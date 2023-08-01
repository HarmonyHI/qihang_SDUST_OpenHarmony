#include <stdio.h>
#include <stdlib.h>
#include "oled.h"
#include "gui.h"
#include "test.h"
#include "bmp.h"
#include "base_type.h"

/*****************************************************************************
 * @name       :void TEST_MainPage(void)
 * @date       :2018-08-27 
 * @function   :Drawing the main Interface of the Comprehensive Test Program
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
void TEST_MainPage(void)
{	
	GUI_ShowString(28,0,"OLED TEST",16,1);
	GUI_ShowString(12,16,"0.96\" SSD1306",16,1);
	GUI_ShowString(40,32,"64X128",16,1);
	GUI_ShowString(4,48,"www.lcdwiki.com",16,1);
	delay_ms(1500);	
	delay_ms(1500);
}

/*****************************************************************************
 * @name       :void Test_Color(void)
 * @date       :2018-08-27 
 * @function   :Color fill test(white,black)
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
void Test_Color(void)
{
	 GUI_Fill(0,0,WIDTH-1,HEIGHT-1,0);
	 GUI_ShowString(10,10,"BLACK",16,1);
	 delay_ms(1000);	
	 GUI_Fill(0,0,WIDTH-1,HEIGHT-1,1);
	 delay_ms(1500);
}

/*****************************************************************************
 * @name       :void Test_Rectangular(void))
 * @date       :2018-08-27
 * @function   :Rectangular display and fill test
								Display black,white rectangular boxes in turn,1000 
								milliseconds later,Fill the rectangle in black,white in turn
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
void Test_Rectangular(void)
{
	GUI_Fill(0,0,WIDTH/2-1,HEIGHT-1,0);
	GUI_Fill(WIDTH/2,0,WIDTH-1,HEIGHT-1,1);
	GUI_DrawRectangle(5, 5, WIDTH/2-1-5, HEIGHT-1-5,1);
	GUI_DrawRectangle(WIDTH/2-1+5, 5, WIDTH-1-5, HEIGHT-1-5,0);
	GUI_FillRectangle(5, 5, WIDTH/2-1-5, HEIGHT-1-5,1);
	GUI_FillRectangle(WIDTH/2-1+5, 5, WIDTH-1-5, HEIGHT-1-5,0);
}


/*****************************************************************************
 * @name       :void Test_Circle(void)
 * @date       :2018-08-27 
 * @function   :circular display and fill test
								Display black,white circular boxes in turn,1000 
								milliseconds later,Fill the circular in black,white in turn
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
void Test_Circle(void)
{
	GUI_Fill(0,0,WIDTH/2-1,HEIGHT-1,0);
	GUI_Fill(WIDTH/2,0,WIDTH-1,HEIGHT-1,1);
	GUI_DrawCircle(WIDTH/2/2-1, HEIGHT/2-1, 1, 27);
	GUI_DrawCircle(WIDTH/2+WIDTH/2/2-1, HEIGHT/2-1, 0, 27);
	GUI_FillCircle(WIDTH/2/2-1, HEIGHT/2-1, 1, 27);
	GUI_FillCircle(WIDTH/2+WIDTH/2/2-1, HEIGHT/2-1, 0, 27);
}

/*****************************************************************************
 * @name       :void Test_Triangle(void)
 * @date       :2018-08-27 
 * @function   :triangle display and fill test
								Display black,white triangle boxes in turn,1000 
								milliseconds later,Fill the triangle in black,white in turn
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
void Test_Triangle(void)
{
	GUI_Fill(0,0,WIDTH/2-1,HEIGHT-1,0);
	GUI_Fill(WIDTH/2,0,WIDTH-1,HEIGHT-1,1);
	GUI_DrawTriangel(5,HEIGHT-1-5,WIDTH/2/2-1,5,WIDTH/2-1-5,HEIGHT-1-5,1);
	GUI_DrawTriangel(WIDTH/2-1+5,HEIGHT-1-5,WIDTH/2+WIDTH/2/2-1,5,WIDTH-1-5,HEIGHT-1-5,0);
	delay_ms(1000);
	GUI_FillTriangel(5,HEIGHT-1-5,WIDTH/2/2-1,5,WIDTH/2-1-5,HEIGHT-1-5,1);
	GUI_FillTriangel(WIDTH/2-1+5,HEIGHT-1-5,WIDTH/2+WIDTH/2/2-1,5,WIDTH-1-5,HEIGHT-1-5,0);
	delay_ms(1500);
}


/*****************************************************************************
 * @name       :void TEST_English(void)
 * @date       :2018-08-27 
 * @function   :English display test
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
void TEST_English(void)
{
	GUI_ShowString(0,5,"6x8:abcdefghijklmnopqrstuvwxyz",8,1);
	GUI_ShowString(0,25,"8x16:abcdefghijklmnopqrstuvwxyz",16,1);
	delay_ms(1000);
	GUI_ShowString(0,5,"6x8:ABCDEFGHIJKLMNOPQRSTUVWXYZ",8,1);
	GUI_ShowString(0,25,"8x16:ABCDEFGHIJKLMNOPQRSTUVWXYZ",16,1);
	delay_ms(1500);
}

/*****************************************************************************
 * @name       :void TEST_Number_Character(void) 
 * @date       :2018-08-27 
 * @function   :number and character display test
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
void TEST_Number_Character(void) 
{
	GUI_Fill(0,0,WIDTH-1,HEIGHT/2-1,0);
	GUI_ShowString(0,0,"6x8:!\"#$%&'()*+,-./:;<=>?@[]\\^_`~{}|",8,1);
	GUI_ShowNum(30,16,1234567890,10,8,1);
	delay_ms(1000);
	OLED_Clear(0); 
  GUI_ShowString(0,0,"8x16:!\"#$%&'()*+,-./:;<=>?@[]\\^_`~{}|",16,1);
	GUI_ShowNum(40,32,1234567890,10,16,1);
	delay_ms(1500);
	OLED_Clear(0);
}

/*****************************************************************************
 * @name       :void TEST_Chinese(void)
 * @date       :2018-08-27
 * @function   :chinese display test
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
void TEST_Chinese(void)
{	
	GUI_ShowCHinese(32,26,16,"0000",1);
	GUI_ShowCHinese(8,48,16,"1111111哈哈哈",1);
	delay_ms(1000);
	OLED_Clear(0);
}

/*****************************************************************************
 * @name       :void TEST_BMP(void)
 * @date       :2018-08-27 
 * @function   :BMP monochromatic picture display test
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
void TEST_BMP(void)
{
   /* 
	GUI_DrawBMP(0,0,128,64, BMP2, 1);
	delay_ms(1000);
	GUI_DrawBMP(0,0,128,64, BMP3, 1);
	delay_ms(1000);
	GUI_DrawBMP(0,0,128,64, BMP4, 1);
	delay_ms(1000);
    */
	GUI_DrawBMP(0,0,128,26, gImage_isoftstone3, 1);
    //GUI_DrawBMP(0,-2,128,26, gImage_isoftstone3, 1);
	//GUI_DrawBMP(0,0,128,38, gImage_isoftstone2, 1);
    //delay_ms(1000);
}

/*****************************************************************************
 * @name       :void TEST_Menu1(void)
 * @date       :2018-08-27 
 * @function   :Chinese selection menu display test
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
void TEST_Menu1(void)
{ 
	OLED_Clear(0); 
	GUI_Fill(0,0,WIDTH-1,15,1);
	GUI_DrawCircle(10, 24, 1,6);
	GUI_ShowString(20,48,"C.",16,0);
	sleep(1);
}

/*****************************************************************************
 * @name       :void TEST_Menu2(void)
 * @date       :2018-08-27 
 * @function   :English weather interface display test
 * @parameters :None
 * @retvalue   :None
******************************************************************************/
void TEST_Menu2(char* mystr1,char* mystr2,char* mystr3,char* mystr4,char* mydate,char* RU_stat)
{
#if 1
	extern int year;
	extern int month;
	extern int day;
    extern float Temperature;
    extern float Humidity;
	extern int RPM;
	extern int mode;
	extern hi_gpio_value Infrared_info;
	extern int is_connected;
	extern char out_name[50];
#endif
	sprintf(mydate,"%04d-%02d-%02d",year,month,day);
    GUI_ShowString(0,1,mydate,8,18);
	sprintf(mystr3,"%s",mode==0?"CLOS":mode==1?"AUTO":"ADJU");
	GUI_ShowString(0+8,HEIGHT-16,mystr3,16,1);
	sprintf(RU_stat,"%.4s %.4s",Infrared_info?"CLOSER":"AWAY",is_connected==0?"----":out_name);
	GUI_ShowString(WIDTH/2-1+2+5,1,RU_stat,8,1);
	sprintf(mystr1,"%02d",RPM);
	GUI_ShowString(0+8,20,mystr1,16,1);
	sprintf(mystr2,"%2.1f C",Temperature);
	GUI_ShowString(WIDTH/2+2+8,20,mystr2,16,1);
	sprintf(mystr4,"%02d R",(int)Humidity);
	GUI_ShowString(WIDTH/2+2+8,HEIGHT-16,mystr4,16,1);
	
	// printf("------%s, %s, %s, %s----------\r\n", mystr1,mystr2,mystr3,mystr4);
	// GUI_DrawBMP(6,16,51,32, BMP5, 1);
}

void TEST_LINE(void)
{
	GUI_DrawLine(0,34,30,34,1);
	GUI_DrawLine(96,34,WIDTH-1,34,1);
}

void TEST_CURVE(void)
{
    extern int hr_valid;
    extern int sp02_valid;
    
    int a = 0;
    int b = 0;
    int c = 0;
    int d = 0;
    int e = 0;
    int f = 0;
    
    a = hr_valid / 100;
    b = (hr_valid - a * 100) / 10;
    c = hr_valid % 10;
    d = sp02_valid / 100;
    e = (sp02_valid  - d * 100) / 10;
    f = sp02_valid % 10;

	GUI_ShowString(14,12,"HR:",16,1);
    if(a == 0){
        GUI_ShowNum(40,12,b,1,16,1);
        GUI_ShowNum(48,12,c,1,16,1);
    }else{
        GUI_ShowNum(40,12,a,1,16,1);
        GUI_ShowNum(48,12,b,1,16,1);
        GUI_ShowNum(56,12,c,1,16,1);
    }

	GUI_ShowString(14,38,"SP02:",16,1);
    if(d == 0){
        GUI_ShowNum(56,38,e,1,16,1);
        GUI_ShowNum(64,38,f,1,16,1);
    }else{
        GUI_ShowNum(56,38,d,1,16,1);
        GUI_ShowNum(64,38,e,1,16,1);
        GUI_ShowNum(72,38,f,1,16,1);
    }
}
