# Lab 5: Quadrature Encoder System Flowchart

## System Overview
This flowchart shows the interrupt-driven architecture of the quadrature encoder speed measurement system, including the main loop and all interrupt service routines.

## Main System Flow

```
┌─────────────────────────────────────────────────────────────────┐
│                        SYSTEM STARTUP                          │
└─────────────────────┬───────────────────────────────────────────┘
                      │
                      ▼
┌─────────────────────────────────────────────────────────────────┐
│                    MAIN INITIALIZATION                          │
│  • SystemInit()                                                 │
│  • Enable clocks (GPIOA, GPIOB, SYSCFG, TIM2)                  │
│  • Configure GPIO pins (PA0, PA1, LED)                         │
│  • Setup pull-up resistors for encoder inputs                  │
│  • Initialize timer (TIM2) with 100ms period                   │
│  • Read initial encoder state                                   │
│  • Setup external interrupts (EXTI0, EXTI1)                    │
│  • Enable global interrupts (__enable_irq())                   │
│  • Print startup messages                                       │
└─────────────────────┬───────────────────────────────────────────┘
                      │
                      ▼
┌─────────────────────────────────────────────────────────────────┐
│                        MAIN LOOP                                │
│                                                                 │
│  while(1) {                                                     │
│      delay_millis(TIM2, 100);  // 100ms delay                  │
│  }                                                              │
│                                                                 │
│  Note: Main loop is mostly idle, system runs on interrupts     │
└─────────────────────┬───────────────────────────────────────────┘
                      │
                      │    ┌─────────────────────────────────────┐
                      │    │        INTERRUPT EVENTS             │
                      │    │                                     │
                      │    │  ┌─────────────┐  ┌─────────────┐   │
                      │    │  │   EXTI0     │  │   EXTI1     │   │
                      │    │  │ (Encoder A) │  │ (Encoder B) │   │
                      │    │  │ PA0 Changes │  │ PA1 Changes │   │
                      │    │  └─────────────┘  └─────────────┘   │
                      │    │                                     │
                      │    │  ┌─────────────┐                    │
                      │    │  │    TIM2     │                    │
                      │    │  │ 100ms Timer │                    │
                      │    │  │   Expires   │                    │
                      │    │  └─────────────┘                    │
                      │    └─────────────────────────────────────┘
                      │
                      │
                      ▼
```

## Interrupt Service Routines (ISRs)

### 1. EXTI0_IRQHandler (Encoder A - PA0)
```
┌─────────────────────────────────────────────────────────────────┐
│                    EXTI0_IRQHandler                            │
│                     (Encoder A - PA0)                          │
└─────────────────────┬───────────────────────────────────────────┘
                      │
                      ▼
┌─────────────────────────────────────────────────────────────────┐
│  if (checkEXTIFlag(ENCODER_A)) {                               │
│      clearEXTIFlag(ENCODER_A);                                 │
│      processEncoder();                                          │
│  }                                                              │
└─────────────────────┬───────────────────────────────────────────┘
                      │
                      ▼
┌─────────────────────────────────────────────────────────────────┐
│                    processEncoder()                             │
│                                                                 │
│  1. Read current encoder state:                                │
│     new_state = (digitalRead(ENCODER_A) << 1) |                │
│                  digitalRead(ENCODER_B)                        │
│                                                                 │
│  2. Check for state transition:                                │
│     if (encoder_state != new_state) {                          │
│                                                                 │
│  3. Apply quadrature decoding:                                 │
│     • CW transitions (0→1, 1→3, 3→2, 2→0): encoder_count++     │
│     • CCW transitions (0→2, 2→3, 3→1, 1→0): encoder_count--    │
│                                                                 │
│  4. Update encoder_state = new_state                           │
└─────────────────────┬───────────────────────────────────────────┘
                      │
                      ▼
┌─────────────────────────────────────────────────────────────────┐
│                    RETURN TO MAIN LOOP                          │
└─────────────────────────────────────────────────────────────────┘
```

### 2. EXTI1_IRQHandler (Encoder B - PA1)
```
┌─────────────────────────────────────────────────────────────────┐
│                    EXTI1_IRQHandler                            │
│                     (Encoder B - PA1)                          │
└─────────────────────┬───────────────────────────────────────────┘
                      │
                      ▼
┌─────────────────────────────────────────────────────────────────┐
│  if (checkEXTIFlag(ENCODER_B)) {                               │
│      clearEXTIFlag(ENCODER_B);                                 │
│      processEncoder();                                          │
│  }                                                              │
└─────────────────────┬───────────────────────────────────────────┘
                      │
                      ▼
┌─────────────────────────────────────────────────────────────────┐
│                    processEncoder()                             │
│                     (Same as EXTI0)                            │
│                                                                 │
│  • Read encoder state                                           │
│  • Check for transitions                                        │
│  • Apply quadrature decoding                                    │
│  • Update encoder_state                                         │
└─────────────────────┬───────────────────────────────────────────┘
                      │
                      ▼
┌─────────────────────────────────────────────────────────────────┐
│                    RETURN TO MAIN LOOP                          │
└─────────────────────────────────────────────────────────────────┘
```

