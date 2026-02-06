void mainMenuSwitchCases(){
 lcd.createChar(0, leftArrowSymbol);                    // left selection arrow symbol adding here because max only 8 allowed
 lcd.createChar(1, rightArrowSymbol);                   // right selection arrow symbol adding here because max only 8 allowed
  switch(mainmenuSelectCaseNo){
  case 1 :
  //********************* display update section ***********************************
   lcd.clear();
   lcd.setCursor(1,0); lcd.print("Operation Mode");
   if(pumpOperationModeSetting){
     lcd.setCursor(6,1); lcd.print("Auto");
   }
   else{
    lcd.setCursor(5,1); lcd.print("Manual");
   }
   //********************* submenu edit section *************************************
   if(!operationModeEditFlag){
    buttonValue = buttonRead(inputSwitchPin);
    if(buttonValue == 5) operationModeEditFlag = true;
   }
   else{
     lcd.setCursor(1,1);lcd.write(0);
     lcd.setCursor(14,1); lcd.write(1);
     buttonValue = buttonRead(inputSwitchPin);
     if(buttonValue == 4) pumpOperationModeSetting = !pumpOperationModeSetting;        // changing the parameter in edit menu by pressing left and right button
     else if(buttonValue == 3) pumpOperationModeSetting = !pumpOperationModeSetting;
     buttonValue = buttonRead(inputSwitchPin);
     if(buttonValue == 5){                                              // pressing enter in edit make save change to eeprom and exit from edit option to operation menu
      EEPROM.update(1, pumpOperationModeSetting);
      operationModeEditFlag = false;
     }
     else if(buttonValue == 2){                                              // back button back from edit to operation mode menu
      operationModeEditFlag = false;
     }
   }
  break;
  case 2:
  //********************* display update section ***********************************
   lcd.clear();
   lcd.setCursor(0,0); lcd.print("Pump CutIN Level");
   lcd.setCursor(9,1); lcd.print("%");
   switch(pumpCutINLevelSetting){
     case 1: 
      lcd.setCursor(6,1); lcd.print("10");
     break;
     case 2:
      lcd.setCursor(6,1); lcd.print("20");
     break;
     case 3:
      lcd.setCursor(6,1); lcd.print("30");
     break;
     case 4:
      lcd.setCursor(6,1); lcd.print("40");
     break;
     case 5:
      lcd.setCursor(6,1); lcd.print("50");
     break;
     case 6:
      lcd.setCursor(6,1); lcd.print("60");
     break;
     case 7:
      lcd.setCursor(6,1); lcd.print("70");
     break;
     case 8:
      lcd.setCursor(6,1); lcd.print("80");
     break;
     case 9:
      lcd.setCursor(6,1); lcd.print("90");
     break;
     default:
     pumpCutINLevelSetting = 1; // to back to default if any error
     break;
   }
   //********************* submenu edit section *************************************
   if(!pumpCutINLevelEditFlag){
    buttonValue = buttonRead(inputSwitchPin);
    if(buttonValue == 5) pumpCutINLevelEditFlag = true;
   }
   else{
     lcd.setCursor(1,1);lcd.write(0);
     lcd.setCursor(14,1); lcd.write(1);
     buttonValue = buttonRead(inputSwitchPin);
     if(buttonValue == 4) {
       pumpCutINLevelSetting = --pumpCutINLevelSetting; // changing the parameter in edit menu by pressing leftbutton
       if (pumpCutINLevelSetting == 0) pumpCutINLevelSetting = 9;
     }
     else if(buttonValue == 3) {
       pumpCutINLevelSetting = ++pumpCutINLevelSetting; // changing the parameter in edit menu by pressing leftbutton
       if (pumpCutINLevelSetting == 10) pumpCutINLevelSetting = 1;
     }
     buttonValue = buttonRead(inputSwitchPin);
     if(buttonValue == 5){                                              // pressing enter in edit make save change to eeprom and exit from edit option to operation menu
      EEPROM.update(8, pumpCutINLevelSetting);
      pumpCutINLevelEditFlag = false;
     }
     else if(buttonValue == 2){                                              // back button back from edit to operation mode menu
      pumpCutINLevelEditFlag = false;
     }
   }
  break;
  case 6:
  //********************* display update section ***********************************
   lcd.clear();
   lcd.setCursor(1,0); lcd.print("Pump Schedule");
   lcd.setCursor(8,1); lcd.print("Hrs");
   switch(pumpScheduleTimeSetting){
     case 1: 
     lcd.setCursor(6,1); lcd.print("OFF  ");
     break;
     case 2:
      lcd.setCursor(5,1); lcd.print("6");
     break;
     case 3:
      lcd.setCursor(5,1); lcd.print("8");
     break;
     case 4:
      lcd.setCursor(5,1); lcd.print("12");
     break;
     case 5:
      lcd.setCursor(5,1); lcd.print("24");
     break;
     default:
     pumpScheduleTimeSetting = 1; // to back to default if any error
     break;
   }
   //********************* submenu edit section *************************************
   if(!pumpScheduleEditFlag){
    buttonValue = buttonRead(inputSwitchPin);
    if(buttonValue == 5) pumpScheduleEditFlag = true;
   }
   else{
     lcd.setCursor(1,1);lcd.write(0);
     lcd.setCursor(14,1); lcd.write(1);
     buttonValue = buttonRead(inputSwitchPin);
     if(buttonValue == 4) {
       pumpScheduleTimeSetting = --pumpScheduleTimeSetting; // changing the parameter in edit menu by pressing leftbutton
       if (pumpScheduleTimeSetting == 0) pumpScheduleTimeSetting = 5;
     }
     else if(buttonValue == 3) {
       pumpScheduleTimeSetting = ++pumpScheduleTimeSetting; // changing the parameter in edit menu by pressing leftbutton
       if (pumpScheduleTimeSetting == 6) pumpScheduleTimeSetting = 1;
     }
     buttonValue = buttonRead(inputSwitchPin);
     if(buttonValue == 5){                                              // pressing enter in edit make save change to eeprom and exit from edit option to operation menu
      EEPROM.update(2, pumpScheduleTimeSetting);
      pumpScheduleEditFlag = false;
     }
     else if(buttonValue == 2){                                              // back button back from edit to operation mode menu
      pumpScheduleEditFlag = false;
     }
   }
  break;
  case 3 :
   //********************* display update section ***********************************
   lcd.clear();
   lcd.setCursor(0,0); lcd.print("Low Volt Cutoff");
   if(lowVoltCutoffSetting){
    lcd.setCursor(7,1); lcd.print("ON");
   }
   else
   {
     lcd.setCursor(6,1); lcd.print("OFF");
   }
   //********************* submenu edit section *************************************
   if(!lowVoltCutoffEditFlag){
    buttonValue = buttonRead(inputSwitchPin);
    if(buttonValue == 5) lowVoltCutoffEditFlag = true;
   }
   else{
     lcd.setCursor(1,1);lcd.write(0);
     lcd.setCursor(14,1); lcd.write(1);
     buttonValue = buttonRead(inputSwitchPin);
     if(buttonValue == 4) lowVoltCutoffSetting = !lowVoltCutoffSetting;        // changing the parameter in edit menu by pressing left and right button
     else if(buttonValue == 3) lowVoltCutoffSetting = !lowVoltCutoffSetting;
     buttonValue = buttonRead(inputSwitchPin);
     if(buttonValue == 5){                                              // pressing enter in edit make save change to eeprom and exit from edit option to operation menu
      EEPROM.update(3, lowVoltCutoffSetting);
      lowVoltCutoffEditFlag = false;
     }
     else if(buttonValue == 2){                                              // back button back from edit to operation mode menu
      lowVoltCutoffEditFlag = false;
     }
   }
   
  break;
  case 5 :
  //********************* display update section ***********************************
   lcd.clear();
   lcd.setCursor(0,0); lcd.print("Pump A.off Timer");
   lcd.setCursor(7,1); lcd.print("Mins");
    switch(pumpAoffTimerSetting){
       case 1: 
       lcd.setCursor(6,1); lcd.print("OFF   ");
       break;
       case 2:
       lcd.setCursor(3,1); lcd.print("30");
       break;
       case 3:
       lcd.setCursor(3,1); lcd.print("45");
       break;
       case 4:
       lcd.setCursor(3,1); lcd.print("60");
       break;
       case 5:
       lcd.setCursor(3,1); lcd.print("90");
       break;
       case 6:
       lcd.setCursor(3,1); lcd.print("120");
       break;
       default:
       pumpAoffTimerSetting = 3; // to back to default if any error
       break;
     }
     //********************* submenu edit section *************************************
   if(!pumpAoffTimerEditFlag){
    buttonValue = buttonRead(inputSwitchPin);
    if(buttonValue == 5) pumpAoffTimerEditFlag = true;
   }
   else{
     lcd.setCursor(1,1);lcd.write(0);
     lcd.setCursor(14,1); lcd.write(1);
     buttonValue = buttonRead(inputSwitchPin);
     if(buttonValue == 4) {
       pumpAoffTimerSetting = --pumpAoffTimerSetting; // changing the parameter in edit menu by pressing leftbutton
       if (pumpAoffTimerSetting == 0) pumpAoffTimerSetting = 6;
     }
     else if(buttonValue == 3) {
       pumpAoffTimerSetting = ++pumpAoffTimerSetting; // changing the parameter in edit menu by pressing leftbutton
       if (pumpAoffTimerSetting == 7) pumpAoffTimerSetting = 1;
     }
     buttonValue = buttonRead(inputSwitchPin);
     if(buttonValue == 5){                                              // pressing enter in edit make save change to eeprom and exit from edit option to operation menu
      EEPROM.update(4, pumpAoffTimerSetting);
      pumpAoffTimerEditFlag = false;
     }
     else if(buttonValue == 2){                                              // back button back from edit to operation mode menu
      pumpAoffTimerEditFlag = false;
     }
    }
  break;
  case 4:
   //********************* display update section ***********************************
   lcd.clear();
   lcd.setCursor(0,0); lcd.print("Dryrun Pump OFF");
   if(dryrunCutoffSetting){
    lcd.setCursor(7,1); lcd.print("ON");
   }
   else
   {
     lcd.setCursor(6,1); lcd.print("OFF");
   }
   //********************* submenu edit section *************************************
   if(!dryrunCutoffEditFlag){
    buttonValue = buttonRead(inputSwitchPin);
    if(buttonValue == 5) dryrunCutoffEditFlag = true;
   }
   else{
     lcd.setCursor(1,1);lcd.write(0);
     lcd.setCursor(14,1); lcd.write(1);
     buttonValue = buttonRead(inputSwitchPin);
     if(buttonValue == 4) dryrunCutoffSetting = !dryrunCutoffSetting;        // changing the parameter in edit menu by pressing left and right button
     else if(buttonValue == 3) dryrunCutoffSetting = !dryrunCutoffSetting;
     buttonValue = buttonRead(inputSwitchPin);
     if(buttonValue == 5){                                              // pressing enter in edit make save change to eeprom and exit from edit option to operation menu
      EEPROM.update(5, dryrunCutoffSetting);
      dryrunCutoffEditFlag = false;
     }
     else if(buttonValue == 2){                                              // back button back from edit to operation mode menu
      dryrunCutoffEditFlag = false;
     }
   }
  break;
  case 7 :
   //********************* display update section ***********************************
   lcd.clear();
   lcd.setCursor(3,0); lcd.print("Brightness");
   lcd.setCursor(9,1); lcd.print("%");
    switch(brightValueSetting){
     case 1: 
     lcd.setCursor(6,1); lcd.print("10");
     break;
     case 2:
      lcd.setCursor(6,1); lcd.print("25");
     break;
     case 3:
      lcd.setCursor(6,1); lcd.print("50");
     break;
     case 4:
      lcd.setCursor(6,1); lcd.print("75");
     break;
     case 5:
      lcd.setCursor(6,1); lcd.print("100");
     break;
     default:
     brightValueSetting = 4; // to back to default if any error
     break;
   }
        //********************* submenu edit section *************************************
   if(!brightValueEditFlag){
    buttonValue = buttonRead(inputSwitchPin);
    if(buttonValue == 5) brightValueEditFlag = true;
   }
   else{
     lcd.setCursor(1,1);lcd.write(0);
     lcd.setCursor(14,1); lcd.write(1);
     buttonValue = buttonRead(inputSwitchPin);
     if(buttonValue == 4) {
       brightValueSetting = --brightValueSetting; // changing the parameter in edit menu by pressing leftbutton
       if (brightValueSetting == 0) brightValueSetting = 5;
     }
     else if(buttonValue == 3) {
       brightValueSetting = ++brightValueSetting; // changing the parameter in edit menu by pressing leftbutton
       if (brightValueSetting == 6) brightValueSetting = 1;
     }
     buttonValue = buttonRead(inputSwitchPin);
     if(buttonValue == 5){                                              // pressing enter in edit make save change to eeprom and exit from edit option to operation menu
      EEPROM.update(6, brightValueSetting);
      if(brightValueSetting  == 1) brightValue = 20;
      else if(brightValueSetting  == 2) brightValue = 55;
      else if(brightValueSetting  == 3) brightValue = 140;
      else if(brightValueSetting  == 4) brightValue = 200;
      else if(brightValueSetting  == 5) brightValue = 255;
      EEPROM.update(7, brightValue);
      brightValueEditFlag = false;
     }
     else if(buttonValue == 2){                                              // back button back from edit to operation mode menu
      brightValueEditFlag = false;
     }
    }
  break;
  
  case 8:                     //Sensor Unit Parameters 
// **************************************************Display Update Section************************************************************************************

   lcd.clear();
   lcd.setCursor(1,0); lcd.print("Tx Unit Detail");
   lcd.setCursor(0,1); lcd.print("B+:"); lcd.setCursor(8,1); lcd.print("Chrg:");
   if(!batLow){lcd.setCursor(3,1); lcd.print("Good");}
   else {lcd.setCursor(3,1); lcd.print("Bad");}
   if(solarCharg){lcd.setCursor(13,1); lcd.print("Yes");}
   else {lcd.setCursor(13,1); lcd.print("No");}

  break;

  case 9:                             // factory reset - default value setting
  //********************* display update section ***********************************
   lcd.clear();
   lcd.setCursor(1,0); lcd.print("Factory Reset");
      //********************* submenu edit section *************************************
   if(!factoryResetEditFlag){
    buttonValue = buttonRead(inputSwitchPin);
    if(buttonValue == 5) factoryResetEditFlag = true;
   }
   else{
     lcd.setCursor(0,1); lcd.print("Confirm:press OK");
     buttonValue = buttonRead(inputSwitchPin);
     if(buttonValue == 5){                                              // pressing enter in edit make save change to eeprom and exit from edit option to operation menu
      EEPROM.update(1, true); // pumpOperationModeSetting
      EEPROM.update(2, 1); // pumpScheduleTimeSetting
      EEPROM.update(3, true); // lowVoltCutoffSetting
      EEPROM.update(4, 3); // pumpAoffTimerSetting
      EEPROM.update(5, true); // dryrunCutoffSetting
      EEPROM.update(6, 4); // brightValueSetting
      EEPROM.update(7, 200); // brightValueSetting
      EEPROM.update(8, 2); // pumpCutINLevel Setting

      factoryResetEditFlag = false;
      lcd.clear();
      lcd.setCursor(1,0); lcd.print("Factory Reset");
      lcd.setCursor(1,1); lcd.print("*Successfull*");
      delay(1000);
      lcd.clear();
      lcd.setCursor(2,0); lcd.print("Resetting...");
      delay(1000);
      resetFunc(); //making system reset 
     }
     else if(buttonValue == 2){                                              // back button back from edit to operation mode menu
      factoryResetEditFlag = false;
     }
   }
  break;
  case 10 :                            // back
   lcd.clear();
   lcd.setCursor(6,0); lcd.print("Back");
   if(buttonValue == 5) {
     homeScreenFlag = 0;                                       // for going to home screen
     menuTimeoutCounterFlag = false;                           // for resetting the timeout timer
     mainmenuSelectCaseNo = 1;                                 // to restart sub menu from 1
     lcd.createChar(0, powerSymbol);                           // ac present and good symbol adding because max 8 char allowed in lcd memory
     lcd.createChar(1, abnormal);                              // abonrmal symbol is ! adding because max 8 char allowed in lcd memory
     // remove edit option after presing back
     operationModeEditFlag = false;
     pumpCutINLevelEditFlag = false;
     pumpScheduleEditFlag = false;
     lowVoltCutoffEditFlag = false;
     pumpAoffTimerEditFlag = false;
     dryrunCutoffEditFlag = false;
     brightValueEditFlag = false;
     factoryResetEditFlag = false;
   }
  break;
  default: 
   mainmenuSelectCaseNo = 1;
   break;
}
}