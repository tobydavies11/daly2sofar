/*Version History
 * 
 * v.23 18/10 confirmed working and remote current and voltage limit functioning
 * v3.22  15 oct added chg volts and amps control
 * v3.21: charge voltage control by mqtt,charge and discharge current control
 * v3.20 
 * 
 */

//-------------------------------------------
/* **** to be fixed****
    1: control bmw fets ideally on reception of mqtt message
    todo: rx mqtt void possibly working
    

    2: send individual battery voltages ***done**
    timings or on x mqtt messages

    3:error messages

    4:establish canbus ops
    4a:
   
    5:dual bms operations
    5a how to receive data
*/

/*
  Daly2Sofar

  An ESP32 based bridge that allows Daly Smart BMS to be used with a Sofar inverter/charger (and others that use SMA protocol).
  It connects to Daly via UART and transmits to the inverter on CANBUS (SMA protocol).
  At the same time, it will transmit the BMS data to an MQTT broker if it can connect to your WiFi.
  If it can't connect to WiFi or your MQTT broker, it will still work as a BMS-to-Inverter bridge.

  Data comms is one way only - data is requested from Daly and sent to the Inverter & over MQTT.
  May be compatible with other inverters that use the SMA protocol.

  My testing showed that Daly UART port works on 3.3v. PLEASE measure yours before connecting the ESP. Some users report 5V.


****** a point on isolation *
  Some battery setups earth the battery negative, others don't.
  For the ones that aren't earthed, you may get a ID05 Fault on the inverter when you connect canbus.
  This is because the Daly UART ground is battery ground, and it makes its way to the inverter.
  The Daly needs to be isolated in this case, using a digital isolation IC.
  Opto-coupler's don't work as the signal becomes inverted. ESP32 serial can be inverted by software but Daly can't.
  I used a IL716-3E isolator IC and soldered dupont pins to it's legs - it's tiny, it's not easy!
  If you find an isolator IC module that is cheap and works without soldering, please let me know!
******


  Using SMA_CAN_protocol.pdf for the CAN IDs and data format info (google will find you the PDF)

  Using https://cryptii.com/pipes/integer-encoder for working out hex values
  VIEW TEXT > DECODE INTEGER (LITTLE ENDIAN) U16 > VIEW BYTES HEX GROUPED BY BYTES

  https://www.scadacore.com/tools/programming-calculators/online-hex-converter/ for checking hex values
  icons made with https://www.pixilart.com/draw/icon-maker-82d9ebbc1fc1fec#
  converted with https://diyusthad.com/image2cpp



****** a point on ESP32 pin remapping *
  Make sure the I2C pins are mapped correctly for the Oled to work. For me, pins 21 and 22 work great
  C:\Users\XXXXX\Documents\Arduino\libraries\Adafruit_SSD1306-1.1.2\Adafruit_SSD1306.cpp
  #define I2C_SDA 21
  #define I2C_SCL 22

  And also Serial2 needs to be correctly mapped in this file (This is Windows. Sorry I don't know where it is on a Mac)
  C:\Users\XXXXX\AppData\Local\Arduino15\packages\esp32\hardware\esp32\1.0.6\cores\esp32\HardwareSerial.cpp
  Look for this near the top:
  #ifndef RX2
  #define RX2 16
  #endif

  #ifndef TX2
  #define TX2 17
  #endif

  #ifndef RX1
  #define RX1 9
  #endif

  #ifndef TX1
  #define TX1 10
  #endif
  sd2 sd3 are gpio 9 and 10
******


  MQTT info:
  Subscribe your MQTT client to:
  Daly2Sofar/state

  topics published are:
  soc
  voltage
  current
  power
  temp
  lowestcell
  highestcell
  cellimbalance

  For extracting the data into Home Asisstant sensors, use Node-Red. Have a look at the github for an example Node-Red setup.

  To do:
  - Do proper CANBUS status check. Currently, CAN indicator will go live after the data send, whether it's succesfull or not. CAN only goes off if BMS not found.
  - Need to figure out how to obtain battery cycles from BMS.
  - Tidy up code/refactor where poss.
  - Longer term stablity testing.

  Notes/dependencies:
  - Developed on Arduino IDE 1.8.5. IDE version 1.6.14 failed to compile. Downgrade if you have issues
  - Using arduino ESP32 Core version 1.0.6
  - https://github.com/maland16/daly-bms-uart
  - https://github.com/miwagner/ESP32-Arduino-CAN
  - https://github.com/me-no-dev/AsyncTCP
  - https://github.com/marvinroger/async-mqtt-client

*/
/* pinouts used
 *  I2C Address currently in use ? 0x3D|0x3C
 *  
 *  scl         IO22
 *  sda         IO21
 *  BMS1 Uart   IO17  TXD (esp 32) - opto isolator RX1
 *  BMs1 Uart   IO16  RXD          - optp isoalor TX1
 *  (not is use yet)
 *  BMS2 uart?  SD0 aka gpio 10
 *  bMS2 Uart     SD1 aka gpio 9
 *  
 *  CanTx       IO5
 *  CanRX       IO4

   possible use for switch
   0 - 4095 steps max 3.3v
    adc         IO36
    adc         IO39

   manual batt control                | adc Value
   Force OFF MQTT commands ignored    | <500
   Auto - controlled by mqtt          | >600 <3500
   Force ON mqtt commands ignored     | >3700
   
*/
//------ defines section
// -- set at compile
//uncomment to acticate
//#define DEBUG_SERIAL Serial

//#ifdef DEBUG_SERIAL
//        DEBUG_SERIAL.print("<DALY-BMS DEBUG> Receive failed, V, I, & SOC values won't be modified!\n");
//#endif

//uncomment one of
//---------------------------------
//#define BANK_A // FIRST BMS
//----------------------------------
#define BANK_B //SECOND BMS
//---------------------------------
//#define BANK_C //third BMS
//---------------------------------
# define OTA // over the air update

//single bank mode
//#define Single_Bank_Only  // set canbus to default to run
//
/*
#ifdef BANK_A
        //CODE IN HERE
#endif

#ifdef BANK_B
        //CODE IN HERE
#endif
*/
//const char* versionNumber = "v3.1"; // used in oledbootup sequence
String versionNumber = "5.18"; // used in oledbootup sequence
//const char* versiondate = "3/8/22"; // used in oled bootup sequence
String versionDate = "180823"; // used in oled bootup sequence

