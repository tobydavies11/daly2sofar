void mqttMessSelect (){


 if (MqttRequestCounter == 1) {
    // runs once at bootup
     sendMQTTData1 (); //         1:publishes normal data
     sendMqttCells (); //         2:individual cell voltages
     sendMQTTDataAlarm1 (); //    3:Alarms 1
     sendMQTTDataAlarm2 (); //    4:Alarms 2
    // sendMQTTDatarx();//          5:response to incoming cmd message
     sendMQTTDatacellbal();    // 6:cell balancing states
     sendmqttcanbusvalues();//    7: report on single and dual mqtt values
   //  interbmsstatus(); //         8:interbms status message sent special syntax
     //interbmsHA();   //         9: used to mirror the interbms messages 
     sendMQTTDatatime(); //       10:uptime monitor 
      }

 else if (MqttRequestCounter == 2) {
      //interbmsstatus(); //        8:publlish interbms data
      sendMQTTDataAlarm1 (); //    3:Alarms 1
      sendMQTTDataAlarm2 (); //    4:Alarms 2
      sendMQTTDatatime();
      }
 else if (MqttRequestCounter == 3) {
       //sendMQTTData1 (); //         1:publishes normal data
       sendmqttcanbusvalues();  //    7: report on single and dual mqtt values
       sendMQTTDatarx();      // send the stored switch values in case of a HA reset
      // interbmsstatus(); //         8:interbms status message sent special syntax 
    }      
 else if (MqttRequestCounter == 4) {
        
        sendMqttCells (); //         2:individual cell voltages
        sendMQTTDatacellbal();    // 6:cell balancing states       
      }
  
 else if (MqttRequestCounter >= 5)   {
     MqttRequestCounter = 1;  //resets the counter after 10   
     }
      
     MqttRequestCounter ++ ; //increments the counter
     sendMQTTData1 (); //         1:publishes normal data 
  //   interbmsstatus(); //         8:interbms status message sent special syntax
     eolterm();
}


void sendMQTTDatatime() {
  // Update all parameters and send to MQTT.
  // Serial.println("sendmqttdata#getfrom daly inside mqtt request");
  //  Serial.println("sendmqttdata#start");
  float mqtt = 10;
  String state = "{";

  if (!( state == "{")) { state += ","; }
   state += "\"SenMQttnNMBR\":" + String(mqtt);
  
  if (!( state == "{")) { state += ","; }  
   state += "\"days\":" + String(days);
   
  if (!( state == "{")) { state += ","; }  
   state += "\"hrs\":" + String(hours);
  
  if (!( state == "{")) { state += ","; }  
   state += "\"mins\":" + String(minutes);

  if (!( state == "{")) { state += ","; }  
   state += "\"secs\":" + String(seconds);

   if (!( state == "{")) { state += ","; }  
   state += "\"dip2\":" + String(DipSw2);
  
  if (!( state == "{")) { state += ","; }  
   state += "\"dip3\":" + String(DipSw3);

  if (!( state == "{")) { state += ","; }  
   state += "\"dip4\":" + String(DipSw4);  

  if (!( state == "{")) { state += ","; }  
   state += "\"dip5\":" + String(DipSw5);

   if (!( state == "{")) { state += ","; }  
   state += "\"BattMaster\":" + String(BattMasterSw);

   if (!( state == "{")) { state += ","; }  
   state += "\"BmsUpErr\":" + String(bmsupdateerror);
   
  state = state+"}";   
  

  //Prefixt the mqtt topic name with deviceName.
  String topic (deviceName); //should be banka or b by define
  topic += "/state";

  uint16_t packetIdPub2 = mqttClient.publish(const_cast<char*>(topic.c_str()), 1, true, const_cast<char*>(state.c_str()));
  Serial.println("Publishing at QoS 1");
  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Payload: ");
  Serial.println(state);

  //  xTimerStart(mqttPublishXTimer, 0);
      eolterm();
}
//==============================================================


