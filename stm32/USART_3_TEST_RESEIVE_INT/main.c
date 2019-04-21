#define USE_STDPERIPH_DRIVER

#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

#include "stm32f10x_usart.h"
#include "usart_lib.h"
#include "tm1637.h"
#include "brown_delay.h"
#include "pc_usart.h"
/*****
Overall functinal:

1) Keyboard on STM32(1)
    If you press on keyboard - char is sent to STM32(2) (to this)
		Then display on tm1637
		and transimt to Flusk Server (python)
2) Web virtual keyboard  - is pressed - char is sent to STM32(2) (to this)
******/


// RESEIVER 
// 1) Connects to PC (PA2,PA3) with two way data change 
// 2) Connect to other STM32(PA9,PA10) only receive
// 3) Displays the symbol that comes from one of (PC or other STM32)
// 4) Usart works like: IRQ save global var 
//     main.c - USART1_IRQHandler - saves tem_res
//     pc_usart.c - USART2_IRQHandler - saves pc_res
// 

/*************************

 *************************/
 
/* User defined function prototypes */

#define KEYPAD_NO_PRESSED 0xFF

void GPIOC_Init(void);
void USART1_Init(void);
void led_toggle(void);
void DisplaySymbol(uint8_t c);
void Display_Loop_1_effect(int times);
void Display_Loop_2_effect(int times);
void Display_Loop_4_effect(int times);
void Display_Loop_4_effect_2(int times);
void Display_Loop_dash_effect_1(int times);
char tem_res;
int displayLoopDalay = 400; 
char to_pc_format(uint8_t key);

int main(void)
{   
    sys_tick_ini();
    TM1637_init();	
    TM1637_brightness(BRIGHT_TYPICAL); 
    
    TM1637_display_all(55);
    delay_from_Brown(1000);
	
    TM1637_display_all(77);
    delay_from_Brown(1000);
	
    TM1637_clearDisplay();		
    USART1_Init();
		init_usart2();
	
	  //save old tem_res
		char old_tem_res=tem_res; 
		//save old pc_res  
		char old_pc_res=pc_res; 
		while(1)
    {			
			
			if((uint8_t)tem_res!=KEYPAD_NO_PRESSED) //check tem-res that comes from another STM32
				uart_putc(USART2,to_pc_format(tem_res)); //send to PC
			if(old_tem_res!=tem_res){
				DisplaySymbol(tem_res); //display on screen
				old_tem_res = tem_res;				
			}
			//high priority:
			if(old_pc_res!=pc_res){ //check pc-res that comes from pc
				DisplaySymbol(pc_res);//display pc_res on screen
				//save old pc_res
				old_pc_res = pc_res;
			}
			
    }
}   

//test a
char to_pc_format(uint8_t key){
	if(key >= 128)
		return 0x01;
	return key;
}

//display only 0-9, *, #
void DisplaySymbol(uint8_t c){
	TM1637_clearDisplay();
	if(c>=48 && c<=57){
		TM1637_display(1,c-48);
	}
	else{	
    switch (c)  {
			case '*': 
					TM1637_display_all_custom(0x00f9cf00 );
					break; 
			case '#': 
					TM1637_display_all_custom(0x00f6f600 );					
					break;
			default:	
					//TM1637_display_custom(0,c);				
					break;
    }
	}
}

/***********************************************
 * Initialize GPIOC PIN8 as push-pull output
 ***********************************************/
void GPIOC_Init(void)
{
    /* Bit configuration structure for GPIOA PIN8 */
    GPIO_InitTypeDef gpioC_init_struct = { GPIO_Pin_13, GPIO_Speed_50MHz, 
                                           GPIO_Mode_Out_PP };
                                             
    /* Enable PORT C clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    /* Initialize GPIOC: 50MHz, PIN8, Push-pull Output */
    GPIO_Init(GPIOC, &gpioC_init_struct);   
     
    /* Turn off LED to start with */
    GPIO_SetBits(GPIOC, GPIO_Pin_13);
}
 
/*****************************************************
 * Initialize USART1: enable interrupt on reception
 * of a character
 *****************************************************/
void USART1_Init(void)
{
    /* USART configuration structure for USART1 */
    USART_InitTypeDef usart1_init_struct;
    /* Bit configuration structure for GPIOA PIN9 and PIN10 */
    GPIO_InitTypeDef gpioa_init_struct;
     
    /* Enalbe clock for USART1, AFIO and GPIOA */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO | 
                           RCC_APB2Periph_GPIOA, ENABLE);
                            
    /* GPIOA PIN9 alternative function Tx */
    gpioa_init_struct.GPIO_Pin = GPIO_Pin_9;
    gpioa_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    gpioa_init_struct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &gpioa_init_struct);
    /* GPIOA PIN10 alternative function Rx */
    gpioa_init_struct.GPIO_Pin = GPIO_Pin_10;
    gpioa_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    gpioa_init_struct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpioa_init_struct);
 
    /* Enable USART1 */
    USART_Cmd(USART1, ENABLE);  
    /* Baud rate 9600, 8-bit data, One stop bit
     * No parity, Do both Rx and Tx, No HW flow control
     */
    usart1_init_struct.USART_BaudRate = 9600;   
    usart1_init_struct.USART_WordLength = USART_WordLength_8b;  
    usart1_init_struct.USART_StopBits = USART_StopBits_1;   
    usart1_init_struct.USART_Parity = USART_Parity_No ;
    usart1_init_struct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    usart1_init_struct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    /* Configure USART1 */
    USART_Init(USART1, &usart1_init_struct);
    /* Enable RXNE interrupt */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    /* Enable USART1 global interrupt */
    NVIC_EnableIRQ(USART1_IRQn);
}
 
///*******************************************
// * Toggle LED 
// *******************************************/
//void led_toggle(void)
//{
//    /* Read LED output (GPIOA PIN8) status */
//    uint8_t led_bit = GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_13);
//     
//    /* If LED output set, clear it */
//    if(led_bit == (uint8_t)Bit_SET)
//    {
//        GPIO_ResetBits(GPIOC, GPIO_Pin_13);
//    }
//    /* If LED output clear, set it */
//    else
//    {
//        GPIO_SetBits(GPIOC, GPIO_Pin_13);
//    }
//}
 
/**********************************************************
 * USART1 interrupt request handler: on reception of a 
 * character 't', toggle LED and transmit a character 'T'
 *********************************************************/
void USART1_IRQHandler(void)
{
    /* RXNE handler */
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        /* If received 't', toggle LED and transmit 'T' */
        tem_res=(char)USART_ReceiveData(USART1);
        USART_SendData(USART1, 'T');
            /* Wait until Tx data register is empty, not really 
             * required for this example but put in here anyway.
             */
            /*
            while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
            {
            }*/
      
    }
     
    /* ------------------------------------------------------------ */

}

