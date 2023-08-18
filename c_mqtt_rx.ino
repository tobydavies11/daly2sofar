//============================================================== 
// this set the listen topic  
void onMqttConnect(bool sessionPresent) {
  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);
//  uint16_t packetIdSub = mqttClient.subscribe("Daly2Sofarbankb/rx", 2);
//  uint16_t packetIdSub2 = mqttClient.subscribe("Daly2Sofar/mqtt/rx", 2);
#ifdef BANK_A
      uint16_t packetIdSub = mqttClient.subscribe("Daly2Sofar/Bank/A/cmd", 2);
       //int16_t packetIdSub = mqttClient.subscribe(String topic (deviceNameCmd)), 2); // dosent compile

#elif defined(BANK_B)
      uint16_t packetIdSub = mqttClient.subscribe("Daly2Sofar/Bank/B/cmd", 2);
      //uint16_t packetIdSub = mqttClient.subscribe(deviceNameCmd), 2); //cant get to work yet

#elif defined(BANK_C)
       uint16_t packetIdSub = mqttClient.subscribe("Daly2Sofar/Bank/C/cmd", 2);
      //uint16_t packetIdSub = mqttClient.subscribe(deviceNameCmd), 2); //cant get to work yet
#endif
 
  MQTTStatus = true;


  //xTimerStart(mqttPublishXTimer, 0);
  //mqttPublisherTimer->Start();
}
//===================================================================
// this parses the incoming message
void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
Serial.print("VOID:on mqtt message rx");
digitalWrite(ledPins[5], LOW); //turn on 6th LED
//this in use
//***WORKING!***

int length = 200 ; // dosent really matter just to create something
length = total ;


char str[length+1];
Serial.print("Message arrived [");
Serial.print(topic);
Serial.print("] ");
int i=0;
for (i=0;i<length;i++) {
Serial.print((char)payload[i]); // this prints the incoming mqtt message on serial for verification
str[i]=(char)payload[i];

}
str[i] = 0; // Null termination
Serial.println();
//practise string
//char json[] = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";

// * this is my example json message
// * buffer is 128 according to calcs
/*
topic is "daly2sofar/rx"
 paste this in to mqtt tx to receive in unit

variables
--------
Mosdis :on/off
Moschg :on/off
Screen : on/off
Soc    :0/100 //%
volts  : 55.3
amps   : amps // how to deal with use 1000 add or subtract
ChgAmps : 650 //max ?
DisAmps ; 650 // max ?
ChgVolts; 558 //***send it as 3 digit number to avoid issues with decimal eg 558=55.8v
--------
 example ** note include all data and quotation marks
"{
  \"MosDisA\":\"1\",
  \"MosChgA\":\"1\",
  \"Screen\":\"1\",
  \"Soc\": \"100\",
  \"Volts\": \"552\",
  \"Amps\": \"1000\",
  \"ChgAmps\": \"650\",
  \"DisAmps\": \"650\",
  \"ChgVolts\": \"558\"
  }"
==================================
inter bms
note AMPs0=1000
Amps 350 = -65 amps
"{
  \"SocB\": \"100\",
  \"VoltsB\": \"552\",
  \"AmpsB\": \"1000\",
  }"

  "{
  \"SocA\": \"100\",
  \"VoltsA\": \"552\",
  \"AmpsA\": \"1000\",
  }"

*/
//StaticJsonDocument <128> doc;
StaticJsonDocument <256> doc; //ori
//Serial.println(payload);
deserializeJson(doc,payload);

DeserializationError error = deserializeJson(doc, payload);

  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
  }
// deserializeJson(doc,str); can use string instead of payload
//const char* sensor = doc["sensor"];
//long time = doc["time"];
//float latitude = doc["data"][0];
//float longitude = doc["data"][1];

//Serial.println("latitude =");
//Serial.println(latitude,2);
//Serial.println(sensor);