// v3.19 added serial message to on mqtt message
// BATTERY CAPACITY (Doesn't really matter, as it's the BMS that works out the SOC)
uint16_t batteryCapacity = 280; // Battery capacity - in AH.
//uint16_t batteryCapacityDual = 560; // Battery capacity - in AH. for twin setup

//float batteryChargeVoltage = 55.6; // Set your charge voltage - this doesn't come from BMS.** this sets the max voltage every time update tx frames is run

// ENTER YOUR WIFI & MQTT BROKER DETAILS HERE



// Set your Static IP address

#ifdef BANK_A
   IPAddress local_IP(***,***, ***,*** );
#elif defined(BANK_B)
    IPAddress local_IP(***,***,***,*** );
#else
   IPAddress local_IP(***,***,***,*** );
#endif   
// Set your Gateway IP address
IPAddress gateway(172, 20, 30, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);   //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional

static const char mqttUser[] = "daly2sofar";
static const char mqttPassword[] = "daly2sofar";
//const char* deviceName = "Daly2Sofar"; //Device name is used as the MQTT base topic.
const char* deviceName1 = "Daly2Sofar"; //Device name is used as the MQTT base topic
//const char* deviceNameCmd = "Daly2Sofar/mqtt/cmd"; //used for cmd or control messages
#ifdef BANK_A
       const char* deviceName = "Daly2Sofar/Bank/A"; //Device name is used as the MQTT base topic
     //  const char* deviceNameCmd = "Daly2Sofar/Bank/B"; //used to send to other bms      
#elif defined(BANK_B)
       const char* deviceName = "Daly2Sofar/Bank/B"; //Device name is used as the MQTT base topic
      // const char* deviceNameCmd = "Daly2Sofar/Bank/A"; //used to send to other bms  

#else
       const char* deviceName = "Daly2Sofar/Bank/C"; //Device name is used as the MQTT base topic
       //const char* deviceNameCmd = "Daly2Sofar/Bank/A"; //used to send to other bms  
#endif

#define MQTT_HOST IPAddress(172, 20, 30, 138) //replace with your MQTT server IP
#define MQTT_PORT 1883

const char* version = "Tobys version";

#include "ArduinoJson.h"
#include <WiFi.h>

#ifdef OTA
 #include <ArduinoOTA.h> // new
#endif

extern "C" {
  #include "freertos/FreeRTOS.h"
  #include "freertos/timers.h"
  }

#include <AsyncMqttClient.h>

AsyncMqttClient mqttClient;

// ESP32 CAN Library: https://github.com/miwagner/ESP32-Arduino-CAN
#include <ESP32CAN.h>
#include <CAN_config.h>
CAN_device_t CAN_cfg;
CAN_frame_t tx_frame;
const int rx_queue_size = 10;       // Receive Queue size
///////////////////////////////////////////////////////////////////



// serial ports  GPIO  TX  RX
// serial 1            10   9
// serial 2            17   16   this is currebntly in use gpio 17 and 16
// Daly UART library: https://github.com/maland16/daly-bms-uart
#include <daly-bms-uart.h>
Daly_BMS_UART bms(Serial2); // normal, serial 0 is for flashing
//Daly_BMS_UART bms2(Serial1); //

// home assistant monirting of rx mqtt data from the bms
// if message hasnt been received for 180seconds it changes to unavailble
// if bms is HaBmsXsts is offline then wipe the rx values
//byte HaBmsAsts = 1; // 1 = online, 2 offline, 3 = error
//byte HaBmsBsts = 1;


 
//float BmsA = 0;  // identify which bms is in charge  
//float BmsB = 0;  // other bms
// 0 = invalid/bootup
// 1 = master (master)
// 2 = slave (slave)
// 3 = indepandant mode, mainly for testing, eg running other bank before dual ops (isol)
// 4 = error

//boolean BMSoppOnline = false; // for monitring opposite bms 
//boolean CANoppOnline = false; // set to true if opposte online
//boolean BmsPrio = false ;  // 

//int battinitFetCtllogic = 0; // this captures where the logic is and reports back to HA

//#ifdef Single_Bank_Only
//byte CanBussPrio = 3;
//byte BmsA = 3;  // identify which bms is in charge  
//byte BmsB = 3;
//#else
byte CanBussPrio = 1; // 0 = unknown | 1: bank A | 2: bank B | 3: independant both run risk of dual transmisiion
//byte BmsA = 0;  // identify which bms is in charge  
//byte BmsB = 0;
//#endif

// battery logic uused to provide single or dual values to the tx frames section of canbus
//byte batteryLogic = 1; //0 = unknown | 1: single pack operation(DEFAULT)| 2: dual operation

// bms state is reported by the bms to indicate on the oled
byte bmsState = 5;   // possibly not required
byte bmsStateA = 5; // 1: stationary 2:charge 3:dishcarge  5: is offline or unknown.startup
byte bmsStateB = 5; // 1: stationary 2:charge 3:dishcarge  5: is offline or unknown.startup
int socDiff = 0;
//battery intital fet control
// this is the pack sync control loop
byte batteryInitialFetControl = 5; // 1: RUN 2: STOP 5:BOOT

//#ifdef BANK_A
byte CanControl = 1; // final control on canbus 1= on 2:stop
byte CanbusInhibit = 1; // remote stop via mqtt of canbus 1:RUN 2:STOP

//#else
//byte CanControl = 2;  //2 = off
//byte CanbusInhibit = 2; // 1:run 2:stopremote stop via mqtt of canbus
//#endif
bool CanStopDipSw = false ; //  false = run | true = stop

/*#ifdef Single_Bank_Only 
float CanControl = 1; // final control on canbus 1= on 2:stop
int CanbusInhibit = 1; // remote stop via mqtt of canbus 1:RUN 2:STOP
#else
float CanControl = 2;  //2 = off
float CanbusInhibit = 2; // 1:run 2:stopremote stop via mqtt of canbus
#endif   
*/
// Vars for this BMS data
bool BattMasterSw = false;
boolean BMSOnline = false;
boolean CANOnline = false;
boolean BMSAlarm = false; // boolean to indicate if bms is in alarm state

//bool battLogicStartSettle = false ; // false = stop| True = can Run
byte canCounter = 0; // used to turn off canonline messgage after going offline

