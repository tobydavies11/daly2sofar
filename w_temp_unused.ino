//=====================bms priority=========================
void bmsPriority(){
  // not used
// this is to decide which bms is to transmit on the canbus
// BMSoppOnline = false; // for monitring opposite bms
// CAANoppOnline = false; // 
// boolean BmsPrio = false ; 
//int BmsA = 0;  // identify which bms is in charge  
//int BmsB = 0;  // other bms
// 0 = invalid/bootup
// 1 = master
// 2 = slave
// 3 = error
//    MosDisA = doc["MosDisA"]; // 1 = ON , 0 = OFF
//    MosChgA = doc["MosChgA"]; // 1 = ON , 0 = OFF
//    SocA= doc["SocA"];        // 
//    VoltsA= doc["VoltsA"];    //  
//    AmpsA= doc["VoltsA"];     //
//    Screen1 = doc["Screen1"];  // 1 = ON , 0 = OFF
//    BMSAsts = doc["BMSAsts"];
//    CANAsts = doc["CANAsts"];   // CAN status 


 //   Bank B     
//    MosDisB = doc["MosDisB"]; // 
//    MosChgB = doc["MosChgB"]; // 
//    SocB= doc["SocB"];        // 
//    VoltsB= doc["VoltsB"];    //  
//    AmpsB= doc["VoltsB"];     //
//    Screen2 = doc["Screen2"];  // 
//    BMSBsts = doc["BMSBsts"];  // 1 = ON , 2 = OFF , 5= unknown
//    CANBsts = doc["CANBsts"];   // CAN status 


//   float SocComb = 0 ;
//   float VoltsComb = 0;
//   float AmpsComb = 0  ; 
//    BMSOnline
//    CANOnline
//   boolean BMSoppOnline = false; // for monitring opposite bms how to trigger this?
//   boolean CANoppOnline = false;

// NORMAL REQUIREMNTS
//bank a only
//bank a and bank b slave

//bank b only
//bank b and bank a master

//failure modes- how to monitor other bms failing

// bank a master and bank b goes offline
// bank b master and bank a goes offline


      eolterm();
}
