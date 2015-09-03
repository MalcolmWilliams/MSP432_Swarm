
#include "UART_A0.h"


void UART_A0::sendChar(char data)
{
    UCA0TXBUF = data;             //send
    while(!(UCA0IFG & UCTXIFG));   //hold until the data is sent
    __no_operation();
}

void UART_A0::sendString(char *s)
{
  do {
    UART_A0::sendChar(*s);
    s++;
  } while(*s != 0);

  UART_A0::sendChar('\n');
}

UART_A0::UART_A0()
{
 // Configure UART pins
  P1SEL0 |= BIT2 | BIT3;                  // set 2-UART pin as second function (P1.2 and P1.3)

  /*
  __enable_interrupt();
  NVIC_ISER0 = 1 << ((INT_EUSCIA0 - 16) & 31); // Enable eUSCIA0 interrupt in NVIC module4
*/
	  // Configure UART
  UCA0CTLW0 |= UCSWRST;             // Put eUSCI in reset
  UCA0CTLW0 |= UCSSEL__SMCLK;

  /****  Baud Rate calculation ****
  N = Fbrclk/baudrate. brclk is the baudrate clock source. in this case the main system clock.
  if N > 16:
  OS16 = 1
  UCBRx = INT(N/16)
  UCBRFx = INT([(N/16) - INT(N/16)]*16)
  UCBRSx -> lookup frational portion in table 22-4, Fraction = N-INT(N)

  48000000/(9600) = 5000
  UCBRx = INT(5000/16) = 312 = 0x0138
  UCBRFx = INT([(312.5) - INT(312.5)]*16) = 8 -> 0x08
  Application Guide Table 22-4: Fraction = 0 -> UCBRSx = 0x00
  **********************************/

  //UCBRx = prescaler
  //UCBRFx = first stage modulator
  //UCBRSx = second stage modulator

  UCA0BR0 = 0x38;
  UCA0BR1 = 0x01;
  UCA0MCTLW = 0xAA00 | UCOS16 | 0x0080;  //UCOS16 = 1, turns on oversampling for baudrate gen.


  /*****  Some Registers *****

  UCAxCTLWx    Control Word
  UCAxBRW      BAudrate Control Word
  UCAxMCTLW    Modulation Control Word
  UCAxSTATW    Status Word

  UCAxRXBUF    Recieve Buffer
  UCAxTXBUF    Transmit Buffer

  UCAxIE       Interrupt Enable
  USAxIFG      Interrupt Flag
  UCAxIV       Interrupt Vector

  *******************************/


  UCA0CTLW0 &= ~UCSWRST;                  // Initialize eUSCI
}