// use float as much easier to work with, for text use char
/*
const char* MosDis = doc["MosDis"]; // "off" or "on"
const char* MosChg = doc["MosChg"]; // "off" or "on"
const char* Screen = doc["Screen"]; // "off" or "on"
const char* Soc = doc["Soc"];       // "12" value to be recevied for experimentation
const char* ChgAmps = doc["ChgAmps"]; //650
const char* DisAmps = doc["DisAmps"]; /650
*/

//float MosDis = doc["MosDis"]; // 1 = ON , 0 = OFF
// floats removed and moved to define section
//serial debug
//    Bank A

      

      
//    Shared values 


//    Bank A
/*
      Serial.println("Debug for mqqt rx before processing");
      Serial.println("MosDisA     :" + String(MosDisA));
      Serial.println("MosChgA     :" + String(MosChgA));
      Serial.println("SocA        :" + String(SocA));      
      Serial.println("VoltsA      :" + String(VoltsA));   
      Serial.println("AmpsA       :" + String(AmpsA));
      Serial.println("Screen1     :" + String(Screen1));
      Serial.println("BMSAsts     :" + String(BMSAsts));   // bms status 
      Serial.println("CANAsts     :" + String(CANAsts));   // CAN status

      Serial.println("MosDisB     :" + String(MosDisB));
      Serial.println("MosChgB     :" + String(MosChgB));
      Serial.println("SocB        :" + String(SocB));      
      Serial.println("VoltsB      :" + String(VoltsB));   
      Serial.println("AmpsB       :" + String(AmpsB));
      Serial.println("Screen2     :" + String(Screen2));
      Serial.println("BMSBsts     :" + String(BMSBsts));   // bms status 
      Serial.println("CANBsts     :" + String(CANBsts));   // CAN status
      Serial.println("percentage  :" + String(percentage));
      Serial.println("percdual    :" + String(percentageDual));
      
      Serial.println("HaBmsAsts   :" + String(HaBmsAsts)); // home assistant monitoring of bms
      Serial.println("HaBmsBsts   :" + String(HaBmsBsts));
      Serial.println("ChgAmps1    :" + String(ChgAmps1));   // control charge current
      Serial.println("DisAmps1    :" + String(DisAmps1));
      Serial.println("ChgVolts1   :" + String(ChgVolts1));
      Serial.println("CanControl  :" + String(CanControl));
      Serial.println("CanInhibit  :" + String(CanbusInhibit));
      Serial.println("CanBussPrio :" + String(CanBussPrio));
*/      
      int tempMess = 0; //temp to hold the data for comparison
// seperated
// to stop the bms transmittting data back to itself and being rx for its own bank      
#ifdef BANK_A      
      tempMess = doc["MosDisA"];
      if (tempMess > 0){
        MosDisA = doc["MosDisA"]; // 0= empty, 1= on 2= off
        MosDisCtl = MosDisA ; 
        if (MosDisA == 1){
          DisFetOn();
        }
        if (MosDisA == 2){
          DisFetOff();
        }      
      }
      tempMess = doc["MosChgA"];
      if (tempMess > 0){
        MosChgA = doc["MosChgA"]; // 0= empty, 1= on 2= off
        MosChgCtl = MosChgA ;
        if(MosChgA == 1){
          ChgFetOn();
        }
        if (MosChgA == 2){
          ChgFetOff();
        }
      }
      
      tempMess = doc["Screen1"];
      if (tempMess > 0){
        Screen1 = doc["Screen1"]; // 1 = ON , 2 = OFF 
        if (Screen1 == 1){
        display.sh1106_command(SH1106_DISPLAYON); // To switch display back on
        }
        if (Screen1 == 2){
        display.sh1106_command(SH1106_DISPLAYOFF);
        }
      }


