#include <stdio.h>
#include "oled.h"
#include "gui.h"
#include "test.h"
void test_led_screen(void)
{	
    OLED_Init();
    OLED_Clear(0); 
    char mystr1[16] = {0};
	char mystr2[16] = {0};
	char mystr3[16] = {0};
	char mystr4[16] = {0};
	char mydate[16] = {0};
	char RU_stat[50] = {0};
    
    OLED_Clear(0); 
    GUI_DrawLine(0, 10, WIDTH-1, 10,1);
    GUI_DrawLine(WIDTH/2-1,11,WIDTH/2-1,HEIGHT-1,1);
    GUI_DrawLine(0,10+(HEIGHT-10)/2-1,WIDTH-1,10+(HEIGHT-10)/2-1,1);
    GUI_ShowString(0,39,"FAN_MODE",8,1);
    GUI_ShowString(0,13,"STH",8,1);
    GUI_ShowString(WIDTH/2+1,13,"TEMP",8,1);
    GUI_ShowString(WIDTH/2+1,39,"HUMI",8,1);
    while(1) 
    {	
        TEST_Menu2(mystr1,mystr2,mystr3,mystr4,mydate,RU_stat); 
    }
}

void display_hr(void)
{	
    OLED_Init();		
    OLED_Clear(0);  
    
    while(1) 
    {	
        TEST_CURVE();
        sleep(1);
        OLED_Clear(0); 
    }
}
