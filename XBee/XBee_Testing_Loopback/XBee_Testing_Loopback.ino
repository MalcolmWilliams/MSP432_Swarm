/*

pinout:

 ______________________________________________
|                                _______       |
|                               |        \     |  
|                               |         \    |
|                               |          |   |
|                               |          |   |
|    ____________________       |         /    |
|   |GND|RTS|RX|TX|CTS|V+|      |________/     |
|______________________________________________|

connections:
GND to GND
V+ to 5v
RX to TX
TX to RX

*/


#include "msp.h"

void setup()
{
  // put your setup code here, to run once:
  
  CSKEY = 0x695A;                       // Unlock CS module for register access

  CSCTL1 = SELA_2 | SELS_3 | SELM_3;    // Select ACLK = REFO, SMCLK = MCLK = DCO  //not sure if this will screw things up
  CSKEY = 0;                                // Lock CS module from unintended accesses
  
  // Configure UART pins
  P3SEL0 |= BIT2 | BIT3;                  // set 2-UART pin as second function


  // Configure UART
  UCA2CTLW0 |= UCSWRST;            // Put eUSCI in reset
  UCA2CTLW0 |= UCSSEL__SMCLK;      //set clock source?    (system master?
  //UCA2CTLW0 &= ~(UCMODE0 | UCMODE1 | UCSYNC); //UART Mode   
  
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
  
  
  UCA2BR0 = 0x38;                           
  UCA2BR1 = 0x01;
  UCA2MCTLW = 0xAA00 | UCOS16 | 0x0080;  //UCOS16 = 1, turns on oversampling for baudrate gen.
  
  /*
    UCA2BR0 = 78;                           // 12000000/16/9600
  UCA2BR1 = 0x00;
  UCA2MCTLW = 0x1000 | UCOS16 | 0x0020;
  */
  
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
  

  UCA2CTLW0 &= ~UCSWRST;                  // Initialize eUSCI
 // UCA2IE |= UCRXIE;                       // Enable USCI_A0 RX interrupt

  /*
  __enable_interrupt();
  NVIC_ISER0 = 1 << ((INT_EUSCIA0 - 16) & 31); // Enable eUSCIA0 interrupt in NVIC module4
*/

  
}

void loop()
{  
  if (UCA2IFG & UCRXIFG) //check if something was recieved
  {
    while(!(UCA2IFG & UCTXIFG));
    UCA2TXBUF = UCA2RXBUF;  //set tx buffer to what was recieved by rx buffer.
    __no_operation();
  } 
}



/*
// UART interrupt service routine
void eUSCIA0IsrHandler(void)
{
    if (UCA0IFG & UCRXIFG)
    {
      while(!(UCA0IFG&UCTXIFG));
      UCA0TXBUF = UCA0RXBUF;
      __no_operation();
    }
}
*/
