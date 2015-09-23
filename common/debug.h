#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

#include "uart.h"

#define DEBUG_ON 1

class Debug
{
public:
    static inline void init()
    {
		#if DEBUG_ON
		DDRD |= (1<<PD1);
		uart_init(UART_BAUD_SELECT(9600, F_CPU));
		#endif
    }

    static inline void endl()
    {
		#if DEBUG_ON
		uart_endl();
		#endif
    }
    //int
    static inline void print(int number)
    {
		#if DEBUG_ON
		uart_putint(number);
		#endif
    }
    static inline void println(int number)
    {
		#if DEBUG_ON
        print(number);
        endl();
		#endif
    }

    //char
    static inline void print(char c)
    {
		#if DEBUG_ON
		uart_putc(c);
		#endif
    }

    static inline void println(char c)
    {
        #if DEBUG_ON
		print(c);
		endl();
		#endif
    }

    //str
    static inline void print(char *str)
    {
		#if DEBUG_ON
		uart_puts(str);
		#endif
    }
    static inline void println(char *str)
    {
		#if DEBUG_ON
        print(str);
        endl();
		#endif
    }








};

#endif // DEBUG_H_INCLUDED
