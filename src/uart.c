// Header Includes
#include <msp430g2553.h>
#include "uart.h"
#include <stdint.h>


//GPIO Definitions
#define TXD BIT2
#define RXD BIT1
#define FIFO_SIZE 256

//Mailboxes and flags for RX and TX
volatile uint8_t txFlag;
//volatile uint8_t txChar;

volatile uint8_t rxFlag;
volatile uint8_t rxChar;
volatile uint8_t rxFifo[FIFO_SIZE];
volatile uint32_t rxFifoPointStart;
volatile uint32_t rxFifoPointEnd;




void uartInit(void){
    //Setup the Baud Rate
    // For now, we hard code to 9600 baud.
    UCA0CTL1 |= UCSWRST;

    UCA0CTL1 |= UCSSEL_2;       // UART clock attached to SMCLK

    UCA0BR0 = 104;              // 1MHz / 9600 = 104.166666...
    UCA0BR1 = 0;

    P1SEL |= BIT1 | BIT2;
    P1SEL2 |= BIT1 | BIT2;

    UCA0MCTL = UCBRS0; // UCBRSx = 1, UCBRFx = 0

    UCA0CTL1 &= ~UCSWRST;

    IE2 |= UCA0RXIE;

    __bis_SR_register(GIE);
}
/*
void uartSendChar(uint8_t toSend){
    txChar = toSend;
    IE2 |= UCA0TXIE;
    while (txFlag == 1); 
    txFlag = 1;
}
/*

void uartSendString(uint8_t *toSend){
    while(*toSend){
        uartSendChar(*toSend);
        toSend++;
    }
}
/**/

static void
__attribute__((__interrupt__(USCIAB0RX_VECTOR)))
USCI0RX_ISR(void){ // RX Interrupt Handler
    while(!( IFG2 & UCA0TXIFG ));
    rxChar = UCA0RXBUF;
    rxFlag = 1; 

    rxFifo[rxFifoPointEnd] = rxChar;

    rxFifoPointEnd++;
    if (rxFifoPointEnd == FIFO_SIZE) rxFifoPointEnd = 0;
}


