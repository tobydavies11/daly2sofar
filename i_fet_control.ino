
// mosfet controls

//====================================
void ChgFetOn(){
  if(bms.get.chargeFetState == 0){
  bms.setChargeMOS(true);// turn on mosfet
  display.drawBitmap(70, 0, chargeOn, 29, 13, SH1106_WHITE);
  display.display();
  if ( BMSOnline){
    Serial.println("BMSOnline status: updating is automatic");
           }
  else{
    bms.get.chargeFetState = 1 ; // possibly to be removed when live bms connected 
    Serial.println("BMSOffline status:updating is manual disfet "); 
    }
  }
}
void ChgFetOff(){
  if(bms.get.chargeFetState == 1){
  bms.setChargeMOS(false);
  display.drawBitmap(70, 0, chargeOff, 29, 13, SH1106_WHITE);
  display.display();
  if ( BMSOnline){
    Serial.println("BMSOnline status: updating is automatic");
           }
  else{
    bms.get.chargeFetState = 0 ; // possibly to be removed when live bms connected 
    Serial.println("BMSOffline status:updating is manual disfet "); 
    }
  }
  }
void DisFetOn(){
  if(bms.get.disChargeFetState == 0){
  bms.setDischargeMOS(true); //turn off mosfet
  display.drawBitmap(70, 14, disOn, 29, 13, SH1106_WHITE);
  display.display();
  if ( BMSOnline){
    Serial.println("BMSOnline status: updating is automatic");
           }
  else{
    bms.get.disChargeFetState = 1 ; // possibly to be removed when live bms connected 
    Serial.println("BMSOffline status:updating is manual disfet "); 
    }
}
}
void DisFetOff(){
  if(bms.get.disChargeFetState  == 1){
  bms.setDischargeMOS(false); //turn off mosfet
  display.drawBitmap(70, 14, disOff, 29, 13, SH1106_WHITE);
  display.display();
    if ( BMSOnline){
    Serial.println("BMSOnline status: updating is automatic");
           }
  else{
    bms.get.disChargeFetState = 0 ; // possibly to be removed when live bms connected 
    Serial.println("BMSOffline status:updating is manual disfet "); 
    }
 }
}
//=============================================================
