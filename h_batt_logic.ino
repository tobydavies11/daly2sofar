/*

//============================ battery logic, to decide on which values to send to inverter
void batterylogic(){
  Serial.println("VOID: battery logic");
//this section compares both bs vales and decides if dual ops can run
// values from this bms ok?
//  vakes from other bms ok?
// HAbmssts ok?
// then allow dual ops

//** needs to be inhibited for x secs after start to allow mqtt mesages to be rx and stabilised


//unsigned long currentMillisBatteryLogic= millis();  // this is just in this loop
      
// bool battLogicStartSettle = false ; // false = stop| True = can Run

if (battLogicStartSettle == false){
     //unsigned long currentMillisBatteryLogic = millis();  // allow to run this calc until a true result occurs then stop
     currentMillisBatteryLogic = millis();  // allow to run this calc until a true result occurs then stop
   }
 if (currentMillisBatteryLogic + previousMillisBatteryLogic >=  intervalBatteryLogic) {
  battLogicStartSettle = true ;
}

#ifdef BANK_A
        //x              +  0                        >= 40000.no run, gretaer than 60000 will run
     // if (currentMillisBatteryLogic + previousMillisBatteryLogic >=  intervalBatteryLogic) {
        if (battLogicStartSettle){
          
         //previousMillisBatteryLogic = currentMillisBatteryLogic; // this may need to be reset when other bms is reported viable again
        
         if(BMSOnline && CANOnline){

          
          BmsA = 1; //set as master for standard config 
           //BMSBsts rx from mqtt to indicate its online 
           // 1 = ON , 2 = OFF , 5= unknown         
            if (BMSBsts == 1){
                   //HAbmsBsts is monitored by HA
                  if (HaBmsBsts == 1){
                         int temp1 = BmsB;
                         if (temp1 != 2){
                           BmsB = 2; //0:unknonw 1:master 2:slave
                           interbmsstatus(); //send update via mqtt
                         }
                          // allow dual values calc                       
                        batteryLogic = 2; // 0:unknown 1:single 2:dual
                        Serial.println("Battery logic should be now 2:" + String(batteryLogic));
                         
                        }
                      }
               else{
                //although its set for 1 at boot, this returns to 1 during a loss of dual syatems
                 batteryLogic = 1; //single bank operations , boots up in 1 for both
                 Serial.println("Battery logic:" + String(batteryLogic));
               }
             }            
          } 
#else
// insert oppositee of above here 
// banks B
     // if (currentMillisBatteryLogic + previousMillisBatteryLogic >=  intervalBatteryLogic) {
      if (battLogicStartSettle){  
      
          //previousMillisBatteryLogic = currentMillisBatteryLogic; // this may need to be reset when other bms is reported viable again
        
         //if(BMSOnline && CANOnline){
         if(BMSOnline){
          //BmsB = 1; //set as master for standard config 
           //BMSBsts rx from mqtt to indicate its online 
           // 1 = ON , 2 = OFF , 5= unknown         
            if (BMSAsts == 1){
                   BmsB = 2; //slave mode
                   //HAbmsBsts is monitored by HA
                  if (HaBmsAsts != 2){
                         int temp1 = BmsA;
                         if (temp1 != 2){
                           BmsA = 2; //0:unknonw 1:master 2:slave
                           interbmsstatus(); //send update via mqtt
                         }
                          // allow dual values calc                       
                        batteryLogic = 2; // 0:unknown 1:single 2:dual
                        Serial.println("Dual Battery values now in use"
                        );
                        Serial.println("Battery logic should be now 2:" + String(batteryLogic));
                         
                        }
                      }
               else{
                 BmsB = 3; //indepandant
                //although its set for 1 at boot, this returns to 1 during a loss of dual syatems
                 batteryLogic = 1; //single bank operations , boots up in 1 for both
                 Serial.println("Battery logic:" + String(batteryLogic));
               }
             }            
          }                  
#endif
    if (batteryLogic == 1){
      digitalWrite(ledPins[1], HIGH); //turn on 6th LED   
    }
    else{
      digitalWrite(ledPins[1], LOW); //turn on 6th LED
    }
}

*/
void canbuscontrollogic(){
// canbus control here
// CanBussPrio = 0; // 0 = unknown | 1: bank A | 2: bank B | 3: indepandant both run
// CanBussPrio = auto mode to allow one other to tx canbus
// CanbusInhibit = from HA to allow a remote stop
// CanControl = final stop
/*#ifdef BANK_A  
   if(CanBussPrio == 1){
        //this loop to transfer can control to Bankb Can interface
             //can inhibit rx from mqtt or ha to allow a remote stop outside of the loop
            if (CanbusInhibit = 1){
                  CanControl = 1; //allow canbus to tx
                  }
            else{
                 CanControl = 2; // stop can bus from tx this side
                }
              }
    if(CanBussPrio == 2){
                 CanControl = 2; // stop can bus from tx this side              
              }
    if(CanBussPrio == 3){
       CanControl = 1;   // allow can to run, possible dual canbus       
    }
#else //bank B

//CanBussPrio = 0; // 0 = unknown/bootup | 1: bank A | 2: bank B | 3: indepandant both run
// 
   if(CanBussPrio == 2){
        //this loop to transfer can control to Bankb Can interface
             //can inhibit rx from mqtt or ha to allow a remote stop outside of the loop
            if (CanbusInhibit = 1){
                  CanControl = 1; //allow canbus to tx
                  }
            else{
                 CanControl = 2; // stop can bus from tx this side
                }
              }
    if(CanBussPrio == 1){
                 CanControl = 2; // stop can bus from tx this side              
              } 
    if(CanBussPrio == 3){
       CanControl = 1;   // allow can to run, possible dual canbus       
    }                   
#endif
*/
    if (CanControl == 1){
      digitalWrite(ledPins[2], LOW); //turn on 6th LED   
    }
    else{
      digitalWrite(ledPins[2], HIGH); //turn on 6th LED
    }
    eolterm(); 
}
//=============================================================