void sendMQTTData1() {
  // Update all parameters and send to MQTT.
  // Serial.println("sendmqttdata#getfrom daly inside mqtt request");
  //  Serial.println("sendmqttdata#start");
  float mqtt = 1;
  String state = "{";

  if (!( state == "{")) { state += ","; }
  
  state += "\"SenMQqqd1\":" + String(mqtt);

  if (!( state == "{")) { state += ","; }
  
  if(bmsDalyFault == false){
  state += "\"soc\":" + String(percentage);
  }
  if (!( state == "{")) { state += ","; }

  if(bmsDalyFault == false){
  state += "\"voltage\":" + String(volts);
  }
  
  if (!( state == "{")) { state += ","; }
  
  if(bmsDalyFault == false){
  state += "\"current\":" + String(amps);
  }
  if (!( state == "{"))  {state += ","; }
  
  state += "\"power\":" + String(watts);

  if (!( state == "{")) { state += ","; }
  
  state += "\"temp\":" + String(temp);

  if (!( state == "{")) { state += ","; }
  
  state += "\"lowestcell\":" + String(minCellVoltage);

  if (!( state == "{")) { state += ","; }
  
  state += "\"highestcell\":" + String(maxCellVoltage);

  if (!( state == "{")) { state += ","; }
  
  state += "\"lowcellNr\":" + String(bms.get.minCellVNum);

  if (!( state == "{")) { state += ","; }
  
  state += "\"higcellNr\":" + String(bms.get.maxCellVNum);

  if (!( state == "{")) { state += ","; }
  
  state += "\"cellimbalance\":" + String(cellImbalance); // Cell imbalance is sent in Millivolts

  if (!( state == "{")) { state += ","; }
  
  state += "\"Cycles\":" + String(Cycles); // charge discharge cycles

  if (!( state == "{")) { state += ","; }
  
  state += "\"Capacity\":" + String(Capacity); // capacity of cells in milliamps

  if (!( state == "{")) { state += ","; }
  
  state += "\"Charge Mosfet\":" + String(bms.get.chargeFetState);

  if (!( state == "{")) { state += ","; }
  
  state += "\"Discharge Mosfet\":" + String(bms.get.disChargeFetState);

  if (!( state == "{")) { state += ","; }
  
  state += "\"BMS Status\":" + String(BMSOnline);

  if (!( state == "{")) { state += ","; }
  
  state += "\"CAN Status\":" + String(CANOnline);
  
  if (!( state == "{")) { state += ","; }
  
  state += "\"MqTT Counter\":" + String(MqttRequestCounter);
  
  if (!( state == "{")) { state += ","; }
  
  state += "\"Output Status\":" + String(bmsState);

  if (!( state == "{")) { state += ","; }
  
  state += "\"Charge Volts\":" + String(ChgVolts1); 

  if (!( state == "{")) { state += ","; }
  
  state += "\"ChgAmps\":" + String(ChgAmps1);
  
  if (!( state == "{")) { state += ","; }
  
  state += "\"DisAmps\":" + String(DisAmps1);

  if (!( state == "{")) { state += ","; }
  
  state += "\"BatCap\":" + String(batteryCapacity);

  //if (!( state == "{")) { state += ","; }
  
  //state += "\"batteryLogic\":" + String(batteryLogic);

 // if (!( state == "{")) { state += ","; }
  
 // state += "\"bifcl\":" + String(battinitFetCtllogic);
    //int battinitFetCtllogic = 0; // this captures where the logic is and reports back to HA
    
  state = state+"}";   
  

  //Prefixt the mqtt topic name with deviceName.
  String topic (deviceName); //should be banka or b by define
  topic += "/state";

  uint16_t packetIdPub2 = mqttClient.publish(const_cast<char*>(topic.c_str()), 1, true, const_cast<char*>(state.c_str()));
  Serial.println("Publishing at QoS 1");
  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Payload: ");
  Serial.println(state);

  //  xTimerStart(mqttPublishXTimer, 0);
      eolterm();
}
//==============================================================
  void sendMqttCells (){
// sends individual cell voltages
  float mqtt = 2;
  String state = "{";

  if (!( state == "{")) { state += ","; }
  state += "\"SenMQcells\":" + String(mqtt);

  if (!( state == "{")) { state += ","; }
  state += "\"RXBool\":" + String(bms.getCellVoltages()); // report on boolean state
  
  if (!( state == "{")) { state += ","; }
  //state += "\"C1\":"+String(cella1);
  state += "\"C1\":"+String(bms.get.cellVmV[0]);
  
  if (!( state == "{")) { state += ","; }
 // state += "\"C2\":"+String(cella2);
  state += "\"C2\":"+String(bms.get.cellVmV[1]);
  
  if (!( state == "{")) { state += ","; }
 // state += "\"C3\":"+String(cella3);
   state += "\"C3\":"+String(bms.get.cellVmV[2]);

  if (!( state == "{")) { state += ","; }
  //state += "\"C4\":"+String(cella4);
  state += "\"C4\":"+String(bms.get.cellVmV[3]);
  
  if (!( state == "{")) { state += ","; }
//  state += "\"C5\":"+String(cella5);
   state += "\"C5\":"+String(bms.get.cellVmV[4]);
   
  if (!( state == "{")) { state += ","; }
  //state += "\"C6\":"+String(cella6);
  state += "\"C6\":"+String(bms.get.cellVmV[5]);
  
  if (!( state == "{")) { state += ","; }
 // state += "\"C7\":"+String(cella7);
  state += "\"C7\":"+String(bms.get.cellVmV[6]);
  
  if (!( state == "{")) { state += ","; }
//  state += "\"C8\":"+String(cella8);  
  state += "\"C8\":"+String(bms.get.cellVmV[7]);

  if (!( state == "{")) { state += ","; }
 // state += "\"C9\":"+String(cella9); // 
  state += "\"C9\":"+String(bms.get.cellVmV[8]);

  if (!( state == "{")) { state += ","; }
//  state += "\"C10\":"+String(cella10); // 
  state += "\"C10\":"+String(bms.get.cellVmV[9]);

  if (!( state == "{")) { state += ","; }
//  state += "\"C11\":"+String(cella11);
   state += "\"C11\":"+String(bms.get.cellVmV[10]);
   
  if (!( state == "{")) { state += ","; }
//  state += "\"C12\":"+String(cella12);
  state += "\"C12\":"+String(bms.get.cellVmV[11]);

  if (!( state == "{")) { state += ","; }
//  state += "\"C13\":"+String(cella13);
  state += "\"C13\":"+String(bms.get.cellVmV[12]);
  
  if (!( state == "{")) { state += ","; }
 // state += "\"C14\":"+String(cella14);
  state += "\"C14\":"+String(bms.get.cellVmV[13]);

  if (!( state == "{")) { state += ","; }
 // state += "\"C15\":"+String(cella15);
  state += "\"C15\":"+String(bms.get.cellVmV[14]);

  if (!( state == "{")) { state += ","; }
//  state += "\"C16\":"+String(cella16);
  state += "\"C16\":"+String(bms.get.cellVmV[15]);
  
  if (!( state == "{")) { state += ","; }
  state += "\"Version\":"+String(versionDate);

  if (!( state == "{")) { state += ","; }
  state += "\"Version Number\":"+String(versionNumber);

  state = state+"}";

  //Prefixt the mqtt topic name with deviceName.
  String topic (deviceName);
  topic += "/state";
  uint16_t packetIdPub2 = mqttClient.publish(const_cast<char*>(topic.c_str()), 1, true, const_cast<char*>(state.c_str()));
  Serial.println("Publishing at QoS 1");
  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Payload: ");
  Serial.println(state);


  //  xTimerStart(mqttPublishXTimer, 0);
    eolterm();
  }
