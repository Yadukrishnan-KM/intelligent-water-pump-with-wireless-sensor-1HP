void lcdRefresh(){
 if(voltPresent) analogWrite(lcdLEDBrightPin, 25);              // to reduce backlight if ac not present
 else analogWrite(lcdLEDBrightPin, brightValue);              // to turn on backlight 

 //********************************************************************************************* HOME SCREEN **************************************************************************************
 if(homeScreenFlag == 0){                                       // entering into home screen, idle condition
   buttonValue = buttonRead(inputSwitchPin);                    // going to main menu and alarm menu by checking input
   if(buttonValue == 3) homeScreenFlag = 1;
   else if(buttonValue == 4)homeScreenFlag = 2;

   homeScreen();
 }
 //********************************************************************************************* MENU SCREEN **************************************************************************************
 else if(homeScreenFlag == 1){                              // entering into menu screen

   buttonValue = buttonRead(inputSwitchPin);                // reading the up/down button press and moving menu up or down
   if(buttonValue == 2) mainmenuSelectCaseNo++;
   if(mainmenuSelectCaseNo == 11) mainmenuSelectCaseNo = 1;
   if(buttonValue == 1) mainmenuSelectCaseNo--;
   if(mainmenuSelectCaseNo == 0) mainmenuSelectCaseNo = 10;
   if(buttonValue != 0) menuTimeoutPreviousMillis = menuTimeoutCurrentMillis; // to restart timeout counter if button is pressed
   mainMenuSwitchCases();                                   // showing corresponding case for main menu with edit option

   // timeout section which return to home screen after timeout

   if(!menuTimeoutCounterFlag){
     menuTimeoutCounterFlag = true;
     menuTimeoutPreviousMillis = millis();
   }
   menuTimeoutCurrentMillis = millis();
   if((menuTimeoutCurrentMillis - menuTimeoutPreviousMillis) >= 6000) {
     homeScreenFlag = 0;                                       // for going to home screen
     menuTimeoutCounterFlag = false;                           // for resettiing the timeout timer
     mainmenuSelectCaseNo = 1;                                 // to restart sub menu from 1
     lcd.createChar(0, powerSymbol);                           // ac present and good symbol adding because max 8 char allowed in lcd memory
     lcd.createChar(1, abnormal);                              // abonrmal symbol is ! adding because max 8 char allowed in lcd memory
     // remove edit option after timeout
     operationModeEditFlag = false;
     pumpCutINLevelEditFlag = false;
     pumpScheduleEditFlag = false;
     lowVoltCutoffEditFlag = false;
     pumpAoffTimerEditFlag = false;
     dryrunCutoffEditFlag = false;
     brightValueEditFlag = false;
     factoryResetEditFlag = false;
   }
 }
  //********************************************************************************************* ALARM SCREEN **************************************************************************************
 else if (homeScreenFlag == 2){                                // entering into alarm screen

   buttonValue = buttonRead(inputSwitchPin);                // reading the up/down button press and moving menu up or down
   if(buttonValue == 2) alarmSelectionCaseNo++;
   if(alarmSelectionCaseNo == 10) alarmSelectionCaseNo = 1;
   if(buttonValue == 1) alarmSelectionCaseNo--;
   if(alarmSelectionCaseNo == 0) alarmSelectionCaseNo = 9;
   if(buttonValue != 0) menuTimeoutPreviousMillis = menuTimeoutCurrentMillis; // to restart timeout counter if button is pressed            
   alarmMenu();                                                 // for getting into alarm menu

 // timeout section which return to home screen after timeout

   if(!menuTimeoutCounterFlag){
     menuTimeoutCounterFlag = true;
     menuTimeoutPreviousMillis = millis();
   }
   menuTimeoutCurrentMillis = millis();
   if((menuTimeoutCurrentMillis - menuTimeoutPreviousMillis) >= 6000) {
     homeScreenFlag = 0;
     menuTimeoutCounterFlag = false;
     alarmSelectionCaseNo = 1;
   }       

 }
 else homeScreenFlag = 0;
}

//************************************************ homescreen updation codes *************************************************************
void homeScreen(){

   //refreshing with all the required data which save in eeprom when it is in home screen, default screen
   pumpOperationModeSetting = EEPROM.read(1);
   pumpScheduleTimeSetting = EEPROM.read(2);
   lowVoltCutoffSetting = EEPROM.read(3);
   pumpAoffTimerSetting = EEPROM.read(4);
   dryrunCutoffSetting = EEPROM.read(5);
   brightValueSetting = EEPROM.read(6);
   brightValue = EEPROM.read(7);
   pumpCutINLevelSetting = EEPROM.read(8);

   lcd.clear();
   if(!transmitterError){lcd.setCursor(0,0); lcd.print("** LEVEL"); lcd.setCursor(9,0); lcd.print(waterLvl); lcd.setCursor(12,0); lcd.print("% **");}  // main lavel indication
   else{lcd.setCursor(0,0); lcd.print("**    ERROR   **");}
   if(!voltPresent) {lcd.setCursor(0,1); lcd.write(0);}                                  // ac present and good symbol
   if(!voltPresent && lowVolt) {lcd.setCursor(1,1); lcd.write(1);}                       // some issue symbol
   if(motorPinStatusFlag) {lcd.setCursor(3,1); lcd.write(2);}                            // pump running and good symbol
   if(motorPinStatusFlag && dryRun) {lcd.setCursor(4,1); lcd.write(1);}                  // some issue symbol
   if(pumpScheduleTimeSetting != 1) {lcd.setCursor(6,1); lcd.write(6);}                  // timer enable symbol
   if(alarm) {lcd.setCursor(8,1); lcd.write(3);}                                         // alarm present symbol
   if(pumpOperationModeSetting) {lcd.setCursor(10,1); lcd.write(7);}                     // auto operation symbol
   if(batV >= 3800) {lcd.setCursor(12,1); lcd.write(4);}                                 // battery good symbol
   //lcd.setCursor(13,1); lcd.write(1);                                                  // some issue symbol
   if(!transmitterError) {lcd.setCursor(14,1); lcd.write(5);                             // transmitter good symbol
   if(batLow || !solarCharg) {lcd.setCursor(15,1); lcd.write(1);}                        // some issue symbol
   } 
   

   }