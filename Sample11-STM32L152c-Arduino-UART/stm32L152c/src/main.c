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
	uint8_t i, resp[]={31,32,33,34,35};
	
	
	char* r1="  ";
int main(void)
{
	
 RCC_Config();
 UART1_Init();
 UART1_Config(9600);
 UART1_Cmd(ENABLE);
 UART1_Write("stm start",9);
 while (1)
 {
	
	 //// solution 2 working String
	   i=0;
	 
            memcpy(buff2,buff, strlen(buff)); // ? buff ??? buff2
            memset(buff, 0, strlen(buff)); // ?? buff ???????  
						
	   while(1){
			if(USART_GetFlagStatus(USART1,USART_FLAG_RXNE) != RESET){
				
				char c = USART_ReceiveData(USART1); 
        i=i+1;				
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
			//	sprintf (buff3,"%c", i+48);
		
			//	UART1_Write(buff3,1);
			//	UART1_Write("(",1);
			UART1_Write(buff, strlen(buff));
			//	UART1_Write(")",1);
	    USART_ClearFlag(USART1, USART_FLAG_RXNE);
	 //// solution 1 working 1 char
	 /*
	  if(USART_GetFlagStatus(USART1,USART_FLAG_RXNE) != RESET){
			char c = USART_ReceiveData(USART1);    
			    // c='1';
			sprintf (buff,"%c", c);
	    USART_ClearFlag(USART1, USART_FLAG_RXNE);
	   //char * data = My_Usart1_ReadLine();
			
			
			
      if(c == '\r'){
       UART1_Write(".R",2);
			}else if (c == '\n'  ){
       UART1_Write(".N",2);
		  }
			
      UART1_Write("(",1);
      UART1_Write(buff,1);
      UART1_Write(")",1);
		}
   */
		
	 
	 /*
	  if(USART_GetFlagStatus(USART1,USART_FLAG_RXNE) != RESET){
			  char c = USART_ReceiveData(USART1);   
     // resp[0] = USART_ReceiveData(USART1);
	    USART_ClearFlag(USART1, USART_FLAG_RXNE);
			sprintf((char*)buffer, "%c", c);
      UART1_Write(buffer,1);
      //DelaySW(10000);
		}*/
	 
  // printf("Positionen: %d \r\n", resp[0]);
	//	i=0;
		// 0x%x 16
 //sprintf((char*)buffer, "%c%c%c%c%c", resp[0], resp[1], resp[2], resp[3], resp[4]);
 

 //UART1_Write(".com",4);
 //DelaySW(10000);
 }
}


//////////////////////////////////////////////////////////////////////////////
int main2(void)
{ 

 /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32l1xx_md.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32l1xx.c file
     */ 

  /* Configure Clocks for Application need */
  RCC_Configuration();
  
  /* Configure RTC Clocks */
  RTC_Configuration();

  /* Set internal voltage regulator to 1.8V */
  PWR_VoltageScalingConfig(PWR_VoltageScaling_Range1);

  /* Wait Until the Voltage Regulator is ready */
  while (PWR_GetFlagStatus(PWR_FLAG_VOS) != RESET) ;

  /* Enable debug features in low power modes (Sleep, STOP and STANDBY) */
#ifdef  DEBUG_SWD_PIN
  DBGMCU_Config(DBGMCU_SLEEP | DBGMCU_STOP | DBGMCU_STANDBY, ENABLE);
#endif
  
  /* Configure SysTick IRQ and SysTick Timer to generate interrupts */
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 500);

  /* Init I/O ports */
  Init_GPIOs();
  
  /* Initializes the LCD glass */
  LCD_GLASS_Configure_GPIO();
  LCD_GLASS_Init();
      
  /* Display Welcome message */ 

  LCD_GLASS_ScrollSentence("   ** TEMPERATURE SENSOR EXAMPLE **    ",1,SCROLL_SPEED);

  /* Disable SysTick IRQ and SysTick Timer */
  SysTick->CTRL  &= ~ ( SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk );

  /* Test user or factory temperature sensor calibration value */
  if ( testFactoryCalibData() == SUCCESS ) getFactoryTSCalibData(&calibdata);
  else if ( testUserCalibData() == SUCCESS ) calibdata = *USER_CALIB_DATA;
  else {
    /* User calibration or factory calibration TS data are not available */
		calibdata.TS_CAL_1 = DEFAULT_COLD_VAL;
    calibdata.TS_CAL_2 = DEFAULT_HOT_VAL;
    writeCalibData(&calibdata);
    calibdata = *USER_CALIB_DATA;
  }

  /* Configure Wakeup from sleep using RTC event*/
  configureWakeup();

  /* Configure direct memory access for ADC usage*/
  configureDMA();

  /* Configure ADC for temperature sensor value conversion */ 
  configureADC_Temp();
   

  while(1){

    /* Re-enable DMA and ADC conf and start Temperature Data acquisition */ 
    acquireTemperatureData();
	
    /* Stay in SLEEP mode untill the data are acquired by ADC */
    __WFI();  
    
    /* for DEBUG purpose uncomment the following line and comment the __WFI call to do not enter STOP mode */
    // while (!flag_ADCDMA_TransferComplete);
    
    /* Disable ADC, DMA and clock*/
    powerDownADC_Temper();
    
    /* Process mesured Temperature data - calculate average temperature value in °C */
    processTempData();

    if (flag_UserButton == TRUE){
       clearUserButtonFlag();
       if (CurrentlyDisplayed == Display_TemperatureDegC)
         CurrentlyDisplayed = Display_ADCval;
       else
         CurrentlyDisplayed = Display_TemperatureDegC;   
    }
    
    if (CurrentlyDisplayed == Display_TemperatureDegC) {
      /* print average temperature value in °C  */
      sprintf(strDisp, "%d °C", temperature_C );
    } else {
      /* print result of ADC conversion  */
      sprintf(strDisp, "> %d", tempAVG );
    }

    LCD_GLASS_Clear();
    LCD_GLASS_DisplayString( (unsigned char *) strDisp );
    
    /* Enable RTC Wakeup */ 
    RTC_WakeUpCmd(ENABLE);

	  /* Clear WakeUp flag */
    PWR_ClearFlag(PWR_FLAG_WU);
	
    /* Enter in wait for interrupt stop mode*/
    PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);

    RCC_Configuration();  // reinitialize clock
    
    /* After Wake up : Disable Wake up from RTC*/
    RTC_WakeUpCmd(DISABLE);
  }

}

