#include "AnalogReadEx.h"

int16_t adcRead = ADC_BUSY;

void setup()
{

  Serial.begin(115200);
  Analog.PinSelect(A0);   //Select ADC Channel
  Analog.adcStart();      //Starts initial the ADC conversion
}

void loop()
{  
  adcRead = Analog.adcComplete(); //Get ADC status
  
  if (adcRead != ADC_BUSY) {      ////Check if conversion is complete
    Serial.println(adcRead);      //print out ADC value
    delay(1000);                  //arbitrary delay
    Analog.adcStart();            //Starts next the ADC conversion
  }
  else{
    Serial.println("ADC BUSY!");    
  }

}