float volts = 0; // first copy from bms
//float voltsDual = 0; // used in dual ops
float amps = 0;
//float ampsDual = 0;
float percentage = 0;
//float percentageDual = 0;

int8_t temp = 0;

int Cycles = 0;
float Capacity = 0;
int bmsupdateerror = 0; // counts up during fail conditions
bool bmsDalyFault = false; // used to identify if spurious results true= yes false = no
byte bmsDalyFaultStop = 0; // to stop a loop preventing the prog from running

float maxCellVoltage = 0;
float minCellVoltage = 0;
uint8_t maxCellNumber = 0;
uint8_t minCellNumber = 0;
int16_t watts = 0;
uint16_t cellImbalance = 0; // in millivolts

//======battery banks values rx from mqtt

//Bank a
byte MosDisA = 0; // 1:on 2: off 0: from bootup
byte MosChgA = 0; // 1:on 2: off 0: from bootup
byte Screen1 = 1; // 1 = on 2:off
float SocA = 0 ;     // 
float VoltsA = 0;    //  
float AmpsA = 0;     //
byte BMSAsts = 5; //captures bms state
byte CANAsts = 5; //captures can state

//bank B
byte MosDisB = 0; // 1:on 2: off 0: from bootup
byte MosChgB = 0; // 1:on 2: off 0: from bootup
byte Screen2 = 1;
float SocB = 0;      // 
float VoltsB = 0;    //
float AmpsB = 0;     //
byte BMSBsts = 5; //captures bms state 5= bootup and not valid
byte CANBsts = 5; //captures can state

// limit control
float ChgAmps1 = 300; // limit the charging amps reduced from 65 as posibly casuing a shutdown
float DisAmps1 = 300; // limit the discharging amps
float ChgVolts1 = 558; // set as 3 digit number this is the max voltage
//float DisVoltLimit = 480 // low voltage cutoff, not in use currently

byte MosDisCtl = 0; //used after define bankA or bank B **** note it returns to zero, so 1 is charge and 2 is discharge***
byte MosChgCtl = 0; //same as above

// used for oled display
String text = "aaaaaaaaaaa";

///////////////////////////////////////////////////////////////////
// these are the gpio in use!
  int ledPins[] = {14, 13, 26, 23, 25, 27, 12, 19};       // an array of pin numbers to which LEDs are attached
  byte timer = 100; //for leds

  int pinCount = 8;           // the number of gpio (i.e. the length of the array)
  byte val = 0; //used in mani loop forled 1
// dip switch inputs
//  int DipSwitchPins[] = {34, 35, 36, 39}; // dipswitch positions, note this 2,3,4,5 (1 is gpio reset cutout)
  int DipSwitchPins[] = {39, 36, 35, 34}; // dipswitch positions, note this 2,3,4,5 (1 is gpio reset cutout)
  byte DipPinCount = 4;

  bool DipSw1 = false; // gpio esp reset
  bool DipSw2 = false; //
  bool DipSw3 = false; //
  bool DipSw4 = false; //
  bool DipSw5 = false; //

// OLED stuff
#include <SPI.h> // added to support 1106 library
#include <Wire.h>
#include <Adafruit_GFX.h>
//#include <Adafruit_SSD1306.h> // original library
//#include <Adafruit_SH110X.h> // working but missing remote commands
#include <Adafruit_SH1106_kbv.h> // latest version to be tested

//#define OLED_RESET 0  // GPIO0 original
#define OLED_RESET -1  // GPIO0
#define i2c_Address 0x3c //initialize with the I2C addr 0x3C Typically eBay OLED's
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); // original
//Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);// sh110x.h library
Adafruit_SH1106_kbv display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); //sh1106 library

//Adafruit_SSD1306 display(OLED_RESET); //original
String oledLine1;
String oledLine2;
String oledLine3;
String oledLine4;
String oledLine5;
String oledLine6;
String oledLine7;
////////////////////////////////////////////////////////

//TimerHandle_t mqttReconnectTimer;
//TimerHandle_t wifiReconnectTimer;
//TimerHandle_t mqttPublishXTimer;
//TimerHandle_t dalyRetryXTimer;


bool WiFiStatus = false;
bool MQTTStatus = false;

uint8_t dalyRequestCounter = 0;

byte MqttRequestCounter = 1; // to decide how often to send individual batt volts

// USER SETUP VALUES:
unsigned long previousMillisUARTCAN = 0;   // will store last time a CAN Message was send
unsigned long previousMillisMqttPublish = 0; //last mqtt tx
unsigned long previousMillisBatteryLogic = 0; // timer to delays batterylogic from running post start
unsigned long currentMillisBatteryLogic = 0;

const int intervalUARTCAN = 1000;          // interval at which send CAN Messages (milliseconds) orig 1000
const int intervalMqttPublish = 10000;     // mqtt publish timer
const int intervalBatteryLogic = 60000;    // time to wait before allowing battery logic to run
unsigned long previousMillisWIFIMQTT = 0;
const int intervalWIFIMQTT = 2000; //2000 orig

//////uptime calcs
  unsigned long currentMillis = 0 ; //millis();
  unsigned long seconds = 0 ; //currentMillis / 1000;
  unsigned long minutes = 0 ; //seconds / 60;
  unsigned long hours = 0 ; //minutes / 60;
  unsigned long days = 0 ; // hours / 24;

