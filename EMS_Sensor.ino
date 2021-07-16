#include <Arduino.h>
#include <stdint.h>
#include <stdbool.h>
#include "SPI.h"
#include "Wire.h"
#include "Linduino.h"
#include "LT_SPI.h"
#include "stdio.h"
#include "math.h"
#include <EEPROM.h>


#include "configuration_constants_LTC2983.h"
#include "support_functions_LTC2983.h"
#include "table_coeffs_LTC2983.h"

#include <mcp_can.h>

unsigned char len = 0;
unsigned char buf[8];
unsigned char ext = 0;

//EMS Sensor Board configuration - 4 or 6 cylinders engine
const int CYL = 4;

const int CAN_CS_PIN = 10;   //CS pin for CAN board
const int TCS_CS_PIN = 9;    //CS pin for Thermo-Couple Sensor chip

unsigned long LoopCounter = 0;
unsigned long LoopTimer = 0;

unsigned long canId;

// Timing variables and constants

unsigned long SystemTime = 0; //System time UTC in Unix format (seconds since Jan 01 1970). Data received from CAN MsgId = 25
unsigned long SystemTimeLocalMillis = 0;

unsigned long Counter0;
unsigned long Counter1;

unsigned long EngineTimestamp = 0;
unsigned long EngineTimerTMP = 0; //temporary variable
unsigned long EngineTimer = 0; //timstemp for Hobbs time recording
unsigned int RPM = 0;
unsigned int PPR = 1; // Pulses per revolution. 

unsigned long FlowCounter = 0;
unsigned long FlowTimestamp = 0;
unsigned long LastPulseTimestamp_Flow = 0;
unsigned int FuelFlow = 0;

// Moving Average array for fuel flow calculation
#define FlowArraySize 10 // averaging accross last 10 values
long FlowCountArray[FlowArraySize];
long FlowTimeArray[FlowArraySize];
int FlowArrayIndex = 0;
long FlowCountSum =0;
long FlowTimeSum =0;



boolean EngineRunning = false;
// END of timing section

//CAN Message details

