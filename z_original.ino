//======================================

void getEverythingFromDalyorig() {

  // //*** this is not used***

  
  Serial.println("geteverythingfromdaly loop");
  // Grab values from BMS
  /*  Serial.println("geteverythingfromdaly main");
    //Serial.println(dalyRequestCounter);
    bms.getPackMeasurements();
    volts = bms.get.packVoltage ;
    amps = bms.get.packCurrent ;
    percentage = bms.get.packSOC ;
    bms.getPackTemp();
    temp = bms.get.tempAverage ;
    bms.getMinMaxCellVoltage(); // change

    minCellVoltage = bms.get.minCellmV ;
    minCellNumber = bms.get.minCellVNum ;
    maxCellVoltage = bms.get.maxCellmV ;
    maxCellNumber = bms.get.maxCellVNum ;




    oledMessage("SOC: "+String(percentage)+"%", "Volts: "+String(volts)+"V", "Amps: "+String(amps)+"A", "Hi/Lo: "+String(minCellVoltage)+"/"+String(maxCellVoltage), "Temp: "+String(temp));

  */

  //bms. (); // interogates bms and load all results into the arrays eg bms.get.tempAverage)
  // bms.getPackMeasurements()
  Serial.println("dalycounter");
  Serial.println(dalyRequestCounter);
  dalyRequestCounter++;
  Serial.println("dalycounter");
  Serial.println(dalyRequestCounter);

  if (dalyRequestCounter > 3) {
    dalyRequestCounter = 1;
  }

  //Serial.println("dalyRequestCounter:" );
  //Serial.println(dalyRequestCounter) ;


  if (dalyRequestCounter == 1) {
    //  bms.getPackMeasurements(volts, amps, percentage); // suspended
    // Serial.println("geteverythingfromdaly main1");
    //changes

    //bms.getPackMeasurements();

    volts = bms.get.packVoltage ;
    amps = bms.get.packCurrent ;
    percentage = bms.get.packSOC ;
    // Serial.println("amps refresh:");
    //Serial.print(amps);

    vTaskDelay(100);
    if ( volts == 0) {
      //if ( bms.getPackMeasurements(volts, amps, percentage) == false) {
      BMSOnline = false;
      CANOnline = false;
      Serial.println("No connection with BMS. Check TX/RX polarity, and that BMS is awake...1");
      oledMessage("No BMS found.", "Check TX/RX", "polarity,", "Make sure", "BMS is awake.", "status");
      //  xTimerStart(dalyRetryXTimer, 0);
    } else {
      watts = amps * volts;
      BMSOnline = true;
      oledMessage("SOC: " + String(percentage) + "%", "Volts: " + String(volts) + "V", "Amps: " + String(amps) + "A", "Hi/Lo: " + String(minCellVoltage * 0.001) + "/" + String(maxCellVoltage * 0.001), "Temp: " + String(temp), "status");
      Serial.println("geteverythingfromdaly main1oled");
    }
    //Serial.println("loop1:");
    //Serial.println(dalyRequestCounter);
  }

  if (dalyRequestCounter == 2) {
    // Serial.println("geteverythingfromdaly main2");
    //bms.getPackTemp(temp);
    bms.getPackTemp();
    // bms.getCellTemperature();
    temp = bms.get.tempAverage ;
    //Serial.println("temps2:");
    // Serial.print(temp);


    //get.tempMax = (this->my_rxBuffer[4] - 40);
    //get.tempMin = (this->my_rxBuffer[6] - 40);
    //get.tempAverage = (get.tempMax + get.tempMin) / 2;


    vTaskDelay(100);
    if ( temp == 0) {
      //if ( bms.getPackTemp(temp) == false) {
      BMSOnline = false;
      CANOnline = false;
      Serial.println("No connection with BMS. Check TX/RX polarity, and that BMS is awake...2");
      oledMessage("No BMS found.", "Check TX/RX", "polarity,", "Make sure", "BMS is awake.", "status");
      //   xTimerStart(dalyRetryXTimer, 0);
    } else {
      BMSOnline = true;
      oledMessage("SOC: " + String(percentage) + "%", "Volts: " + String(volts) + "V", "Amps: " + String(amps) + "A", "Hi/Lo: " + String(minCellVoltage) + "/" + String(maxCellVoltage), "Temp: " + String(temp), "status");
      Serial.println("geteverythingfromdaly main2oled");
    }
    //Serial.println("loop2:");
    //Serial.println(dalyRequestCounter);
  }

  if (dalyRequestCounter == 3) {
    //bms.getMinMaxCellVoltage(minCellVoltage, minCellNumber, maxCellVoltage, maxCellNumber);
    // Serial.println("geteverythingfromdaly main3");
    bms.getMinMaxCellVoltage(); // change

    minCellVoltage = bms.get.minCellmV ;
    minCellNumber = bms.get.minCellVNum ;
    maxCellVoltage = bms.get.maxCellmV ;
    maxCellNumber = bms.get.maxCellVNum ;

    //Serial.println("min max v:");
    //Serial.print(minCellVoltage);

    //get.maxCellmV = (float)((this->my_rxBuffer[4] << 8) | this->my_rxBuffer[5]);
    //get.maxCellVNum = this->my_rxBuffer[6];
    //get.minCellmV = (float)((this->my_rxBuffer[7] << 8) | this->my_rxBuffer[8]);
    //get.minCellVNum = this->my_rxBuffer[9];
    //get.cellDiff = (get.maxCellmV - get.minCellmV);

    vTaskDelay(100);
    if (  minCellVoltage == 0) {
      //if ( bms.getMinMaxCellVoltage(minCellVoltage, minCellNumber, maxCellVoltage, maxCellNumber) == false) {
      BMSOnline = false;
      CANOnline = false;
      Serial.println("No connection with BMS. Check TX/RX polarity, and that BMS is awake...3");
      oledMessage("No BMS found.", "Check TX/RX", "polarity,", "Make sure", "BMS is awake.", "unused");
      //    xTimerStart(dalyRetryXTimer, 0);
    } else {
      BMSOnline = true;
      cellImbalance = (maxCellVoltage - minCellVoltage) * 1000;
      oledMessage("SOC: " + String(percentage) + "%", "Volts: " + String(volts) + "V", "Amps: " + String(amps) + "A", "Hi/Lo: " + String(minCellVoltage) + "/" + String(maxCellVoltage), "Temp: " + String(temp), "Status:" );
      //Serial.println("geteverythingfromdaly main3oled");
    }
    //Serial.println("loop3:");
    //Serial.println(dalyRequestCounter);
  }


  /*
    if (bms.getPackMeasurements(volts, amps, percentage) == false){
      BMSOnline = false;
      CANOnline = false;
      Serial.println("No connection with BMS. Check TX/RX polarity, and that BMS is awake...");
      ("No BMS found.", "Check TX/RX", "polarity,", "Make sure", "BMS is awake.");
      xTimerStart(dalyRetryXTimer, 0);
    } else {
      //delay(20);
      bms.getPackTemp(temp);
      bms.getMinMaxCellVoltage(minCellVoltage, minCellNumber, maxCellVoltage, maxCellNumber);
      //delay(20);
      //delay(20);
      watts = amps*volts;
      oledMessage("SOC: "+String(percentage)+"%", "Volts: "+String(volts)+"V", "Amps: "+String(amps)+"A", "Hi/Lo: "+String(minCellVoltage)+"/"+String(maxCellVoltage), "Temp: "+String(temp));
      BMSOnline = true;
      // Print above to Serial
      //Serial.printf("V: %4.1f, I: %4.1f, \%:%4.1f\n",volts, amps, percentage);

      // Now the same thing, but for temperature
      //Serial.printf("Temp: %d\n",temp);

      // And again, for min/max cell voltages
      //Serial.printf("Max Cell: Cell #%d at %4.3f\n",maxCellNumber,maxCellVoltage);
      //Serial.printf("Min Cell: Cell #%d at %4.3f\n",minCellNumber,minCellVoltage);
    }

  */
}
//=================
/*
void onMqttMessagenotinuse(char* topic, char* payload, const AsyncMqttClientMessageProperties& properties, 
                   const size_t& len, const size_t& index, const size_t& total)
{
  (void) payload;
  
  Serial.println("Publish received.");
  Serial.print("  topic: ");  Serial.println(topic);
  Serial.print("  qos: ");    Serial.println(properties.qos);
  Serial.print("  dup: ");    Serial.println(properties.dup);
  Serial.print("  retain: "); Serial.println(properties.retain);
  Serial.print("  len: ");    Serial.println(len);
  Serial.print("  index: ");  Serial.println(index);
  Serial.print("  total: ");  Serial.println(total);
}
*/
//========================================================================

