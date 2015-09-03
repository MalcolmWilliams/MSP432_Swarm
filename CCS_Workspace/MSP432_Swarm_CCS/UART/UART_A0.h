#ifndef UART_A0_H
#define UART_A0_H

#include "msp.h"

class UART_A0
{
public:
	UART_A0();
	void sendString(char * s);
	void sendChar(char data);
};

#endif	//UART_A0_H
