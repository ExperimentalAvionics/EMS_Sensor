void GetFuel() {

// sensor resistance in Ohms at different volumes of fuel in the tank
// this will need to be configurable via display
// you can measure the values with an multimeter or by uncommenting the lines down below and read the values of the terminal screen

unsigned int  TankReading_100 = 27; // full tank
unsigned int  TankReading_80 = 30;  // 80% capacity Note: the accuracy of the RV-12 fuel level sender is very poor above 80% of the tank
unsigned int  TankReading_50 = 113;  // 50% capacity
unsigned int  TankReading_25 = 173;  // 25% capacity
unsigned int  TankReading_0 = 243;     // empty tank
unsigned int  TankCapacity = 75;     //Tank capacity 75 litres

// liner finction coefficients
float a,b,k;

float R2 = 1000; // ballast resistor in the fuel sensor circuit. 1k is good starting point
float R1 = 0;

    R1 = TankLevel1*R2/(1023-TankLevel1);

//  uncomment the two lines below to get the resistance reading at different volumes of fuel in the tank
  Serial.print("Fuel sensor resistance = ");
  Serial.println(R1);


if (R1 > TankReading_0) {
  k = 0;
} else if (R1 > TankReading_25) { 
  a = 0.25/((float)TankReading_25 - (float)TankReading_0);
  b = 0.25 - a * (float)TankReading_25;
  k = a * R1 + b;
} else if (R1 > TankReading_50) {
  a = 0.25/((float)TankReading_50 - (float)TankReading_25);
  b = 0.5 - a * (float)TankReading_50;
  k = a * R1 + b;
} else if (R1 > TankReading_80) {
  a = 0.3/((float)TankReading_80 - (float)TankReading_50);
  b = 0.8 - a * (float)TankReading_80;
  k = a * R1 + b;
} else if (R1 > TankReading_100) {
  a = 0.2/((float)TankReading_100 - (float)TankReading_80);
  b = 1 - a * (float)TankReading_100;
  k = a * R1 + b;
} else if (R1 <= TankReading_100) {
  k = 1;
}

//  Serial.print("k = ");
//  Serial.print(k);
//  Serial.print("  a = ");
//  Serial.print(a);
//  Serial.print("  b = ");
//  Serial.println(b);

 // k = a * R1 + b;

  TankLevel1 = (float)TankCapacity * k;

  //Serial.print("TankLevel1 = ");
  //Serial.println(TankLevel1);
  
}
