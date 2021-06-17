void TimingMeter0()
 {
   Counter0++;  // RPM Counter
   LastPulseTimestamp_RPM = micros();
 }

 void TimingMeter1()
 {
   Counter1++;  // Fuel Flow counter
   LastPulseTimestamp_Flow = micros();
 }

 void Get_RPM () {

  unsigned long CurrentTimestap = 0;
  unsigned long CurrentCounter = 0;

  CurrentCounter = Counter0/PPR;
  CurrentTimestap = LastPulseTimestamp_RPM;

// weird trick to deal with limitations of integer calculations 
  RPM = (30000000UL * (CurrentCounter - HobbsRevs))/(CurrentTimestap - EngineTimestamp);
  RPM = RPM * 2 + 2;
   
   HobbsRevs = CurrentCounter;
   EngineTimestamp = CurrentTimestap;
  
 }

 void Get_FuelFlow() {
// procedure built for FT60 flow transducer 
// K-factor is 68000 pulses per gallon (17964 pulses per litre)
// Fuel Flow Range: 0.6 to 70+ Gal/Hr (2.3 - 265 L/h)
// Pulse frequency range 11.5 - 1322 Hz
 
  const float Kfactor = 17964;
  const float ReturnFlow = 6.67; // fuel flow (L/HR) when engine is not running (return line flow)
  unsigned long CurrentTimestap = 0;
  unsigned long CurrentCounter = 0;
  

// note the counter recorder twice. because it could increase while the function is running.
// didnt want to disable interrupt.

  CurrentCounter = Counter1;  // grab the latest value of the counter 
  Counter1 = 0;               // ... and reset it
  CurrentTimestap = millis(); // grab the timestamp at this point
   
  FlowCountSum -= FlowCountArray[FlowArrayIndex];  
  FlowTimeSum  -= FlowTimeArray[FlowArrayIndex];
  
  FlowCountArray[FlowArrayIndex] = CurrentCounter;
  FlowTimeArray[FlowArrayIndex] = CurrentTimestap - FlowTimestamp;
  FlowTimestamp = CurrentTimestap;

  FlowCountSum += FlowCountArray[FlowArrayIndex];
  FlowTimeSum += FlowTimeArray[FlowArrayIndex];
 
  FlowArrayIndex +=1;                         // shift the index for next time
  if (FlowArrayIndex == FlowArraySize) {      // if we reached the top of the array
    FlowArrayIndex = 0;                       //go to the start of the array
  }

// time kept in milliseconds. we need flow in L per hour hence 3600*1000 = 3600000. 
// We have 2 bytes to send over CAN, so we can multiply the number by 100 to increase precision. It will come handy for performance calculations


  FuelFlow = (float)FlowCountSum*3600000.0*100.0/((float)FlowTimeSum * Kfactor) - (ReturnFlow * 100.0);
  
  if (FuelFlow < 100) {  // Flow can't be lower than 1 L/HR so this elogic eliminates useless noise around 0
    FuelFlow = 0;
  }


  
 }
