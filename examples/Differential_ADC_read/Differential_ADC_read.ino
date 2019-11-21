//Differential ADC read
//refer to tables for Analog pins to used for differential ADC

#include "AnalogReadEx.h"

void setup() {

  Serial.begin(115200);

  //Select Diff Channels A0/A1 with x10 Gain setting. If no gain entered defaults to X1
  Analog.DiffPinSelect(A0, A1, X10);
  
  //Start ADC conversion.
  Analog.ReadStart();

}

void loop() {
  int16_t adcRead = Analog.ReadComplete();
  
  if (adcRead != ADC_BUSY) {
    
    Serial.println(adcRead);

    //start next Analog conversion
    Analog.ReadStart();
  }

  //arbitrary delay
  delay(1000);
}
