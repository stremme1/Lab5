#ifndef STM32L432KC_EXTI_H
#define STM32L432KC_EXTI_H

#include "STM32L432KC.h"

// EXTI trigger types
#define EXTI_RISING_EDGE    0x01
#define EXTI_FALLING_EDGE   0x02
#define EXTI_BOTH_EDGES     0x03

// Function prototypes
void enableEXTI(int gpio_pin, int trigger_type);
void disableEXTI(int gpio_pin);
void clearEXTIFlag(int gpio_pin);
int checkEXTIFlag(int gpio_pin);

#endif
