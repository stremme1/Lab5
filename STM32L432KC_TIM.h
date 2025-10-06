// STM32F401RE_TIM.h
// Header for TIM functions

#ifndef STM32L4_TIM_H
#define STM32L4_TIM_H

#include <stdint.h> // Include stdint header
#include <stm32l432xx.h>
#include "STM32L432KC_GPIO.h"

///////////////////////////////////////////////////////////////////////////////
// Function prototypes
///////////////////////////////////////////////////////////////////////////////

void initTIM(TIM_TypeDef * TIMx);
void delay_millis(TIM_TypeDef * TIMx, uint32_t ms);
void delay_micros(TIM_TypeDef * TIMx, uint32_t us);
void enableTIMInterrupt(TIM_TypeDef * TIMx, uint32_t period_ms);
void disableTIMInterrupt(TIM_TypeDef * TIMx);
void clearTIMFlag(TIM_TypeDef * TIMx);
int checkTIMFlag(TIM_TypeDef * TIMx);

#endif
