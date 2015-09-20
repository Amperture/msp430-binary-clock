#include <stdint.h>

//Mailboxes and flags for RX and TX
extern volatile uint8_t txFlag;
//extern volatile uint8_t txChar;
extern volatile uint8_t rxFlag;
extern volatile uint8_t rxChar;

void uartInit(void);
void uartSendChar(uint8_t toSend);
void uartSendString(uint8_t *toSend);