/*
void onMqttMessageorig(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  // added here
  Serial.println("void:onmqttmessage possible rx");
  Serial.println("Publish received.");
  Serial.print("  topic: ");
  Serial.println(topic);
  Serial.print("  qos: ");
  Serial.println(properties.qos);
  Serial.print("  dup: ");
  Serial.println(properties.dup);
  Serial.print("  retain: ");
  Serial.println(properties.retain);
  Serial.print("  len: ");
  Serial.println(len);
  Serial.print("  index: ");
  Serial.println(index);
  Serial.print("  total: ");
  Serial.println(total);
}

*/

//========================
/*
void dalyRetry() {
  Serial.println("dalyretry");
  //  xTimerStop(dalyRetryXTimer, 0);

}
*/
void updateTXFramesorig() {
// copy of orginal
  // this section adjusts the can frames with the uodates from the bms ready for transmission back to the inverter
 // Serial.println("Void:updatetxframes about to run");
  // SOC. 1:1 scaling, 2 bytes - easy.

  CANData355[2] = (int)percentage;
 // Serial.print("percentage:");
 // Serial.println(percentage);

  // Charge voltage required set in define stage
//  uint16_t uint16chargevolts = batteryChargeVoltage * 10; //original

  uint16_t uint16chargevolts = ChgVolts1; // this is controllable from mqtt mx already sent as 3 digit
  CANData351[2] = uint16chargevolts;        // Arduino/C is big endian. So needs to shift the opposite way of the SMA protocol.
  CANData351[3] = uint16chargevolts >> 8;
//  Serial.print("batteryChargeVoltage:");
//  Serial.println(batteryChargeVoltage);
//  Serial.print("CANData351[2]:");
//  Serial.println(CANData351[2]);
//  Serial.print("CANData351[3]");
//  Serial.println(CANData351[3]);

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
  //ampsDual = amps + Amps
  int16_t int16amps = amps * 10;
  CANData356[4] = int16amps;        // Arduino/C is big endian. So needs to shift the opposite way of the SMA protocol.
  CANData356[5] = int16amps >> 8;

  //bms batt temps tx to inverter
  // Battery temp. 0.1 scale, 2 bytes SIGNED. Not easy. First rescale, then bitshift
  int16_t int16temp = temp * 10;
  CANData356[6] = int16temp;        // Arduino/C is big endian. So needs to shift the opposite way of the SMA protocol.
  CANData356[7] = int16temp >> 8;

  // Battery capacity = fixed value from setup section
//  CANData35F[6] = batteryCapacityDual;    // Arduino/C is big endian. So needs to shift the opposite way of the SMA protocol.
//  CANData35F[7] = batteryCapacityDual >> 8; // Currently reporting 51200ah :-D obvs wrong...

  // Battery capacity = fixed value from setup section
  CANData35F[6] = batteryCapacity;    // Arduino/C is big endian. So needs to shift the opposite way of the SMA protocol.
  CANData35F[7] = batteryCapacity >> 8; // Currently reporting 51200ah :-D obvs wrong...
//supplemtary for control of change and discharge amps

  
  //charge power
  uint16_t uint16chargeamps = ChgAmps1  ;// arrive as 3 digit number
  CANData351[4] = uint16chargeamps;    // Arduino/C is big endian. So needs to shift the opposite way of the SMA protocol.
  CANData351[5] = uint16chargeamps >> 8; // 
  //Serial.print("ChgAmps:");

 // Serial.print("CANData351[3]:");
 // Serial.println(CANData351[3]);
 // Serial.print("CANData351[4]");
  //Serial.println(CANData351[4]);

  
  uint16_t uint16dischargeamps = DisAmps1 ;// needed as 3 digit number
  CANData351[6] = uint16dischargeamps;    // Arduino/C is big endian. So needs to shift the opposite way of the SMA protocol.
  CANData351[7] = uint16dischargeamps >> 8; //

  eolterm();
}
