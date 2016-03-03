/**
  ******************************************************************************
  * @file    main.c
  * @author  Microcontroller Division
  * @version V1.0.4
  * @date    Feb-2014
  * @brief   Main program body
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  */
 
/* Includes ------------------------------------------------------------------*/
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "stm32l1xx.h"
#include "stdio.h"
#include "discover_board.h"
#include "stm32l_discovery_lcd.h"
#include "main.h"

/* Private typedef -----------------------------------------------------------*/

typedef struct
{
    uint16_t VREF;
    uint16_t TS_CAL_1; // low temperature calibration data
    uint16_t reserved;
    uint16_t TS_CAL_2; // high temperature calibration data
} TSCALIB_TypeDef;

typedef enum 
{
    Display_TemperatureDegC,
    Display_ADCval
} DisplayState_TypeDef;

/* Private define ------------------------------------------------------------*/

#define DEBUG_SWD_PIN  1  /* needs to be set to 1 to enable SWD debug pins, set to 0 for power consumption measurement*/

#define FACTORY_TSCALIB_MD_BASE         ((uint32_t)0x1FF80078)    /*!< Calibration Data Bytes base address for medium density devices*/
#define FACTORY_TSCALIB_MDP_BASE        ((uint32_t)0x1FF800F8)    /*!< Calibration Data Bytes base address for medium density plus devices*/
#define FACTORY_TSCALIB_MD_DATA         ((TSCALIB_TypeDef *) FACTORY_TSCALIB_MD_BASE)
#define FACTORY_TSCALIB_MDP_DATA        ((TSCALIB_TypeDef *) FACTORY_TSCALIB_MDP_BASE)
#define USER_CALIB_BASE           ((uint32_t)0x08080000)    /*!< USER Calibration Data Bytes base address */
#define USER_CALIB_DATA           ((TSCALIB_TypeDef *) USER_CALIB_BASE)
#define TEST_CALIB_DIFF           (int32_t) 50  /* difference of hot-cold calib
                                               data to be considered as valid */ 

#define HOT_CAL_TEMP 		110
#define COLD_CAL_TEMP  	30

#define DEFAULT_HOT_VAL 0x362
#define DEFAULT_COLD_VAL 0x2A8

#define MAX_TEMP_CHNL 16

#define ADC_CONV_BUFF_SIZE 20

#define VDD_CORRECTION 1  /* definition for correction of VDD if <> 3V */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

ADC_InitTypeDef ADC_InitStructure;
ADC_CommonInitTypeDef ADC_CommonInitStructure;
DMA_InitTypeDef DMA_InitStructure;

__IO uint16_t 	ADC_ConvertedValue, T_StartupTimeDelay;

uint32_t ADC_Result, INTemperature, refAVG, tempAVG, vdd_ref, Address = 0;
int32_t temperature_C; 

uint16_t ADC_ConvertedValueBuff[ADC_CONV_BUFF_SIZE];

char strDisp[20] ;

DisplayState_TypeDef CurrentlyDisplayed = Display_TemperatureDegC;

TSCALIB_TypeDef calibdata;    /* field storing temp sensor calibration data */

volatile bool flag_ADCDMA_TransferComplete;
volatile bool flag_UserButton;

static volatile uint32_t TimingDelay;
RCC_ClocksTypeDef RCC_Clocks;

__IO FLASH_Status FLASHStatus = FLASH_COMPLETE; 


/* Private function prototypes -----------------------------------------------*/
void  RCC_Configuration(void);
void  RTC_Configuration(void);
void  Init_GPIOs (void);
void  acquireTemperatureData(void);
void  configureADC_Temp(void);
void  configureDMA(void);
void  powerDownADC_Temper(void);
void  processTempData(void);
void  configureWakeup (void);
void  writeCalibData(TSCALIB_TypeDef *calibStruct);
void getFactoryTSCalibData(TSCALIB_TypeDef *calibdata);
ErrorStatus  testUserCalibData(void);
ErrorStatus  testFactoryCalibData(void);
void insertionSort(uint16_t *numbers, uint32_t array_size);
uint32_t interquartileMean(uint16_t *array, uint32_t numOfSamples);
void clearUserButtonFlag(void);




/*******************************************************************************/
//void UART1_Init();
void UART1_Config(uint32_t BaudRate);
void UART1_Cmd(FunctionalState NewState);
void UART1_Write(char *Buffer, uint32_t Len);





void UART1_Init()
{
 GPIO_InitTypeDef GPIO_InitStructure; 

 /* Pin configuration */
 RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);  //PB06 , PB07
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
 GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
 GPIO_Init(GPIOB, &GPIO_InitStructure);

 /* Alternate function conf */
 GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_USART1 );
 GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_USART1 );

 /* Enable the USART 1 clock */
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
}

