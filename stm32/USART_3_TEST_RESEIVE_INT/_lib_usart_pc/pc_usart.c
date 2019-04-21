#include "pc_usart.h"

char pc_res = PC_NO_SYMBOL;

void USART2_IRQHandler(void)
{
    /* RXNE handler */
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        /* If received 't', toggle LED and transmit 'T' */
        pc_res=(char)USART_ReceiveData(USART2);
    }     
  
}


void init_usart2(){
		
	/* USART configuration structure for USART1 */
    USART_InitTypeDef usart2_init_struct;
    /* Bit configuration structure for GPIOA PIN2 and PIN3 */
    GPIO_InitTypeDef gpioa_init_struct;
    
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
                            
    /* GPIOA PIN2 alternative function Tx */
    gpioa_init_struct.GPIO_Pin = GPIO_Pin_2;
    gpioa_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    gpioa_init_struct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &gpioa_init_struct);
    /* GPIOA PIN3 alternative function Rx */
    gpioa_init_struct.GPIO_Pin = GPIO_Pin_3;
    gpioa_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    gpioa_init_struct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpioa_init_struct);
 
    /* Enable USART1 */
    USART_Cmd(USART2, ENABLE);  
    /* Baud rate 9600, 8-bit data, One stop bit
     * No parity, Do both Rx and Tx, No HW flow control
     */
    usart2_init_struct.USART_BaudRate = 9600;   
    usart2_init_struct.USART_WordLength = USART_WordLength_8b;  
    usart2_init_struct.USART_StopBits = USART_StopBits_1;   
    usart2_init_struct.USART_Parity = USART_Parity_No ;
    usart2_init_struct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    usart2_init_struct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    /* Configure USART1 */
    USART_Init(USART2, &usart2_init_struct);
    /* Enable RXNE interrupt */
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    /* Enable USART1 global interrupt */
    NVIC_EnableIRQ(USART2_IRQn);
}


