#include "AnalogReadEx.h"

int16_t adcRead = ADC_BUSY;

//user defined ISR routine
void adc_intr()
{
  //Your ISR code here
}

void setup()
{

  Serial.begin(115200);
  Analog.PinSelect(A0);             //Select ADC Channel
  Analog.adcIntr_val = ADC_BUSY;    //Initialise class public variable
  Analog.EnableADCIntr(adc_intr);   //Attach user ISR routine to ADC interrupt and enable in Free Running mode (default)
  Analog.adcStart();               	//Starts initial the ADC conversion. subsequent reads triggered by interrupt.
                                    //Also applies for Differential ADC read
}

void loop()
{
  noInterrupts(); //disable interrupts to avoid data corruption 
  if (Analog.adcIntr_val != ADC_BUSY) { //check if data is available
    adcRead = Analog.adcIntr_val;       //copy data (that way can be used safely in main program after interrupts are re-enabled)
    Analog.adcIntr_val = ADC_BUSY;      //re-Initialise class public variable
  }
  interrupts();   //re-enable interrrupts

  //print out ADC value
  if (adcRead != ADC_BUSY) {
    Serial.println(adcRead);
    adcRead = ADC_BUSY;
  }

}
