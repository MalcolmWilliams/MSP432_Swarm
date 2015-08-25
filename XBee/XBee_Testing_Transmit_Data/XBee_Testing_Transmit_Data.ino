#include "msp.h"

void setup()
{
  // put your setup code here, to run once:
  
  Serial.begin(9600);
  Serial.println("Begin Serial");
  
  CSKEY = 0x695A;                       // Unlock CS module for register access
  //CSCTL1 |= (SELA_2 | SELS_3 | SELM_3);
  CSCTL1 = SELA_2 | SELS_3 | SELM_3;    // Select ACLK = REFO, SMCLK = MCLK = DCO    //this makes Serial screw up
  CSKEY = 0;                                // Lock CS module from unintended accesses
  
  // Configure UART pins
  P3SEL0 |= BIT2 | BIT3;                  // set 2-UART pin as second function

    __enable_interrupt();
    NVIC_ISER0 = 1 << ((INT_EUSCIA0 - 16) & 31); // Enable eUSCIA0 interrupt in NVIC module  //how do

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
  UCA2IE |= UCRXIE;                       // Enable USCI_A2 RX interrupt


  /*
  __enable_interrupt();
  NVIC_ISER0 = 1 << ((INT_EUSCIA0 - 16) & 31); // Enable eUSCIA0 interrupt in NVIC module4
*/

  
}

void loop()
{  
  /*
  char data;
  if(UCA2IFG & UCRXIFG)
  {
    data = UCA2RXBUF;
    
    Serial.write(data);
    
    Serial.println(" | that was some data");
  }
  */
  //UART_A2_SendString("test");
  //delay(30);
  
  Serial.println("test");
  delay(100);
  
  /*loopback*/
  if (UCA2IFG & UCRXIFG) //check if something was recieved
  {
    while(!(UCA2IFG & UCTXIFG));
    UCA2TXBUF = UCA2RXBUF;  //set tx buffer to what was recieved by rx buffer.
    __no_operation();
  } 
  /**/
}

void UART_A2_SendChar(char data)
{
    UCA2TXBUF = data;             //send
    while(!(UCA2IFG & UCTXIFG));   //hold until the data is sent
    __no_operation();
}

void UART_A2_SendString(char *s)
{
  do {
    UART_A2_SendChar(*s);
    s++;
  } while(*s != 0);
  
  UART_A2_SendChar('\n');
}


// UART interrupt service routine
void eUSCIA2IsrHandler(void)
{
    if (UCA0IFG & UCRXIFG)  //if recieved
    {
        UCA0IFG &=~ UCRXIFG;            // Clear interrupt
        RXData = UCA0RXBUF;             // Clear buffer    
        __no_operation();
    }
}