unsigned char canMsg[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

//RPM
const unsigned int CAN_RPM_Msg_ID = 50; // RPM CAN Msg ID in DEC
const unsigned int CAN_RPM_Period = 500; // How often message sent in milliseconds
unsigned long CAN_RPM_Timestamp = 0; // when was the last message sent
unsigned long LastPulseTimestamp_RPM = 0;
unsigned int FuelPressure;   // Fuel pressure value in millibars 


//Fuel Tanks levels
const unsigned int CAN_FT_Msg_ID = 80; // CAN Msg ID in DEC
const unsigned int CAN_FT_Period = 1000; // How often message sent in milliseconds
unsigned long CAN_FT_Timestamp = 0; // when was the last message sent
unsigned int TankLevel1 = 0;

// Fuel pressure
#define FuelPressureArraySize 20 // averaging accross last 20 values
unsigned int FuelPressureArray[FuelPressureArraySize];
unsigned int FuelPressureArrayIndex = 0;
unsigned int FuelPressureSum = 0;

//Oil temperatures and pressure
const unsigned int CAN_OIL_Msg_ID = 81; // CAN Msg ID in DEC 
const unsigned int CAN_OIL_Period = 600; // How often message sent in milliseconds
unsigned long CAN_OIL_Timestamp = 0; // when was the last message sent
int OIL_Pressure = 0;
int OIL_Temperature = 0;

#define OilPressureArraySize 40 // averaging accross last 40 values
unsigned int OilPressureArray[OilPressureArraySize];
unsigned int OilPressureArrayIndex = 0;
unsigned int OilPressureSum = 0;

//Head and exhaust temperatures
const unsigned int CAN_TMP_Msg_ID = 82; // CAN Msg ID in DEC ************* THE FIRST OF THREE **************************
const unsigned int CAN_TMP_Period = 300; // How often message sent in milliseconds
unsigned long CAN_TMP_Timestamp = 0; // when was the last message sent
unsigned int tmp_TMP = 0;

unsigned int EGT[7] = {0,0,0,0,0,0,0}; // first two bytes will be wasted for the sake of convenience and clarity :)
unsigned int CHT[7] = {0,0,0,0,0,0,0};

float TCS_Result = 0;
unsigned int TCS_Error = 1;
unsigned int TCS_Channel = 1;


//Volts and Amps
const unsigned int CAN_EL_Msg_ID = 85; // CAN Msg ID in DEC 
const unsigned int CAN_EL_Period = 300; // How often message sent in milliseconds
unsigned long CAN_EL_Timestamp = 0; // when was the last message sent
int tmp_EL = 0;



// Total Time counters
const unsigned int CAN_TT_Msg_ID = 112; // CAN Msg ID in DEC
const unsigned int CAN_TT_Period = 10000; // How often message sent in milliseconds
unsigned long CAN_TT_Timestamp = 0; // when was the last message sent

//


// END of CAN Message

int k = 0;
MCP_CAN CAN(CAN_CS_PIN);                                    // Set CS pin

//**************** EEPROM (1024 bytes) *******************
// Engine data will be stored in the EEPROM
// Format
// <- 4 bytes date and time (Unix: seconds since Jan 01 1970) -><- 4 bytes cummulative total time in seconds -><- 4 bytes engine revolutions -> 
// Data will be saved into EEPROM every minute in a new location on each startup to avoid EEPROM wearout
// First 240 bytes will be storing the data (last 20 runs)

int Hobbs_Offset = 0; //current offset for this run
unsigned long HobbsTotalTime = 0;
unsigned long HobbsRevs = 0;
unsigned long tmpHobbsRevs = 0;
unsigned int HobbsWritePeriod = 120000; // Write (update) data into EEPROM every 2 minutes
unsigned long HobbsWriteTimestamp = 0; // when was the last update


void setup()
{

    analogReference(DEFAULT);   // All analog measurements will be done referencing 5v bus.
    
    attachInterrupt(0, TimingMeter0, RISING);
    attachInterrupt(1, TimingMeter1, RISING);

    // Set the reset pin of the LTC2983 to HIGH
     pinMode(4, OUTPUT);
     digitalWrite(4, HIGH);  

     // reset the LTC2983
     delay(200); 
     digitalWrite(4, LOW);
     delay(20);
     digitalWrite(4, HIGH);
     delay(200);    

/**************************************************************** LTC2983 init stuff *************************/
  quikeval_SPI_init();          // Configure the spi port for 4MHz SCK
  quikeval_SPI_connect();       // Connect SPI to main data port

  
  pinMode(TCS_CS_PIN, OUTPUT); // Configure chip select pin Temperature sensors (LTC2983)

  Serial.begin(115200);         // Initialize the serial port to the PC
  
  configure_channels();
  configure_global_parameters();



 //  while (CAN_OK != CAN.begin(MCP_ANY, CAN_1000KBPS, MCP_8MHZ))              // init can bus : baudrate = 500k
   while (CAN_OK != CAN.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ))              // init can bus : baudrate = 500k
    {
        Serial.println("CAN BUS Shield init fail");
        Serial.println(" Init CAN BUS Shield again");
        delay(100);
    }
    CAN.setMode(MCP_NORMAL);
    Serial.println("CAN BUS Shield init ok!");


// read current HobbsRevs from EEPROM or CAN and set it it here along with Counter0
// find a new offset for this run by looking for the max value of the Engine rev counter (last 4 bytes in the 12 bytes blocks
   Hobbs_Offset = 0;
   Serial.println("==== Log ===="); 
   while(Hobbs_Offset < 120){
      HobbsRevs = tmpHobbsRevs;
      EEPROM.get(Hobbs_Offset, SystemTime);
      EEPROM.get(Hobbs_Offset + 4, HobbsTotalTime);
      EEPROM.get(Hobbs_Offset + 8, tmpHobbsRevs);
      if (tmpHobbsRevs == 4294967295) {
        tmpHobbsRevs = 0;
      }

      Serial.print("EEPROM offset: ");
      Serial.print(Hobbs_Offset);    

      Serial.print(" Date: ");
      Serial.print(SystemTime); 

      Serial.print(" Total Time: ");
      Serial.print(HobbsTotalTime); 

      Serial.print(" Hobbs Revs: ");
      Serial.println(tmpHobbsRevs);
      if (HobbsRevs > tmpHobbsRevs) {
        break;
      }
      
      Hobbs_Offset = Hobbs_Offset + 12;
   }

    Hobbs_Offset = Hobbs_Offset - 12; // go back 1 step
    
    EEPROM.get(Hobbs_Offset, SystemTime);
    EEPROM.get(Hobbs_Offset + 4, HobbsTotalTime);
    EEPROM.get(Hobbs_Offset + 8, HobbsRevs);

    if (HobbsRevs == 4294967295) {
      HobbsRevs = 0;
      HobbsTotalTime = 0;
    }

    Hobbs_Offset = Hobbs_Offset + 12;
    if (Hobbs_Offset >= 120) {
      Hobbs_Offset = 0;
    }

   Counter0 = HobbsRevs * PPR;
Serial.println(" "); 
Serial.print("EEPROM offset: ");
Serial.println(Hobbs_Offset);    

Serial.print("Max Timestamp: ");
Serial.println(SystemTime);  

Serial.print("Max HobbsTotalTime: ");
Serial.println(HobbsTotalTime);    

Serial.print("Max Hobbs Revs: ");
Serial.println(HobbsRevs);
Serial.println(" "); 
SystemTime = 0; // reset system time to zero. It will be updated later to actual time when the message with id 25 arrives.

}

