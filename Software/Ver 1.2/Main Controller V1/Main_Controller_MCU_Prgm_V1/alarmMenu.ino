  void alarmMenu(){
   if(alarm){
     lcd.clear();
     switch(alarmSelectionCaseNo){
       case 1: 
       if(!voltPresentFlag) {
        lcd.setCursor(1,0); lcd.print("Power Failure");
       }
       else ++alarmSelectionCaseNo;
       break;
       case 2:
       if(lowVoltFlag) {
        lcd.setCursor(1,0); lcd.print("AC Low Voltage");
       }
       else ++alarmSelectionCaseNo;
       break;
       case 3:
       if(dryRunFlag) {
        lcd.setCursor(2,0); lcd.print("Pump DryRun");
       }
       else ++alarmSelectionCaseNo;
       break;
       case 4:
       if(transmitterError) {
        lcd.setCursor(0,0); lcd.print("Sensor unit eror");
        lcd.setCursor(1,1); lcd.print("or unreachable");
       }
       else ++alarmSelectionCaseNo;
       break;
       case 5:
       if(batLow) {
        lcd.setCursor(2,0); lcd.print("Sensor unit");
        lcd.setCursor(2,1); lcd.print("Battery Low");
       }
       else ++alarmSelectionCaseNo;
       break;
       case 6:
       if(sensorOutofRange) {
        lcd.setCursor(2,0); lcd.print("Sensor unit");
        lcd.setCursor(2,1); lcd.print("Out of Range");
       }
       else ++alarmSelectionCaseNo;
       break;
       case 7:
       if(pumpTimerAlarm) {
        lcd.setCursor(0,0); lcd.print("PumpCutOFF Timer");
        lcd.setCursor(3,1); lcd.print("Activated");
       }
       else ++alarmSelectionCaseNo;
       break;
       case 8:
       if(pumpTxErrorAlarm) {
        lcd.setCursor(1,0); lcd.print("Tx Error While");
        lcd.setCursor(2,1); lcd.print("Pump Running");
       }
       else ++alarmSelectionCaseNo;
       break;
       case 9:
        lcd.setCursor(6,0); lcd.print("END");
        lcd.setCursor(0,1); lcd.print("Press OK to exit");

        buttonValue = buttonRead(inputSwitchPin);
        if(buttonValue == 5) {
         homeScreenFlag = 0;                                       // for going to home screen
         menuTimeoutCounterFlag = false;                           // for resettiing the timeout timer
         alarmSelectionCaseNo = 1;
        }
       break;
       default:
       alarmSelectionCaseNo = 1; // to back to default if any error
       break;
      }
    }
    else{
      lcd.clear();
      lcd.setCursor(4,0); lcd.print("NO Alarm ");
        buttonValue = buttonRead(inputSwitchPin);
        if(buttonValue == 2) {
         homeScreenFlag = 0;                                       // for going to home screen
         menuTimeoutCounterFlag = false;                           // for resettiing the timeout timer
        }
    }
  }