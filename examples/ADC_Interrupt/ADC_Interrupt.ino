#include "AnalogReadEx.h"

volatile int16_t adcRead = ADC_BUSY;

//user defines ISR routine
void adc_intr()
{
  adcRead = Analog.adc_intr_rd;
}

void setup()
{

  Serial.begin(115200);
  Analog.PinSelect(A0);             //Select ADC Channel
  Analog.EnableInterrupt(adc_intr); //Attach user ISR routine to ADC interrupt and enable in Free Running mode (default)
  Analog.ReadStart();               //Starts initial the ADC conversion. subsequent reads triggered by interrupt.
                                    //Also applies for Differential ADC read
}

void loop()
{

  if (adcRead != ADC_BUSY) {
    Serial.println(adcRead);
    adcRead = ADC_BUSY;
  }

}
