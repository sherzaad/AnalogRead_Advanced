#ifndef AnalogReadEx_cpp
#define AnalogReadEx_cpp

#include "AnalogReadEx.h"

AnalogReadEx Analog;              // preinstatiate

static uint8_t AnalogReadEx::analog_reference = DEFAULT;
static volatile uint16_t AnalogReadEx::adcIntr_val = 0;
static uint8_t *AnalogReadEx::adc = (uint8_t*)&adcIntr_val; //not need for 'volatile' as only used by ISR

void AnalogReadEx::SetReference(uint8_t mode)
{
	// Arduino provides DEFAULT, INTERNAL, INTERNAL1V1, INTERNAL2V56 and EXTERNAL. 
	// can't actually set the register here because the default setting
	// will connect AVCC and the AREF pin, which would cause a short if
	// there's something connected to AREF.
	analog_reference = mode;
}

void AnalogReadEx::SetPrescaler(uint8_t prescaler)
{
#if defined(ADCSRA)
	ADCSRA = (ADCSRA & 0xF8)|(prescaler&0x07);
#else
	#error "library does not support hardware!"
#endif
}

uint8_t AnalogReadEx::getpin(uint8_t pin)
{
#if defined(analogPinToChannel)
#if defined(__AVR_ATmega32U4__)
	if (pin >= 18) pin -= 18; // allow for channel or pin numbers
#endif
	pin = analogPinToChannel(pin);
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	if (pin >= 54) pin -= 54; // allow for channel or pin numbers
#elif defined(__AVR_ATmega32U4__)
	if (pin >= 18) pin -= 18; // allow for channel or pin numbers
#elif defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega644__) || defined(__AVR_ATmega644A__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644PA__)
	if (pin >= 24) pin -= 24; // allow for channel or pin numbers
#else
	if (pin >= 14) pin -= 14; // allow for channel or pin numbers
#endif

#if defined(ADCSRB) && defined(MUX5)
	// the MUX5 bit of ADCSRB selects whether we're reading from channels
	// 0 to 7 (MUX5 low) or 8 to 15 (MUX5 high).
	ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((pin >> 3) & 0x01) << MUX5);
#endif

	return pin & 0x07;
}

void AnalogReadEx::PinSelect(uint8_t pin)
{
	uint8_t pin_select = getpin(pin);
  
	// set the analog reference (high two bits of ADMUX) and select the
	// channel (low 4 bits).  this also sets ADLAR (left-adjust result)
	// to 0 (the default).
#if defined(ADMUX)
#if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
	ADMUX = (analog_reference << 4) | pin_select;
#else
	ADMUX = (analog_reference << 6) | pin_select;
#endif
#endif
}

void AnalogReadEx::adcStart()
{
#if defined(ADCSRA) && defined(ADCL)
	// start the conversion
	ADCSRA |= (1<<ADSC);
#else
	#error "library does not support hardware!"
#endif
}

void AnalogReadEx::DiffPinSelect(uint8_t pos,uint8_t neg, uint8_t gain)
{
/* 	uint8_t _pos = getpin(pos);
	uint8_t _neg = getpin(neg);
	uint8_t mux;

	// set the analog reference (high two bits of ADMUX) and select the
	// channel (low 4 bits).  this also sets ADLAR (left-adjust result)
	// to 0 (the default).	
#if defined(ADMUX)
#if defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
	mux = (_neg<<1)|gain;
	if(_pos==0) mux|=0x08;

	ADMUX = (analog_reference << 4) | mux;
#elif defined(__AVR_ATmega32U4__)
  	if(gain == X1){
		mux =  0x10|_pos;
	else if(gain ==X10){
	    if(_pos==1) mux = 0x09;
		else if(_neg==0) mux = 0x28|(_pos&0x03);
		else mux = 0x28|_pos;
	}
	else if(gain ==X40){
		if(_pos==1) mux = 0x26;
		else if(_neg==0) mux = 0x30|(_pos&0x03);
		else mux = 0x30|_pos;
	}
	else if(gain ==X200){
		if(_pos==1) mux = 0x0B;
		else if(_neg==0) mux = 0x38|(_pos&0x03);
		else mux = 0x38|_pos;
	}
	ADMUX = (analog_reference << 6) | mux;
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) ||defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega644__) || defined(__AVR_ATmega644A__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644PA__)
	if(gain == X1){
		mux = ((_neg==1) ? 0x10 : 0x18)|_pos;
	}
	else{
		mux = (_pos+_neg)|gain;	
	}
	
	ADMUX = (analog_reference << 6) | mux;
#else
#error "library does not support hardware!"
#endif
#endif */
}

