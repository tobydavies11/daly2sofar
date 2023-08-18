//================ current bms uart =================================
void getEverythingFromDaly() {
  Serial.println("VOID:get Everything From Daly is about to run ");
  digitalWrite(ledPins[6], LOW); //turn on 6th LED
  // loop below is to reset volts to zero during a malfunction, this is checking a BOOL
  
 // if (bms.getPackMeasurements() == 0){
 //       bms.get.packVoltage = 0;
 //       BMSOnline = false;
 //      //CANOnline = false;   
 //     }
      
  bms.update(); // interrogate daly for current values
 // vTaskDelay(100); // to help it settle?
  
  // temp
 /*   
  *    this doseent work as was acutally calling the read again and messing it up
  Serial.println("getDischargeChargeMosStatus:    ChgFet:" + (String)bms.get.chargeFetState + "DisFet:" + (String)bms.get.disChargeFetState);
  Serial.println("***----boolean Checks----***"); 
  Serial.println("Main update:                 " + (String)bms.update() + "Bool:");
  Serial.println("getPackMeasurements:         " + (String)bms.getPackMeasurements() + "Bool:" );
  Serial.println("getMinMaxCellVoltage:        " + (String)bms.getMinMaxCellVoltage() + "Bool:");
  Serial.println("getPackTemp:                 " + (String)bms.getPackTemp() + "Bool:");
  Serial.println("getDischargeChargeMosStatus: " + (String)bms.getDischargeChargeMosStatus() + "Bool:");
  Serial.println("getStatusInfo:               " + (String)bms.getStatusInfo() + "Bool:");
  Serial.println("getCellVoltages:             " + (String)bms.getCellVoltages() + "Bool:");
  Serial.println("getCellTemperature:          " + (String)bms.getCellTemperature() + "Bool:");
  Serial.println("getCellBalanceState:         " + (String)bms.getCellBalanceState() + "Bool:");
  Serial.println("getFailureCodes:             " + (String)bms.getFailureCodes() + "Bool:");
  Serial.println("BMS status:                  " + (String)bms.get.chargeDischargeStatus);
*/


  volts = bms.get.packVoltage ;
  amps = bms.get.packCurrent ;
  percentage = bms.get.packSOC ;
  
  temp = bms.get.tempAverage ;
  minCellVoltage = bms.get.minCellmV ;
  minCellNumber = bms.get.minCellVNum ;
  maxCellVoltage = bms.get.maxCellmV ;
  maxCellNumber = bms.get.maxCellVNum ;
  cellImbalance = bms.get.cellDiff ;//(maxCellVoltage - minCellVoltage); // 
  Cycles = bms.get.bmsCycles ;
  Capacity = bms.get.resCapacitymAh ;
// used to moves values to respective banks for interbms comms
#ifdef BANK_A
  SocA = bms.get.packSOC ;       // sent out via mqtt inter bms comms
  VoltsA = bms.get.packVoltage;       //  
  AmpsA = bms.get.packCurrent ; // 
  //percentageDual = ((SocB + percentage) / 2); // passed to the dual floats
  //ampsDual = ((AmpsB + amps));
  //voltsDual = ((VoltsB + volts) / 2);
   
#else // BANK_A
  SocB = bms.get.packSOC ;       // 
  VoltsB = bms.get.packVoltage;       //  
  AmpsB = bms.get.packCurrent;  //
  //Serial.println("Pecentdual" + String (percentageDual));
  //percentageDual = ((SocA + percentage) / 2); // passed to the dual floats
  //ampsDual = ((AmpsA + amps));
  //voltsDual = ((VoltsA + volts) / 2);
#endif

  //getEverythingFromDalyCells(); // copies individual cell voltages to floats
   //char like[] = "I like coffee and cake"; // create a string 
 //  char text[] = "xxxxxxxxxxxxx";
  // text = bms.get.chargeDischargeStatus ; // this maybe a status indicator stationary, charge, discharge
/*
  Serial.println("bms chgdisstatus" + String(bms.get.chargeDischargeStatus));
  if ( bms.get.chargeDischargeStatus == "Stationary" ) {
    bmsState = 0;
  }
  if (bms.get.chargeDischargeStatus == "charge"){
    bmsState = 1;
  }
  if (bms.get.chargeDischargeStatus == "Discharge"){
    bmsState = 2;
  } 
  else {
   bmsState = 5; // fault
  }
*/   

/*
// Define 
String str = bms.get.chargeDischargeStatus; 
//String str = "This is my string"; 
// Length (with one extra character for the null terminator)

int str_len = str.length() + 1; 

// Prepare the character array (the buffer) 
char char_array[str_len];

// Copy it over 
str.toCharArray(char_array, str_len);

//char* numbers = "charge";
//char numbers = bms.get.chargeDischargeStatus;
if (strcmp (char_array,"Stationary") == 0) {
//Serial.println("Letter A");
bmsState = 0;
}
if(strcmp(char_array,"charge") == 0) {
bmsState = 1;
//Serial.println("Letter B");
if(strcmp(char_array,"Discharge") == 0) {
bmsState = 2;
//Serial.println("Letter C");
}
*/

  bmsState = bms.get.chgDisStsNumerical ; // simple numerical version 
#ifdef BANK_A
  bmsStateA = bms.get.chgDisStsNumerical ; // simple numerical version
#else
  bmsStateB = bms.get.chgDisStsNumerical ; // simple numerical version
#endif  
// -------  

// --------------------------------

  //if ( volts == 0) { // temp suspended

  // || = or
  if ((volts >=59) || (volts <= 45)){
    bmsupdateerror++; // to count error results
    BMSOnline = false;
    //batteryLogic = 1; //single bank operations this is to make sure bms going offline reports and goes back to single bank logic
    CANOnline = false;
    Serial.println("No connection with BMS. Check TX/RX polarity, and that BMS is awake...1");
    if ( volts == 0) {
    oledMessage("No BMS found.", "Check TX/RX", "polarity,", "Make sure", "BMS is awake.", "status");
    }
    if ((volts >=59) || (volts <= 45)) {
    oledMessage("BMS ", "Spurious values", "temp ", "offline", "_", "status");
    bmsDalyFault = true;
    bmsDalyFaultStop++;
    }
    } 
  else if((percentage >=100) || (percentage <= 1)){
     bmsupdateerror++; // to count error results
     bmsDalyFault = true;
     bmsDalyFaultStop++;
     BMSOnline = false;
     CANOnline = false;
  }
 // else if((amps >=100) || (percentage <= -100)){
 //    BMSOnline = false;
 //    CANOnline = false;
  //}
  else {
    watts = amps * volts;
    BMSOnline = true;
    bmsDalyFault = false;
    bmsDalyFaultStop = 0;
#ifdef BANK_A
    //BMSAsts = BMSOnline; // this is wrong it a boolean andbmsAsts is a 1,2,3,5,
    if(BMSOnline == true){
      BMSAsts = 1;
    }
    else{
      BMSAsts = 2;
    }
#else
    //BMSBsts = BMSOnline;
    if(BMSOnline == true){
      BMSBsts = 1;
    }
    else{
      BMSBsts = 2;
    }
#endif
    // oledMessage("SOC: "+String(percentage)+"%", "Volts: "+String(volts)+"V", "Amps: "+String(amps)+"A", "Hi/Lo: "+String(minCellVoltage *0.001)+"/"+String(maxCellVoltage * 0.001), "Temp: "+String(temp),"Output:" +String(text));
    //oledMessage("SOC: " + String(percentage) + "%", "Volts: " + String(volts) + "V", "Amps: " + String(amps) + "A", "Hi/Lo: " + String(minCellVoltage * 0.001) + "/" + String(maxCellVoltage * 0.001), "STS:" + String(text), "Temp: " + String(temp));
  //  oledMessage("SOC: " + String(percentage) + "%", "Volts: " + String(volts) + "V", "Amps: " + String(amps) + "A", "ImbL: " + String(cellImbalance)+ "mV", "STS:" + String(text), "Temp: " + String(temp));
    oledMessage("SOC: " + String(percentage) + "%", "Volts: " + String(volts) + "V", "Amps: " + String(amps) + "A", "ImbL: " + String(cellImbalance)+ "mV", "STS:" + String(bms.get.chargeDischargeStatus), "Temp: " + String(temp));
      }
      digitalWrite(ledPins[6], HIGH); //turn off 6th LED
      Serial.println("void:getEverythingFromDaly has been run ");
      eolterm(); 
}
//===================  check individual cells=================
void getEverythingFromDalyCells() {
  // reads individual cell volatges
  
  
  //code from dalybms uart software
  // this runs throught the arrays
  /* for (size_t i = 0; i < size_t(bms.get.numberOfCells); i++)
    {
     Serial.println("Remaining Capacity mAh:      " + (String)bms.get.cellVmV[i]);
    }
  */  
 /*   
    for (size_t i = 0; i < size_t(bms.get.numberOfCells); i++)
    {
     //Serial.println("Remaining Capacity mAh:      " + (String)bms.get.cellVmV[i]);
     cell[i] = bms.get.cellVmV[i];
     // need to create cell array
    }
  */  
  // cells update is done inside geteverythingfrom daly
  //changed first array to zero
/*  cella1 = bms.get.cellVmV[0];
  cella2 = bms.get.cellVmV[1];
  cella3 = bms.get.cellVmV[2];
  cella4 = bms.get.cellVmV[3];
  cella5 = bms.get.cellVmV[4];
  cella6 = bms.get.cellVmV[5];
  cella7 = bms.get.cellVmV[6];
  cella8 = bms.get.cellVmV[7];
  cella9 = bms.get.cellVmV[8];
  cella10 = bms.get.cellVmV[9];
  cella11 = bms.get.cellVmV[10];
  cella12 = bms.get.cellVmV[11];
  cella13 = bms.get.cellVmV[12];
  cella14 = bms.get.cellVmV[13];
  cella15 = bms.get.cellVmV[14];
  cella16 = bms.get.cellVmV[15];
*/  
  eolterm();
}
//===============================
//============== check alarms==================
 void getEverythingFromDalyAlarm() {
  // this is for bms alarms
  // I assume its reporting the cell number as a result
// test each for a result, if in alarm then allow to transmit
  if (bms.alarm.levelOneCellVoltageTooHigh){
    BMSAlarm = true ;    
    }
  if (bms.alarm.levelTwoCellVoltageTooHigh){
    BMSAlarm = true;
  }
  if (bms.alarm.levelOneCellVoltageTooLow ){
    BMSAlarm = true;
  }
  if (bms.alarm.levelTwoCellVoltageTooLow){
    BMSAlarm = true;
  }
  else {
      BMSAlarm = false ;
  }
/*  

     
      bms.alarm.levelTwoCellVoltageTooLow ;
      bms.alarm.levelOnePackVoltageTooHigh ;
      bms.alarm.levelTwoPackVoltageTooHigh ;
      bms.alarm.levelOnePackVoltageTooLow ;
      bms.alarm.levelTwoPackVoltageTooLow ;


      // 0x01
      bms.alarm.levelOneChargeTempTooHigh ;
      bms.alarm.levelTwoChargeTempTooHigh ;
      bms.bms.alarm.levelOneChargeTempTooLow ;
      bms.alarm.levelTwoChargeTempTooLow ;
      bms.alarm.levelOneDischargeTempTooHigh ;
      bms.alarm.levelTwoDischargeTempTooHigh ;
      bms.alarm.levelOneDischargeTempTooLow ;
      bms.alarm.levelTwoDischargeTempTooLow ;

      // 0x02
      bms.alarm.levelOneChargeCurrentTooHigh ;
      bms.alarm.levelTwoChargeCurrentTooHigh ;
      bms.alarm.levelOneDischargeCurrentTooHigh ;
      bms.alarm.levelTwoDischargeCurrentTooHigh ;
      bms.alarm.levelOneStateOfChargeTooHigh ;
      bms.alarm.levelTwoStateOfChargeTooHigh ;
      bms.alarm.levelOneStateOfChargeTooLow ;
      bms.alarm.levelTwoStateOfChargeTooLow ;

      // 0x03
      bms.alarm.levelOneCellVoltageDifferenceTooHigh ;
      bms.alarm.levelTwoCellVoltageDifferenceTooHigh ;
      bms.alarm.levelOneTempSensorDifferenceTooHigh ;
      bms.alarm.levelTwoTempSensorDifferenceTooHigh ;

      // 0x04
      bms.alarm.chargeFETTemperatureTooHigh ;
      bms.alarm.dischargeFETTemperatureTooHigh ;
      bms.alarm.failureOfChargeFETTemperatureSensor ;
      bms.alarm.failureOfDischargeFETTemperatureSensor ;
      bms.alarm.failureOfChargeFETAdhesion ;
      bms.alarm.failureOfDischargeFETAdhesion ;
      bms.alarm.failureOfChargeFETTBreaker ;
      bms.alarm.failureOfDischargeFETBreaker ;

      // 0x05
      bms.alarm.failureOfAFEAcquisitionModule ;
      bms.alarm.failureOfVoltageSensorModule ;
      bms.alarm.failureOfTemperatureSensorModule ;
      bms.alarm.failureOfEEPROMStorageModule ;
      bms.alarm.failureOfRealtimeClockModule ;
      bms.alarm.failureOfPrechargeModule ;
      bms.alarm.failureOfVehicleCommunicationModule ;
      bms.alarm.failureOfIntranetCommunicationModule ;

      // 0x06
      bms.alarm.failureOfCurrentSensorModule ;
      bms.alarm.failureOfMainVoltageSensorModule ;
      bms.alarm.failureOfShortCircuitProtection ;
      bms.alarm.failureOfLowVoltageNoCharging ;
  */
      eolterm();
}
