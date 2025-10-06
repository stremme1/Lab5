#include "STM32L432KC_EXTI.h"

// Enable external interrupt for a GPIO pin
void enableEXTI(int gpio_pin, int trigger_type) {
    // Get pin offset and port
    int pin_offset = gpioPinOffset(gpio_pin);
    int port = gpioPinToPort(gpio_pin);
    
    // Enable SYSCFG clock
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    
    // Configure EXTICR register
    int exti_reg = pin_offset / 4;  // Which EXTICR register (0, 1, 2, or 3)
    int exti_pos = (pin_offset % 4) * 4;  // Position within the register
    
    // Clear and set the port selection bits
    SYSCFG->EXTICR[exti_reg] &= ~(0xF << exti_pos);
    SYSCFG->EXTICR[exti_reg] |= (port << exti_pos);
    
    // Configure trigger type
    if (trigger_type & EXTI_RISING_EDGE) {
        EXTI->RTSR1 |= (1 << pin_offset);
    } else {
        EXTI->RTSR1 &= ~(1 << pin_offset);
    }
    
    if (trigger_type & EXTI_FALLING_EDGE) {
        EXTI->FTSR1 |= (1 << pin_offset);
    } else {
        EXTI->FTSR1 &= ~(1 << pin_offset);
    }
    
    // Enable interrupt mask
    EXTI->IMR1 |= (1 << pin_offset);
    
    // Enable NVIC interrupt
    if (pin_offset <= 4) {
        NVIC->ISER[0] |= (1 << (EXTI0_IRQn + pin_offset));
    } else if (pin_offset <= 9) {
        NVIC->ISER[0] |= (1 << EXTI9_5_IRQn);
    } else if (pin_offset <= 15) {
        NVIC->ISER[0] |= (1 << EXTI15_10_IRQn);
    }
}

// Disable external interrupt for a GPIO pin
void disableEXTI(int gpio_pin) {
    int pin_offset = gpioPinOffset(gpio_pin);
    
    // Disable interrupt mask
    EXTI->IMR1 &= ~(1 << pin_offset);
    
    // Disable NVIC interrupt
    if (pin_offset <= 4) {
        NVIC->ICER[0] |= (1 << (EXTI0_IRQn + pin_offset));
    } else if (pin_offset <= 9) {
        NVIC->ICER[0] |= (1 << EXTI9_5_IRQn);
    } else if (pin_offset <= 15) {
        NVIC->ICER[0] |= (1 << EXTI15_10_IRQn);
    }
}

// Clear EXTI pending flag
void clearEXTIFlag(int gpio_pin) {
    int pin_offset = gpioPinOffset(gpio_pin);
    EXTI->PR1 |= (1 << pin_offset);
}

// Check if EXTI flag is set
int checkEXTIFlag(int gpio_pin) {
    int pin_offset = gpioPinOffset(gpio_pin);
    return (EXTI->PR1 & (1 << pin_offset)) ? 1 : 0;
}
