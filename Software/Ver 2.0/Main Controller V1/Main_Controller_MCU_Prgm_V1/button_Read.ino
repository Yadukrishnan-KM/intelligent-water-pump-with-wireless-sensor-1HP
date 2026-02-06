int buttonRead(int analogPin){
  int button =0;
  int buttonThreshold = 50;                                         // range swing for detecting correct input between 0-1023

  int buttonValue = analogRead(analogPin);
  //                                                                                                                                     Serial.println(buttonValue);
  if (abs(buttonValue - 145) < buttonThreshold) button = 1;         // up button
  else if (abs(buttonValue - 329) < buttonThreshold) button = 2;    // down button
  else if (abs(buttonValue - 0) < buttonThreshold) button = 3;      // right button
  else if (abs(buttonValue - 505) < buttonThreshold) button = 4;    // left button
  else if (abs(buttonValue - 741) < buttonThreshold) button = 5;    // enter button
  else button = 0;
  return button;
}