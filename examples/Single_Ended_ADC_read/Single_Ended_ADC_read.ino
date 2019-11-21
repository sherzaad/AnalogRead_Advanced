//Single Ended ADC read

#include "AnalogReadEx.h"

void setup() {

  Serial.begin(115200);

  //Select Channel. 
  Analog.PinSelect(A0);

  //Start ADC conversion.
  Analog.ReadStart();

}

void loop() {
  int16_t adcRead;

  //Check in ADC conversion is complete
  adcRead = Analog.ReadComplete();

  if(adcRead != ADC_BUSY) {
    Serial.println(adcRead);

    //Start next ADC conversion.
    Analog.ReadStart();
  }

  //arbitrary delay
  delay(1000);
}