/*---------------------------------------------------------------------------*/


void getFactoryTSCalibData(TSCALIB_TypeDef* data)
{
uint32_t deviceID;

	deviceID = DBGMCU_GetDEVID();
	
	if (deviceID == 0x427) *data = *FACTORY_TSCALIB_MDP_DATA;
		else if (deviceID == 0x416) *data = *FACTORY_TSCALIB_MD_DATA;
			else while(1); // add error handler - device cannot be identified calibration data not loaded!
}


ErrorStatus  testUserCalibData(void)
{
  int32_t testdiff;
  ErrorStatus retval = ERROR;
  
  testdiff = USER_CALIB_DATA->TS_CAL_2 - USER_CALIB_DATA->TS_CAL_1;
  
  if ( testdiff > TEST_CALIB_DIFF )    retval = SUCCESS;
  
  return retval;
}

ErrorStatus  testFactoryCalibData(void)
{
  int32_t testdiff;
  ErrorStatus retval = ERROR;
  TSCALIB_TypeDef datatotest;
	
	getFactoryTSCalibData (&datatotest);
	
  testdiff = datatotest.TS_CAL_2 - datatotest.TS_CAL_1;
  
  if ( testdiff > TEST_CALIB_DIFF )    retval = SUCCESS;
  
  return retval;
}

void  writeCalibData(TSCALIB_TypeDef* calibStruct)
{

  uint32_t  Address = 0;
  uint32_t  dataToWrite;
  
  /* Unlock the FLASH PECR register and Data EEPROM memory */
  DATA_EEPROM_Unlock();
  
  /* Clear all pending flags */      
  FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
                  | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR);	
  
  /*  Data EEPROM Fast Word program of FAST_DATA_32 at addresses defined by 
      DATA_EEPROM_START_ADDR and DATA_EEPROM_END_ADDR */  
 
  Address = (uint32_t) USER_CALIB_DATA;


  dataToWrite = 0x00;
  dataToWrite = (uint32_t)(calibStruct->TS_CAL_1) << 16;
  
  FLASHStatus = DATA_EEPROM_ProgramWord(Address, dataToWrite);

  if(FLASHStatus != FLASH_COMPLETE)
  {
    while(1); /* stay in loop in case of crittical programming error */
  }

  Address += 4;

  dataToWrite = 0x00;
  dataToWrite = (uint32_t)(calibStruct->TS_CAL_2) << 16;
  
  FLASHStatus = DATA_EEPROM_ProgramWord(Address, dataToWrite);
  
}


