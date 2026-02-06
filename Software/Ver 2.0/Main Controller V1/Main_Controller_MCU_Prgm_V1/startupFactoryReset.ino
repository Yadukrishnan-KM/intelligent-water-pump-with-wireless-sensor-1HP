void initialFactoryReset(){
 lcd.clear();
lcd.setCursor(0,0); lcd.print("Factory Reset???");
lcd.setCursor(0,1); lcd.print("Yes-Press&Hold <");
delay(3000);
buttonValue = buttonRead(inputSwitchPin);
if(buttonValue == 4){
  delay(1000);
  buttonValue = buttonRead(inputSwitchPin);
  if(buttonValue == 4){
   lcd.clear();
   lcd.setCursor(2,0); lcd.print("Reseting...");
   lcd.setCursor(6,1); lcd.print("Wait");
   delay(1500);
   EEPROM.update(1, true); // pumpOperationModeSetting
   EEPROM.update(2, 1); // pumpScheduleTimeSetting
   EEPROM.update(3, true); // lowVoltCutoffSetting
   EEPROM.update(4, 3); // pumpAoffTimerSetting
   EEPROM.update(5, true); // dryrunCutoffSetting
   EEPROM.update(6, 4); // brightValueSetting
   EEPROM.update(7, 200); // brightValueSetting
   EEPROM.update(8, 2); // pumpCutINLevel Setting
   lcd.clear();
   lcd.setCursor(6,0); lcd.print("Done");
   delay(1000);
   lcd.clear();
  }
  else{
   lcd.clear();
   lcd.setCursor(2,0); lcd.print("Not Pressed");
   lcd.setCursor(3,1); lcd.print("Exiting...");
   delay(1500);
   lcd.clear();
  }
}
else{
  lcd.clear();
  lcd.setCursor(2,0); lcd.print("Not Pressed");
  lcd.setCursor(3,1); lcd.print("Exiting...");
  delay(1500);
  lcd.clear();
}
}
