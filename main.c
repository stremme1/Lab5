// main.c
// Lab 5: Quadrature Encoder Motor Speed Measurement
// Self-contained implementation using copied libraries
// 
// Author: Emmett Stralka
// Email: estralka@hmc.edu
// Date: 9/5/25

#include "STM32L432KC.h"

// Pin definitions (5V tolerant pins)
#define ENCODER_A PA0  // Channel A (5V tolerant)
#define ENCODER_B PA1  // Channel B (5V tolerant)
#define LED PB3        // Status LED

// Encoder parameters
#define PPR 120        // Pulses per revolution (25GA-370 motor)
#define EDGES_PER_PULSE 4  // Using all edges for 4x resolution

// Global variables
volatile uint32_t encoder_count = 0;
volatile uint32_t last_count = 0;
volatile int32_t speed_rev_per_sec = 0;
volatile int8_t direction = 0;  // 1=CW, -1=CCW, 0=stopped
volatile int encoder_state = 0;  // Combined A and B state

// Function prototypes
void setupEncoderInterrupts(void);
void updateVelocity(void);
void processEncoder(void);

int main(void) {
    // System initialization
    SystemInit();
    
    // Enable clocks
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOBEN;
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
    
    // Configure GPIO
    pinMode(ENCODER_A, GPIO_INPUT);
    pinMode(ENCODER_B, GPIO_INPUT);
    pinMode(LED, GPIO_OUTPUT);
    pinResistor(ENCODER_A, GPIO_PULL_UP);
    pinResistor(ENCODER_B, GPIO_PULL_UP);
    
    // Setup timer (0.1Hz updates for better resolution)
    initTIM(TIM2);
    enableTIMInterrupt(TIM2, 100);  // 100ms period
    
    // Initialize encoder state
    encoder_state = (digitalRead(ENCODER_A) << 1) | digitalRead(ENCODER_B);
    
    // Setup encoder interrupts
    setupEncoderInterrupts();
    
    // Enable global interrupts
    __enable_irq();
    
    printf("Lab 5: Quadrature Encoder Speed Measurement\n");
    printf("Encoder A: PA0, Encoder B: PA1\n");
    printf("Ready for motor test (expected ~10 rev/s)\n\n");
    
    // Main loop
    while(1) {
        delay_millis(TIM2, 100);
    }
}

void setupEncoderInterrupts(void) {
    // Configure EXTI for both encoder channels with both edges
    enableEXTI(ENCODER_A, EXTI_BOTH_EDGES);
    enableEXTI(ENCODER_B, EXTI_BOTH_EDGES);
}

void updateVelocity(void) {
    // Calculate velocity every second
    int32_t count_diff = encoder_count - last_count;
    
    // Determine direction
    if (count_diff > 0) {
        direction = 1;  // Clockwise
    } else if (count_diff < 0) {
        direction = -1; // Counter-clockwise
    } else {
        direction = 0;  // Stopped
    }
    
    // Calculate velocity in rev/s using integer math
    // Using all edges: 4 pulses per encoder pulse
    // Timer period is 0.1s, so multiply by 10 to get rev/s
    // Calculate speed in thousandths of rev/s for better precision
    int32_t speed_thousandths = (count_diff * 10000) / (PPR * EDGES_PER_PULSE);
    speed_rev_per_sec = speed_thousandths / 1000;  // Integer part
    
    // Update for next calculation
    last_count = encoder_count;
    
    // Display results with debugging info (integer math)
    if (speed_thousandths >= 0) {
        printf("Count: %ld, Diff: %ld, Speed: %ld.%03ld rev/s, Direction: ", 
               encoder_count, count_diff, speed_thousandths/1000, speed_thousandths%1000);
    } else {
        printf("Count: %ld, Diff: %ld, Speed: -%ld.%03ld rev/s, Direction: ", 
               encoder_count, count_diff, (-speed_thousandths)/1000, (-speed_thousandths)%1000);
    }
    if (direction > 0) {
        printf("CW (Clockwise)\n");
    } else if (direction < 0) {
        printf("CCW (Counter-clockwise)\n");
    } else {
        printf("Stopped\n");
    }
    
    // Toggle LED to show system is running
    togglePin(LED);
}

// Simple quadrature decoding function
void processEncoder(void) {
    // Read current encoder state (A=bit1, B=bit0)
    int new_state = (digitalRead(ENCODER_A) << 1) | digitalRead(ENCODER_B);
    

    if (encoder_state != new_state) {
        // Check for valid transitions
        if ((encoder_state == 0 && new_state == 1) ||
            (encoder_state == 1 && new_state == 3) ||
            (encoder_state == 3 && new_state == 2) ||
            (encoder_state == 2 && new_state == 0)) {
            encoder_count++;  // CW
        } else if ((encoder_state == 0 && new_state == 2) ||
                   (encoder_state == 2 && new_state == 3) ||
                   (encoder_state == 3 && new_state == 1) ||
                   (encoder_state == 1 && new_state == 0)) {
            encoder_count--;  // CCW
        }
        encoder_state = new_state;
    }
}

// EXTI0 interrupt handler (Encoder A)
void EXTI0_IRQHandler(void) {
    if (checkEXTIFlag(ENCODER_A)) {
        clearEXTIFlag(ENCODER_A);
        processEncoder();
    }
}

// EXTI1 interrupt handler (Encoder B)
void EXTI1_IRQHandler(void) {
    if (checkEXTIFlag(ENCODER_B)) {
        clearEXTIFlag(ENCODER_B);
        processEncoder();
    }
}

// TIM2 interrupt handler (Velocity calculation and display)
void TIM2_IRQHandler(void) {
    if (checkTIMFlag(TIM2)) {
        clearTIMFlag(TIM2);
        updateVelocity();
    }
}

// Function used by printf to send characters to the laptop (ITM debug channel)
int _write(int file, char *ptr, int len) {
    int i = 0;
    for (i = 0; i < len; i++) {
        ITM_SendChar((*ptr++));
    }
    return len;
}