void loop()
{
  if(CAN_MSGAVAIL == CAN.checkReceive())            // check if data coming
    {
      CAN.readMsgBuf(&canId, &ext, &len, buf);    // read data,  len: data length, buf: data buf
      switch (canId) {
        case 25:
        {
        // System Time (Clock)
        // "Master" clock unit sends current time every 30 seconds
           SystemTime = ((unsigned long)buf[3] << 24) | ((unsigned long)buf[2] << 16) | ((unsigned long)buf[1] << 8) | (unsigned long)buf[0];
           SystemTimeLocalMillis = millis();  // use this variable to calculate current time when required
           Serial.print("Received SystemTime: ");
           Serial.println(SystemTime); 
        }
        break;
        default: 
        // if nothing else matches, do the default
        // default is optional
        break;
    }

    }

  if (EngineRunning and millis() > EngineTimer + 15000) {
      EngineTimerTMP = millis();
      HobbsTotalTime = HobbsTotalTime + (EngineTimerTMP - EngineTimer)/1000;
      EngineTimer = EngineTimerTMP;
    }
  

// ============================ Update Hobbs - Engine total time =======================================================
if ((millis() > HobbsWritePeriod + HobbsWriteTimestamp) and tmpHobbsRevs < HobbsRevs) {  // save engine time data every 2 minutes. do not write EEPROM if engine is not running
    EEPROM.put(Hobbs_Offset, SystemTime+(millis()-SystemTimeLocalMillis));
    EEPROM.put(Hobbs_Offset + 4, HobbsTotalTime);
    EEPROM.put(Hobbs_Offset + 8, HobbsRevs);

    Serial.print("Writng to EEPROM offset: ");
    Serial.println(Hobbs_Offset);   
    
    Serial.print("Writng HobbsTotalTime: ");
    Serial.println(HobbsTotalTime);   

    Serial.print("Writng Hobbs Revs: ");
    Serial.println(HobbsRevs);   
    
    HobbsWriteTimestamp = millis();
    tmpHobbsRevs = HobbsRevs;
}



// ================= Engine RPM and fuel flow stuff ======================================================================
//if ((Counter0 > HobbsRevs*PPR + 200) || (millis() - EngineTimestamp > 1000)) {
//if (micros() - EngineTimestamp > 2000000) {
//
//  Get_RPM();  
//}
    // update engine time counters 
    //engine started
    if (RPM > 100 && not EngineRunning) {
      EngineTimer = millis();
      EngineRunning = true;
      Serial.println("=== Engine Started ===");
    } 
    
    //engine stopped
    if (RPM < 100 && EngineRunning) {
      EngineRunning = false;
      Serial.println("=== Engine Stopped ===");
    } 



// ====================  Send total time (Hobbs)  ================================================================
if (millis() > CAN_TT_Timestamp + CAN_TT_Period + random(0, 500)) {

    canMsg[0] = HobbsRevs;
    canMsg[1] = HobbsRevs >> 8;
    canMsg[2] = HobbsRevs >> 16;
    canMsg[3] = HobbsRevs >> 24;
    canMsg[4] = HobbsTotalTime;
    canMsg[5] = HobbsTotalTime >> 8;
    canMsg[6] = HobbsTotalTime >> 16;
    canMsg[7] = HobbsTotalTime >> 24;

    
    Serial.print("Sending Hobbs Revs: ");
    Serial.println(HobbsRevs);   

    Serial.print("Sending HobbsTotalTime: ");
    Serial.println(HobbsTotalTime);   
    

    // send data:  
    CAN.sendMsgBuf(CAN_TT_Msg_ID, 0, 8, canMsg);  

    CAN_TT_Timestamp = millis();
}


// ============================== Send Fuel Tank Levels ============================================================
if (millis() > CAN_FT_Timestamp + CAN_FT_Period + random(0, 50)) {

  TankLevel1 = analogRead(2);

  GetFuel(); // adjust this procedure to calibrate fuel reading

  canMsg[0] = TankLevel1;
  canMsg[1] = TankLevel1 >> 8;

  CAN.sendMsgBuf(CAN_FT_Msg_ID, 0, 2, canMsg); 

 // LoopTimer = millis();
  
  CAN_FT_Timestamp = millis();
}


// ======================== Send OIL data =========================================================================
if (millis() > CAN_OIL_Timestamp + CAN_OIL_Period + random(0, 50)) {


  canMsg[0] = OIL_Pressure;
  canMsg[1] = OIL_Pressure >> 8;

// Oil Temperature. A5 input
  //OIL_Temperature = analogRead(5);
  // Information sent in degrees Celsius x 10
   
  Get_OilTemperature(); // OIL_Temperature variable populated by this function
  
  canMsg[2] = OIL_Temperature;
  canMsg[3] = OIL_Temperature >> 8;

  
  CAN.sendMsgBuf(CAN_OIL_Msg_ID, 0, 4, canMsg); 
  
  CAN_OIL_Timestamp = millis();
}

// ==================================== Send Electric data ====================================================
if (millis() > CAN_EL_Timestamp + CAN_EL_Period + random(0, 50)) {

// Main Electric Bus Volts. A0 input
  tmp_EL = analogRead(0);
  // the ADC works via trim pot adjusted to return max reading 1024 at 20v

  // convert the value into millivolts = tmp_EL * 20* 1000/1024;
  tmp_EL = (float)tmp_EL * 19.5;
  
  canMsg[0] = tmp_EL;
  canMsg[1] = tmp_EL >> 8;

// Alternator Amps. A1 input
  tmp_EL = analogRead(1);
  
  // convert the value into milliamps based on the parameters of the sensor
  // for 40A/75mV shunt with LMP8603 amplifier (zero-center with x100 amplification) with standard 5v reference voltage
  // K = (5*40)/(1024*0.075*100) = 0.026041666(A)
  // * 1000(mA) = 26.0417
  
  tmp_EL = tmp_EL - 512;  // amplifier is centre-zero. At zero current the voltage will be 2.5 volts (half the 1024 units range)
  
  tmp_EL = (float)tmp_EL * 26.0417;
  
  canMsg[2] = tmp_EL;
  canMsg[3] = tmp_EL >> 8;

  
// Battery Amps. A7 input
  tmp_EL = analogRead(7);

  // convert the value into milliamps based on the parameters of the sensor
  // for 40A/75mV shunt with LMP8603 amplifier (zero-center with x100 amplification) with standard 5v reference voltage
  // K = (5*40)/(1024*0.075*100) = 0.026041666(A)
  // * 1000(mA) = 26.0417
  
  tmp_EL = tmp_EL - 512;  // amplifier is centre-zero. At zero current the voltage will be 2.5 volts (half the 1024 units range)
  
  tmp_EL = (float)tmp_EL * 26.0417;  
  
  canMsg[4] = tmp_EL;
  canMsg[5] = tmp_EL >> 8;

  
  CAN.sendMsgBuf(CAN_EL_Msg_ID, 0, 6, canMsg); 
  
  CAN_EL_Timestamp = millis();
}

// Send temperatures
if (millis() > CAN_TMP_Timestamp + CAN_TMP_Period + random(0, 50)) {


  canMsg[0] = EGT[1];
  canMsg[1] = EGT[1] >> 8;

  canMsg[2] = EGT[2];
  canMsg[3] = EGT[2] >> 8;

  canMsg[4] = CHT[1];
  canMsg[5] = CHT[1] >> 8;

  canMsg[6] = CHT[2];
  canMsg[7] = CHT[2] >> 8;

  delay(35);
  CAN.sendMsgBuf(CAN_TMP_Msg_ID, 0, 8, canMsg); 
  
  canMsg[0] = EGT[3];
  canMsg[1] = EGT[3] >> 8;

  canMsg[2] = EGT[4];
  canMsg[3] = EGT[4] >> 8;

  canMsg[4] = CHT[3];
  canMsg[5] = CHT[3] >> 8;

  canMsg[6] = CHT[4];
  canMsg[7] = CHT[4] >> 8;

  delay(35);
  CAN.sendMsgBuf(CAN_TMP_Msg_ID+1, 0, 8, canMsg); 

  if (CYL == 6) {
    canMsg[0] = EGT[5];
    canMsg[1] = EGT[5] >> 8;
  
    canMsg[2] = EGT[6];
    canMsg[3] = EGT[6] >> 8;
  
    canMsg[4] = CHT[5];
    canMsg[5] = CHT[5] >> 8;
  
    canMsg[6] = CHT[6];
    canMsg[7] = CHT[6] >> 8;
  
    delay(35);
    CAN.sendMsgBuf(CAN_TMP_Msg_ID+2, 0, 8, canMsg); 
  }
  CAN_TMP_Timestamp = millis();
}



// ==================  Send RPM  ================================================================
if (millis() > CAN_RPM_Timestamp + CAN_RPM_Period + random(0, 50)) {
  


  Get_FuelFlow();

  Get_RPM();

// Serial.print("RPM = ");
// Serial.println(RPM);

  canMsg[0] = RPM;
  canMsg[1] = RPM >> 8;
  canMsg[2] = FuelPressure;
  canMsg[3] = FuelPressure >> 8;
  canMsg[4] = FuelFlow;
  canMsg[5] = FuelFlow >> 8;

  // send data:  id = 0x123, standrad flame, data len = 8, message
  CAN.sendMsgBuf(CAN_RPM_Msg_ID, 0, 6, canMsg);  
  

  CAN_RPM_Timestamp = millis();
}


//if (LoopCounter % 100 == 0) {
//  Serial.println(CAN.checkError());
//  Serial.print("CHT[6]");
//  Serial.println(CHT[6]);
//  Serial.print("Avg Time: ");
//  Serial.println((millis() - LoopTimer)/100);
//  LoopTimer = millis();
//}



/********************/
/* populate global variable as the data becomes available  */

  TCS_Read_All_Sensors();

  Get_Fuel_Pressure();
  Get_Oil_Pressure();
  
/*********************/


 LoopCounter++;
 
}
