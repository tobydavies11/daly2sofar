/*

void interbmsstatus(){
// just sends the data to the other bms

// *****messages need extra syntax**********

Serial.print("VOID:interbmsstatus");
 float mqtt = 8;
#ifdef BANK_A
      // send bank a messages which will be received  by bms B
      String state = "\"{"; //new
      
      if (!( state == "\"{")) { state += "\\\","; }
  
      state += "\\\"SocA\\\":\\\"" + String(SocA);

  
      if (!( state == "\"{")) { state += "\\\","; }
  
      state += "\\\"VoltsA\\\":\\\"" + String(VoltsA);

      if (!( state == "\"{")) { state += "\\\","; }
  
      state += "\\\"AmpsA\\\":\\\"" + String(AmpsA + 100); //added bias to cope with negative numbers

      if (!( state == "\"{")) { state += "\\\","; }
  
      state += "\\\"BMSAsts\\\":\\\"" + String(BMSAsts);

      if (!( state == "\"{")) { state += "\\\","; }
  
      state += "\\\"CANAsts\\\":\\\"" + String(CANAsts);

      if (!( state == "\"{")) { state += "\\\","; }
  
      state += "\\\"BmsA\\\":\\\"" + String(BmsA);

      if (!( state == "\"{")) { state += "\\\","; }
  
      state += "\\\"MosChgA\\\":\\\"" + String(MosChgA);
      
      if (!( state == "\"{")) { state += "\\\","; }
  
      state += "\\\"MosDisA\\\":\\\"" + String(MosDisA);

      if (!( state == "\"{")) { state += "\\\","; }
  
      state += "\\\"bmsStateA\\\":\\\"" + String(bmsStateA);

      if (!( state == "\"{")) { state += "\\\","; }
  
      state += "\\\"HaBmsAsts\\\":\\\"" + String(HaBmsAsts);
           
      state = state+"\\\"}\""; // this works at end
      
      //Daly2Sofar/Bank/B/cmd
      String topic (deviceNameCmd); //should be banka or b by define
      topic += "/cmd";
     // const char* deviceName = "Daly2Sofar/Bank/A"; //Device name is used as the MQTT base topic
       uint16_t packetIdPub2 = mqttClient.publish(const_cast<char*>(topic.c_str()), 1, true, const_cast<char*>(state.c_str()));
     Serial.println("Publishing at QoS 1");
     Serial.print("Topic: ");
     Serial.println(topic);
     Serial.print("Payload: ");
     Serial.println(state);

   // send againbut for HA  

#else
       // send bank B messages to bms A
     
      String state = "\"{"; //new
      
      if (!( state == "\"{")) { state += "\\\","; }
  
      state += "\\\"SocB\\\":\\\"" + String(SocB);
  
      if (!( state == "\"{")) { state += "\\\","; }
  
      state += "\\\"VoltsB\\\":\\\"" + String(VoltsB);

      if (!( state == "\"{")) { state += "\\\","; }
  
      state += "\\\"AmpsB\\\":\\\"" + String(AmpsB + 100); //bias added to cope with negative numbers

      if (!( state == "\"{")) { state += "\\\","; }
  
      state += "\\\"BMSBsts\\\":\\\"" + String(BMSBsts);

      if (!( state == "\"{")) { state += "\\\","; }
  
      state += "\\\"CANBsts\\\":\\\"" + String(CANBsts);

      if (!( state == "\"{")) { state += "\\\","; }
  
      state += "\\\"BMSB\\\":\\\"" + String(BmsB);

      if (!( state == "\"{")) { state += "\\\","; }
  
      state += "\\\"MosChgB\\\":\\\"" + String(MosChgB);
      
      if (!( state == "\"{")) { state += "\\\","; }
  
      state += "\\\"MosDisB\\\":\\\"" + String(MosDisB); 

      if (!( state == "\"{")) { state += "\\\","; }
  
      state += "\\\"bmsStateB\\\":\\\"" + String(bmsStateB); // SENDS THE STATE EG STATIONARY,CHG,DISHCHARGE

      if (!( state == "\"{")) { state += "\\\","; }
  
      state += "\\\"HaBmsBsts\\\":\\\"" + String(HaBmsBsts);
           
      state = state+"\\\"}\""; // this works at end
      
      String topic (deviceNameCmd); //should be banka or b by define
    
      topic += "/cmd"; 
       //const char* deviceName = "Daly2Sofar/Bank/A"; //Device name is used as the MQTT base topic
         uint16_t packetIdPub2 = mqttClient.publish(const_cast<char*>(topic.c_str()), 1, true, const_cast<char*>(state.c_str()));
    Serial.println("Publishing at QoS 1");
    Serial.println(topic);
    Serial.print("Payload: ");
    Serial.println(state); 
// same as above but HA
      
      
#endif
       


   interbmsHA(); // send to HA
   
   eolterm(); 
}
//====================================================
void interbmsHA(){

    Serial.println("void interbmsHA");

    float mqtt = 9;
    
    String state = "{";

    if (!( state == "{")) { state += ","; }
   
    state += "\"InterBMSHA\":" + String(mqtt);

//#ifdef BANK_A
      if (!( state == "{")) { state += ","; }
  
      state += "\"SocA\":" + String(SocA);
  
      if (!( state == "{")) { state += ","; }
  
      state += "\"VoltsA\":" + String(VoltsA);

      if (!( state == "{")) { state += ","; }
  
      state += "\"AmpsA\":" + String(AmpsA);

      if (!( state == "{")) { state += ","; }
  
      state += "\"BMSAsts\":" + String(BMSAsts);

      if (!( state == "{")) { state += ","; }
  
      state += "\"CANAsts\":" + String(CANAsts);

      if (!( state == "{")) { state += ","; }
  
      state += "\"BmsA\":" + String(BmsA);   
//#else
              
      if (!( state == "{")) { state += ","; }
  
      state += "\"SocB\":" + String(SocB);
  
      if (!( state == "{")) { state += ","; }
  
      state += "\"VoltsB\":" + String(VoltsB);

      if (!( state == "{")) { state += ","; }
  
      state += "\"AmpsB\":" + String(AmpsB);

      if (!( state == "{")) { state += ","; }
  
      state += "\"BMSBsts\":" + String(BMSBsts);

      if (!( state == "{")) { state += ","; }
  
      state += "\"CANBsts\":" + String(CANBsts);

      if (!( state == "{")) { state += ","; }
  
      state += "\"BmsB\":" + String(BmsB);     

//#endif    

       if (!( state == "{")) { state += ","; }
  
      state += "\"CanBussPrio\":" + String(CanBussPrio);
      
      state = state+"}"; 
      //Daly2Sofar/Bank/A/cmd
      String topic (deviceName); //should be banka or b by define
    //  topic += "/cmd";
      topic += "/state";
     // const char* deviceName = "Daly2Sofar/Bank/A"; //Device name is used as the MQTT base topic
       uint16_t packetIdPub2 = mqttClient.publish(const_cast<char*>(topic.c_str()), 1, true, const_cast<char*>(state.c_str()));
     Serial.println("Publishing at QoS 1");
     Serial.print("Topic: ");
     Serial.println(topic);
     Serial.print("Payload: ");
     Serial.println(state);
     eolterm(); 
}

*/