//===========================================================================
  void sendMQTTDataAlarm1 (){
// used for alarms

 // these report either 1 or 0
 // 0 = no alarm
 //1 = alarm
 // based on thresholds set in the bms 
 float mqtt = 3;    
  String state = "{";

  if (!( state == "{")) { state += ","; }
  state += "\"SenMQtt alarm 3\":" + String(mqtt);
  
  if (!( state == "{")) { state += ","; }
  state += "\"A1\":"+String(bms.alarm.levelOneCellVoltageTooHigh);
 
  if (!( state == "{")) { state += ","; }
  state += "\"A2\":"+String(bms.alarm.levelTwoCellVoltageTooHigh);

  if (!( state == "{")) { state += ","; }
  state += "\"A3\":"+String(bms.alarm.levelOneCellVoltageTooLow);

  if (!( state == "{")) { state += ","; }
  state += "\"A4\":"+String(bms.alarm.levelTwoCellVoltageTooLow);

  if (!( state == "{")) { state += ","; }
  state += "\"A5\":"+String(bms.alarm.levelOnePackVoltageTooHigh);

  if (!( state == "{")) { state += ","; }
  state += "\"A6\":"+String(bms.alarm.levelTwoPackVoltageTooHigh);

  if (!( state == "{")) { state += ","; }
  state += "\"A7\":"+String(bms.alarm.levelOnePackVoltageTooLow);

  if (!( state == "{")) { state += ","; }
  state += "\"A8\":"+String(bms.alarm.levelTwoPackVoltageTooLow);  
   
  if (!( state == "{")) { state += ","; }
  state += "\"A9\":"+String(bms.alarm.levelOneChargeTempTooHigh); 

  if (!( state == "{")) { state += ","; }
  state += "\"A10\":"+String(bms.alarm.levelTwoChargeTempTooHigh); 

  if (!( state == "{")) { state += ","; }
  state += "\"A11\":"+String(bms.alarm.levelOneChargeTempTooLow);

  if (!( state == "{")) { state += ","; }
  state += "\"A12\":"+String(bms.alarm.levelTwoChargeTempTooLow);

  if (!( state == "{")) { state += ","; }
  state += "\"A13\":"+String(bms.alarm.levelOneDischargeTempTooHigh);

  if (!( state == "{")) { state += ","; }
  state += "\"A14\":"+String(bms.alarm.levelTwoDischargeTempTooHigh);

  if (!( state == "{")) { state += ","; }
  state += "\"A15\":"+String(bms.alarm.levelOneDischargeTempTooLow);

  if (!( state == "{")) { state += ","; }
  state += "\"A16\":"+String(bms.alarm.levelTwoDischargeTempTooLow);


  state = state+"}";

  //Prefixt the mqtt topic name with deviceName.
  String topic (deviceName);
  topic += "/state";
  uint16_t packetIdPub2 = mqttClient.publish(const_cast<char*>(topic.c_str()), 1, true, const_cast<char*>(state.c_str()));
  Serial.println("Publishing at QoS 1");
  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Payload: ");
  Serial.println(state);


  //  xTimerStart(mqttPublishXTimer, 0);
    eolterm();
  }  
