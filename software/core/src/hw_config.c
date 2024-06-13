#include <stm32l0xx_hal_tim.h>
#include <stm32l0xx_hal_adc.h>
#include <stm32l031xx.h>

static TIM_HandleTypeDef timer2 = {
	.Instance = TIM2,
	.Init = {
		.Prescaler		= 32-1,		/* 1MHz timer clk*/
		.Period 		= 65535,
		.CounterMode		= TIM_COUNTERMODE_UP,
		.AutoReloadPreload	= TIM_AUTORELOAD_PRELOAD_DISABLE,
		.ClockDivision		= TIM_CLOCKDIVISION_DIV1
	},
};

static TIM_HandleTypeDef timer22 = {
	.Instance			= TIM22,
	.Init = {
		.Prescaler		= 12-1,		/* 2.67 MHz */
		.Period			= 128,		/* 20.8 KHz PWM */
		.CounterMode		= TIM_COUNTERMODE_UP,
		.AutoReloadPreload	= TIM_AUTORELOAD_PRELOAD_ENABLE,
		.ClockDivision		= TIM_CLOCKDIVISION_DIV1
	}
};

static ADC_HandleTypeDef adc1 = {
	.Instance	= ADC1,
	.Init = {
		.ClockPrescaler		= ADC_CLOCK_ASYNC_DIV32,
		.Resolution		= ADC_RESOLUTION12b,
		.DataAlign		= ADC_DATAALIGN_RIGHT,
		.ScanConvMode		= ADC_SCAN_DIRECTION_FORWARD,
		.ContinuousConvMode	= DISABLE,
		.DiscontinuousConvMode	= DISABLE,
		.DMAContinuousRequests	= DISABLE,
		.EOCSelection 		= ADC_EOC_SINGLE_CONV,
		.Overrun 		= ADC_OVR_DATA_PRESERVED,
		.LowPowerAutoWait	= DISABLE,
		.LowPowerFrequencyMode	= ENABLE,
		.LowPowerAutoPowerOff	= DISABLE,
		.OversamplingMode	= DISABLE,
		.SamplingTime		= ADC_SAMPLETIME_7CYCLES_5,
		.ExternalTrigConvEdge	= ADC_EXTERNALTRIGCONVEDGE_NONE,
		.ExternalTrigConv	= ADC_SOFTWARE_START
	},
	.DMA_Handle = NULL
};
