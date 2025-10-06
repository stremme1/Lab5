// STM32F401RE_TIM.c
// TIM functions

#include "STM32L432KC_TIM.h"
#include "STM32L432KC_RCC.h"

void initTIM(TIM_TypeDef * TIMx){
  // Set prescaler to give 1 ms time base
  uint32_t psc_div = (uint32_t) ((SystemCoreClock/1e3));

  // Set prescaler division factor
  TIMx->PSC = (psc_div - 1);
  // Generate an update event to update prescaler value
  TIMx->EGR |= 1;
  // Enable counter
  TIMx->CR1 |= 1; // Set CEN = 1
}

void delay_millis(TIM_TypeDef * TIMx, uint32_t ms){
  TIMx->ARR = ms;// Set timer max count
  TIMx->EGR |= 1;     // Force update
  TIMx->SR &= ~(0x1); // Clear UIF
  TIMx->CNT = 0;      // Reset count

  while(!(TIMx->SR & 1)); // Wait for UIF to go high
}

void delay_micros(TIM_TypeDef * TIMx, uint32_t us){
  TIMx->ARR = us;// Set timer max count
  TIMx->EGR |= 1;     // Force update
  TIMx->SR &= ~(0x1); // Clear UIF
  TIMx->CNT = 0;      // Reset count

  while(!(TIMx->SR & 1)); // Wait for UIF to go high
}

// Enable timer interrupt with specified period
void enableTIMInterrupt(TIM_TypeDef * TIMx, uint32_t period_ms) {
    // Set ARR for the desired period
    TIMx->ARR = period_ms - 1;
    
    // Enable update interrupt
    TIMx->DIER |= TIM_DIER_UIE;
    
    // Enable NVIC interrupt based on timer
    if (TIMx == TIM2) {
        NVIC->ISER[0] |= (1 << TIM2_IRQn);
    } else if (TIMx == TIM6) {
        NVIC->ISER[0] |= (1 << TIM6_DAC_IRQn);
    } else if (TIMx == TIM7) {
        NVIC->ISER[0] |= (1 << TIM7_IRQn);
    }
}

// Disable timer interrupt
void disableTIMInterrupt(TIM_TypeDef * TIMx) {
    // Disable update interrupt
    TIMx->DIER &= ~TIM_DIER_UIE;
    
    // Disable NVIC interrupt based on timer
    if (TIMx == TIM2) {
        NVIC->ICER[0] |= (1 << TIM2_IRQn);
    } else if (TIMx == TIM6) {
        NVIC->ICER[0] |= (1 << TIM6_DAC_IRQn);
    } else if (TIMx == TIM7) {
        NVIC->ICER[0] |= (1 << TIM7_IRQn);
    }
}

// Clear timer flag
void clearTIMFlag(TIM_TypeDef * TIMx) {
    TIMx->SR &= ~TIM_SR_UIF;
}

// Check if timer flag is set
int checkTIMFlag(TIM_TypeDef * TIMx) {
    return (TIMx->SR & TIM_SR_UIF) ? 1 : 0;
}
