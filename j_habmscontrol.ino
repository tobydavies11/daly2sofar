// this is to allow Ha to monitor the individual bms and report a failure
//=============================================================

/*
void Habmsstatus(){

// home assistant monirting of rx mqtt data from the bms
// if message hasnt been received for 180seconds it changes to unavailble
// if bms is HaBmsXsts is offline then wipe the rx values
//float HaBmsAsts = 0; // 0 startup, 1 = online, 2 offline, 3 = error
//float HaBmsBsts = 0;
 #ifdef BANK_A
      if (HaBmsBsts == 2){ // b is monitored by HA during a failure 2 will be sent to tell this working bms its gone offline
        SocB = 0;
        VoltsB = 0;
        AmpsB = 0;
        Screen2 = 0;
        bmsStateB = 5; // stationary/chg/discharge/unknwon
        BmsA = 1; //master should already be master
        BMSBsts = 5; //captures bms state
        CANBsts = 5;
       // batteryLogic = 1; // set to single banks
       // CanBussPrio 
         }
       
 #else
      if (HaBmsAsts == 2){
        SocA = 0;
        VoltsA = 0;
        AmpsA = 0;
        Screen1 = 0;
        bmsStateA = 5; // stationary/chg/discharge/unknwon
        //BmsB = 1; //master should already be master
        BMSAsts = 5; //captures bms state
        CANAsts = 5;
        //BmsB = 1; //master should already be master
        batteryLogic = 1; // set to single banks
         }
      
 #endif
}
*/
