void startUpScreen(){
 lcd.clear();  
 lcd.setCursor(5,0);
 lcd.print("WELCOME");                              // showing all the details about system in startup with delay
 delay (2000);
 lcd.clear();
 delay (100);
 lcd.setCursor(0,0);
 lcd.print("Auto Water Pump");
 lcd.setCursor(2,1);
 lcd.print("version 1.0");
 delay (3000);
 lcd.clear();
 delay (100);
 lcd.setCursor(0,0);
 lcd.print("GYM Electronics");
 lcd.setCursor(0,1);
 lcd.print("Designed By Yadu");
 delay (4000);
 lcd.clear();
 delay (100);
 lcd.setCursor(1,0);
 lcd.print("Starting UP....");
 for (int i = 0; i < 4; i++)                        // checking working condition of all leds and buzzer 4 times
 {
   digitalWrite(alarmLightPin, HIGH);
   digitalWrite(alarmBuzzerPin, HIGH);
   digitalWrite(onLEDPin, HIGH);
   digitalWrite(offLEDPin, HIGH);
   delay(450);
   digitalWrite(alarmLightPin, LOW);
   digitalWrite(alarmBuzzerPin, LOW);
   digitalWrite(onLEDPin, LOW);
   digitalWrite(offLEDPin, LOW);
   delay(450);
  }
 lcd.clear();
 lcd.setCursor(6,0);
 lcd.print("Done");
 delay (1000);
 lcd.clear();
 delay (100);
}