/*
  // PYLON GENERAL LITHIUM EMULATION
  // Constructing a test data packet. Little endian, 8 byte messages, 4 halfwords. CAN ID (11bit id 0x351 split into 2 bytes, little endian) for the CAN send function, to reduce manual labour.
  byte CANData1[]=   {0x03, 0x51,  // CAN ID (11bit id 0x351 split into 2 bytes, little endian). Battery charge voltage, DC charge current limitation, DC discharge currentlimitation, discharge voltage
                    0x14, 0x02,   // Battery charge voltage - U16 - 0.1 scale. 41-63. Default 60 (560)
                    0x74, 0x0E,   // DC charge current limitation - S16 - 0.1 scale. 0-1200. Default 99.9 (999)
                    0x74, 0x0E,   // DC discharge current limitation - S16 - 0.1 scale. 0-1200. Test: 99.9 (999)
                    0xCC, 0x01};  // Voltage discharge limit - U16 - 0.1 scale. 41-48. Default 41.5 (415)

  byte CANData2[]=   {0x03, 0x55,  // CAN ID (11bit id 0x355 split into 2 bytes, little endian).. SOC value, SOH value, HiResSOC
                    0x39, 0x00,    // State of Charge (SOC) - U16 - 1:1 scale. 0-100. (Should be 57%)
                    0x64, 0x00};   // State of Health (SOH) - U16 - 1:1 scale. 0-100. (Should be 99%)

  byte CANData3[]=   {0x03,0x56,   // CAN ID (11bit id 0x356 split into 2 bytes, little endian). Battery Voltage, Battery Current, Battery Temperature
                    0x4E, 0x13,   // Battery Voltage - S16 - 0.01 scale. (Should be 52.13V - 5213)
                    0x02, 0x03,   // Battery Current - S16 - 0.01 scale. (Should be 7.31A - 731)
                    0x04, 0x05};   // Battery Temperature - S16 - 0.01 scale. (Should be 17.43c - 1743)

  byte CANData4[]=   {0x03, 0x59,  // CAN ID (11bit id 0x359 split into 2 bytes, little endian). Alarms Warnings (See table in the PDF)
                    0x00, 0x00,     // See table
                    0x00, 0x00,     // See table
                    0x0A, 0x50,     // See table
                    0x4E};    // See table

  byte CANData5[]=   {0x03, 0x5C,  // CAN ID (11bit id 0x35C split into 2 bytes, little endian). Events. Not sure if only 2 bytes or full 8
                    0xC0, 0x00};     // See table

  byte CANData6[]=   {0x03, 0x5E,  // CAN ID (11bit id 0x35E split into 2 bytes, little endian). Manufacturer name - ASCII
                    0x50, 0x59,   // See table.
                    0x4C, 0x4F,    // See table.
                    0x4E, 0x20,     // See table.
                    0x20, 0x20};    // See table.
*/
/* my notes on values
charging voltage 3.45 x 16= 55.2 : 0x28 0x02
3.5 x 16 =56 = 560 :0x30 0x02
3.48 x 16 55.7 -557 ; 0x2D 0x02
voltage discharge limit 2.8 x 16= 44.8v ; 0xC0 0x01
volatge discharge 0x28, 0x02    // modifed 44.8 (448)2.8v per cell
*/
/*
// SMA GENERAL LITHIUM EMULATION
// Constructing a test data packet. Little endian, 8 byte messages, 4 halfwords. CAN ID (11bit id 0x351 split into 2 bytes, little endian) for the CAN send function, to reduce manual labour.
//****original byte**
byte CANData351[] =   {0x03, 0x51,   // CAN ID (11bit id 0x351 split into 2 bytes, little endian). Battery charge voltage, DC charge current limitation, DC discharge currentlimitation, discharge voltage
                       0x27, 0x02,   // Battery charge voltage - U16 - 0.1 scale. 41-63. Default 55.2 (551)
                       0xE7, 0x03,   // DC charge current limitation - S16 - 0.1 scale. 0-1200. Default 99.9 (999)
                       0xBC, 0x02,   // DC discharge current limitation - S16 - 0.1 scale. 0-1200. Test: 70 (700)
                       0x9F, 0x01    // Voltage discharge limit - U16 - 0.1 scale. 41-48. Default 41.5 (415)
                      };  
*/
byte CANData351[] =   {0x03, 0x51,   // CAN ID (11bit id 0x351 split into 2 bytes, little endian). Battery charge voltage, DC charge current limitation, DC discharge currentlimitation, discharge voltage
                       0x2D, 0x02,   // 55.7 0x2D 0x02 ** this is updated from within update tx frames
                       0x8A, 0x02,   // DC charge current limitation - 65 amps (650) **fixed here for bootup
                       0x8A, 0x02,   // DC discharge current limitation - 65 amps (650) **fixed here
                       0xE0, 0x01    // modifed 48.0v (448)3v per cell  ** fixed here
                      }; 
                      
byte CANData355[] =   {0x03, 0x55,  // CAN ID (11bit id 0x355 split into 2 bytes, little endian).. SOC value, SOH value, HiResSOC
                       0x39, 0x00,   // State of Charge (SOC) - U16 - 1:1 scale. 0-100. (Should be 57%)
                       0x63, 0x00,   // State of Health (SOH) - U16 - 1:1 scale. 0-100. (Should be 99%)
                       0xCC, 0x26,   // High resolution SOC value - U16 - 0.01 scale. 0-100 (Should be 99.32%)
                       0x00, 0x00
                      };  // nothing
//356 feeds bms data back into inverter,its optional
byte CANData356[] =   {0x03, 0x56,  // CAN ID (11bit id 0x356 split into 2 bytes, little endian). Battery Voltage, Battery Current, Battery Temperature
                       0x5D, 0x14,   // Battery Voltage - S16 - 0.01 scale. (Should be 52.13V - 5213)
                       0x00, 0x00,   // Battery Current - S16 - 0.01 scale. (Should be 7.31A - 731)
                       0xCF, 0x06,   // Battery Temperature - S16 - 0.01 scale. (Should be 17.43c - 1743)
                       0x00, 0x00
                      };  // nothing
//35A error alarms fed back into inverter to shutdown
byte CANData35A[] =   {0x03, 0x5A,  // CAN ID (11bit id 0x35A split into 2 bytes, little endian). Alarms Warnings (See table in the PDF)
                       0x00, 0x00,   // See table
                       0x00, 0x00,   // See table
                       0x00, 0x00,   // See table
                       0x00, 0x00
                      };  // See table

byte CANData35B[] =   {0x03, 0x5B,  // CAN ID (11bit id 0x35B split into 2 bytes, little endian). Events. Not sure if only 2 bytes or full 8
                       0x00, 0x00,   // See table
                       0x00, 0x00,   // See table
                       0x00, 0x00,   // See table
                       0x00, 0x00
                      };  // See table

byte CANData35E[] =   {0x03, 0x5E,  // CAN ID (11bit id 0x35E split into 2 bytes, little endian). Manufacturer name - ASCII
                       0x53, 0x4D,   // See table. S M
                       0x41, 0x00,   // See table. A
                       0x00, 0x00,   // See table
                       0x00, 0x00
                      };  // See table

