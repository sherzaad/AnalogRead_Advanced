#ifndef AnalogReadEx_h
#define AnalogReadEx_h

#include <arduino.h>
#include <wiring_private.h>
#include <pins_arduino.h>

//ADC constants
#define ADC_BUSY -1
#define X1 0x00
#define X10 0x08
#define X20 0x01
#define X40 0x04
#define X200 0x0A
#define AIN 0xFF //Analog Compare dummy value

//Prescaler
#define Div2 0x00
#define Div4 0x02
#define Div8 0x03
#define Div16 0x04
#define Div32 0x05
#define Div64 0x06
#define Div128 0x07

//Trigger Source (ADT0:2)
#define Free_Running 0x00
#define Analog_Comp  0x01
#define External_Intr_0 0x02
#define Compare_Tmr0_Match 0x03
#define Overflow_Tmr0 0x04
#define CompareB_Match 0x05
#define Overflow_Tmr1 0x06
#define Capture_Event_Tmr1 0x07

class AnalogReadEx
{
  public:
	static volatile uint16_t adcIntr_val;
	static uint8_t *adc; //not need for 'volatile' as only used by ISR
	
    void SetReference(uint8_t mode); // Arduino provides DEFAULT, INTERNAL, INTERNAL1V1, INTERNAL2V56 and EXTERNAL. 
	void SetPrescaler(uint8_t prescaler); //see prescaler defines
	void PinSelect(uint8_t pin); //Single ended ADC
	void DiffPinSelect(uint8_t pos,uint8_t neg, uint8_t gain = X1); //Differential ADC <---- TO DO
	void adcStart(); //start AD conversion
	int16_t adcComplete(); //returns ADC value when complete.
	void EnableADCIntr(void (*isr)(), uint8_t trigger_source = Free_Running);  //ADC interrupt handler
	void DisableADCIntr(); //disable ADC interrupt
	void EnableAnalogCompIntr(void (*isr)(), uint8_t pos_input=AIN,uint8_t neg_input=AIN, uint8_t Intr_mode=RISING);  //Analog Comparator handler <-- TO DO
	void DisableAnalogCompIntr(); //disable Analog Comparator <-- TO DO
	void (*isrCallback)();
		
  private:
	static uint8_t analog_reference;
	

	uint8_t getpin(uint8_t pin1); //return MUX2:0 for selected adc pin 
};

extern AnalogReadEx Analog;
#endif
