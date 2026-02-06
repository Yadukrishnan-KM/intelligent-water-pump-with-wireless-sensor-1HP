void pwrBordReading(){
  // dry run section, check for low current only if the motor is on

if(motorPinStatusFlag){                                                // only check if motor is in run condition
  //                                                                             Serial.println("dryrun inside first if");
  dryRun = digitalRead(dryRunPin);                                     // read current only if motor is on. 1 indicating normal 0 indicating dryrun
  if (dryRun && dryrunCutoffSetting){
  dryrunCounter = ++dryrunCounter;                                     // section for ensuring the previous mills only works once in the if condition
  if (dryrunCounter == 1) previousMills0 = millis();
  currentMills0 = millis();
  if((currentMills0 - previousMills0) >= 30000) dryRunFlag = true;     // time period of 30 sec for ensuring that it is dry run. detect bt low current draw(by pcb hardware)
}
else {
   dryrunCounter = 0;                                                  // reseting the dryrunCounter
   dryRunFlag = false;
  }
} 
else{
   dryrunCounter = 0;                                                  // reseting the dryrunCounter
   //dryRunFlag = false;
}

// low volt section, check always, give delay if motor is on

 lowVolt = digitalRead(lowVoltPin);                                    // 0 indicating low volt condition 1 indicate no voltage prblm
 if(motorPinStatusFlag){                                               // during pump running
  //                                                                             Serial.println("low volt inside first if");
  if(lowVolt && lowVoltCutoffSetting) {                               // checking low voltage condition and the seetings
    lowVoltCounter = ++lowVoltCounter;                                 // section for ensuring the previous mills only works once in the if condition
    if (lowVoltCounter == 1) previousMills1 = millis();
   currentMills1 = millis();
  if((currentMills1 - previousMills1) >= 30000) lowVoltFlag = true;    // if pump is on wait for 30sec for confirming the low voltage condition

  }
  else {
    lowVoltFlag = false;
    lowVoltCounter = 0;                                                // resting the counter
  }
 }
 else {                                                                // if motor is not on instantly making lowvoltflag to flase to prevent new motor power on, normal
  //                                                                              Serial.println("low volt last else normal");
  if(lowVolt && lowVoltCutoffSetting) lowVoltFlag = true;              // checking low voltage condition and the seetings
  else lowVoltFlag = false;
  lowVoltCounter = 0;
 }           


  // Volt present section, always check, if motor is on suply fails then restart only after some time
 voltPresent = digitalRead(voltPresentPin);                         // 0 indicating voltage present and 1 indication not present
 if(motorPinStatusFlag){
  //                                                                            Serial.println("volt present inside first if");
  if(voltPresent) {
    voltPresentCounter = ++voltPresentCounter;                                // section for ensuring the previous mills only works once in the if condition
    //                                                                          Serial.println(voltPresentCounter);
    if (voltPresentCounter == 1) previousMills2 = millis();
    currentMills2 = millis();
    //                                                                          Serial.print("volt present previous millis: ");Serial.println(previousMills2);
    //                                                                          Serial.print("volt present current millis: ");Serial.println(currentMills2);
    if((currentMills2 - previousMills2) >= 1000) {
      voltPresentFlag = false;                                                // if pump is on, wait for 1sec for confirming the no voltage condition
      pwrFailFlag = true;
                                                                             // Serial.print("volt present powerfailflag is: ");Serial.println(pwrFailFlag);
  }
  }
   else {
    voltPresentFlag = true;
    pwrFailFlag = false;
    voltPresentCounter = 0;
    voltPresentCounter1 = 0;                                                   // resting the counter
  }  
 }
 else if(motorStateMemory && pwrFailFlag){              
    //                                                                          Serial.println("volt present inside elseif");                       // if powerfailed during running
    if(!voltPresent){
      voltPresentCounter1 = ++voltPresentCounter1;                             // section for ensuring the previous mills only works once in the if condition
      if (voltPresentCounter1 == 1) previousMills3 = millis();
      currentMills3 = millis();
      if((currentMills3 - previousMills3) >= 20000) {                          // pump is waiting to turn on for 20 sec after a powewrfailure in running condition
        voltPresentFlag = true;
        pwrFailFlag = false;
    }

  }
  else{
    voltPresentFlag = false;
    voltPresentCounter1 = 0;
    voltPresentCounter = 0;
  }

 }
 else{                                                                          // for pump off condition, normal condition
  if(!voltPresent) voltPresentFlag = true;
  else voltPresentFlag = false;
  voltPresentCounter = 0;
  voltPresentCounter1 = 0;
   //                                                                              Serial.println("voltpresent inside last else normal");
 }

}