//Attiny2313 running at 4MHz.
//Fuse settings: -U lfuse:w:0xfd:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m 

#include <avr/io.h>
#include<avr/interrupt.h>

#define LEDY_HIGH (PORTD |= 1<<PIND0)
#define LEDY_LOW (PORTD &=~(1<<PIND0))
#define LEDY_TOGGLE (PORTD ^=(1<<PIND0))

#define LEDR_HIGH (PORTD |= 1<<PIND1)
#define LEDR_LOW (PORTD &=~(1<<PIND1))
#define LEDR_TOGGLE (PORTD ^=(1<<PIND1))

#define BTN1_STATE (PIND & (1<<PIND4))
#define BTN2_STATE (PIND & (1<<PIND6))

#define STEPPER_TIMER_CNT 155 // 4MHz/64/(155+1) = ~400Hz (stepper pulse rate)
#define DAILY_TIMER_CNT 31249 // 4MHz/1024/(31249+1=8s
#define DAILY_TIMER_OVF 10800 // 1 day = 10800 * 8s

#define STEPS_PER_FLAP 202 //2048=full turn, 205 steps per flap
#define ZERO_STEP_OFFSET 120

#pragma message ("test")

volatile uint8_t stepperStateM1=0;        //Keep track of the stepper driving mode phase.
volatile uint8_t stepperStateM2=0;        //Keep track of the stepper driving mode phase.
volatile uint16_t remainingStepsM1=2048;  //Keep track of remaining steps to go
volatile uint16_t remainingStepsM2=2048;  //Keep track of remaining steps to go
volatile uint16_t timer1OVF_cnt=0;      //Time keeping
volatile uint8_t zeroing=0b11;             //True when zeroing is ongoing.
volatile uint8_t btn_debounce=0x0F;

int main(void){
    DDRD |=  (1 << PIND0) | (1 << PIND1);              //LEDY & LEDR init as outputs
    DDRD &=~((1 << PIND2) | (1 << PIND3) | (1 << PIND4) | (1 << PIND6));   //INT0 & INT1 init for wheel 1&2 limit switches inputs
    PORTD=0b1011100;                    //Enable input internal pullups
    DDRB=0xFF;                          //PORTB drives the steppers (2*4 lines)
    PORTB=0;                            //Low=coils OFF
    LEDY_LOW;
    LEDR_LOW;

    OCR0A = STEPPER_TIMER_CNT;  //~400Hz
    OCR1A = DAILY_TIMER_CNT;    // 8s
    TCCR0A = (1 << WGM01);
    TCCR0B = (1 << CS01)  | (1 << CS00);               // prescaler 64, clear timer on OCR0A match
    TCCR1B|= (1 << WGM12) | (1 << CS12) | (1 << CS10); // prescaler 1024, clear timer on OCR1A match
    TIMSK |= (1 << OCIE1A)| (1 << OCIE0A);             //Enable Timer compare interrupts
    GIMSK |= (1 << INT1)  | (1 << INT0);               //Enable INT0 & INT1 interrupts
    MCUCR |= (1<<SE) | (1 << ISC11) | (1 << ISC01);              //Falling edges generates interrupts
    
    
    ACSR=1<<7;  //Disable analog comparator for power saving
    
    sei();                                              //Global interurpt enable

    while(1){
        __asm__ __volatile__ ( "sleep" "\n\t" :: );
    }
    return 0;
}     

ISR (INT0_vect){//Unit wheel zero hit
    
    if(zeroing & 0b01)zeroing &=~ 1;
    remainingStepsM1 = ZERO_STEP_OFFSET;
        
    if((zeroing & 0b10)==0b00 && remainingStepsM2==0){
        remainingStepsM2 = STEPS_PER_FLAP;
        LEDR_HIGH;
    }

}

ISR (INT1_vect){//Tens wheel zero hit
    if(zeroing & 0b10){
        zeroing &= ~0b10;
        remainingStepsM2 = 1940;//2048 - ZERO_STEP_OFFSET;
    }else{
        remainingStepsM2 = 100;//ZERO_STEP_OFFSET;
    }
}


ISR (TIMER1_COMPA_vect){// 8s counter for day keeping
    if(++timer1OVF_cnt >= DAILY_TIMER_OVF){
        timer1OVF_cnt = 0;
        remainingStepsM1 = STEPS_PER_FLAP;
    }
}

ISR(TIMER0_COMPA_vect){
    if(BTN1_STATE){
        if((btn_debounce & 0x0F) != 0x0F){
            btn_debounce++;
        }
    }else{
        if((btn_debounce & 0x0F)!=0x0){
            btn_debounce--;
            if((btn_debounce & 0x0F) == 0x0){   //Reset counters
                zeroing = 0b11;
                remainingStepsM1 = 2048;
                remainingStepsM2 = 2048;
                timer1OVF_cnt = 0;
            }
        }
    }
    
    if(BTN2_STATE){
        if((btn_debounce & 0xF0) != 0xF0){
            btn_debounce += 0x10;
        }
    }else{
        if((btn_debounce & 0xF0) != 0x0){
            btn_debounce -= 0x10;
            if((btn_debounce & 0xF0) == 0x0)remainingStepsM1 += STEPS_PER_FLAP;
        }
    }
    
    
    if(remainingStepsM1>0){
        remainingStepsM1--;
        if(--stepperStateM1>3)stepperStateM1=3;
        PORTB = (1<<stepperStateM1) | (PORTB & 0xF0);
    }else{
        PORTB&=~0x0F;
    }
    if(remainingStepsM2>0){
        remainingStepsM2--;
        if(--stepperStateM2>3)stepperStateM2=3;
        PORTB = (1<<(stepperStateM2+4)) | (PORTB & 0x0F);
    }else{
        PORTB &= ~0xF0;
        LEDR_LOW;
    }
}