//================================================================
  void sendMQTTDataAlarm2 (){
// used for alarms

 // these report either 1 or 0
 // 0 = no alarm
 //1 = alarm
 // based on thresholds set in the bms 
  float mqtt = 4;    
  String state = "{";
  
  if (!( state == "{")) { state += ","; }
  state += "\"SenMQtt al2 4\":" + String(mqtt);
  
  if (!( state == "{")) { state += ","; }
  state += "\"A17\":"+String(bms.alarm.levelOneChargeCurrentTooHigh);

  if (!( state == "{")) { state += ","; }
  state += "\"A18\":"+String(bms.alarm.levelTwoChargeCurrentTooHigh);

  if (!( state == "{")) { state += ","; }
  state += "\"A19\":"+String(bms.alarm.levelOneDischargeCurrentTooHigh);

  if (!( state == "{")) { state += ","; }
  state += "\"A20\":"+String(bms.alarm.levelTwoDischargeCurrentTooHigh);

  if (!( state == "{")) { state += ","; }
  state += "\"A21\":"+String(bms.alarm.levelOneStateOfChargeTooHigh);

  if (!( state == "{")) { state += ","; }
  state += "\"A22\":"+String(bms.alarm.levelTwoStateOfChargeTooHigh);

  if (!( state == "{")) { state += ","; }
  state += "\"A23\":"+String(bms.alarm.levelOneStateOfChargeTooLow);
    
  if (!( state == "{")) { state += ","; }
  state += "\"A24\":"+String(bms.alarm.levelTwoStateOfChargeTooLow);

  if (!( state == "{")) { state += ","; }
  state += "\"A25\":"+String(bms.alarm.levelOneCellVoltageDifferenceTooHigh); 
  
  if (!( state == "{")) { state += ","; }
  state += "\"A26\":"+String(bms.alarm.levelTwoCellVoltageDifferenceTooHigh);

  if (!( state == "{")) { state += ","; }
  state += "\"A27\":"+String(bms.alarm.levelOneTempSensorDifferenceTooHigh);

  if (!( state == "{")) { state += ","; }
  state += "\"A28\":"+String(bms.alarm.levelTwoTempSensorDifferenceTooHigh);
    
  if (!( state == "{")) { state += ","; }
  state += "\"A29\":"+String(bms.alarm.chargeFETTemperatureTooHigh);

  if (!( state == "{")) { state += ","; }
  state += "\"A30\":"+String(bms.alarm.dischargeFETTemperatureTooHigh);

  if (!( state == "{")) { state += ","; }
  state += "\"A31\":"+String(bms.alarm.failureOfChargeFETTemperatureSensor);

  if (!( state == "{")) { state += ","; }
  state += "\"A32\":"+String(bms.alarm.failureOfDischargeFETTemperatureSensor);
  
  state = state+"}";

  //Prefixt the mqtt topic name with deviceName.
  String topic (deviceName);
  topic += "/state";
  uint16_t packetIdPub2 = mqttClient.publish(const_cast<char*>(topic.c_str()), 1, true, const_cast<char*>(state.c_str()));
  Serial.println("Publishing at QoS 1");
  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Payload: ");
  Serial.println(state);


  //  xTimerStart(mqttPublishXTimer, 0);
    eolterm();
  } 

