/*******************************************************************************
 * @file    :   bsp_gpio.c
 * @brief   :   Board support package for the handling the GPIOs
 * @author  :   Davide Campagna
 * @date    :   May 02, 2024
 * @version :   V1.0
 ******************************************************************************/

/* Includes ----------------------------------------------------------------- */
#include "bsp.h"
#include "qpc.h"

/* Private defines ---------------------------------------------------------- */

Q_DEFINE_THIS_MODULE("BSP_GPIO")

/* Front headlight - PluX16 pin 7 */
#define C_FOF_Pin 		GPIO_PIN_0
#define C_FOF_GPIO_Port		GPIOA
/* Rear headlight - PluX16 pin 13 */
#define C_FOR_Pin		GPIO_PIN_1
#define C_FOR_GPIO_Port		GPIOA
/* PluX16 pins 3 and 4*/
#define C_GPIOB_Pin		GPIO_PIN_2
#define C_GPIOB_GPIO_Port	GPIOA
#define C_GPIOA_Pin		GPIO_PIN_3
#define C_GPIOA_GPIO_Port	GPIOA
/* DRV8872 control pins */
#define nFAULT_Pin		GPIO_PIN_5
#define nFAULT_GPIO_Port	GPIOA
#define IN_1_Pin		GPIO_PIN_6
#define IN_1_GPIO_Port		GPIOA
#define IN_2_Pin		GPIO_PIN_7
#define IN_2_GPIO_Port		GPIOA
/* PluX16 pins 16 and 18 */
#define C_AUX1_Pin		GPIO_PIN_0
#define C_AUX1_GPIO_Port	GPIOB
#define C_AUX2_Pin		GPIO_PIN_1
#define C_AUX2_GPIO_Port	GPIOB
/* DCC signal coming from the tracks */
#define DCC_DATA_Pin		GPIO_PIN_9
#define DCC_DATA_GPIO_Port	GPIOA

/* Private typedefs --------------------------------------------------------- */
typedef struct {
	GPIO_TypeDef * port;
	GPIO_InitTypeDef cfg;
	void (*isr)(void);
} GPIOPin;

/* Private functions declarations ------------------------------------------- */

static void PluX16_GPIO_Init(void);
static void DCC_GPIO_Init(void);
static void Motor_GPIO_Init(void);

static inline void init_pin(GPIOPin * const pin);
static inline void write_pin(GPIOPin * const pin, GPIO_PinState const state);

/* PluX16 Functions pins ---------------------------------------------------- */
static GPIOPin pin_C_FOF = {
	.port = C_FOF_GPIO_Port,
	.cfg = {
		.Pin = C_FOF_Pin,
		.Mode = GPIO_MODE_OUTPUT_PP,
		.Pull = GPIO_NOPULL,
		.Speed = GPIO_SPEED_FREQ_LOW
	},
	.isr = NULL
};

static GPIOPin pin_C_FOR = {
	.port = C_FOR_GPIO_Port,
	.cfg = {
		.Pin = C_FOR_Pin,
		.Mode = GPIO_MODE_OUTPUT_PP,
		.Pull = GPIO_NOPULL,
		.Speed = GPIO_SPEED_FREQ_LOW
	},
	.isr = NULL
};

static GPIOPin pin_C_GPIOA = {
	.port = C_GPIOA_GPIO_Port,
	.cfg = {
		.Pin = C_GPIOA_Pin,
		.Mode = GPIO_MODE_OUTPUT_PP,
		.Pull = GPIO_NOPULL,
		.Speed = GPIO_SPEED_FREQ_LOW
	},
	.isr = NULL
};

static GPIOPin pin_C_GPIOB = {
	.port = C_GPIOB_GPIO_Port,
	.cfg = {
		.Pin = C_GPIOB_Pin,
		.Pull = GPIO_NOPULL,
#ifdef Q_SPY
		.Mode = GPIO_MODE_AF_PP,
		.Alternate = GPIO_AF4_USART2,
		.Speed = GPIO_SPEED_FREQ_VERY_HIGH,
#else
		.Mode = GPIO_MODE_OUTPUT_PP,
		.Speed = GPIO_SPEED_FREQ_LOW,
#endif
	},
	.isr = NULL
};

static GPIOPin pin_C_AUX1 = {
	.port = C_AUX1_GPIO_Port,
	.cfg = {
		.Pin = C_AUX1_Pin,
		.Mode = GPIO_MODE_OUTPUT_PP,
		.Pull = GPIO_NOPULL,
		.Speed = GPIO_SPEED_FREQ_LOW
	},
	.isr = NULL
};