//byte CANData35F[] =   {0x03, 0x5F,  // CAN ID (11bit id 0x35F split into 2 bytes, little endian). Bat-Type BMS Version Bat-Capacity reserved Manufacturer ID
//                       0x00, 0x00,   // "Bat-Type". No more known
//                       0x00, 0x00,   // "BMS Version". No more known
//                       0xC8, 0x00,   // "Bat capacity". Need to figure out encoding. Currently shows as 51200 
//                       0x00, 0x00
//                      };  // "reserved Manufacturer ID". No more known.
byte CANData35F[] =   {0x03, 0x5F,  // CAN ID (11bit id 0x35F split into 2 bytes, little endian). Bat-Type BMS Version Bat-Capacity reserved Manufacturer ID
                       0x00, 0x00,   // "Bat-Type". No more known
                       0x00, 0x00,   // "BMS Version". No more known
                       0x11, 0x08,   // "Bat capacity". 280ah
                       0x00, 0x00
                      };  // "reserved Manufacturer ID". No more known.
// my new canbus frames
// enables bms to comunicate with each other
// this bms
//need to work out how to decide on priority
byte CANData400[] =   {0x04, 0x00,  // CAN ID (11bit id 0x35F split into 2 bytes, little endian). 
                       0x00, 0x00,   // 
                       0x00, 0x00,   // 
                       0x00, 0x00,   // 
                       0x00, 0x00
                      };  
// opposite bms
byte CANData401[] =   {0x04, 0x00,  // CAN ID (11bit id 0x35F split into 2 bytes, little endian). 
                       0x00, 0x00,   // 
                       0x00, 0x00,   // 
                       0x00, 0x00,   // 
                       0x00, 0x00
                      };


// 'daly-on', 29x13px
const unsigned char dalyOn [] PROGMEM = {
  0xff, 0xff, 0xff, 0xf8, 0x80, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x08, 0x9e, 0x39, 0x04, 0x48,
  0x91, 0x45, 0x02, 0x88, 0x91, 0x7d, 0x01, 0x08, 0x91, 0x45, 0x01, 0x08, 0x91, 0x45, 0x01, 0x08,
  0x91, 0x45, 0x01, 0x08, 0x9e, 0x45, 0xf1, 0x08, 0x80, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x08,
  0xff, 0xff, 0xff, 0xf8
};
// 'daly-off', 29x13px
const unsigned char dalyOff [] PROGMEM = {
  0xff, 0xff, 0xff, 0xf8, 0xb0, 0x00, 0x00, 0x08, 0x8c, 0x00, 0x00, 0x08, 0x9f, 0xb9, 0x04, 0x48,
  0x91, 0x65, 0x02, 0x88, 0x91, 0x7d, 0x01, 0x08, 0x91, 0x47, 0x01, 0x08, 0x91, 0x45, 0xc1, 0x08,
  0x91, 0x45, 0x31, 0x08, 0x9e, 0x45, 0xff, 0x08, 0x80, 0x00, 0x01, 0x88, 0x80, 0x00, 0x00, 0x68,
  0xff, 0xff, 0xff, 0xf8
};
// 'can-on', 29x13px
const unsigned char canOn [] PROGMEM = {
  0xff, 0xff, 0xff, 0xf8, 0x80, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x08, 0x81, 0xc7, 0x22, 0x08,
  0x82, 0x28, 0xb2, 0x08, 0x82, 0x0f, 0xaa, 0x08, 0x82, 0x08, 0xa6, 0x08, 0x82, 0x08, 0xa2, 0x08,
  0x82, 0x28, 0xa2, 0x08, 0x81, 0xc8, 0xa2, 0x08, 0x80, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x08,
  0xff, 0xff, 0xff, 0xf8
};
// 'can-off', 29x13px
const unsigned char canOff [] PROGMEM = {
  0xff, 0xff, 0xff, 0xf8, 0xb0, 0x00, 0x00, 0x08, 0x8c, 0x00, 0x00, 0x08, 0x83, 0xc7, 0x22, 0x08,
  0x82, 0x68, 0xb2, 0x08, 0x82, 0x1f, 0xaa, 0x08, 0x82, 0x0f, 0xa6, 0x08, 0x82, 0x08, 0xe2, 0x08,
  0x82, 0x28, 0xb2, 0x08, 0x81, 0xc8, 0xae, 0x08, 0x80, 0x00, 0x01, 0x88, 0x80, 0x00, 0x00, 0x68,
  0xff, 0xff, 0xff, 0xf8
};
// 'wifi-on', 29x13px
const unsigned char wifiOn [] PROGMEM = {
  0xff, 0xff, 0xff, 0xf8, 0x80, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x08, 0x84, 0x49, 0xf2, 0x08,
  0x84, 0x41, 0x00, 0x08, 0x84, 0x49, 0xc2, 0x08, 0x84, 0x49, 0x02, 0x08, 0x85, 0x49, 0x02, 0x08,
  0x86, 0xc9, 0x02, 0x08, 0x84, 0x49, 0x02, 0x08, 0x80, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x08,
  0xff, 0xff, 0xff, 0xf8
};
// 'wifi-off', 29x13px
const unsigned char wifiOff [] PROGMEM = {
  0xff, 0xff, 0xff, 0xf8, 0xb0, 0x00, 0x00, 0x08, 0x8c, 0x00, 0x00, 0x08, 0x87, 0xc9, 0xf2, 0x08,
  0x84, 0x61, 0x00, 0x08, 0x84, 0x59, 0xc2, 0x08, 0x84, 0x4f, 0x02, 0x08, 0x85, 0x49, 0xc2, 0x08,
  0x86, 0xc9, 0x32, 0x08, 0x84, 0x49, 0x0e, 0x08, 0x80, 0x00, 0x01, 0x88, 0x80, 0x00, 0x00, 0x68,
  0xff, 0xff, 0xff, 0xf8
};
// 'mqtt-on', 29x13px
const unsigned char mqttOn [] PROGMEM = {
  0xff, 0xff, 0xff, 0xf8, 0x80, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x08, 0x91, 0x39, 0xf7, 0xc8,
  0x9b, 0x44, 0x41, 0x08, 0x95, 0x44, 0x41, 0x08, 0x91, 0x44, 0x41, 0x08, 0x91, 0x44, 0x41, 0x08,
  0x91, 0x48, 0x41, 0x08, 0x91, 0x34, 0x41, 0x08, 0x80, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x08,
  0xff, 0xff, 0xff, 0xf8
};
// 'mqtt-off', 29x13px
const unsigned char mqttOff [] PROGMEM = {
  0xff, 0xff, 0xff, 0xf8, 0xb0, 0x00, 0x00, 0x08, 0x8c, 0x00, 0x00, 0x08, 0x93, 0xb9, 0xf7, 0xc8,
  0x9b, 0x64, 0x41, 0x08, 0x95, 0x5c, 0x41, 0x08, 0x91, 0x47, 0x41, 0x08, 0x91, 0x44, 0xc1, 0x08,
  0x91, 0x48, 0x71, 0x08, 0x91, 0x34, 0x4f, 0x08, 0x80, 0x00, 0x01, 0x88, 0x80, 0x00, 0x00, 0x68,
  0xff, 0xff, 0xff, 0xf8
};