//=====================================
//=====================================================================================
  void sendMQTTDatacellbal(){
//Serial.println("VOID:sendMQTTDatacellballancing");
//*********this is when cell balacing is actice
/*
const char* MosDis = doc["MosDis"]; // "off" or "on"
const char* MosChg = doc["MosChg"]; // "off" or "on"
const char* Screen = doc["Screen"]; // "off" or "on"
const char* Soc = doc["Soc"];       // "12" value to be recevied for experimentation
const char* ChgAmps = doc["ChgAmps"];
const char* DisAmps = doc["DisAmps"];
*/
  float  mqtt = 6;
  
  String state = "{";

  if (!( state == "{")) { state += ","; }
  state += "\"SenMQtt cell bal\":" + String(mqtt);
     
  if (!( state == "{")) { state += ","; }
  state += "\"CellBal\":"+String(bms.get.cellBalanceActive);

  if (!( state == "{")) { state += ","; }
  state += "\"Cb1\":"+String(bms.get.cellBalanceState[0]);

  if (!( state == "{")) { state += ","; }
  state += "\"Cb2\":"+String(bms.get.cellBalanceState[1]);  

  if (!( state == "{")) { state += ","; }
  state += "\"Cb3\":"+String(bms.get.cellBalanceState[2]);  
  
  if (!( state == "{")) { state += ","; }
  state += "\"Cb4\":"+String(bms.get.cellBalanceState[3]);
  
  if (!( state == "{")) { state += ","; }
  state += "\"Cb5\":"+String(bms.get.cellBalanceState[4]);

  if (!( state == "{")) { state += ","; }
  state += "\"Cb6\":"+String(bms.get.cellBalanceState[5]);  
  
  if (!( state == "{")) { state += ","; }
  state += "\"Cb7\":"+String(bms.get.cellBalanceState[6]);
  
  if (!( state == "{")) { state += ","; }
  state += "\"Cb8\":"+String(bms.get.cellBalanceState[7]);
  
  if (!( state == "{")) { state += ","; }
  state += "\"Cb9\":"+String(bms.get.cellBalanceState[8]);

  if (!( state == "{")) { state += ","; }
  state += "\"Cb10\":"+String(bms.get.cellBalanceState[9]);  
  
  if (!( state == "{")) { state += ","; }
  state += "\"Cb11\":"+String(bms.get.cellBalanceState[10]);
  
  if (!( state == "{")) { state += ","; }
  state += "\"Cb12\":"+String(bms.get.cellBalanceState[11]);

  if (!( state == "{")) { state += ","; }
  state += "\"Cb13\":"+String(bms.get.cellBalanceState[12]); 

  if (!( state == "{")) { state += ","; }
  state += "\"Cb14\":"+String(bms.get.cellBalanceState[13]);   
  
  if (!( state == "{")) { state += ","; }
  state += "\"Cb15\":"+String(bms.get.cellBalanceState[14]);

  if (!( state == "{")) { state += ","; }
  state += "\"Cb16\":"+String(bms.get.cellBalanceState[15]);

  state = state+"}";
    
  //Prefixt the mqtt topic name with deviceName.
  String topic (deviceName);
  topic += "/state";
  uint16_t packetIdPub2 = mqttClient.publish(const_cast<char*>(topic.c_str()), 1, true, const_cast<char*>(state.c_str()));
  Serial.println("Publishing at QoS 1");
  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Payload: ");
  Serial.println(state);


  //  xTimerStart(mqttPublishXTimer, 0);
    eolterm();
  } 
  //=======================================
  void sendMQTTDatarx(){
  Serial.println("VOID: Send mqtt rx, sent when a mqtt message recevied on CMD");
// variables that can be received and adjusted in the esp32

// when an mqtt messges is rec this is pushed out
//    Bank A
/*
      Serial.println("Debug for mqqt rx ");
      Serial.println("MosDisA     :" + String(MosDisA));
      Serial.println("MosChgA     :" + String(MosChgA));
      Serial.println("SocA        :" + String(SocA));      
      Serial.println("VoltsA      :" + String(VoltsA));   
      Serial.println("AmpsA       :" + String(AmpsA));
      Serial.println("Screen1     :" + String(Screen1));
      Serial.println("BMSAsts     :" + String(BMSAsts));   // bms status 
      Serial.println("CANAsts     :" + String(CANAsts));   // CAN status
      
 //   Bank B     
      Serial.println("MosDisB     :" + String(MosDisB));
      Serial.println("MosChgB     :" + String(MosChgB));
      Serial.println("SocB        :" + String(SocB));      
      Serial.println("VoltsB      :" + String(VoltsB));   
      Serial.println("AmpsB       :" + String(AmpsB));
      Serial.println("Screen2     :" + String(Screen2));
      Serial.println("BMSBsts     :" + String(BMSBsts));   // bms status 
      Serial.println("CANBsts     :" + String(CANBsts));   // CAN status
      
//    Shared values 
      Serial.println("HaBmsAsts   :" + String(HaBmsAsts)); // home assistant monitoring of bms
      Serial.println("HaBmsBsts   :" + String(HaBmsBsts));
      Serial.println("ChgAmps1    :" + String(ChgAmps1));   // control charge current
      Serial.println("DisAmps1    :" + String(DisAmps1));
      Serial.println("ChgVolts1   :" + String(ChgVolts1));
      Serial.println("=======eng ofDebug for mqqt rx=========");
//Serial.println("voidmqtt 4 ");

//#ifdef BANK_A
//#else
       
//#endif  
  
/*      Serial.println("MosDisA     :" + String(MosDisA));
      Serial.println("MosChgA     :" + String(MosChgA));
      Serial.println("SocA        :" + String(SocA));      
      Serial.println("VoltsA      :" + String(VoltsA));   
      Serial.println("AmpsA       :" + String(AmpsA));
      Serial.println("Screen1     :" + String(Screen1));
      Serial.println("BMSAsts     :" + String(BMSAsts));   // bms status 
      Serial.println("CANAsts     :" + String(CANAsts));   // CAN status
*/      
  float mqtt = 5;
     String state = "{";
     
     if (!( state == "{")) { state += ","; }
     state += "\"SenMQtt rx cmd\":" + String(mqtt);
          if (!( state == "{")) { state += ","; }
     state += "\"MosDisA\":"+String(MosDisA);
  
     if (!( state == "{")) { state += ","; }
     state += "\"MosChgA\":"+String(MosChgA);

     if (!( state == "{")) { state += ","; }
     state += "\"MosDisB\":"+String(MosDisB);
  
     if (!( state == "{")) { state += ","; }
     state += "\"MosChgB\":"+String(MosChgB);

#ifdef BANK_A    
     
     if (!( state == "{")) { state += ","; }
     state += "\"Screen1\":"+String(Screen1);
 
#else

     if (!( state == "{")) { state += ","; }
     state += "\"Screen2\":"+String(Screen2);
  
#endif  

//     if (!( state == "{")) { state += ","; }
//     state += "\"HaBmsAsts\":"+String(HaBmsAsts); // this whats held locally about each bms
  
//     if (!( state == "{")) { state += ","; }
//     state += "\"HaBmsBsts\":"+String(HaBmsBsts);
     
     if (!( state == "{")) { state += ","; }
     state += "\"ChgAmps\":"+String(ChgAmps1);
  
     if (!( state == "{")) { state += ","; }
     state += "\"DisAmps\":"+String(DisAmps1);

     if (!( state == "{")) { state += ","; }
     state += "\"ChgVolts\":"+String(ChgVolts1);
      
      if (!( state == "{")) { state += ","; }
     state += "\"CanControl\":"+String(CanControl);

      if (!( state == "{")) { state += ","; }
     state += "\"CanbusInhibit\":"+String(CanbusInhibit);

     // if (!( state == "{")) { state += ","; }
     //state += "\"CanBussPrio\":"+String(CanBussPrio);

     // if (!( state == "{")) { state += ","; }
     //state += "\"battInitFetCtl\":"+String(batteryInitialFetControl);

      
     state = state+"}";

  
  //Prefixt the mqtt topic name with deviceName.
  
  String topic (deviceName);

  topic += "/state";
  uint16_t packetIdPub2 = mqttClient.publish(const_cast<char*>(topic.c_str()), 1, true, const_cast<char*>(state.c_str()));
  Serial.println("Publishing at QoS 1");
  Serial.print("Topic: ");
  Serial.println(topic);
  Serial.print("Payload: ");
  Serial.println(state);


  //  xTimerStart(mqttPublishXTimer, 0);
    eolterm();
  }