static GPIOPin pin_C_AUX2 = {
	.port = C_AUX2_GPIO_Port,
	.cfg = {
		.Pin = C_AUX2_Pin,
		.Mode = GPIO_MODE_OUTPUT_PP,
		.Pull = GPIO_NOPULL,
		.Speed = GPIO_SPEED_FREQ_LOW
	},
	.isr = NULL
};

/* DCC Pins ----------------------------------------------------------------- */
static GPIOPin pin_DCC_DATA = {
	.port = DCC_DATA_GPIO_Port,
	.cfg = {
		.Pin = DCC_DATA_Pin,
		.Mode = GPIO_MODE_IT_RISING_FALLING,
		.Pull = GPIO_PULLUP
	}
};

/* Motor dirver Pins -------------------------------------------------------- */
static GPIOPin pin_nFAULT = {
	.port = nFAULT_GPIO_Port,
	.cfg = {
		.Pin = nFAULT_Pin,
		.Mode = GPIO_MODE_INPUT,
		.Pull = GPIO_NOPULL
	}
};

static GPIOPin pin_IN1 = {
	.port = IN_1_GPIO_Port,
	.cfg = {
		.Pin = IN_1_Pin,
		.Mode = GPIO_MODE_AF_PP,
		.Pull = GPIO_NOPULL,
		.Speed = GPIO_SPEED_FREQ_VERY_HIGH,
		.Alternate = GPIO_AF5_TIM22
	}
};

static GPIOPin pin_IN2 = {
	.port = IN_2_GPIO_Port,
	.cfg = {
		.Pin = IN_2_Pin,
		.Mode = GPIO_MODE_AF_PP,
		.Pull = GPIO_NOPULL,
		.Speed = GPIO_SPEED_FREQ_VERY_HIGH,
		.Alternate = GPIO_AF5_TIM22
	}
};

/* Private helper functions definitions ------------------------------------- */

static inline void init_pin(GPIOPin * const pin)
{
	/* Pre-conditions */
	Q_REQUIRE(NULL != pin);

	HAL_GPIO_Init(pin->port, &pin->cfg);
}

static inline void write_pin(GPIOPin * const pin, GPIO_PinState const state)
{
	/* Pre-conditions */
	Q_REQUIRE(NULL != pin);

	HAL_GPIO_WritePin(pin->port, pin->cfg.Pin, state);
}

/* Private initialization functions ----------------------------------------- */

static void PluX16_GPIO_Init(void)
{
	/* Init pins */
	init_pin(&pin_C_FOF);
	init_pin(&pin_C_FOR);
	init_pin(&pin_C_GPIOA);
	init_pin(&pin_C_GPIOB);
	init_pin(&pin_C_AUX1);
	init_pin(&pin_C_AUX2);
}

static void DCC_GPIO_Init(void)
{
	init_pin(&pin_DCC_DATA);
}

static void Motor_GPIO_Init(void)
{
	/**
	 * TIM22 GPIO Configuration
	 * PA6     ------> TIM22_CH1
	 * PA7     ------> TIM22_CH2
	 */

	init_pin(&pin_nFAULT);
	init_pin(&pin_IN1);
	init_pin(&pin_IN2);
}

/* BSP GPIO Functions ------------------------------------------------------- */

void BSP_InitGPIOs(void)
{
	/* Enable clock for used GPIO Ports */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	PluX16_GPIO_Init();
	DCC_GPIO_Init();
	Motor_GPIO_Init();
}

void BSP_SetCFOF_pin(GPIO_PinState const state)
{
	write_pin(&pin_C_FOF, state);
}

void BSP_SetCFOR_pin(GPIO_PinState const state)
{
	write_pin(&pin_C_FOR, state);
}

void BSP_SetAUX1_pin(GPIO_PinState const state)
{
	write_pin(&pin_C_AUX1, state);
}

void BSP_SetAUX2_pin(GPIO_PinState const state)
{
	write_pin(&pin_C_AUX2, state);
}

void BSP_SetGPIOA_pin(GPIO_PinState const state)
{
	write_pin(&pin_C_GPIOA, state);
}

void BSP_SetGPIOB_pin(GPIO_PinState const state)
{
	write_pin(&pin_C_GPIOB, state);
}

void BSP_RegisterDCCExtIIsr(void (*isr)(void))
{
	pin_DCC_DATA.isr = isr;
}

/* ISR ---------------------------------------------------------------------- */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == pin_DCC_DATA.cfg.Pin) {
		if (pin_DCC_DATA.isr != NULL) {
			pin_DCC_DATA.isr();
		}
	}
}