// new for charge and discharge

// 'Chg_on', 29x13px
const unsigned char chargeOn [] PROGMEM = {
  0xff, 0xff, 0xff, 0xf8, 0x80, 0x00, 0x00, 0x08, 0xbf, 0xff, 0xff, 0xe8, 0xbf, 0xff, 0xff, 0xe8,
  0xbf, 0x36, 0xcf, 0xe8, 0xbe, 0xd6, 0xbf, 0xe8, 0xbe, 0xf0, 0xa7, 0xe8, 0xbe, 0xd6, 0xb7, 0xe8,
  0xbf, 0x36, 0xcf, 0xe8, 0xbf, 0xff, 0xff, 0xe8, 0xbf, 0xff, 0xff, 0xe8, 0x80, 0x00, 0x00, 0x08,
  0xff, 0xff, 0xff, 0xf8
};

// 'chg_off', 29x13px
const unsigned char chargeOff [] PROGMEM = {
  0xff, 0xff, 0xff, 0xf8, 0x80, 0x00, 0x00, 0x08, 0xbf, 0xff, 0xfe, 0xa8, 0xbf, 0xff, 0xfe, 0x28,
  0xbf, 0x36, 0xce, 0xa8, 0xbe, 0xd6, 0xbe, 0x28, 0xbe, 0xf0, 0xa6, 0xa8, 0xbe, 0xd6, 0xb6, 0x28,
  0xbf, 0x36, 0xce, 0xa8, 0xbf, 0xff, 0xfe, 0x28, 0xbf, 0xff, 0xfe, 0xa8, 0x80, 0x00, 0x00, 0x08,
  0xff, 0xff, 0xff, 0xf8
};
  
// 'dis_off', 29x13px
const unsigned char disOff [] PROGMEM = {
  0xff, 0xff, 0xff, 0xf8, 0x80, 0x00, 0x00, 0x08, 0xbf, 0xff, 0xfe, 0xa8, 0xbf, 0xff, 0xfe, 0x28,
  0xbe, 0x31, 0x8e, 0xa8, 0xbe, 0xdb, 0x7e, 0x28, 0xbe, 0xdb, 0x9e, 0xa8, 0xbe, 0xdb, 0xee, 0x28,
  0xbe, 0x31, 0x1e, 0xa8, 0xbf, 0xff, 0xfe, 0x28, 0xbf, 0xff, 0xfe, 0xa8, 0x80, 0x00, 0x00, 0x08,
  0xff, 0xff, 0xff, 0xf8
};

// 'dis_on', 29x13px
const unsigned char disOn [] PROGMEM = {
  0xff, 0xff, 0xff, 0xf8, 0x80, 0x00, 0x00, 0x08, 0xbf, 0xff, 0xff, 0xe8, 0xbf, 0xff, 0xff, 0xe8,
  0xbe, 0x31, 0x8f, 0xe8, 0xbe, 0xdb, 0x7f, 0xe8, 0xbe, 0xdb, 0x9f, 0xe8, 0xbe, 0xdb, 0xef, 0xe8,
  0xbe, 0x31, 0x1f, 0xe8, 0xbf, 0xff, 0xff, 0xe8, 0xbf, 0xff, 0xff, 0xe8, 0x80, 0x00, 0x00, 0x08,
  0xff, 0xff, 0xff, 0xf8
};

// 'canwait', 29x13px
const unsigned char canwait [] PROGMEM = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x99, 0xdf, 0x00,
0x08, 0xa4, 0x84, 0x00, 0x0a, 0xbc, 0x84, 0x00, 0x0d, 0xa4, 0x84, 0x00, 0x08, 0xa5, 0xc4, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00
};
// 'cantx', 29x13px
const unsigned char cantx [] PROGMEM = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x31, 0x92, 0x3e, 0x90,
0x4a, 0x5a, 0x08, 0x90, 0x43, 0xd6, 0x08, 0x60, 0x4a, 0x52, 0x08, 0x90, 0x32, 0x52, 0x08, 0x90,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00
};
// 'slave', 29x13px
const unsigned char slave [] PROGMEM = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x0e, 0x83, 0x22, 0xf0, 0x10, 0x84, 0xa2, 0x80, 0x0c, 0x87, 0x94, 0xe0, 0x02, 0x84, 0x94, 0x80,
0x1c, 0xf4, 0x88, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00
};
// 'canrx', 29x13px
const unsigned char canrx [] PROGMEM = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0xc9, 0x1c, 0x90,
0x25, 0x2d, 0x12, 0x90, 0x21, 0xeb, 0x1c, 0x60, 0x25, 0x29, 0x14, 0x90, 0x19, 0x29, 0x12, 0x90,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00
};
// 'canrun', 29x13px
const unsigned char canrun [] PROGMEM = {
0xff, 0xff, 0xff, 0xf8, 0x80, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x08,
0x81, 0xc9, 0x48, 0x08, 0x81, 0x29, 0x68, 0x08, 0x81, 0xc9, 0x58, 0x08, 0x81, 0x49, 0x48, 0x08,
0x81, 0x26, 0x48, 0x08, 0x80, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x08,
0xff, 0xff, 0xff, 0xf8
};