///----------------------------------       
 // each pass returns the float to zero which is undeseriabe     
 //     MosDisA = doc["MosDisA"]; // 1 = ON , 0 = OFF
 //     MosChgA = doc["MosChgA"]; // 1 = ON , 0 = OFF
 //     SocA= doc["SocA"];        // 
 //     VoltsA= doc["VoltsA"];    //  
 //     AmpsA= doc["AmpsA"];     //
 //     Screen1 = doc["Screen1"];  // 1 = ON , 0 = OFF
 //     BMSAsts = doc["BMSAsts"];   // bms status 
 //     CANAsts = doc["CANAsts"];   // CAN status

 //     HaBmsAsts = doc["HaBmsAsts"]; // home assistant monitoring of bms
 //     HaBmsAsts = doc["HaBmsBsts"];
 //     ChgAmps1 = doc["ChgAmps"];   // control charge current
 //     DisAmps1 = doc["DisAmps"];
 //     ChgVolts1 = doc["ChgVolts"];
#else
      tempMess = doc["MosDisB"];
      if (tempMess > 0){
        MosDisB = doc["MosDisB"]; // 1 = ON , 2 = OFF , 5= unknown
        MosDisCtl = MosDisB ;
        if (MosDisB == 1){
          DisFetOn();
        }
        if (MosDisB == 2){
          DisFetOff();
        }  
      }      
      tempMess = doc["MosChgB"];
      if (tempMess > 0){      
        MosChgB = doc["MosChgB"]; // 1 = ON , 2 = OFF , 5= unknown
        MosChgCtl = MosChgB ;
        if(MosChgB == 1){
          ChgFetOn();
        }
        if (MosChgB == 2){
          ChgFetOff();
            }
      }

      tempMess = doc["Screen2"]; 
      if (tempMess > 0){
        Screen2 = doc["Screen2"]; // 1 = ON , 2 = OFF , 5= unknown
        if (Screen2 == 1){
        display.sh1106_command(SH1106_DISPLAYON); // To switch display back on
        }
        if (Screen2 == 2){
        display.sh1106_command(SH1106_DISPLAYOFF);
        }
      }
      
      
//---- extra to allow remote switch of canbus
 #endif
//------------------Common Values----------------------------------       
      
      tempMess = doc["ChgAmps"];
      if (tempMess > 0){
        
        // zero amps = 1000
        // +65 amps = 1650
        int tempMessfilter1 = doc["ChgAmps"];
         
        ChgAmps1 = tempMessfilter1 - 1000; 
      
      //  ChgAmps1 = doc["ChgAmps"];
      }
      
      tempMess = doc["DisAmps"];  
      if (tempMess > 0){
        int tempMessfilter2 = doc["DisAmps"];
         
        DisAmps1 = tempMessfilter2 - 1000; 
        //DisAmps1 = doc["DisAmps"]; // 1 = ON , 0 = OFF
      }
      tempMess = doc["ChgVolts"];
      if (tempMess > 0){
        ChgVolts1 = doc["ChgVolts"]; // 1 = ON , 0 = OFF
      }
      tempMess  = doc["CanbusInhibit"];
      if (tempMess > 0){
        CanbusInhibit = doc["CanbusInhibit"]; //1 = ON , 2 = OFF , 5= unknown
       // used on final stop of canbus //CanControl
         canbuscontrollogic();
      }      
      tempMess  = doc["CanControl"];
      if (tempMess > 0){
        CanControl = doc["CanControl"]; //1 = ON , 2 = OFF , 5= unknown
       // used on final stop of canbus //CanControl
       canbuscontrollogic();
      }
     // tempMess  = doc["CanBussPrio"]; // 1 = bank a , 2 = bank b , 3 = dual 5= unknown
     // if (tempMess > 0){
     //   CanBussPrio = doc["CanBussPrio"];
     //   canbuscontrollogic();
     // } 
     // tempMess  = doc["battInitFetCtl"]; // 1: run 2: stop 5: bootup
      //if (tempMess > 0){
      //  batteryInitialFetControl = doc["battInitFetCtl"];
      //}
      tempMess  = doc["espreset"]; // 1: run 2: stop 5: bootup
      if (tempMess > 0){
        int  espreset = 0;
        espreset = doc["espreset"];
        if (espreset == 99){
          ESP.restart();
        }
        if (espreset ==89){
          //gpio15 pull low
          // screen display needed
          digitalWrite(15, LOW); //this will force a hard restart
        }
      }
       
 //   Bank B     