void UART1_Config(uint32_t BaudRate)
{
 USART_InitTypeDef USART_InitStructure;

 /* USART conf */
 USART_InitStructure.USART_BaudRate = BaudRate;
 USART_InitStructure.USART_WordLength = USART_WordLength_8b;
 USART_InitStructure.USART_StopBits = USART_StopBits_1;
 USART_InitStructure.USART_Parity = USART_Parity_No;
 USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
 USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
 USART_Init(USART1, &USART_InitStructure);
}

void UART1_Cmd(FunctionalState NewState)
{
 /* Enable/disable the USART */
 USART_Cmd(USART1, NewState);
}

void UART1_Write(char *Buffer, uint32_t Len)
{
 for (; Len > 0; Len--) {

 while (USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET)
 ;

 USART_SendData(USART1, *Buffer);
 Buffer++;
 }
 while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
}

// USART_ReceiveData

char buff [100]="";
char buff2 [100]="";
char buff3 [10]="";
int32_t i2;
char* My_Usart1_ReadLine(){    
	  i2=0;
	  while(1){
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET){   
        char c = USART_ReceiveData(USART1);   
        i2=i2+1;			
        if(c != '\r' && c != '\n' && i2<9 ){
            sprintf (buff, "%c", c);
        }else{
           // char buff2 [100] = "";
            memcpy(buff2,buff, strlen(buff)); // ? buff ??? buff2
            memset(buff, 0, strlen(buff)); // ?? buff ???????    
			      USART_ClearFlag(USART1, USART_FLAG_RXNE);
            return buff2;
        }
    }
			USART_ClearFlag(USART1, USART_FLAG_RXNE);
	  }
    return "";
}


/**
  * @brief main entry point.
  * @par Parameters None
  * @retval void None
  * @par Required preconditions: None
  */

void DelaySW(uint32_t delay)
{
 uint32_t i=10000;

 for(;delay > 0; delay--){
 for(;i>0;i--)
 ;
 }
}

void RCC_Config(void)
{ 
 RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

 /* Allow access to the RTC */
 PWR_RTCAccessCmd(ENABLE);

 /* Reset Backup Domain */
 RCC_RTCResetCmd(ENABLE);
 RCC_RTCResetCmd(DISABLE);

 /*!< LSE Enable */
 RCC_LSEConfig(RCC_LSE_ON);

 /*!< Wait till LSE is ready */
 while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
 {}
 /*!< LCD Clock Source Selection */
 RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
}

	char* buffer="              ";
	
	
	char* r1="  ";
	
    GPIO_InitTypeDef g;
int main(void)
{
	
// RCC_Config();
 UART1_Init();
 UART1_Config(9600);
 UART1_Cmd(ENABLE);
 UART1_Write("stm start",9);
	////////
	
    g.GPIO_Pin = GPIO_Pin_9;    // 指定 6,7 腳
    g.GPIO_Mode = GPIO_Mode_OUT;           // 設定針腳為輸出
    //g.GPIO_PuPd = GPIO_PuPd_UP;            // 使用上拉電阻
    //g.GPIO_PuPd = GPIO_PuPd_DOWN;      // 使用下拉電阻
    g.GPIO_Speed = GPIO_Speed_40MHz;  // 指定 GPIO 頻率為40 MHz

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);    // 啟用 GPIOB

    GPIO_Init(GPIOB, &g);    // 初始化 GPIO B

      
	
	//////////
 while (1)
 {
	
	 //// solution 2 working String
	 //  i=0;
	 
     memcpy(buff2,buff, strlen(buff)); // ? buff ??? buff2
     memset(buff, 0, strlen(buff)); // ?? buff ???????  
						
	   while(1){
			if(USART_GetFlagStatus(USART1,USART_FLAG_RXNE) != RESET){
				
				char c = USART_ReceiveData(USART1); 
       // i=i+1;				
				if(c == '\r'){
					break;
				}else if (c == '\n'  ){
					break;
				}else{
           sprintf (buff, "%s%c", buff, c);
				}
			
			}else{	
			}
		}
		
			UART1_Write(buff, strlen(buff));
	    USART_ClearFlag(USART1, USART_FLAG_RXNE);
		
		if(strcmp(buff, "H")==0){
		
			UART1_Write("APPLE", 5);
			  GPIO_SetBits(GPIOB,GPIO_Pin_9);     // 設定9腳為高電位
   
		}else if(strcmp(buff, "L")==0){
			  GPIO_ResetBits(GPIOB,GPIO_Pin_9);     // 設定9腳為低電位
   
		}
		
		
		
	}
 
 }
void TimingDelay_Decrement(void)
{
}
void Delay(__IO uint32_t nTime){
}
void setADCDMA_TransferComplete(void){}
void setUserButtonFlag(void){}
	
	

