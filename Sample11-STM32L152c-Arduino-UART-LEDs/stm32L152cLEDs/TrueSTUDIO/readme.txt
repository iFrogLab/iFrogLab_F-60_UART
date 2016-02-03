/**
  @page TrueSTUDIO TrueSTUDIO Project
 
  @verbatim
  ******************** (C) COPYRIGHT 2013 STMicroelectronics *******************
  * @file    readme.txt
  * @author  Microcontroller Division
  * @version V1.0.0
  * @date    May-2013
  * @brief   This sub-directory contains all the TrueSTUDIO project files.
  ******************************************************************************
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  ******************************************************************************
  @endverbatim
 
 @par Directory contents
 
 - project .cproject/.project: A pre-configured project file with the provided 
                               library structure that produces an executable 
                               image with TrueSTUDIO.

 - *.ld: 			                 This file is the TrueSTUDIO linker script used to 
                               place program code (readonly) in internal FLASH 
                               and data (readwrite, Stack and Heap)in internal 
                               SRAM. 
                               You can customize this file to your need.
                               
 - *.launch                    This is the TrueSTUDIO debug configuration file.
                               It contains necessary information to enter  
                               debug session such as the debug probe and SWV frequency.
                               
                           
 @par How to use it ?

 - Open the TrueSTUDIO toolchain.
 - Click on File->Switch Workspace->Other and browse to TrueSTUDIO workspace 
   directory.
 - Click on File->Import, select General->'Existing Projects into Workspace' 
   and then click "Next". 
 - Browse to the TrueSTUDIO workspace directory and select the project: 
   - STM32L152RB(STM32L-Discovery): to configure the project for STM32L-Discovery 
     based on STM32L152RB device.
   - STM32L152RC(32L152C-Discovery): to configure the project for STM32L-Discovery 
     based on STM32L152RC device. 
 - Rebuild all project files: Select the project in the "Project explorer" 
   window then click on Project->build project menu.
 - Run program: Select the project in the "Project explorer" window then click 
   Run->Debug (F11)

@note
- Ultra Low Power Medium-density devices are STM32L151xx and STM32L152xx 
  microcontrollers where the Flash memory density ranges between 64 and 128 Kbytes.
- Ultra Low Power Medium-density Plus devices are STM32L151xx, STM32L152xx and 
  STM32L162xx microcontrollers where the Flash memory density is 256 Kbytes.
- Ultra Low Power High-density devices are STM32L151xx, STM32L152xx and STM32L162xx 
  microcontrollers where the Flash memory density is 384 Kbytes.
   
 * <h3><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h3>
 */
