void Get_Fuel_Pressure(){
// pressure sensors appear to be noisy
// MA filter over 10 readings used to reduce noise
  
    FuelPressure = analogRead(4);
  FuelPressure = FuelPressure - 102; // voltage at 0 bars is 0.5v  ~ 100 ADC units
  if (FuelPressure < 0) {
    FuelPressure = 0;
  }

  FuelPressureSum -= FuelPressureArray[FuelPressureArrayIndex];
  FuelPressureArray[FuelPressureArrayIndex] = FuelPressure;
  FuelPressureSum += FuelPressureArray[FuelPressureArrayIndex];

  FuelPressureArrayIndex +=1;                         // shift the index for next time
  if (FuelPressureArrayIndex == FuelPressureArraySize) {      // if we reached the top of the array
    FuelPressureArrayIndex = 0;                       //go to the start of the array
  }
  
  // Pressure sensor range: up to 1 Bar
  // sensor supplies voltage 0.5 - 4.5v proportional to pressure 0 - 1 bars
  // Information sent in millibars, Bars x 1000 (kPa x 10)
  // assumptions
  //    voltage is linear from 0 to 1 Bars gauge pressure (to be confirmed experimentally)
  //    voltage at 0 bars is 0.5v (0.489 in my case)
  //    Voltage at 1 Bars is 4.5v
  // Reading conversion coefficient:
  // K = (1/(4.5-0.489))*(5/1024) = 0.001217
  // Convert to millibars K * 1000 = 1.217


//  FuelPressure = FuelPressure * 1.217;
  FuelPressure = round((float)FuelPressureSum * 1.217/(float)FuelPressureArraySize);

  
//  Serial.print("Fuel Pressure = ");
//  Serial.println(FuelPressure);

}

void Get_Oil_Pressure(){
// pressure sensors appear to be noisy
// MA filter over 10 readings used to reduce noise

// Oil pressure, A3 input
  OIL_Pressure = analogRead(3);

  OIL_Pressure = OIL_Pressure - 100; // voltage at 0 bars is 0.5v (0.489 in my case) ~ 100 ADC units
  if (OIL_Pressure < 0) {
    OIL_Pressure = 0;
  }

  OilPressureSum -= OilPressureArray[OilPressureArrayIndex];
  OilPressureArray[OilPressureArrayIndex] = OIL_Pressure;
  OilPressureSum += OilPressureArray[OilPressureArrayIndex];

  OilPressureArrayIndex +=1;                         // shift the index for next time
  if (OilPressureArrayIndex == OilPressureArraySize) {      // if we reached the top of the array
    OilPressureArrayIndex = 0;                       //go to the start of the array
  }

  // Pressure sensor range: up to 150 PSI (~10 bars)
  // sensor supplies voltage 0.5 - 4.5v proportional to pressure 1.01 - 10 bars
  // Information sent in millibars, Bars x 1000 (kPa x 10)
  // assumptions
  //    voltage is linear from 1 to 10 Bars gauge pressure (to be confirmed experimentally)
  //    voltage at 0 bars is 0.5v (0.489 in my case)
  //    Voltage at 10 Bars is 4.5v
  // Reading conversion coefficient:
  // K = (10/(4.5-0.489))*(5/1024) = 0.01217
  // Convert to millibars K * 1000 = 12.17

//   OIL_Pressure = (float)OIL_Pressure * 12.17;
   OIL_Pressure = round((float)OilPressureSum * 12.17/(float)OilPressureArraySize);
  
}
