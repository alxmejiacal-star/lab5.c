#ifndef PWM2_H_
#define PWM2_H_
#include <avr/io.h>
#include <avr/interrupt.h>

#define SERVO2_MIN  16   // 1.0 ms → 0°
#define SERVO2_MAX  31   // 2.0 ms → 180°
#define SERVO2_PER  255  // periodo completo en overflows

volatile uint8_t servo2_pulse = 23; // 90° al inicio
static  uint8_t  servo2_last  = 23;

void initPWM2();
void setServo2_T2(uint16_t adc);

ISR(TIMER2_OVF_vect)
{
    static uint8_t cnt = 0;

    cnt++;

    if(cnt == 1)
    {
        PORTB |= (1 << PB2);          // inicio del periodo: sube pin
    }
    else if(cnt >= servo2_pulse)
    {
        PORTB &= ~(1 << PB2);         // fin del pulso: baja pin
    }

    if(cnt >= SERVO2_PER)
    {
        cnt = 0;                       // reinicia el periodo
    }
}

void initPWM2()
{
    DDRB  |=  (1 << DDB2);
    PORTB &= ~(1 << PB2);

    TCCR2A = 0;                        // modo normal, sin OC2A/OC2B
    TCCR2B = 0;
    TCNT2  = 0;
    TCCR2B |= (1<<CS22)|(1<<CS21)|(1<<CS20); // prescaler 1024
    TIMSK2 |= (1 << TOIE2);           // solo overflow, sin OCIE2A
}

void setServo2_T2(uint16_t adc)
{
    uint8_t nuevo = SERVO2_MIN +
                    ((uint32_t)adc * (SERVO2_MAX - SERVO2_MIN)) / 1023;
    if(nuevo != servo2_last)
    {
        servo2_last  = nuevo;
        servo2_pulse = nuevo;          // escritura atomica (uint8_t en AVR)
    }
}

#endif