void configureWakeup (void)
{
  /* Declare initialisation structures for (NVIC) and external interupt (EXTI) */
  NVIC_InitTypeDef NVIC_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;

  /* Clear IT pending bit from external interrupt Line 20 */
  EXTI_ClearITPendingBit(EXTI_Line20);
  
  /* Initialise EXTI using its init structure */
  EXTI_InitStructure.EXTI_Line = EXTI_Line20;			 // interrupt generated on RTC Wakeup event (Line 20)
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;    // Use EXTI line as interrupt
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; // Trigg interrupt on rising edge detection
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;				 // Enable EXTI line
  EXTI_Init(&EXTI_InitStructure);						 
  
  /* Initialise the NVIC interrupts (IRQ) using its init structure */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;        // set IRQ channel to RTC Wakeup Interrupt  
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 // set channel Preemption priority to 0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;         // set channel sub priority to 0
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	         // Enable channel
  NVIC_Init(&NVIC_InitStructure);
  
  /* Clear Wake-up flag */  
  PWR->CR |= PWR_CR_CWUF;

  /* Enable PWR clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); 

  /* Allow access to RTC */
  PWR_RTCAccessCmd(ENABLE);

  /* Enable Low Speed External clock */
  RCC_LSEConfig(RCC_LSE_ON); 

  /* Wait till LSE is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);

  /* Select LSE clock as RCC Clock source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

  /* Enable the RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC APB registers synchronisation */
  RTC_WaitForSynchro();

  /* Select 1Hz clock for RTC wake up*/
  RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
  
  /* Set Wakeup auto-reload value to 2 sec */
  RTC_SetWakeUpCounter(1); 

  /* Clear RTC Interrupt pending bit */
  RTC_ClearITPendingBit(RTC_IT_WUT);
  
  /* Clear EXTI line20 Interrupt pending bit */
  EXTI_ClearITPendingBit(EXTI_Line20);

  /* Enable the Wakeup Interrupt */
  RTC_ITConfig(RTC_IT_WUT, ENABLE);
}

void setUserButtonFlag(void)
{
  flag_UserButton = TRUE;
}

void clearUserButtonFlag(void)
{
  flag_UserButton = FALSE;
}

void setADCDMA_TransferComplete(void)
{
  flag_ADCDMA_TransferComplete = TRUE;
}

void clearADCDMA_TransferComplete(void)
{
  flag_ADCDMA_TransferComplete = FALSE;
}

void acquireTemperatureData(void)
{
  /* Enable ADC clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

  /* Enable DMA1 clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);

  /* Wait until the ADC1 is ready */
  while(ADC_GetFlagStatus(ADC1, ADC_FLAG_ADONS) == RESET); 

  /* re-initialize DMA -- is it needed ?*/
  DMA_DeInit(DMA1_Channel1);
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  DMA_Cmd(DMA1_Channel1, ENABLE);
  
  /* Enable DMA channel 1 Transmit complete interrupt*/
  DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);

  /* Disable DMA mode for ADC1 */ 
  ADC_DMACmd(ADC1, DISABLE);

   /* Enable DMA mode for ADC1 */  
  ADC_DMACmd(ADC1, ENABLE);
  
  /* Clear global flag for DMA transfert complete */
  clearADCDMA_TransferComplete(); 
  
  /* Start ADC conversion */
  ADC_SoftwareStartConv(ADC1);
}