int16_t AnalogReadEx::adcComplete()
{
#if defined(ADCSRA) && defined(ADCL)
	// ADSC is cleared when the conversion finishes
	if (ADCSRA & (1<<ADSC)) return ADC_BUSY;
	
	*adc = ADCL;  //Make certain to read ADCL first, it locks the values
	*(adc+1) = ADCH;  //and ADCH releases them.

	//uint16_t *temp_ptr;
	//temp_ptr = (uint16_t*)&ADCL; 
	
	return adcIntr_val;
#else
	#error "library does not support hardware!"
#endif
}

void AnalogReadEx::EnableADCIntr(void (*isr)(), uint8_t trigger_source)
{
#if defined(ADCSRB)
  // set ADTS2..0 in ADCSRB trigger mode
  ADCSRB = (ADCSRB & 0xF8)|(trigger_source&0x07);
#endif

#if defined(ADCSRA)	
	isrCallback = isr;
	
	// Set ADATE to enable auto-triggering and ADIE to enable ADC interrupt.
	ADCSRA |= (1<<ADATE)|(1<<ADIE);
	
	// Enable global interrupts
	sei();
#endif
	
}
void AnalogReadEx::DisableADCIntr()
{
#if defined(ADCSRA)	
	// Clear ADATE to dis able auto-triggering and ADIE to disable ADC interrupt.
	ADCSRA &= ~((1<<ADATE)|(1<<ADIE));
#endif
}

void AnalogReadEx::EnableAnalogCompIntr(void (*isr)(), uint8_t pos_input,uint8_t neg_input, uint8_t Intr_mode)
{
/* 	//choose the input for non-inverting input
    if (pos_input == INTERNAL) {
        ACSR |= (1<<ACBG); //set +ve comaparator input to bandgap internal reference voltage
    } 
	else if (pos_input == AIN) {
        ACSR &= ~(1<<ACBG); //set +ve comaparator pin AIN0
    }
	else{
		#error "Positive Analog Comparator Input should be either set to AIN0 or INTERNAL!"
		return;
	}
	
	if(neg_input != AIN){
		PinSelect(neg_input); //choose the ADC channel
		#if defined(ADCSRA)	
		ADCSRA &= ~(1<<ADEN);
		#endif
		SFIOR |= (1<<ACME); //set -ve comaparator pin to ADC channel
	}
	else{
		SFIOR &= ~(1<<ACME); //set -ve comaparator pin to AIN1
	}
	
	//set the interrupt mode
	if (Intr_mode == CHANGE) {
		ACSR &= ~((1<<ACIS1) | (1<<ACIS0)); //interrupt on toggle event
	} 
	else if (tempMode == FALLING) {
		ACSR &= ~(1<<ACIS0);
		ACSR |= (1<<ACIS1);
	} 
	else{ //default RISING 
		ACSR |= ((1<<ACIS1) | (1<<ACIS0));
	}
	
	isrCallback = isr;
	
	ACSR |= (1<<ACIE); //enable interrupts on AC events
	
	ACSR &= ~(1<<ACD); //switch on the AC
	
	sei(); */
}

void AnalogReadEx::DisableAnalogCompIntr()
{
/* 	ACSR &= ~(1<<ACIE); //disable interrupts on AC events
	ACSR |= (1<<ACD); //switch off the AC */
}

// Interrupt service routine for the ADC
ISR(ADC_vect)
{
#if defined(ADCL) 
  Analog.adc[0] = ADCL;        //Make certain to read ADCL first, it locks the values
  Analog.adc[1] = ADCH;  //and ADCH releases them.
  Analog.isrCallback();
#endif
}

#ifdef ANALOG_COMP_vect
ISR(ANALOG_COMP_vect) {
#else
ISR(ANA_COMP_vect) { //alternative name depending on board
#endif
	Analog.isrCallback();
}
#endif