// 'canstop', 29x13px#
const unsigned char canstop [] PROGMEM = {
0xff, 0xff, 0xff, 0xf8, 0x80, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x08,
0x83, 0xbe, 0x67, 0x08, 0x84, 0x08, 0x94, 0x88, 0x83, 0x08, 0x97, 0x08, 0x80, 0x88, 0x94, 0x08,
0x87, 0x08, 0x64, 0x08, 0x80, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x08,
0xff, 0xff, 0xff, 0xf8
};
// 'master', 29x13px
const unsigned char master [] PROGMEM = {
0xff, 0xff, 0xff, 0xf8, 0xff, 0xff, 0xff, 0xf8, 0xff, 0xff, 0xff, 0xf8, 0xff, 0xff, 0xff, 0xf8,
0xf7, 0x67, 0x10, 0x78, 0xf2, 0x5a, 0xfd, 0xf8, 0xf5, 0x43, 0x3d, 0xf8, 0xf7, 0x5b, 0xdd, 0xf8,
0xf7, 0x5a, 0x3d, 0xf8, 0xff, 0xff, 0xff, 0xf8, 0xff, 0xff, 0xff, 0xf8, 0xff, 0xff, 0xff, 0xf8,
0xff, 0xff, 0xff, 0xf8
};

// 'dual', 29x13px
const unsigned char dual [] PROGMEM = {
0xff, 0xff, 0xff, 0xf8, 0x80, 0x00, 0x00, 0x08, 0xbf, 0xff, 0xff, 0xe8, 0xbf, 0xff, 0xff, 0xe8,
0xb8, 0xdb, 0x37, 0xe8, 0xbb, 0x5a, 0xd7, 0xe8, 0xbb, 0x5a, 0x17, 0xe8, 0xbb, 0x5a, 0xd7, 0xe8,
0xb8, 0xe6, 0xd0, 0xe8, 0xbf, 0xff, 0xff, 0xe8, 0xbf, 0xff, 0xff, 0xe8, 0x80, 0x00, 0x00, 0x08,
0xff, 0xff, 0xff, 0xf8
};
// 'isol', 29x13px
const unsigned char isol [] PROGMEM = {
0xff, 0xff, 0xff, 0xf8, 0x80, 0x00, 0x00, 0x08, 0xbf, 0xff, 0xff, 0xe8, 0xbf, 0xff, 0xff, 0xe8,
0xbf, 0xff, 0xff, 0xe8, 0xbf, 0xff, 0xff, 0xe8, 0xbf, 0xff, 0xff, 0xe8, 0xbf, 0xff, 0xff, 0xe8,
0xbf, 0xff, 0xff, 0xe8, 0xbf, 0xff, 0xff, 0xe8, 0xbf, 0xff, 0xff, 0xe8, 0x80, 0x00, 0x00, 0x08,
0xff, 0xff, 0xff, 0xf8
};
// 'single', 29x13px
const unsigned char single [] PROGMEM = {
0x7f, 0xff, 0xff, 0xf0, 0x40, 0x00, 0x00, 0x10, 0x40, 0x00, 0x00, 0x10, 0x43, 0xba, 0x4c, 0x10,
0x44, 0x13, 0x50, 0x10, 0x43, 0x12, 0xd6, 0x10, 0x40, 0x92, 0x52, 0x10, 0x47, 0x3a, 0x4c, 0x10,
0x40, 0x00, 0x00, 0x10, 0x40, 0x00, 0x00, 0x10, 0x40, 0x00, 0x00, 0x10, 0x7f, 0xff, 0xff, 0xf0,
0x00, 0x00, 0x00, 0x00
};

// 'boot', 29x13px
const unsigned char boot [] PROGMEM = {
0xff, 0xff, 0xff, 0xf8, 0x80, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x08,
0x8e, 0x31, 0x9f, 0x08, 0x89, 0x4a, 0x44, 0x08, 0x8e, 0x4a, 0x44, 0x08, 0x89, 0x4a, 0x44, 0x08,
0x8e, 0x31, 0x84, 0x08, 0x80, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x08,
0xff, 0xff, 0xff, 0xf8
};

// 'bank A', 29x13px
const unsigned char bnkA [] PROGMEM = {
0xff, 0xff, 0xff, 0xf8, 0x80, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x08,
0x80, 0x06, 0x00, 0x08, 0x80, 0x09, 0x00, 0x08, 0x80, 0x0f, 0x00, 0x08, 0x80, 0x09, 0x00, 0x08,
0x80, 0x09, 0x00, 0x08, 0x80, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x08,
0xff, 0xff, 0xff, 0xf8
};

// 'bank B', 29x13px
const unsigned char bnkB [] PROGMEM = {
0xff, 0xff, 0xff, 0xf8, 0x80, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x08,
0x80, 0x0e, 0x00, 0x08, 0x80, 0x09, 0x00, 0x08, 0x80, 0x0e, 0x00, 0x08, 0x80, 0x09, 0x00, 0x08,
0x80, 0x0e, 0x00, 0x08, 0x80, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x08,
0xff, 0xff, 0xff, 0xf8
};

