#ifndef TIMER0_h
#define TIMER0_h
/* Timer0 class
 * if you call start(), you need to have ISR in your main file, otherwise weird stuff happens
 *
 *
 */

#define INTERRUPT_PERIOD_MS 10
#define NULL 0

class Timer0
{
public:
	void init()
	{
		TCCR0A |= (1<<WGM01); // CTC mode
		TCCR0B |= (1<<CS02) | (1<<CS00); // 1024 prescaler
		OCR0A = 155; //compare match every INTERRUPT_PERIOD_MS
	}
	
	void start()
	{
		enableInterrupt();
	}
	
private:
	void enableInterrupt()
	{
		TIMSK0 |= (1<<OCIE0A); //enable Output Compare interrupt
	}
};




#endif