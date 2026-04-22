#ifndef PWM3_H_
#define PWM3_H_
#include <avr/io.h>
#include <avr/interrupt.h>

#define PWM3_TOP  255

volatile uint8_t led_duty = 0;

void initPWM3();
void setLED(uint16_t adc);

ISR(TIMER0_OVF_vect)
{
    static uint8_t counter = 0;

    if(counter == 0)
    {
        PORTD |=  (1 << PD6);   // ← corregido PD5 → PD6
    }

    if(counter >= led_duty)
    {
        PORTD &= ~(1 << PD6);   // ← corregido PD5 → PD6
    }

    counter++;
}

void initPWM3()
{
    DDRD  |=  (1 << DDD6);      // ← corregido DDD5 → DDD6
    PORTD &= ~(1 << PD6);       // ← corregido PD5 → PD6

    TCCR0A = 0;
    TCCR0B = 0;
    TCNT0  = 0;
    TCCR0B |= (1<<CS01)|(1<<CS00); // prescaler 64
    TIMSK0 |= (1 << TOIE0);
}

void setLED(uint16_t adc)
{
    led_duty = (uint8_t)((uint32_t)adc * PWM3_TOP / 1023);
}

#endif
