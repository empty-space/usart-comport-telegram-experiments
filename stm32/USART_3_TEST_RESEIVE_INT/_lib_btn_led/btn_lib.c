#include "btn_lib.h"
#include "brown_delay.h"




void led_init_out_c13 (void){
   GPIO_InitTypeDef GPIO_InitStructure;
// Enable Peripheral Clocks
//... (1) ...
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE); // (1)
// Configure Pins
//... (2) ...
    GPIO_StructInit (& GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC , &GPIO_InitStructure); 
}


void btn_init_in_c15 (void){
    //кнопка
   
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC , ENABLE); // (1)
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;    
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}


void led_toggled_brown(void){
    static int ledval = 0;
// toggle led
//... (4) ...
    GPIO_WriteBit(GPIOC , GPIO_Pin_13 , (ledval) ? Bit_SET : Bit_RESET);
    ledval = 1 - ledval; 

}

// for tetsing this lib !!!
uint32_t btn_test_GND_connect(void){
    if (!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_15))// Catch from GND 
		{//slow, press and connect GPIOC 13 to GND 
		 led_toggled_brown();
         delay_from_Brown(1000);
         return 1;
		}
		
		else 
        {//fast no press !!!
        led_toggled_brown();
        delay_from_Brown(100);
        return 0;    
        }
		
	}

uint32_t btn_test_GND_connect_fast(void){
    if (!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_15))// Catch from GND 
		{//slow, press and connect GPIOC 13 to GND 
		 delay_from_Brown(10);
         return 1;
		}
		
		else 
        {//fast no press !!!
        delay_from_Brown(10);
        return 0;    
        }
		
	}
    
   
// toggle led

    
    uint32_t btn_test_GND_connect_toggled(void){
    static int ledval = 0;
    ledval =    GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_15);
    if ((ledval) ? Bit_SET : Bit_RESET)// Catch from GND 
		{//slow, press and connect GPIOC 13 to GND 
		 delay_from_Brown(10);
         return ledval;
		}
		else 
        {//fast no press !!!
        delay_from_Brown(10);
        return 0;    
        }
        ledval = 1 - ledval; 
		
	}
