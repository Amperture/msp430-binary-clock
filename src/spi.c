/*
 * =========================================================================
 *
 *       Filename:  spi.c
 *
 *    Description:  Serial Peripheral Interface library for MSP430G2553 chip.
 *
 *        Version:  0.1.0
 *        Created:  07/12/2015 07:55:40 PM
 *       Revision:  none
 *       Compiler:  msp430-elf-gcc
 *
 *         Author:  W. Alex Best (mn), theamp@gmail.com
 *        Company:  Amperture Engineering
 *
 * ==========================================================================
 */

#include <msp430.h>
#include <stdint.h>

uint8_t txChar;
void spiInit(){
    UCB0CTL1 |= UCSWRST;

    //UCCKPH    - Phase Select, 0 = change -> capture
    //UCCKPL    - Polarity Select, 0 = rising edge
    //UC7BIT    - data length, 0 = 8-bit data
    UCB0CTL0 &= ~(UCCKPL | UC7BIT | UCMODE0 | UCMODE1);

    //UCMSB     - Most Significant Bit first
    //UCMST     - Master Mode
    UCB0CTL0 |= (UCCKPH | UCMSB | UCSYNC | UCMST);

    //UCSSEL_2    - Clock Source, SMCLK
    UCB0CTL1 |= UCSSEL_2;

    // Baud Rate 4,000,000, SMCLK 
    UCB0BR0=0x00; UCB0BR1=0x00;

    UCB0CTL1    &=      ~UCSWRST;

    P1SEL       |=      BIT6 | BIT7 | BIT5;
    P1SEL2      |=      BIT6 | BIT7 | BIT5; 
}

void spiSendChar(uint8_t send){
    txChar = send;
    while(!(IFG2 & UCB0TXIFG));
    UCB0TXBUF = send;
}

uint8_t spiReceiveChar(){
    while(!(IFG2 & UCB0TXIFG));
    return UCB0RXBUF; 
}

static void
__attribute__((__interrupt__(USCIAB0TX_VECTOR)))
USCI0TX_ISR(void){ // TX Interrupt Handler
    // Place mailbox byte into buffer.
    UCB0TXBUF = txChar;

    //Disable TX Interrupt
    IE2 &= ~(UCB0TXIE);
}
