void initialSetup(){
  digitalWrite(ledPin, LOW);
  delay(100);
  digitalWrite(ledPin, HIGH);
  delay(500);
  digitalWrite(ledPin, LOW);
  delay(100);
  digitalWrite(ledPin, HIGH);
  previousMillis = millis();
  currentMillis = millis();
  while((!input) && ((currentMillis - previousMillis) <= timeOut) ){    // waiting for input or timeout
   currentMillis = millis();
   input = digitalRead(inputSetPin);
                                                                         Serial.println("waiting for input or timeout,first while");
                                                                         //Serial.println(currentMillis - previousMillis);
                                                                        // Serial.println(input);
   delay(10);
  }
  if(input){                                                            // if input available enter to low and high level settings
   digitalWrite(ledPin, LOW);
   delay(500);
   digitalWrite(ledPin, HIGH);
   input = digitalRead(inputSetPin);
   while(!input){                                                       // waiting input for enter in low setting menu
     input = digitalRead(inputSetPin);
                                                                              Serial.println("waiting for low set menu, second while");
   }
   digitalWrite(ledPin, LOW);
   delay(500);
   input = digitalRead(inputSetPin);
   while(!input){                                                       // enter to setting the current low vale
                                                                                 Serial.println("waiting for low set value, press and hold, third while");
     input = digitalRead(inputSetPin);
     if(input) {
      delay(300);
      input = digitalRead(inputSetPin);
     }
   } 
   finalDepth = ultrasonicRead();                                                    // reading and saving the low value
   EEPROM.put(0,  finalDepth);
                                                                          Serial.print("minLevel save is ");
                                                                          Serial.println(finalDepth);
   for (int i = 0; i <= 3; i++){                                        // blinking led to show successful save
     digitalWrite(ledPin, HIGH);
     delay(100);
     digitalWrite(ledPin, LOW);
     delay(100);
   }
   digitalWrite(ledPin, HIGH);
   delay(500);
   input = digitalRead(inputSetPin);
   while(!input){                                                       //waiting input for enter in high setting menu
                                                                                  Serial.println("waiting for high set menu, fourth while");
     input = digitalRead(inputSetPin);
   }
   digitalWrite(ledPin, LOW);
   delay(500);
   input = digitalRead(inputSetPin);
   while(!input){                                                       // enter to setting the current high vale
                                                                                    Serial.println("waiting for high set value, press and hold, fifth while");
     input = digitalRead(inputSetPin);
     if(input) {
      delay(300);
      input = digitalRead(inputSetPin);
     }
   }
   finalDepth = ultrasonicRead();                                                    // reading and saving the high value
   EEPROM.put(4,  finalDepth);
                                                                          Serial.print("maxLevel save is ");
                                                                          Serial.println(finalDepth);
   for (int i = 0; i <= 3; i++){
     digitalWrite(ledPin, HIGH);
     delay(100);
     digitalWrite(ledPin, LOW);
     delay(100);
   }
  }
 else digitalWrite(ledPin, LOW);
}