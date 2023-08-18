//
// led usuage
// 0 =  change to when mqtt has been sent ///main program loop - oscilaltes each run
// 1 = batt logic, off single batt, on dual batt,        
// 2 = can control on = can run , off is is off       
// 3 = canbus send, off using single batt data, on using dual data  ** yet to be added**
// 4 = canbus rx message
// 5 = mqtt message has been received
// 6 = call data from daly
// 7 = send data on canbus
//====================

void eolterm(){
  Serial.println("=======================END of Void===========================");
}
void gpioInputread(){
  //read the gpio here
     DipSw2 = digitalRead(DipSwitchPins[0]);
     DipSw3 = digitalRead(DipSwitchPins[1]);
     DipSw4 = digitalRead(DipSwitchPins[2]);
     DipSw5 = digitalRead(DipSwitchPins[3]);
     BattMasterSw = digitalRead(32);
     CanStopDipSw = DipSw2; // final hardware stop of can
     
}
void runTimeUpdate(){
  //calculate uptime here
 // unsigned long currentMillis = millis();
 // unsigned long seconds = currentMillis / 1000;
 // unsigned long minutes = seconds / 60;
 // unsigned long hours = minutes / 60;
 // unsigned long days = hours / 24;
  currentMillis = millis();
  seconds = currentMillis / 1000;
  minutes = seconds / 60;
  hours = minutes / 60;
  days = hours / 24;
  currentMillis %= 1000;
  seconds %= 60;
  minutes %= 60;
  hours %= 24;
}
void ledcontrol(){
  // only used during startp currently
 // int ledPins[] = {14, 13, 26, 23, 25, 27, 12, 19};
  // loop from the lowest pin to the highest:

  for (int thisPin = 0; thisPin < pinCount; thisPin++) {
      digitalWrite(ledPins[thisPin], LOW);
      delay(timer);
      digitalWrite(ledPins[thisPin], HIGH);
      }

  // loop from the highest pin to the lowest:

  for (int thisPin = pinCount - 1; thisPin >= 0; thisPin--) {
      digitalWrite(ledPins[thisPin], LOW);
      delay(timer);
      digitalWrite(ledPins[thisPin], HIGH);
      }
 
  // used for led control to keep common
  /// turn LED on
  //  digitalWrite(ledPin1, HIGH);    
   // digitalWrite(ledPin1, LOW);
       eolterm();
}
//================ update oled screen========================
void oledMessage(String line1, String line2, String line3, String line4, String line5, String line6) {//bool WiFiStatus, bool MQTTStatus) {
  Serial.println("oledmessage");
  display.clearDisplay();
  display.setTextSize(1);
  //display.setTextColor(WHITE);
  display.setTextColor(SH1106_WHITE); //added for 1106 library
  
  display.setCursor(0, 0);
  if (line1 != "NULL") {
    display.println(line1);
    oledLine1 = line1;
  } else {
    display.println(oledLine1);
  }

  display.setCursor(0, 12);
  if (line2 != "NULL") {
    display.println(line2);
    oledLine2 = line2;
  } else {
    display.println(oledLine2);
  }

  display.setCursor(0, 24);
  if (line3 != "NULL") {
    display.println(line3);
    oledLine3 = line3;
  } else {
    display.println(oledLine3);
  }

  display.setCursor(0, 36);
  if (line4 != "NULL") {
    display.println(line4);
    oledLine4 = line4;
  } else {
    display.println(oledLine4);
  }

  display.setCursor(0, 48);
  if (line5 != "NULL") {
    display.println(line5);
    oledLine5 = line5;
  } else {
    display.println(oledLine5);
  }

  display.setCursor(0, 60);
  if (line6 != "NULL") {
    display.println(line6);
    oledLine6 = line6;
  } else {
    display.println(oledLine6);
  }
//float BmsA = 0;  // identify which bms is in charge  
//float BmsB = 0;  // other bms
// 0 = invalid/bootup
// 1 = master (master)
// 2 = slave (slave)
// 3 = indepandant mode, mainly for testing, eg running other bank before dual ops (isol)
// 4 = error
  if (BMSOnline) {
    display.drawBitmap(99, 0, dalyOn, 29, 13, SH1106_WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
/*#ifdef BANK_A
   if (BmsA == 1){display.drawBitmap(99, 0, master, 29, 13, SH1106_WHITE);}
   if (BmsA == 2){display.drawBitmap(99, 0, slave, 29, 13, SH1106_WHITE);}
   if (BmsA == 3){display.drawBitmap(99, 0, isol, 29, 13, SH1106_WHITE);}
   if (BmsA == 0){display.drawBitmap(99, 0, boot, 29, 13, SH1106_WHITE);}
#else
   if (BmsB == 1){display.drawBitmap(99, 0, master, 29, 13, SH1106_WHITE);}
   if (BmsB == 2){display.drawBitmap(99, 0, slave, 29, 13, SH1106_WHITE);}
   if (BmsB == 3){display.drawBitmap(99, 0, isol, 29, 13, SH1106_WHITE);}
   if (BmsB == 0){display.drawBitmap(99, 0, boot, 29, 13, SH1106_WHITE);}
#endif
*/
  } 
  else {
    display.drawBitmap(99, 0, dalyOff, 32, 13, SH1106_WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
  }

  if (CANOnline) {
  //  display.drawBitmap(99, 14, canOn, 29, 13, SH110X_WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
    display.drawBitmap(99, 28, canOn, 29, 13, SH1106_WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
  } else {
  //  display.drawBitmap(99, 14, canOff, 32, 13, SH110X_WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
    display.drawBitmap(99, 28, canOff, 32, 13, SH1106_WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
  }
#ifdef BANK_A
 display.drawBitmap(99, 14, bnkA, 29, 13, SH1106_WHITE);
#else
 display.drawBitmap(99, 14, bnkB, 29, 13, SH1106_WHITE);
#endif 
/*  if (WiFiStatus) {
  //  display.drawBitmap(99, 28, wifiOn, 29, 13, SH110X_WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
    display.drawBitmap(99, 14, wifiOn, 29, 13, SH1106_WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
  } else {
  //  display.drawBitmap(99, 28, wifiOff, 32, 13, SH110X_WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
    display.drawBitmap(99, 14, wifiOff, 32, 13, SH1106_WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
  }

  if (MQTTStatus) {
    display.drawBitmap(99, 42, mqttOn, 29, 13, SH1106_WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
   // display.drawBitmap(99, 42, chargeOn, 29, 13, SH110X_WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
  } else {
    display.drawBitmap(99, 42, mqttOff, 29, 13, SH1106_WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
   //display.drawBitmap(99, 42, chargeOff, 29, 13, SH110X_WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color) 
  }
  */
  if (MQTTStatus) {
    display.drawBitmap(99, 42, mqttOn, 29, 13, SH1106_WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
  } 
  else {
    if(WiFiStatus){
      display.drawBitmap(99, 42, mqttOff, 29, 13, SH1106_WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
    }
    else{
      display.drawBitmap(99, 42, wifiOff, 29, 13, SH1106_WHITE);
     }
     }
     
  
  
// new for charge mosfet
  if (bms.get.chargeFetState == true) {
   // display.drawBitmap(99, 42, mqttOn, 29, 13, SH110X_WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
    display.drawBitmap(70, 0, chargeOn, 29, 13, SH1106_WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
  } else {
    //display.drawBitmap(99, 42, mqttOff, 29, 13, SH110X_WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
   display.drawBitmap(70, 0, chargeOff, 29, 13, SH1106_WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color) 
  }

  // new for charge mosfet
  if (bms.get.disChargeFetState == true) {
   // display.drawBitmap(99, 42, mqttOn, 29, 13, SH110X_WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
    display.drawBitmap(70, 14, disOn, 29, 13, SH1106_WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
  } else {
    //display.drawBitmap(99, 42, mqttOff, 29, 13, SH110X_WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
   display.drawBitmap(70, 14, disOff, 29, 13, SH1106_WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color) 
  }

// can stop on final control
  if (CanControl == 1) {
   // display.drawBitmap(99, 42, mqttOn, 29, 13, SH110X_WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
    display.drawBitmap(70, 28, canrun, 29, 13, SH1106_WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
  } else {
    //display.drawBitmap(99, 42, mqttOff, 29, 13, SH110X_WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
   display.drawBitmap(70, 28, canstop, 29, 13, SH1106_WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color) 
  }

/*// battery single or dual logic
  if (batteryLogic == 2) {
   // display.drawBitmap(99, 42, mqttOn, 29, 13, SH110X_WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
    display.drawBitmap(70, 42, dual, 29, 13, SH1106_WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
  } else {
    //display.drawBitmap(99, 42, mqttOff, 29, 13, SH110X_WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
   display.drawBitmap(70, 42, single, 29, 13, SH1106_WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color) 
  } 
  */
  display.display();
  //display.drawBitmap(99, 0, chargeOn, 29, 13, SH110X_WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
   //display.drawBitmap(99, 14, chargeOff, 29, 13, SH110X_WHITE);
   //display.drawBitmap(99, 28, disOn, 29, 13, SH110X_WHITE);
   //display.drawBitmap(99, 42, disOff, 29, 13, SH110X_WHITE);
    //display.drawBitmap(0, 0, chargeOn, 29, 13, SH110X_WHITE); // display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)
   //display.drawBitmap(30, 0, chargeOff, 29, 13, SH110X_WHITE);
   //display.drawBitmap(59, 0, disOn, 29, 13, SH110X_WHITE);
   //display.drawBitmap(88, 0, disOff, 29, 13, SH110X_WHITE);
       eolterm(); 
}