void powerDownADC_Temper(void)
{
  /* Disable DMA channel1 */
  DMA_Cmd(DMA1_Channel1, ENABLE);  
  /* Disable ADC1 */
  ADC_Cmd(ADC1, DISABLE);

  /* Disable ADC1 clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);  
  /* Disable DMA1 clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, DISABLE);
}

void configureADC_Temp(void)
{
  uint32_t ch_index;

  /* Enable ADC clock & SYSCFG */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  
  /* Enable the internal connection of Temperature sensor and with the ADC channels*/
  ADC_TempSensorVrefintCmd(ENABLE); 
  
  /* Wait until ADC + Temp sensor start */
  T_StartupTimeDelay = 1024;
  while (T_StartupTimeDelay--);

  /* Setup ADC common init struct */
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
  ADC_CommonInit(&ADC_CommonInitStructure);
  
  
  /* Initialise the ADC1 by using its init structure */
  ADC_StructInit(&ADC_InitStructure);
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;	          // Set conversion resolution to 12bit
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;	                          // Enable Scan mode (single conversion for each channel of the group)
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;			  // Disable Continuous conversion
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConvEdge_None; // Disable external conversion trigger
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;                  // Set conversion data alignement to right
  ADC_InitStructure.ADC_NbrOfConversion = ADC_CONV_BUFF_SIZE;             // Set conversion data alignement to ADC_CONV_BUFF_SIZE
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular Temperature sensor channel16 and internal reference channel17 configuration */ 

    for (ch_index = 1; ch_index <= MAX_TEMP_CHNL; ch_index++){
      ADC_RegularChannelConfig(ADC1, ADC_Channel_16, ch_index, 
                               ADC_SampleTime_384Cycles);
    }

  ADC_RegularChannelConfig(ADC1, ADC_Channel_17, 17, ADC_SampleTime_384Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_17, 18, ADC_SampleTime_384Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_17, 19, ADC_SampleTime_384Cycles);
  ADC_RegularChannelConfig(ADC1, ADC_Channel_17, 20, ADC_SampleTime_384Cycles);
}

void configureDMA(void)
{
  /* Declare NVIC init Structure */
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Enable DMA1 clock */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  /* De-initialise  DMA */
  DMA_DeInit(DMA1_Channel1);
  
  /* DMA1 channel1 configuration */
  DMA_StructInit(&DMA_InitStructure);
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADC1->DR);	     // Set DMA channel Peripheral base address to ADC Data register
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_ConvertedValueBuff;  // Set DMA channel Memeory base addr to ADC_ConvertedValueBuff address
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                         // Set DMA channel direction to peripheral to memory
  DMA_InitStructure.DMA_BufferSize = ADC_CONV_BUFF_SIZE;                     // Set DMA channel buffersize to peripheral to ADC_CONV_BUFF_SIZE
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;	     // Disable DMA channel Peripheral address auto increment
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                    // Enable Memeory increment (To be verified ....)
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;// set Peripheral data size to 8bit 
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;	     // set Memeory data size to 8bit 
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                              // Set DMA in normal mode
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;	                     // Set DMA channel priority to High
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                               // Disable memory to memory option 
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);								 // Use Init structure to initialise channel1 (channel linked to ADC)

  /* Enable Transmit Complete Interrup for DMA channel 1 */ 
  DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
  
  /* Setup NVIC for DMA channel 1 interrupt request */
  NVIC_InitStructure.NVIC_IRQChannel =   DMA1_Channel1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
}


		

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval None
  */
void RCC_Configuration(void)
{  
  
  /* Enable HSI Clock */
  RCC_HSICmd(ENABLE);
  
  /*!< Wait till HSI is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET)
  {}

  RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
  
  RCC_MSIRangeConfig(RCC_MSIRange_6);

  RCC_HSEConfig(RCC_HSE_OFF);  
  if(RCC_GetFlagStatus(RCC_FLAG_HSERDY) != RESET )
  {
    while(1);
  }
 
  /* Enable  comparator clock LCD and PWR mngt */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_LCD | RCC_APB1Periph_PWR, ENABLE);
    
  /* Enable ADC clock & SYSCFG */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_SYSCFG, ENABLE);

}


void RTC_Configuration(void)
{
  
/* Allow access to the RTC */
  PWR_RTCAccessCmd(ENABLE);

  /* Reset Backup Domain */
  RCC_RTCResetCmd(ENABLE);
  RCC_RTCResetCmd(DISABLE);

  /* LSE Enable */
  RCC_LSEConfig(RCC_LSE_ON);

  /* Wait till LSE is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
  {}
  
  RCC_RTCCLKCmd(ENABLE);
   
  /* LCD Clock Source Selection */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

}

/**
  * @brief  To initialize the I/O ports
  * @caller main
  * @param None
  * @retval None
  */


void conf_analog_all_GPIOS(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable GPIOs clock */ 	
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | 
                        RCC_AHBPeriph_GPIOC | RCC_AHBPeriph_GPIOD | 
                        RCC_AHBPeriph_GPIOE | RCC_AHBPeriph_GPIOH, ENABLE);

  /* Configure all GPIO port pins in Analog Input mode (floating input trigger OFF) */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_Init(GPIOD, &GPIO_InitStructure);
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  GPIO_Init(GPIOH, &GPIO_InitStructure);

