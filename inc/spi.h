/*
 * =====================================================================================
 *
 *       Filename:  spi.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/07/2015 03:37:25 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  W. Alex Best (mn), alexbest@alexbest.me
 *        Company:  Amperture Engineering
 *
 * =====================================================================================
 */
#include <stdint.h>

void spiInit();
void spiSendChar(uint8_t);
uint8_t spiReceiveChar();