### 3. TIM2_IRQHandler (Velocity Calculation)
```
┌─────────────────────────────────────────────────────────────────┐
│                    TIM2_IRQHandler                             │
│                   (100ms Timer Expires)                        │
└─────────────────────┬───────────────────────────────────────────┘
                      │
                      ▼
┌─────────────────────────────────────────────────────────────────┐
│  if (checkTIMFlag(TIM2)) {                                     │
│      clearTIMFlag(TIM2);                                       │
│      updateVelocity();                                          │
│  }                                                              │
└─────────────────────┬───────────────────────────────────────────┘
                      │
                      ▼
┌─────────────────────────────────────────────────────────────────┐
│                    updateVelocity()                             │
│                                                                 │
│  1. Calculate count difference:                                │
│     count_diff = encoder_count - last_count                    │
│                                                                 │
│  2. Calculate speed (integer math):                            │
│     speed_thousandths = (count_diff * 1000) /                  │
│                         (PPR * EDGES_PER_PULSE * 0.1)          │
│                                                                 │
│  3. Determine direction:                                        │
│     if (count_diff > 0): direction = 1 (CW)                    │
│     if (count_diff < 0): direction = -1 (CCW)                  │
│     if (count_diff == 0): direction = 0 (Stopped)              │
│                                                                 │
│  4. Print results:                                              │
│     printf("Count: %d, Diff: %d, Speed: %d.%03d rev/s,         │
│             Direction: %s\n", ...)                             │
│                                                                 │
│  5. Update last_count = encoder_count                          │
│                                                                 │
│  6. Toggle LED (visual indicator)                              │
└─────────────────────┬───────────────────────────────────────────┘
                      │
                      ▼
┌─────────────────────────────────────────────────────────────────┐
│                    RETURN TO MAIN LOOP                          │
└─────────────────────────────────────────────────────────────────┘
```

## Quadrature Decoding State Machine

```
┌─────────────────────────────────────────────────────────────────┐
│                QUADRATURE ENCODER STATES                       │
│                                                                 │
│  State 0: A=0, B=0    State 1: A=0, B=1                       │
│  ┌─────────────┐      ┌─────────────┐                         │
│  │ A: 0        │      │ A: 0        │                         │
│  │ B: 0        │      │ B: 1        │                         │
│  └─────────────┘      └─────────────┘                         │
│         │                    │                                 │
│         │ CW                 │ CW                              │
│         ▼                    ▼                                 │
│  State 3: A=1, B=1    State 2: A=1, B=0                       │
│  ┌─────────────┐      ┌─────────────┐                         │
│  │ A: 1        │      │ A: 1        │                         │
│  │ B: 1        │      │ B: 0        │                         │
│  └─────────────┘      └─────────────┘                         │
│                                                                 │
│  Valid CW Transitions:  0→1→3→2→0                              │
│  Valid CCW Transitions: 0→2→3→1→0                              │
└─────────────────────────────────────────────────────────────────┘
```

## System Timing Diagram

```
Time:     0ms    100ms   200ms   300ms   400ms   500ms
          │       │       │       │       │       │
          ▼       ▼       ▼       ▼       ▼       ▼
Main:     │       │       │       │       │       │
Loop:     └───────┴───────┴───────┴───────┴───────┴───────
          │       │       │       │       │       │
EXTI0:    │   A   │   A   │   A   │   A   │   A   │
(PA0):    └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─
          │       │       │       │       │       │
EXTI1:    │   B   │   B   │   B   │   B   │   B   │
(PA1):    └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─
          │       │       │       │       │       │
TIM2:     │       │       │       │       │       │
(100ms):  └───────┴───────┴───────┴───────┴───────┴───────
          │       │       │       │       │       │
          ▼       ▼       ▼       ▼       ▼       ▼
Output:   │  Speed │  Speed │  Speed │  Speed │  Speed │
          │  Calc  │  Calc  │  Calc  │  Calc  │  Calc  │
```

## Key Features

1. **Interrupt-Driven Architecture**: System responds immediately to encoder changes
2. **4x Resolution**: Both rising and falling edges detected
3. **Robust Quadrature Decoding**: State machine prevents false readings
4. **Real-time Speed Calculation**: Updates every 100ms
5. **Clean Library Functions**: All bit manipulation abstracted away
6. **ITM Debug Output**: printf works through SEGGER debug interface

## Performance Characteristics

- **Interrupt Latency**: < 1μs (immediate response to encoder changes)
- **Update Rate**: 10 Hz (100ms timer period)
- **Resolution**: 480 counts/revolution (120 PPR × 4 edges)
- **Maximum Speed**: Limited by interrupt processing time
- **Accuracy**: ±1 count (0.2% at 1 rev/s)
