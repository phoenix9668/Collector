#ifndef __BSP_ADC_H
#define	__BSP_ADC_H

#include "stm32f4xx.h"

/*=====================sleep IO======================*/
// sleep IO宏定义
#define MMA7361L_SL_GPIO_PORT				GPIOE
#define MMA7361L_SL_GPIO_PIN				GPIO_Pin_7
#define MMA7361L_SL_GPIO_CLK				RCC_AHB1Periph_GPIOE

#define MMA7361L_SL_ON()       			GPIO_ResetBits(MMA7361L_SL_GPIO_PORT,MMA7361L_SL_GPIO_PIN)
#define MMA7361L_SL_OFF()        		GPIO_SetBits(MMA7361L_SL_GPIO_PORT,MMA7361L_SL_GPIO_PIN)

/*=====================g-select IO======================*/
// g-select IO宏定义
#define MMA7361L_GS_GPIO_PORT				GPIOC
#define MMA7361L_GS_GPIO_PIN				GPIO_Pin_13
#define MMA7361L_GS_GPIO_CLK				RCC_AHB1Periph_GPIOC

#define MMA7361L_GS_1G5()       		GPIO_ResetBits(MMA7361L_GS_GPIO_PORT,MMA7361L_GS_GPIO_PIN)
#define MMA7361L_GS_6G()        		GPIO_SetBits(MMA7361L_GS_GPIO_PORT,MMA7361L_GS_GPIO_PIN)

/*=====================通道1 IO======================*/
// ADC IO宏定义
#define MMA7361L_ADC1_GPIO_PORT			GPIOC
#define MMA7361L_ADC1_GPIO_PIN			GPIO_Pin_5
#define MMA7361L_ADC1_GPIO_CLK			RCC_AHB1Periph_GPIOC

// ADC 序号宏定义
#define MMA7361L_ADC1             	ADC1
#define MMA7361L_ADC1_CLK         	RCC_APB2Periph_ADC1
#define MMA7361L_ADC1_CHANNEL     	ADC_Channel_15
/*=====================通道2 IO ======================*/
// ADC IO宏定义
#define MMA7361L_ADC2_GPIO_PORT    	GPIOC
#define MMA7361L_ADC2_GPIO_PIN     	GPIO_Pin_1
#define MMA7361L_ADC2_GPIO_CLK     	RCC_AHB1Periph_GPIOC

// ADC 序号宏定义
#define MMA7361L_ADC2             	ADC2
#define MMA7361L_ADC2_CLK         	RCC_APB2Periph_ADC2
#define MMA7361L_ADC2_CHANNEL     	ADC_Channel_11
/*=====================通道3 IO ======================*/
// ADC IO宏定义
#define MMA7361L_ADC3_GPIO_PORT    	GPIOC
#define MMA7361L_ADC3_GPIO_PIN     	GPIO_Pin_2
#define MMA7361L_ADC3_GPIO_CLK     	RCC_AHB1Periph_GPIOC

// ADC 序号宏定义
#define MMA7361L_ADC3            		ADC3
#define MMA7361L_ADC3_CLK         	RCC_APB2Periph_ADC3
#define MMA7361L_ADC3_CHANNEL		   	ADC_Channel_12

// ADC CDR寄存器宏定义，ADC转换后的数字值则存放在这里
#define MMA7361L_ADC_CDR_ADDR    		((uint32_t)0x40012308)

// ADC DMA 通道宏定义，这里我们使用DMA传输
#define MMA7361L_ADC_DMA_CLK      	RCC_AHB1Periph_DMA2
#define MMA7361L_ADC_DMA_CHANNEL  	DMA_Channel_0
#define MMA7361L_ADC_DMA_STREAM   	DMA2_Stream0


void MMA7361L_Init(void);

#endif /* __BSP_ADC_H */



