
#include <avr/io.h>
#include <avr/interrupt.h>
#include "pwm.h"
#include "pwm2.h"
#include "pwm3.h"

void setup();
void initADC();

// Valores filtrados de cada canal
volatile uint16_t adc_val1 = 0;
volatile uint16_t adc_val2 = 0;
volatile uint16_t adc_val3 = 0;

// Valores anteriores para histeresis (evita temblor)
static uint16_t last_val1 = 0;
static uint16_t last_val2 = 0;
static uint16_t last_val3 = 0;

// Canal actual: 0=A0, 1=A1, 2=A2
volatile uint8_t canal_actual = 0;

// Flag: descarta primera conversion tras cambio de canal
volatile uint8_t descartar = 0;

#define HISTERESIS  4   // umbral minimo de cambio en el ADC (0-1023)
                        // ajusta este valor si el servo sigue temblando

int main(void)
{
    cli();
    setup();
    initADC();
    initPWM();
    initPWM2();
    initPWM3();
    sei();

    while(1)
    {
        // Todo se maneja desde ISR(ADC_vect)
    }
}

void setup()
{
    DDRB  |=  (1 << DDB1);
    PORTB &= ~(1 << PB1);
    // PB2 lo configura initPWM2()
    // PD5 lo configura initPWM3()
}

void initADC()
{
    ADMUX  = 0;
    ADMUX |= (1<<REFS0);              // referencia 5V, inicia en A0

    ADCSRA  = 0;
    ADCSRA |= (1<<ADEN)
            | (1<<ADIE)
            | (1<<ADPS2)
            | (1<<ADPS1)
            | (1<<ADPS0);            // prescaler 128

    descartar = 1;                   // descarta la primera lectura
    ADCSRA |= (1<<ADSC);
}

ISR(ADC_vect)
{
    // Si acabamos de cambiar de canal, descartamos esta conversion
    if(descartar)
    {
        descartar = 0;
        ADCSRA |= (1<<ADSC);         // lanza conversion valida
        return;
    }

    uint16_t val = ADC;

    if(canal_actual == 0)
    {
        // Histeresis: solo actualiza servo si cambio es significativo
        if((val > last_val1 + HISTERESIS) || (val + HISTERESIS < last_val1))
        {
            last_val1 = val;
            adc_val1  = val;
            setServo1(adc_val1);
        }
        canal_actual = 1;
        ADMUX = (1<<REFS0) | 0x01;   // cambia a A1
    }
    else if(canal_actual == 1)
    {
        if((val > last_val2 + HISTERESIS) || (val + HISTERESIS < last_val2))
        {
            last_val2 = val;
            adc_val2  = val;
            setServo2_T2(adc_val2);
        }
        canal_actual = 2;
        ADMUX = (1<<REFS0) | 0x02;   // cambia a A2
    }
    else
    {
        if((val > last_val3 + HISTERESIS) || (val + HISTERESIS < last_val3))
        {
            last_val3 = val;
            adc_val3  = val;
            setLED(adc_val3);
        }
        canal_actual = 0;
        ADMUX = (1<<REFS0) | 0x00;   // regresa a A0
    }

    descartar  = 1;                  // descarta primera conv del nuevo canal
    ADCSRA    |= (1<<ADSC);
}
