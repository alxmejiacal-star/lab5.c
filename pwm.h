#ifndef PWM_H_
#define PWM_H_
#include <avr/io.h>

#define TOP_SERVO  40000
#define SERVO_MIN   2000   // 1.0 ms → 0°
#define SERVO_MAX   4000   // 2.0 ms → 180°  ← cambiado de 3000 a 4000

void initPWM();
void setServo1(uint16_t adc);

void initPWM()
{
    TCCR1A = 0;
    TCCR1B = 0;
    ICR1   = TOP_SERVO;
    TCCR1A |= (1<<COM1A1)|(1<<WGM11);
    TCCR1B |= (1<<WGM13)|(1<<WGM12)|(1<<CS11); // prescaler 8
    OCR1A   = (SERVO_MIN + SERVO_MAX) / 2;      // posicion inicial 90°
}

void setServo1(uint16_t adc)
{
    OCR1A = SERVO_MIN + ((uint32_t)adc * (SERVO_MAX - SERVO_MIN)) / 1023;
}

#endif
