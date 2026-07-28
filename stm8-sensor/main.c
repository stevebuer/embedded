/**
  ******************************************************************************
  * @file    Project/main.c 
  * @author  MCD Application Team
  * @version V2.2.0
  * @date    30-September-2014
  * @brief   Main program body
   ******************************************************************************
  * @attention
  *
  * COPYRIGHT 2014 STMicroelectronics
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 


/* Includes ------------------------------------------------------------------*/
#include "stm8s.h"
#include "stm8s_it.h"    /* SDCC patch: required by SDCC for interrupts */

/* Private defines -----------------------------------------------------------*/
#define TX_BUF_SIZE 64
#define I2C_SPEED 100000

/* Private variables -----------------------------------------------------------*/
uint8_t TxBuffer[TX_BUF_SIZE];
uint8_t TxCounter = 0;

/* Private function prototypes -----------------------------------------------*/
static void CLK_Config(void);
static void UART_Config(void);
static void GPIO_Config(void);
static void I2C_Config(void);
static uint8_t I2C_ReadSensor(void);

/* Private functions ---------------------------------------------------------*/

void main(void)
{
  /* GPIO configuration -----------------------------------------*/
  GPIO_Config();

  /* CLK configuration -----------------------------------------*/
  CLK_Config();

  /* UART configuration -----------------------------------------*/
  UART_Config();  

  /* I2C configuration -----------------------------------------*/
  I2C_Config();  

  /* Infinite loop */
  while (1)
  {
   
        /* Wait until end of transmit */
        while (UART2_GetFlagStatus(UART2_FLAG_TXE) == RESET)
        {
        }

        /* Write one byte in the UART2 Transmit Data Register */
        UART2_SendData8(TxBuffer[TxCounter++]);
  }
  
}

/**
  * @brief  Configure system clock to run at 16Mhz
  * @param  None
  * @retval None
  */
static void CLK_Config(void)
{
    /* Initialization of the clock */
    /* Clock divider to HSI/1 */
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
}

/**
  * @brief  Configure UART1 Tx pin as Output open-drain high-impedance level 
  * @param  None
  * @retval None
  */
static void GPIO_Config(void)
{
    /* Set PA5 as Output open-drain high-impedance level (UART1_Tx)*/
    GPIO_Init(GPIOA, GPIO_PIN_5, GPIO_MODE_OUT_OD_HIZ_FAST);
}

/**
  * @brief  UART2 Configuration for full duplex communication
  * @param  None
  * @retval None
  */
static void UART_Config(void)
{
  /* UART2 configured as follow:
        - BaudRate = 9600 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Transmit/Receive enabled
  */
  UART2_DeInit();
  UART2_Init((uint32_t)9600, UART2_WORDLENGTH_8D, UART2_STOPBITS_1, UART2_PARITY_NO,
              UART2_SYNCMODE_CLOCK_DISABLE, UART2_MODE_TXRX_ENABLE);

  /* Enable UART2 Half Duplex Mode*/
  // UART2_HalfDuplexCmd(ENABLE);
}

/**
  * @brief  I2C Configuration for Master
  * @param  None
  * @retval None
  */
static void I2C_Config(void)
{
   /* Set bus speed and mode */
   I2C_Init(I2C_SPEED, 0xA0, I2C_DUTYCYCLE_2, I2C_ACK_CURR, I2C_ADDMODE_7BIT, 16);

   /* enable controller interrupts */
   I2C_ITConfig((I2C_IT_TypeDef) (I2C_IT_EVT | I2C_IT_BUF), ENABLE);
}

static uint8_t I2C_ReadSensor(void)
{
	return 255;
}

#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
