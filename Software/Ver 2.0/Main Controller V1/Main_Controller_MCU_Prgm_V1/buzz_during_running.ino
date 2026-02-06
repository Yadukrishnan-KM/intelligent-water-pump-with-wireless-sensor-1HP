void buzzer(){
 if( BuzzState == HIGH )
 {
    if( (millis()- rememberTime) >= onDuration){   
     BuzzState = LOW;
     digitalWrite(alarmBuzzerPin, BuzzState);
     rememberTime = millis();                      // remember Current millis() time
    }
 }
 else
 {   
    if( (millis()- rememberTime) >= offDuration){     
     BuzzState = HIGH;
     digitalWrite(alarmBuzzerPin, BuzzState);
     rememberTime = millis();                      // remember Current millis() time
    }
 }
}