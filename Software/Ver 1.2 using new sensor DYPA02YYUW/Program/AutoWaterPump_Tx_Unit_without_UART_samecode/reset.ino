/* Reset done by connecting digital pin to the reset pin and initially keep it high and when reset condition satisfies 
it will change state to low. So the microcontroller gets reset. Ensure that a resistor in series with the digital 
output and the reset pin. Otherwise the output pin will get shortcircuited if we manually reset the microcontroller. */

void reset(){
  currentMillisReset = millis();
  if((currentMillisReset - previousMillisReset) >= resetTimePeriod){
    digitalWrite(resetPin, LOW);                                        // reseting the uc, low triggered
  }
}