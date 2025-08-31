#include <avr/io.h>
#include<avr/interrupt.h>

#define LEDG_HIGH (PORTD |= 1<<PIND4)
#define LEDG_LOW (PORTD &=~(1<<PIND4))
#define LEDG_TOGGLE (PORTD ^=(1<<PIND4))

#define LEDR_HIGH (PORTD |= 1<<PIND5)
#define LEDR_LOW (PORTD &=~(1<<PIND5))
#define LEDR_TOGGLE (PORTD ^=(1<<PIND5))

#pragma message ("test")

volatile uint8_t stepperState=0;
volatile uint16_t remainingSteps=2048;

int main(void){
    DDRD |= (1 << PIND4) ; //LEDG init
    DDRB |= (1 << PIND5) ; //LEDR init
    DDRD&=~((1<<PIND2) | (1<<PIND3));  //INT0 & INT1 init
    DDRB=0xFF;
    PORTB=0;
    LEDG_LOW;
    LEDR_LOW;

    
    OCR0A = 155;    // 4MHz/64/(155+1) = ~400Hz
    OCR1A = 31249;  // 4MHz/1024/(31249+1=8s
    //1 day = 10800 * 8s


    TCCR0A = (1 << WGM01);
    TCCR0B = (1 << CS01) | (1 << CS00);// prescaler 64, clear timer on OCR0A match
    TCCR1B |= (1 << WGM12) | (1 << CS12) | (1 << CS10);// prescaler 1024, clear timer on OCR1A match
    TIMSK |= (1 << OCIE1A) | (1 << OCIE0A);
    GIMSK |= (1<<INT1) | (1<<INT0);
    MCUCR |=(1<<ISC11) | (1<<ISC01);//falling edge of INT1 generates interrupt
    sei();

    while(1){
    }
    return 0;
}     

ISR (INT0_vect){
    
    if(remainingSteps==0){
        remainingSteps+=206;
        LEDR_HIGH;
    }
}

ISR (INT1_vect){
    LEDG_TOGGLE;
}


ISR (TIMER1_COMPA_vect){
}

ISR(TIMER0_COMPA_vect){
    if(remainingSteps>0){
        remainingSteps--;
        if(--stepperState>3)stepperState=3;
        PORTB=1<<stepperState;
    }else{
        PORTB=0;
        LEDR_LOW;
    }
}