//      MosDisB = doc["MosDisB"]; // 1 = ON , 0 = OFF
//      MosChgB = doc["MosChgB"]; // 1 = ON , 0 = OFF
//      SocB= doc["SocB"];        // 
//      VoltsB= doc["VoltsB"];    //  
//      AmpsB= doc["AmpsB"];     //
//      Screen2 = doc["Screen2"];  // 1 = ON , 0 = OFF
//      BMSBsts = doc["BMSAsts"];   // 1 = ON , 2 = OFF , 5= unknown 
//      CANAsts = doc["CANAsts"];   // CAN status

//    Shared values 
//      HaBmsAsts = doc["HaBmsAsts"]; // home assistant monitoring of bms
//      HaBmsAsts = doc["HaBmsBsts"];
//      ChgAmps1 = doc["ChgAmps"];   // control charge current
//      DisAmps1 = doc["DisAmps"];
//      ChgVolts1 = doc["ChgVolts"]; 


      
      sendMQTTDatarx (); // send outs changes to variables requested on incoming mqtt message
      
 // oledMessage("" ,"" ,"" ,"" , "", ""); // this is to force the icons update on the display

/*
 //    Bank A
      Serial.println("Debug for mqqt rx ** post processing ");
      Serial.println("MosDisA     :" + String(MosDisA));
      Serial.println("MosChgA     :" + String(MosChgA));
      Serial.println("SocA        :" + String(SocA));      
      Serial.println("VoltsA      :" + String(VoltsA));   
      Serial.println("AmpsA       :" + String(AmpsA));
      Serial.println("Screen1     :" + String(Screen1));
      Serial.println("BMSAsts     :" + String(BMSAsts));   // bms status 
      Serial.println("CANAsts     :" + String(CANAsts));   // CAN status //CanBussPrio
      
 //   Bank B     
      Serial.println("MosDisB     :" + String(MosDisB));
      Serial.println("MosChgB     :" + String(MosChgB));
      Serial.println("SocB        :" + String(SocB));      
      Serial.println("VoltsB      :" + String(VoltsB));   
      Serial.println("AmpsB       :" + String(AmpsB));
      Serial.println("Screen2     :" + String(Screen2));
      Serial.println("BMSBsts     :" + String(BMSBsts));   // 1 = ON , 2 = OFF , 5= unknown
      Serial.println("CANBsts     :" + String(CANBsts));   // CAN status

      Serial.println("percentage  :" + String(percentage));
      Serial.println("percdual    :" + String(percentageDual));
//    Shared values 
      Serial.println("HaBmsAsts   :" + String(HaBmsAsts)); // home assistant monitoring of bms
      Serial.println("HaBmsBsts   :" + String(HaBmsBsts));
      Serial.println("ChgAmps1    :" + String(ChgAmps1));   // control charge current
      Serial.println("DisAmps1    :" + String(DisAmps1));
      Serial.println("ChgVolts1   :" + String(ChgVolts1));
      Serial.println("CanControl  :" + String(CanControl)); //final can control
      Serial.println("CanInhibit  :" + String(CanbusInhibit)); //remote stop
      Serial.println("CanBussPrio :" + String(CanBussPrio));
      Serial.println("=======eng ofDebug for mqqt rx=========");
 */     
      digitalWrite(ledPins[5], HIGH); //turn on 6th LED
      
      eolterm();

}
//=============================
