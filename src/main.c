/*
 * ===========================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  Binary LED Clock via Shift Registers
 *
 *        Version:  0.0.1
 *        Created:  09/19/2015 5:40 PM
 *       Revision:  none
 *       Compiler:  msp430-gcc
 *
 *         Author:  W. Alex Best (mn), theamp@gmail.com
 *        Company:  Amperture Engineering
 *
 * ===========================================================================
 */

// Library inclusions
#include "msp430.h"
#include "spi.h"

/* Defines for bytes to send to Shift Registers */
#define NUM_SHIFT_REGISTERS 3
#define RTC_STATE_SECONDS 2
#define RTC_STATE_MINUTES 1
#define RTC_STATE_HOURS 0

#define BTN_INC_SECONDS BIT0
#define BTN_INC_MINUTES BIT1
#define BTN_INC_HOURS BIT2
#define BTN_ALT_DECREMENT BIT4

#define CYCLES_PER_MS_1MHZ_CLOCK 1000

volatile uint8_t timerCount = 0;
volatile uint8_t rtcState[NUM_SHIFT_REGISTERS] = {0, 0, 0};

void latch595(void){
    __delay_cycles(20);
    P1OUT |= BIT3;
    __delay_cycles(10);
    P1OUT &= ~BIT3;
    __delay_cycles(10);
}

void timerInit(void){
    TACCTL0 = CCIE;
    TA0CTL = TASSEL_2 + MC_1 + ID_3;
    TACCR0 = 5000;        // (1,000,000 Hz / 8 Div) / 5000) = 25 Hz
}

int main(void){

    //Disable Watchdog
    WDTCTL = WDTPW + WDTHOLD;

    // Set up 1MHz Clock
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;

    // Setup GPIO
    P1DIR |= BIT0 + BIT3;
    P1OUT = 0;

    P1DIR &= ~( BTN_INC_SECONDS + BTN_INC_MINUTES + BTN_INC_HOURS
            + BTN_ALT_DECREMENT);
    P1IE |= ( BTN_INC_SECONDS + BTN_INC_MINUTES + BTN_INC_HOURS);
    P1IES |= ( BTN_INC_SECONDS + BTN_INC_MINUTES + BTN_INC_HOURS);
    P1IFG &= ~( BTN_INC_SECONDS + BTN_INC_MINUTES + BTN_INC_HOURS);
    P1OUT |= ( BTN_INC_SECONDS + BTN_INC_MINUTES + BTN_INC_HOURS
            + BTN_ALT_DECREMENT);
    P1REN |= ( BTN_INC_SECONDS + BTN_INC_MINUTES + BTN_INC_HOURS
            + BTN_ALT_DECREMENT);
    
    // Initialize Peripherals & Interrupts
    spiInit();
    timerInit();
    _BIS_SR(GIE);

    // Initialize Variables
    uint8_t i = 0;
    uint8_t sendByteBitMask = 1;

    // SUPER LOOP
    while(1){
        // Futz with Bit Masking
        sendByteBitMask = (sendByteBitMask << 1);
        if (sendByteBitMask >= 0x80){
            sendByteBitMask =1;

        }

        // Send bytes to shift register
        for(i = 0; i < NUM_SHIFT_REGISTERS; i++){
            spiSendChar((rtcState[i] & sendByteBitMask) << 1);
        }
        latch595();
    }
}
        
// Timer Interrupt
static void
__attribute__((__interrupt__(TIMER0_A0_VECTOR)))
Timer_A(void){ // TX Interrupt Handler
    timerCount++;
    /**/
    if (timerCount >= 25){
        timerCount = 0;
        rtcState[RTC_STATE_SECONDS]++; 
        if (rtcState[RTC_STATE_SECONDS] >= 60){
            rtcState[RTC_STATE_SECONDS] = 0;
            rtcState[RTC_STATE_MINUTES]++;
            if (rtcState[RTC_STATE_MINUTES] >= 60){
                rtcState[RTC_STATE_MINUTES] = 0;
                rtcState[RTC_STATE_HOURS]++;
                if (rtcState[RTC_STATE_HOURS] >= 24) {
                    rtcState[RTC_STATE_HOURS] = 0;
                }
            }  
        }
    }
    /**/
    /*
    seconds++; 
    if (seconds >= 60){
        seconds = 0;
    }  
    /**/
}

// Button Interrupts
static void
__attribute__((__interrupt__(PORT1_VECTOR)))
P1ISR(void){ // TX Interrupt Handler

    // Lower the Interrupt flag
    P1IFG = 0;

    // Disable the Timer Interrupt Temporarily
    TACCTL0 &= ~CCIE;

    // Debounce the button
    int i = 0;
    for (i = 0; i <= 50; i++){
        __delay_cycles(CYCLES_PER_MS_1MHZ_CLOCK);
    }

    // If the ALT button is pressed
    if ((P1IN & BTN_ALT_DECREMENT) == 0){
        if ((P1IN & BTN_INC_SECONDS) == 0){
            if (rtcState[RTC_STATE_SECONDS] == 0) 
                rtcState[RTC_STATE_SECONDS] = 59;
            else rtcState[RTC_STATE_SECONDS]--;
        }

        if ((P1IN & BTN_INC_MINUTES) == 0){
            rtcState[RTC_STATE_MINUTES]--;
            if (rtcState[RTC_STATE_MINUTES] == 0) 
                rtcState[RTC_STATE_MINUTES] = 59;
        }

        if ((P1IN & BTN_INC_HOURS) == 0){
            rtcState[RTC_STATE_HOURS]--;
            if (rtcState[RTC_STATE_HOURS] == 0) 
                rtcState[RTC_STATE_HOURS] = 23;
        }

    // If the ALT button is not pressed
    } else {
        
        if ((P1IN & BTN_INC_SECONDS) == 0){
            rtcState[RTC_STATE_SECONDS]++;
            if (rtcState[RTC_STATE_SECONDS] == 60) 
                rtcState[RTC_STATE_SECONDS] = 0;
        }

        if ((P1IN & BTN_INC_MINUTES )== 0){
            rtcState[RTC_STATE_MINUTES]++;
            if (rtcState[RTC_STATE_MINUTES] == 60) 
                rtcState[RTC_STATE_MINUTES] = 0;
        }

        if ((P1IN & BTN_INC_HOURS) == 0){
            rtcState[RTC_STATE_HOURS]++;
            if (rtcState[RTC_STATE_HOURS] == 24) 
                rtcState[RTC_STATE_HOURS] = 0;
        }

    }

    // Re-enable Timer Interrupts
    TACCTL0 = CCIE;
}
