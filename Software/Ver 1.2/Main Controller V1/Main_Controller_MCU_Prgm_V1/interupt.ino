void interupt(){
  for(int i = 0; i <= 3; i++){                           // saving all the received data bits into a temporary locaton
    b[i] = digitalRead((5+i));
  }
  if(b[3] && b[2] && !b[1] && !b[0]) batLow = true;
  else if (b[3] && b[2] && !b[1] && b[0]) batLow = false;
  else if (b[3] && b[2] && b[1] && !b[0]) solarCharg = true;
  else if (b[3] && !b[2] && b[1] && b[0]) solarCharg = false;
  else if (b[3] && b[2] && b[1] && b[0]) sensorOutofRange = true;
  else if (!b[3] && !b[2] && !b[1] && !b[0]) sensorOutofRange = false;
  
  else{
    for(int j =0; j <= 3; j++){                           // saving all the received data bits into a global variable after confirming it is water level
      d[j] = b[j];
    }
  }
  if(voltPresent) analogWrite(lcdLEDBrightPin, 25);              // to reduce backlight if ac not present
  else analogWrite(lcdLEDBrightPin, brightValue);              // to turn on backlight 
  previousCounterTransmitterTimeout = 0;                  // for resetting the transmitter missing timer
  currentCountertransmitterTimeout = 0;
  transmitterTimeoutCounterFlag = true;
}