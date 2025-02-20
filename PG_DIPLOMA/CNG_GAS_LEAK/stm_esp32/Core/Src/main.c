/* Includes */
#include "main.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <string.h>

/* Private variables */
CAN_HandleTypeDef hcan2;

/* Function Prototypes */
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CAN2_Init(void);
void CAN_Filter_Config(void);
void CAN_Transmit(void);
void Error_Handler(void);

/* Sensor Function Prototypes */
int ReadSE015FlameSensor(void);
int ReadMQ9GasSensor(void);

/* CAN Variables */
CAN_TxHeaderTypeDef TxHeader;
uint8_t TxData[8];
uint32_t TxMailbox;

int flame = 0;
int gas = 0;

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_CAN2_Init();
    CAN_Filter_Config();

    if(HAL_CAN_Start(&hcan2) != HAL_OK)
    {
        Error_Handler();
    }

    while (1)
    {
        flame = ReadSE015FlameSensor();
        gas = ReadMQ9GasSensor();
        CAN_Transmit();
        HAL_Delay(500);
    }
}

/* System Clock Configuration */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 50;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
    {
        Error_Handler();
    }
}

/* GPIO Initialization */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_1 | GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_14;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

/* CAN2 Initialization */
static void MX_CAN2_Init(void)
{
    hcan2.Instance = CAN2;
    hcan2.Init.Prescaler = 16;
    hcan2.Init.Mode = CAN_MODE_NORMAL;
    hcan2.Init.SyncJumpWidth = CAN_SJW_1TQ;
    hcan2.Init.TimeSeg1 = CAN_BS1_1TQ;
    hcan2.Init.TimeSeg2 = CAN_BS2_1TQ;
    hcan2.Init.AutoBusOff = DISABLE;
    hcan2.Init.AutoWakeUp = DISABLE;
    hcan2.Init.AutoRetransmission = DISABLE;
    hcan2.Init.ReceiveFifoLocked = DISABLE;
    hcan2.Init.TransmitFifoPriority = DISABLE;

    if (HAL_CAN_Init(&hcan2) != HAL_OK)
    {
        Error_Handler();
    }
}

/* CAN Filter Configuration */
void CAN_Filter_Config(void)
{
    CAN_FilterTypeDef sFilterConfig;
    sFilterConfig.FilterBank = 14;
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
    sFilterConfig.FilterIdHigh = 0x0000;
    sFilterConfig.FilterIdLow = 0x0000;
    sFilterConfig.FilterMaskIdHigh = 0x0000;
    sFilterConfig.FilterMaskIdLow = 0x0000;
    sFilterConfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;
    sFilterConfig.FilterActivation = ENABLE;
    sFilterConfig.SlaveStartFilterBank = 14;

    if (HAL_CAN_ConfigFilter(&hcan2, &sFilterConfig) != HAL_OK)
    {
        Error_Handler();
    }
}

/* CAN Transmit Function */
void CAN_Transmit(void)
{
    TxHeader.DLC = 2;
    TxHeader.IDE = CAN_ID_STD;
    TxHeader.RTR = CAN_RTR_DATA;
    TxHeader.StdId = 0x245;

    TxData[0] = (uint8_t)flame;
    TxData[1] = (uint8_t)gas;

    if (HAL_CAN_AddTxMessage(&hcan2, &TxHeader, TxData, &TxMailbox) != HAL_OK)
    {
        Error_Handler();
    }
}

/* Placeholder function for reading Flame Sensor */
int ReadSE015FlameSensor(void)
{
    return HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1);
}

/* Placeholder function for reading MQ9 Gas Sensor */
int ReadMQ9GasSensor(void)
{
    return HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2);
}

/* Error Handler */
void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}
