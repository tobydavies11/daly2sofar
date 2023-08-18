//
//Gpio

//    3.3 | GND       |SVP | SUN
//    TXD | EN        |IO34 |IO35
//    RXD | IO23      |IO32 | IO33
//    GND | IO22      |IO25  |IO26
//    I021| IO19      |IO27  |IO14
//    IO18| IO5       |IO12  |IO13
//    IO17| IO16      |SD2   |SD3
//    IO4 | IOO       |CMD   |CLK
//    IO2 | IO15      |SD0   |SDI

// IN USE
//    OUTER           INNER                   INNER      OUTER
//    3.3 = POWER     | GND            =     | SVP = dp 36   | SvN = dp 39     GPIO 36 &39 **THESE ARE INPUTS ONLY AND HAVE ADC AKA 
//    TXD = FTDI      | EN = NOTE**    =     | IO34 = dp    | IO35 = dp
//    RXD = FTDI      | IO23 = LED 4    =    | IO32 = NC     | IO33 = batt master
//    GND = GRD       | IO22 = SCK     =     | IO25 = LED 5  | IO26 = LED 3
//    I021 = SDA      | IO19 = LED 8   =     | IO27 = LED 6  | IO14 = LED 1
//    IO18 = NC       | IO5 = CAN TX   =     | IO12 = LED 7  | IO13 = LED 2
//    IO17 = TX BMS   | IO16 = BMS     =     | SD2  = NC     | SD3 = NC **DONT USE
//    IO4  = CAN RX   | IOO = NC       =     | CMD  = NC     | CLK = NC ** DONT USE
//    IO2  = NC       | IO15 = reset   =     | SD0  = NC     | SDI = NC ** DONT USE
// ENABLE IS PULLED TO VCC,SWITCH TO GROUND PERFORMS RESET

//  int ledPins[] = {14, 13, 26, 23, 25, 27, 12, 19};
// DIP switches
//  1 = RESET DISABLE  
//  2   
//  3  
//  4 
//  5

//SPARE GPIO
// 18 = INPUT OR OUTPUT
// 2  = AVOID BUT CAN BE USED
// 0  = OUTPUT ONLY
// 15 = INPUT OR OUTPUT ===  gpio reset
// 32 = INPUT OR OUTPUT
// 33 = INPUT OR OUTPUT = batt master
// 34 = ADC INPUT ONLY = dipswitch
// 35 = ADC INPUT ONLY = dipswitch
// 36 = ADC INPUT ONLY = dipswitch
// 39 = ADC INPUT ONLY = dipswitch
