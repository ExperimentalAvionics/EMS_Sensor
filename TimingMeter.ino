void TimingMeter0()
 {
   Counter0++;  // RPM Counter
 }

 void TimingMeter1()
 {
   Counter1++;  // Fuel Flow counter
 }

 void Get_RPM () {

  unsigned long CurrentTimestap = 0;
  unsigned long CurrentCounter = 0;

// note the counter recorded twice. because it could increase while the function is running.
// didnt want to disable interrupt.

  CurrentCounter = Counter0/PPR;
  CurrentTimestap = millis();

  RPM = (60000 * (CurrentCounter - HobbsRevs))/(CurrentTimestap - EngineTimestamp);
   
   HobbsRevs = CurrentCounter;
   EngineTimestamp = CurrentTimestap;
  
 }

 void Get_FuelFlow () {

  unsigned long CurrentTimestap = 0;
  unsigned long CurrentCounter = 0;

// note the counter recorder twice. because it could increase while the function is running.
// didnt want to disable interrupt.

  CurrentCounter = Counter1;
  CurrentTimestap = millis();

// dummy line
  FuelFlow = 10 + random(10, 20);
   
   FlowCounter = Counter1;
   FlowTimestamp = millis();
  
 }