/*
void BatteryInitialFetControl(){
  // on boot bank a mosfet on, bank b mosfert off - if bank a running solo it will be ok
  // if bank b and bank a failed need to allow it to come online
  // allow time to settle
  // 
#ifdef BANK_A
// mosftets should be on
#else //BANK_B
  // mosfets should be off
  // allow to settle
  // check to see if bank a values recevied?
  // if yes compare values for possble connection
  // compare volts
  //compare Soc within 3% allow join of all mosfets
 // float socDiff = 0;
//  int battinitFetCtllogic = 0; // this captures where the logic is and reports back to HA
//  bool batteryInitialFetControl = false;
    battinitFetCtllogic = 1; // 0 is bootup
    socDiff = SocA - SocB; 
    if(batteryInitialFetControl == 1){// remote control inhibit 1:run 2: stop
      battinitFetCtllogic = 2; // 0 is bootup
     
     if ((SocA >=15)&&(SocB >=15)){               // to allow it only to run on reception of both data 
        battinitFetCtllogic = 3; // 0 is bootup
       
        if ((SocA <=99)&&(SocB <=99)){             // under 99% inc case of full cutouts
           battinitFetCtllogic = 4; // 0 is bootup
          
          if((socDiff >= -3)&&(socDiff <= 3) ){    // within 3% allow normal join
             battinitFetCtllogic = 5; // 0 is bootup
              //mosfets on normal operations
              //ChgFetOn();
              //DisFetOn();
             }
          if (bmsStateA == 1){// 1:stationary 2:charge 3:discharge
              battinitFetCtllogic = 6; // 0 is bootup
              // do nothing and leave these mosfets off
             }
          if (bmsStateA == 2){// 1:stationary 2:charge 3:discharge
             battinitFetCtllogic = 7; // 0 is bootup
              if(SocA > SocB){
                battinitFetCtllogic = 8; // 0 is bootup
              // turn on chg mosfet on bank b 
               // ChgFetOn();
              // turn off chg mosfet on bank A
                 }
               if(SocB > SocA){
                battinitFetCtllogic = 9; // 0 is bootup
                   if (MosChgA == 1){
                    battinitFetCtllogic = 10; // 0 is bootup
                   // ChgFetOff();
                   }                   
                   // check bms a chg fet is on
                 }
             }
              
          if (bmsStateA == 3){// 1:stationary 2:charge 3:discharge
              battinitFetCtllogic = 11; // 0 is bootup
              if(SocA > SocB){
                battinitFetCtllogic = 12; // 0 is bootup
                if (MosDisA == 1){
                  battinitFetCtllogic = 13; // 0 is bootup
                  //DisFetOff();
                   }                 
                 //ensure a disfet is on
                 }
              if(SocB > SocA){
                battinitFetCtllogic = 14; // 0 is bootup
              // turn on dis mosfet on bank b
                 // DisFetOn();// this side
              // turn off dis mosfet on bank A
                 }   
             }
             display.display() ; //refresh the display   
         }
      }
    }
 // cater for bank a either off or failed and bank b to run 
 //HAbmsAsts offline detection && bank B only?
      if (BmsB == 1){ // slave bmaA in charge
         
         battinitFetCtllogic = 15; // 0 is bootup
         // not sure yet
      }
 
     if (BmsB == 3){ // this is indepandant mode for testing its not called for anywhere in this sketch to prevent unwanted activation
      battinitFetCtllogic = 16; // 0 is bootup
        ChgFetOn();
        DisFetOn(); 
       }
#endif 


}

*/
