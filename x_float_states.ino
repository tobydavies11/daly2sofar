//=========================
//float HaBmsAsts = 1; //  1 = online, 2 offline, 3 = error
//float HaBmsBsts = 1;
//=========================================
//float BmsA = 0;  // identify which bms is in charge  
//float BmsB = 0;  // other bms
// 0 = invalid/bootup
// 1 = master (master)
// 2 = slave (slave)
// 3 = indepandant mode, mainly for testing, eg running other bank before dual ops (isol)
// 4 = error
//=========================
//float CanBussPrio = 1; // 0 = unknown | 1: bank A | 2: bank B | 3: independant both run risk of dual transmisiion
// battery logic uused to provide single or dual values to the tx frames section of canbus
//float batteryLogic = 1; //0 = unknown | 1: single pack operation(DEFAULT)| 2: dual operation
//float bmsState = 5;   // possibly not required
//float bmsStateA = 5; // 1: stationary 2:charge 3:dishcarge  5: is offline or unknown.startup
//float bmsStateB = 5; // 1: stationary 2:charge 3:dishcarge  5: is offline or unknown.startup

//float CanControl = 1; // final control on canbus 1= on 2:0FF
// int CanbusInhibit = 1; // remote stop via mqtt of canbus 1:RUN 2:STOP
//boolean BMSOnline = false;
//boolean CANOnline = false;
//boolean BMSAlarm = false; // boolean to indicate if bms is in alarm state

//float volts = 0; // first copy from bms
//float voltsDual = 0; // used in dual ops
//float amps = 0;
//float ampsDual = 0;
//float percentage = 0;
//float percentageDual = 0;
