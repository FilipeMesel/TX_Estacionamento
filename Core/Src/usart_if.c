/**
  ******************************************************************************
  * @file    usart_if.c
  * @author  MCD Application Team
  * @brief   Configuration of UART MX driver interface for hyperterminal communication
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "usart_if.h"

/* USER CODE BEGIN Includes */
uint8_t rxBuffer[USART_RX_BUFFER_SIZE];

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* Uart Handle */
extern UART_HandleTypeDef husart2;
#define UartHandle husart2
uint8_t rxBuf[16];

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/**
  * @brief Trace driver callbacks handler
  */
const UTIL_ADV_TRACE_Driver_s UTIL_TraceDriver =
{
  vcom_Init,
  vcom_DeInit,
  vcom_ReceiveInit,
  vcom_Trace_DMA,
};

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/**
  * @brief  TX complete callback
  * @return none
  */
static void (*TxCpltCallback)(void *);
/**
  * @brief  RX complete callback
  * @param  rxChar ptr of chars buffer sent by user
  * @param  size buffer size
  * @param  error errorcode
  * @return none
  */
static void (*RxCpltCallback)(uint8_t *rxChar, uint16_t size, uint8_t error);

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions --------------------------------------------------------*/

UTIL_ADV_TRACE_Status_t vcom_Init(void (*cb)(void *))
{
  /* USER CODE BEGIN vcom_Init_1 */

  /* USER CODE END vcom_Init_1 */
  TxCpltCallback = cb;
  MX_DMA_Init();
  MX_USART2_Init();
  /* USER CODE BEGIN vcom_Init_2 */
  //HAL_UART_ReceiverTimeout_Config(&UartHandle, 2);
  //HAL_UART_EnableReceiverTimeout(&UartHandle);
  //HAL_UART_Receive_IT(&UartHandle, rxBuffer, USART_RX_BUFFER_SIZE);
  /* USER CODE END vcom_Init_2 */
  return UTIL_ADV_TRACE_OK;
}

UTIL_ADV_TRACE_Status_t vcom_DeInit(void)
{
  /* USER CODE BEGIN vcom_DeInit_1 */

  /* USER CODE END vcom_DeInit_1 */
  /* ##-1- Reset peripherals ################################################## */
  __HAL_RCC_USART2_FORCE_RESET();
  __HAL_RCC_USART2_RELEASE_RESET();

  /* ##-2- MspDeInit ################################################## */
  HAL_UART_MspDeInit(&UartHandle);

  /* ##-3- Disable the NVIC for DMA ########################################### */
  /* temporary while waiting CR 50840: MX implementation of  MX_DMA_DeInit() */
  /* For the time being user should change manually the channel according to the MX settings */
  /* USER CODE BEGIN 1 */
  HAL_NVIC_DisableIRQ(DMA1_Channel4_5_6_7_IRQn);

  /* USER CODE END 1 */
  /* USER CODE BEGIN vcom_DeInit_2 */

  /* USER CODE END vcom_DeInit_2 */
  return UTIL_ADV_TRACE_OK;
}

void vcom_Trace(uint8_t *p_data, uint16_t size)
{
  /* USER CODE BEGIN vcom_Trace_1 */

  /* USER CODE END vcom_Trace_1 */
  HAL_UART_Transmit(&UartHandle, p_data, size, 1000);
  /* USER CODE BEGIN vcom_Trace_2 */

  /* USER CODE END vcom_Trace_2 */
}

UTIL_ADV_TRACE_Status_t vcom_Trace_DMA(uint8_t *p_data, uint16_t size)
{
  /* USER CODE BEGIN vcom_Trace_DMA_1 */

  /* USER CODE END vcom_Trace_DMA_1 */
  HAL_UART_Transmit_DMA(&UartHandle, p_data, size);
  /* USER CODE BEGIN vcom_Trace_DMA_2 */

  /* USER CODE END vcom_Trace_DMA_2 */
  return UTIL_ADV_TRACE_OK;
}

UTIL_ADV_TRACE_Status_t vcom_ReceiveInit(void (*RxCb)(uint8_t *rxChar, uint16_t size, uint8_t error))
{
  /* USER CODE BEGIN vcom_ReceiveInit_1 */

  /* USER CODE END vcom_ReceiveInit_1 */
  UART_WakeUpTypeDef WakeUpSelection;

  /*record call back*/
  RxCpltCallback = RxCb;

  /*Set wakeUp event on start bit*/
  WakeUpSelection.WakeUpEvent = UART_WAKEUP_ON_STARTBIT;

  HAL_UARTEx_StopModeWakeUpSourceConfig(&UartHandle, WakeUpSelection);

  /* Make sure that no UART transfer is on-going */
  while (__HAL_UART_GET_FLAG(&UartHandle, USART_ISR_BUSY) == SET);

  /* Make sure that UART is ready to receive)   */
  while (__HAL_UART_GET_FLAG(&UartHandle, USART_ISR_REACK) == RESET);

  /* Enable USART interrupt */
  __HAL_UART_ENABLE_IT(&UartHandle, UART_IT_WUF);//UART_IT_IDLE);//UART_IT_WUF);

  /*Enable wakeup from stop mode*/
  HAL_UARTEx_EnableStopMode(&UartHandle);

  /*Start LPUART receive on IT*/
  HAL_UART_Receive_IT(&UartHandle, rxBuf, sizeof(rxBuf));

  /* USER CODE BEGIN vcom_ReceiveInit_2 */
  /* USER CODE END vcom_ReceiveInit_2 */
  return UTIL_ADV_TRACE_OK;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  /* USER CODE BEGIN HAL_UART_TxCpltCallback_1 */

  /* USER CODE END HAL_UART_TxCpltCallback_1 */
  /* buffer transmission complete*/
  TxCpltCallback(NULL);
  /* USER CODE BEGIN HAL_UART_TxCpltCallback_2 */

  /* USER CODE END HAL_UART_TxCpltCallback_2 */
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  /* USER CODE BEGIN HAL_UART_RxCpltCallback_1 */

  /* USER CODE END HAL_UART_RxCpltCallback_1 */
  if ((NULL != RxCpltCallback))// && (HAL_UART_ERROR_NONE == UartHandle->ErrorCode))
  {
	    RxCpltCallback(rxBuf, UartHandle->RxXferSize - UartHandle->RxXferCount, 0);
  }
  HAL_UART_Receive_IT(UartHandle, rxBuf, sizeof(rxBuf));
  /* USER CODE BEGIN HAL_UART_RxCpltCallback_2 */

  /* USER CODE END HAL_UART_RxCpltCallback_2 */
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *UartHandle, uint16_t size){
	if ((NULL != RxCpltCallback))// && (HAL_UART_ERROR_NONE == UartHandle->ErrorCode))
	{
		RxCpltCallback(rxBuf, size, 0);
	}
	HAL_UART_Receive_IT(UartHandle, rxBuf, sizeof(rxBuf));
}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private Functions Definition -----------------------------------------------*/

/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
