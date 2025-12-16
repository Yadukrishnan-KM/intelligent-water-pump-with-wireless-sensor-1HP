  void rfTransmission(){
   digitalWrite(d0Pin, !d[0]);                     // complement because hardware using one npn transitor at output; to cancel out that effect
   digitalWrite(d1Pin, !d[1]);
   digitalWrite(d2Pin, !d[2]);
   digitalWrite(d3Pin, !d[3]);
   digitalWrite(rfTxPwrPin, HIGH);
                                                                Serial.println("rf transmission enable");
   delay(50);
   for (int i = 0; i <= 2; i++){                   // sending 2 data stremas of same
     digitalWrite(txEnablePin, HIGH);
     delay(50);
     digitalWrite(txEnablePin, LOW);
     delay(100);
    }
   digitalWrite(txEnablePin, LOW);                 // turning off transmission
                                                                Serial.println("rf transmission disable");
   digitalWrite(rfTxPwrPin, LOW);
  }