#if  DEBUG_SWD_PIN == 1
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All & (~GPIO_Pin_13) & (~GPIO_Pin_14);
#endif
  
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  /* Disable GPIOs clock */ 	
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | 
                        RCC_AHBPeriph_GPIOC | RCC_AHBPeriph_GPIOD | 
                        RCC_AHBPeriph_GPIOE | RCC_AHBPeriph_GPIOH, DISABLE);
}

void  Init_GPIOs (void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  conf_analog_all_GPIOS();   /* configure all GPIOs as analog input */
  
  /* Enable GPIOs clock */ 	
  RCC_AHBPeriphClockCmd(LD_GPIO_PORT_CLK | USERBUTTON_GPIO_CLK, ENABLE);
 
  /* USER button and WakeUP button init: GPIO set in input interrupt active mode */
  
  /* Configure User Button pin as input */
  GPIO_InitStructure.GPIO_Pin = USERBUTTON_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_Init(USERBUTTON_GPIO_PORT, &GPIO_InitStructure);

  /* Connect Button EXTI Line to Button GPIO Pin */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource0);

  /* Configure User Button and IDD_WakeUP EXTI line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line0 ;  // PA0 for User button AND IDD_WakeUP
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set User Button and IDD_WakeUP EXTI Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStructure); 

/* Configure the GPIO_LED pins  LD3 & LD4*/
  GPIO_InitStructure.GPIO_Pin = LD_GREEN_GPIO_PIN | LD_BLUE_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(LD_GPIO_PORT, &GPIO_InitStructure);
  GPIO_LOW(LD_GPIO_PORT, LD_GREEN_GPIO_PIN);	
  GPIO_LOW(LD_GPIO_PORT, LD_BLUE_GPIO_PIN);

/* Disable all GPIOs clock */ 	
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | 
                        RCC_AHBPeriph_GPIOC | RCC_AHBPeriph_GPIOD | 
                        RCC_AHBPeriph_GPIOE | RCC_AHBPeriph_GPIOH, DISABLE);

}  

void insertionSort(uint16_t *numbers, uint32_t array_size) 
{
  
	uint32_t i, j;
	uint32_t index;

  for (i=1; i < array_size; i++) {
    index = numbers[i];
    j = i;
    while ((j > 0) && (numbers[j-1] > index)) {
      numbers[j] = numbers[j-1];
      j = j - 1;
    }
    numbers[j] = index;
  }
}

uint32_t interquartileMean(uint16_t *array, uint32_t numOfSamples)
{
    uint32_t sum=0;
    uint32_t  index, maxindex;
    /* discard  the lowest and the highest data samples */ 
	maxindex = 3 * numOfSamples / 4;
    for (index = (numOfSamples / 4); index < maxindex; index++){
            sum += array[index];
    }
	/* return the mean value of the remaining samples value*/
    return ( sum / (numOfSamples / 2) );
}


void processTempData(void)
{
  uint32_t index,dataSum;
  dataSum = 0;

  /* sort received data in */
  insertionSort(ADC_ConvertedValueBuff, MAX_TEMP_CHNL);
  
  /* Calculate the Interquartile mean */
  tempAVG = interquartileMean(ADC_ConvertedValueBuff, MAX_TEMP_CHNL);
  
  /* Sum up all mesured data for reference voltage average calculation */ 
  for (index = MAX_TEMP_CHNL; index < ADC_CONV_BUFF_SIZE; index++){
    dataSum += ADC_ConvertedValueBuff[index];
  }
  /* Devide sum up result by 4 for the temperature average calculation*/
  refAVG = dataSum / 4 ;

#if VDD_CORRECTION == 1
	/* estimation of VDD=VDDA=Vref+ from ADC measurement of Vrefint reference voltage in mV */
  vdd_ref = calibdata.VREF * 3000 / refAVG;
	
	/* correction factor if VDD <> 3V */
	tempAVG = tempAVG * vdd_ref / 3000;
	
#endif
	
  /* Calculate temperature in °C from Interquartile mean */
  temperature_C = ( (int32_t) tempAVG - (int32_t) calibdata.TS_CAL_1 ) ;	
  temperature_C = temperature_C * (int32_t)(HOT_CAL_TEMP - COLD_CAL_TEMP);                      
  temperature_C = temperature_C / 
                  (int32_t)(calibdata.TS_CAL_2 - calibdata.TS_CAL_1); 
  temperature_C = temperature_C + COLD_CAL_TEMP; 
}



/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in 10 ms.
  * @retval None
  */
void Delay(uint32_t nTime)
{
  TimingDelay = nTime;

  while(TimingDelay != 0);
  
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{

  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }

}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}

#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
