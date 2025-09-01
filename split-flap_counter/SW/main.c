//Attiny2313 running at 4MHz.
//Fuse settings: -U lfuse:w:0xfd:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m 

#include <avr/io.h>
#include<avr/interrupt.h>

#define LEDG_HIGH (PORTD |= 1<<PIND4)
#define LEDG_LOW (PORTD &=~(1<<PIND4))
#define LEDG_TOGGLE (PORTD ^=(1<<PIND4))

#define LEDR_HIGH (PORTD |= 1<<PIND5)
#define LEDR_LOW (PORTD &=~(1<<PIND5))
#define LEDR_TOGGLE (PORTD ^=(1<<PIND5))

#define STEPPER_TIMER_CNT 155 // 4MHz/64/(155+1) = ~400Hz (stepper pulse rate)
#define DAILY_TIMER_CNT 31249 // 4MHz/1024/(31249+1=8s
#define DAILY_TIMER_OVF 10800 // 1 day = 10800 * 8s

#define STEPS_PER_FLAP 205 //2048=full turn, 205 steps per flap

#pragma message ("test")

volatile uint8_t stepperState=0;        //Keep track of the stepper driving mode phase.
volatile uint16_t remainingSteps=2048;  //Keep track of remaining steps to go
volatile uint16_t timer1OVF_cnt=0;      //Time keeping
volatile uint8_t zeroing=0;             //True when zeroing is ongoing.
void increment();

int main(void){
    DDRD |= (1 << PIND4) ;              //LEDG init
    DDRD |= (1 << PIND5) ;              //LEDR init
    DDRD&=~((1<<PIND2) | (1<<PIND3));   //INT0 & INT1 init for wheel 1&2 limit switches
    DDRB=0xFF;                          //PORTB drives the steppers (2*4 lines)
    PORTB=0;                            //Low=coils OFF
    LEDG_LOW;
    LEDR_LOW;

    OCR0A = STEPPER_TIMER_CNT;  //~400Hz
    OCR1A = DAILY_TIMER_CNT;    // 8s
    TCCR0A = (1 << WGM01);
    TCCR0B = (1 << CS01) | (1 << CS00);// prescaler 64, clear timer on OCR0A match
    TCCR1B |= (1 << WGM12) | (1 << CS12) | (1 << CS10);// prescaler 1024, clear timer on OCR1A match
    TIMSK |= (1 << OCIE1A) | (1 << OCIE0A); //Enable Timer compare interrupts
    GIMSK |= (1<<INT1) | (1<<INT0);         //Enable INT0 & INT1 interrupts
    MCUCR |=(1<<ISC11) | (1<<ISC01);        //Falling edges generates interrupts
    sei();                                  //Global interurpt enable

    while(1){
    }
    return 0;
}     

ISR (INT0_vect){//Unit wheel zero hit
    

}

ISR (INT1_vect){//Tens wheel zero hit
    LEDG_TOGGLE;
}


ISR (TIMER1_COMPA_vect){// 8s counter for day keeping
    if(++timer1OVF_cnt>=DAILY_TIMER_OVF){
        timer1OVF_cnt=0;
        increment();
    }
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
void increment(){
    if(remainingSteps==0){
        remainingSteps+=STEPS_PER_FLAP;
    }
}
