//===============send can bus===============================
void sendCanTXFrame(byte dataArray[], int dataLength) {
 // Serial.println("sendcantxframe and will repeat"); // prints out 8 times for each can register
  tx_frame.FIR.B.FF = CAN_frame_std;
  uint16_t combinedId = 0;                  // Storage var
  uint16_t x1 = (uint16_t)dataArray[0] << 8; // MSB
  uint16_t x0 = (uint16_t)dataArray[1];     // LSB
  combinedId = x1 | x0;
  tx_frame.MsgID = combinedId; // concatenated address from the 1st 2 values in the data array.
  tx_frame.FIR.B.DLC = dataLength;
  tx_frame.data.u8[0] = dataArray[2];
  tx_frame.data.u8[1] = dataArray[3];
  tx_frame.data.u8[2] = dataArray[4];
  tx_frame.data.u8[3] = dataArray[5];
  tx_frame.data.u8[4] = dataArray[6];
  tx_frame.data.u8[5] = dataArray[7];
  tx_frame.data.u8[6] = dataArray[8];
  tx_frame.data.u8[7] = dataArray[9];
  ESP32Can.CANWriteFrame(&tx_frame);
  CANOnline = true;
  delay(5); // just to make sure it gets sent in peace. TRY INCREASING THIS
      eolterm();
}
//=======================================================
void updateTXFrames() {

  // this section adjusts the can frames with the uodates from the bms ready for transmission back to the inverter
 // Serial.println("Void:updatetxframes about to run");
  // SOC. 1:1 scaling, 2 bytes - easy.
  //if (batteryLogic == 1){
    Serial.println("single Battery values now in about to be sent");
    CANData355[2] = (int)percentage;

      // High res SOC: 0.01 scale, 2 bytes. rescale, then bitshift.
  uint16_t uint16soc = percentage * 100;
  CANData355[6] = uint16soc;        // Arduino/C is big endian. So needs to shift the opposite way of the SMA protocol.
  CANData355[7] = uint16soc >> 8;

  // voltage read by bms and tx to inverter
  // Voltage. 0.01 scale, over 2 bytes SIGNED. Not easy. First rescale, then bitshift
  int16_t int16volts = volts * 100;
  CANData356[2] = int16volts;       // Arduino/C is big endian. So needs to shift the opposite way of the SMA protocol.
  CANData356[3] = int16volts >> 8;

  //bms amps tx to inverter
  // Current. 0.1 scale, over 2 bytes SIGNED. Not easy. First rescale, then bitshift
  int16_t int16amps = amps * 10;
  CANData356[4] = int16amps;        // Arduino/C is big endian. So needs to shift the opposite way of the SMA protocol.
  CANData356[5] = int16amps >> 8;
   digitalWrite(ledPins[2], HIGH); //turn on 6th LE
//  }

/*  
  if (batteryLogic == 2){
    Serial.println("Dual Battery values now in use");
        
    digitalWrite(ledPins[2], LOW); //turn on 6th LED   
      
    CANData355[2] = (int)percentageDual;
      
      // High res SOC: 0.01 scale, 2 bytes. rescale, then bitshift.
  uint16_t uint16soc = percentageDual * 100;
  CANData355[6] = uint16soc;        // Arduino/C is big endian. So needs to shift the opposite way of the SMA protocol.
  CANData355[7] = uint16soc >> 8;

  // voltage read by bms and tx to inverter
  // Voltage. 0.01 scale, over 2 bytes SIGNED. Not easy. First rescale, then bitshift
  int16_t int16volts = voltsDual * 100;
  CANData356[2] = int16volts;       // Arduino/C is big endian. So needs to shift the opposite way of the SMA protocol.
  CANData356[3] = int16volts >> 8;

  //bms amps tx to inverter
  // Current. 0.1 scale, over 2 bytes SIGNED. Not easy. First rescale, then bitshift
  int16_t int16amps = ampsDual * 10;
  CANData356[4] = int16amps;        // Arduino/C is big endian. So needs to shift the opposite way of the SMA protocol.
  CANData356[5] = int16amps >> 8;
  }
*/


//  uint16_t uint16chargevolts = batteryChargeVoltage * 10; //original
  uint16_t uint16chargevolts = ChgVolts1; // this is controllable from mqtt mx already sent as 3 digit
  CANData351[2] = uint16chargevolts;        // Arduino/C is big endian. So needs to shift the opposite way of the SMA protocol.
  CANData351[3] = uint16chargevolts >> 8;

  //bms batt temps tx to inverter
  // Battery temp. 0.1 scale, 2 bytes SIGNED. Not easy. First rescale, then bitshift
  int16_t int16temp = temp * 10;
  CANData356[6] = int16temp;        // Arduino/C is big endian. So needs to shift the opposite way of the SMA protocol.
  CANData356[7] = int16temp >> 8;

  //supplemtary for control of change and discharge amps 
  //charge power
  uint16_t uint16chargeamps = ChgAmps1  ;// arrive as 3 digit number
  CANData351[4] = uint16chargeamps;    // Arduino/C is big endian. So needs to shift the opposite way of the SMA protocol.
  CANData351[5] = uint16chargeamps >> 8; // 
  
  uint16_t uint16dischargeamps = DisAmps1 ;// needed as 3 digit number
  CANData351[6] = uint16dischargeamps;    // Arduino/C is big endian. So needs to shift the opposite way of the SMA protocol.
  CANData351[7] = uint16dischargeamps >> 8; //

 // Battery capacity = fixed value from setup section
  CANData35F[6] = batteryCapacity;    // Arduino/C is big endian. So needs to shift the opposite way of the SMA protocol.
  CANData35F[7] = batteryCapacity >> 8; // 
  
  eolterm();
}
//=================================================================
void sendmqttcanbusvalues(){
  Serial.println("Void:sendmqttcanbusvalues");
  
  // send values back to HA
  //percentageDual
  //voltsdual
  //ampsdual
  float mqtt = 7;
  String state = "{";

  if (!( state == "{")) { state += ","; }
  state += "\"SenMQtt can val\":" + String(mqtt);
  
  if (!( state == "{")) { state += ","; }
  state += "\"Canpercent\":"+String(percentage);

  if (!( state == "{")) { state += ","; }
  state += "\"Canvolts\":"+String(volts);

  if (!( state == "{")) { state += ","; }
  state += "\"Canamps\":"+String(amps);
  
  if (!( state == "{")) { state += ","; }
  state += "\"CanBatCap\":"+String(batteryCapacity);

 // if (!( state == "{")) { state += ","; }
 // state += "\"voltsDual\":"+String(voltsDual);

  //if (!( state == "{")) { state += ","; }
  //state += "\"ampsDual\":"+String(ampsDual);

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
//------------------------------------------------------------
/* rx can data
 *  ------- CAN Read ----------
ID  DLC   DATA
305 8 0 0 0 0 0 0 0 0 
71F 8 0 0 0 0 0 0 0 0 
305 8 0 0 0 0 0 0 0 0 
71F 8 0 0 0 0 0 0 0 0 
351 8 2E 2 8A 2 8A 2 E0 1 
355 6 63 0 63 0 F2 26 
356 6 E6 14 0 0 96 0 
35A 8 0 0 0 0 0 0 0 0 
35B 2 0 0 
35E 8 53 4D 41 0 0 0 0 0 
35F 8 0 0 0 0 18 1 0 0 
305 8 0 0 0 0 0 0 0 0 
71F 8 0 0 0 0 0 0 0 0 
305 8 0 0 0 0 0 0 0 0 
71F 8 0 0 0 0 0 0 0 0 
351 8 2E 2 8A 2 8A 2 E0 1 
355 6 63 0 63 0 F2 26 
356 6 E6 14 0 0 96 0 
35A 8 0 0 0 0 0 0 0 0 
35B 2 0 0 
35E 8 53 4D 41 0 0 0 0 0 
35F 8 0 0 0 0 18 1 0 0 
305 8 0 0 0 0 0 0 0 0 
71F 8 0 0 0 0 0 0 0 0 
305 8 0 0 0 0 0 0 0 0 
71F 8 0 0 0 0 0 0 0 0 
351 8 2E 2 8A 2 8A 2 E0 1 
355 6 63 0 63 0 F2 26 
356 6 E6 14 0 0 96 0 
35A 8 0 0 0 0 0 0 0 0 
35B 2 0 0 
35E 8 53 4D 41 0 0 0 0 0 
35F 8 0 0 0 0 18 1 0 0 
305 8 0 0 0 0 0 0 0 0 
71F 8 0 0 0 0 0 0 0 0 
305 8 0 0 0 0 0 0 0 0 
71F 8 0 0 0 0 0 0 0 0 
351 8 2E 2 8A 2 8A 2 E0 1 
355 6 63 0 63 0 F2 26 
356 6 E6 14 0 0 96 0 
35A 8 0 0 0 0 0 0 0 0 
35B 2 0 0 
35E 8 53 4D 41 0 0 0 0 0 
35F 8 0 0 0 0 18 1 0 0 
305 8 0 0 0 0 0 0 0 0 
--------------
*/
//============== can bus RX================================================
void checkForRX() {
  Serial.println("VOID:check for Can rx");
  CAN_frame_t rx_frame;
  if (xQueueReceive(CAN_cfg.rx_queue, &rx_frame, 3 * portTICK_PERIOD_MS) == pdTRUE) {
    digitalWrite(ledPins[4], LOW); //turn on 6th LED
    Serial.println("canrx: A");
    if (rx_frame.FIR.B.FF == CAN_frame_std) {
      printf("New standard frame");
      Serial.println("canrx: A");
    } else {
      printf("New extended frame");
      Serial.println("canrx: b");
    }

    if (rx_frame.FIR.B.RTR == CAN_RTR) {
      printf(" RTR from 0x%08X, DLC %d\r\n", rx_frame.MsgID,  rx_frame.FIR.B.DLC);
      Serial.println("canrx: C");
    } else {
      printf(" from 0x%08X, DLC %d, Data ", rx_frame.MsgID,  rx_frame.FIR.B.DLC);
      for (int i = 0; i < rx_frame.FIR.B.DLC; i++) {
        printf("0x%02X ", rx_frame.data.u8[i]);
      }
      printf("\n");
    }
    digitalWrite(ledPins[4], HIGH); //turn on 6th LED
  }
      eolterm();
}
