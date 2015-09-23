#ifndef TIMER0_h
#define TIMER0_h

#define INTERRUPT_PERIOD_MS 10

class Timer0
{
public:
	void init(void (*isrPtr)() = NULL)
	{
		TCCR0A |= (1<<WGM01); // CTC mode
		TCCR0B |= (1<<CS02) | (1<<CS00); // 1024 prescaler
		OCR0A = 155; //compare match every INTERRUPT_PERIOD_MS
		isr = isrPtr;
	}
	
	void start()
	{
		//if(isr != NULL) {
			enableInterrupt();
			//return true;
		//}
		//return false;
	}
	
	void setIsr(void (*isrPtr)())
	{
		isr = isrPtr;
	}
	
	inline void interrupt()
	{
		isr();
	}
	
private:
	void enableInterrupt()
	{
		TIMSK0 |= (1<<OCIE0A); //enable Output Compare interrupt
	}

	void (*isr)();
};




#endif