//==============================================================================================
void setup() {
  //rtc_wdt_protect_off();
  //rtc_wdt_disable();

  // gpio esp reset
   pinMode(15, OUTPUT);    // sets the digital pin 15 as output
   digitalWrite(15, HIGH); // hold this high to prevent a stuck in reset loop

  Serial.begin(115200);
  Serial.println("Daly2Sofar-UARTtoCANBUS"  + String (versionNumber));
  //eolterm();
#ifdef BANK_A
     Serial.println("BANK A CONFIGURATION");
#else
     Serial.println("BANK B CONFIGURATION");
#endif
     Serial.println("CanBussPrio:" + String (CanBussPrio));
//#ifdef Single_Bank_Only
//     Serial.println("*** Single Bank configuration***");
//     Serial.println("*** canbus will be running***");
//     Serial.println("*** mosfets will be on!***");
//#endif
  // array to set up gpio for led indicator
   for (int thisPin = 0; thisPin < pinCount; thisPin++) {
        pinMode(ledPins[thisPin], OUTPUT);
        digitalWrite(ledPins[thisPin], HIGH);
      }
   ledcontrol(); // test leds count up and down

// sets the gpio input pins for dipswitch
   for (int thisPin1 = 0; thisPin1 < DipPinCount; thisPin1++) {
        pinMode(DipSwitchPins[thisPin1], INPUT);
       // digitalWrite(ledPins[thisPin], HIGH);
      }
      pinMode(32, INPUT); // set the batt master is input
      gpioInputread();
  // Set CAN pins and baudrate
  
  CAN_cfg.speed = CAN_SPEED_500KBPS;
  CAN_cfg.tx_pin_id = GPIO_NUM_5;
  CAN_cfg.rx_pin_id = GPIO_NUM_4;

  // Create a queue for CAN receiving */
  CAN_cfg.rx_queue = xQueueCreate(rx_queue_size, sizeof(CAN_frame_t));

  // Initialize CAN Module
  ESP32Can.CANInit();

  // Initialise the Daly comms
  bms.Init();

#ifdef BANK_A
   // TURN ON MOSFETS
#endif

/*#ifdef Single_Bank_Only
// #ifdef BANK_B
   ChgFetOn();
   DisFetOn();
  // #endif
#else 
   //TURN OFF ALL MOSFETS until safe to do so and checked soc
  // bms.setChargeMOS(false);
   ChgFetOff();
   DisFetOff();
  // bms.setDischargeMOS(false); //turn off mosfet
#endif
*/   
  Wire.begin();   // sda= GPIO_21 /scl= GPIO_22
  // for 1306 display
  //display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize OLED with the I2C addr 0x3C (for the 64x48) current
  
  //  display.begin(SSD1306_SWITCHCAPVCC, 0x7A);  // initialize OLED with the I2C addr 0x3C (for the 64x48)
  //  display.begin(SSD1306_SWITCHCAPVCC, 0x78);  // initialize OLED with the I2C addr 0x3C (for the 64x48)
  //display.clearDisplay();
  //display.display();
  // for 1106
  //display.begin(i2c_Address, true); // Address 0x3C default //110x library
  display.begin(SH1106_SWITCHCAPVCC, 0x3C); //to turn the 1106 display on hopefully
  display.display();
  display.clearDisplay();

  //  oledMessage("Hi", "Ola!", "Hello", "Labas", "Zdravstvuyte"); //line 1,2,3,4,5
  //oledMessage("Daly2sofar", "version", "29 july 22", "booting", "", "");
  oledMessage(String (deviceName), "Version=" + String(versionNumber), "Date=" + String(versionDate), "booting Up", "Please Wait", "");
     

  //  mqttReconnectTimer = xTimerCreate("mqttTimer", pdMS_TO_TICKS(5000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToMqtt));
  //  wifiReconnectTimer = xTimerCreate("wifiTimer", pdMS_TO_TICKS(2000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(connectToWifi));
  //  mqttPublishXTimer = xTimerCreate("mqttPubTimer", pdMS_TO_TICKS(5000), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(sendMQTTData));
  //  dalyRetryXTimer = xTimerCreate("dalyRetryTimer", pdMS_TO_TICKS(200), pdFALSE, (void*)0, reinterpret_cast<TimerCallbackFunction_t>(dalyRetry));


  WiFi.onEvent(WiFiEvent);

  mqttClient.setCredentials(mqttUser, mqttPassword);
  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);

  connectToWifi();
  Serial.println("MQTT output deviceName:"  +String(deviceName));
  Serial.println("MQTT Input deviceName:"  +String(deviceName1));

  #ifdef OTA
  setupOTA();
  #endif
  eolterm();
  Serial.println("=======================END of startup===========================");
}


void loop() {
  //Serial.println ("Main loop start of run");
   gpioInputread(); // read the gpio pins
   runTimeUpdate(); // update the runtime
 //  val = digitalRead(ledPins[0]); //led 1
//   val2 = digitalRead(ledPins[1]); //led 2
   val++;
  if (val >= 50){
    digitalWrite(ledPins[0], LOW); //turn on 1st LED
    digitalWrite(ledPins[1], HIGH); //turn oFF 2st LED
    if (val >= 100){
        val = 0;
        }
    }
    else{
    digitalWrite(ledPins[0], HIGH); //TURN OFF
    digitalWrite(ledPins[1], LOW); // TURN ON
    //val = 0;
  }
  //checkForRX(); //added to check for can rx
 // BatteryInitialFetControl(); // initial battery connection
 if(bmsDalyFault){
               if(bmsDalyFaultStop <=10){
               delay(200);// let bms settle
               getEverythingFromDaly();// drives another pull of data to try and fix results
               }
               else{
                // do nothing
               }
              }
  unsigned long currentMillisMqttPublish = millis();

      if (currentMillisMqttPublish - previousMillisMqttPublish >= intervalMqttPublish) {  
           
            previousMillisMqttPublish = currentMillisMqttPublish;

            getEverythingFromDaly(); // normal call
           
            mqttMessSelect (); // both mqtt messages with cells every 10 messages
            //temp added here for debugging
            //updateTXFrames(); //
                 }

  unsigned long currentMillisUARTCAN = millis();
      
      if (currentMillisUARTCAN - previousMillisUARTCAN >= intervalUARTCAN) {
          previousMillisUARTCAN = currentMillisUARTCAN;
           
             canCounter++; // increment the can counter
             if (canCounter >=5){
              CANOnline = false; // if greater than 5 set to off
            }

            
            if (BMSOnline) { 
               // Serial.println ("inside can loop");
               // moved for testing batterylogic(); // this decidies on whcih uodate tx frames to suse
               updateTXFrames(); // copy data to the frame, but not sent
        
              // suspended as wont work without can bus connection  
              
               Serial.println(" Can frames are about to be sent");
               
               if (CanControl == 1)// final software control to send can frames
               {
                if (CanStopDipSw == false){ // final hardware stop true will block
               digitalWrite(ledPins[7], LOW); //turn on 8th LED
               sendCanTXFrame(CANData351, 8); // 0x0351 - 8 bytes
               sendCanTXFrame(CANData355, 6); // 0x0355 - 6 bytes
               sendCanTXFrame(CANData356, 6); // 0x0356 - 6 bytes
               sendCanTXFrame(CANData35A, 8); // 0x035A - 8 bytes
               sendCanTXFrame(CANData35B, 2); // 0x035B - 2 bytes
               sendCanTXFrame(CANData35E, 8); // 0x035E - 8 bytes
               sendCanTXFrame(CANData35F, 8); // 0x035F - 8 bytes
               digitalWrite(ledPins[7], HIGH) ;//turn Off 7th LED 
               canCounter = 1; //reset can fault counter              
               }
               }
              }
              
              }
          //  if (val == HIGH){
          //      digitalWrite(ledPins[0], LOW); //turn on 1st LED
          //     }
          //   else{  
          //    digitalWrite(ledPins[0], HIGH); //turn off 1st LED
          //       }
     // Serial.println("=======================END of Main Loop===========================");
}
