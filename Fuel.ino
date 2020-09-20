void GetFuel() {

// sensor resistance in Ohms at different volumes of fuel in the tank
// this will need to be configurable via display
// you can measure the values with an multimeter or by uncommenting the lines down below and read the values of the terminal screen

unsigned int  TankReading_100 = 246; // full tank
unsigned int  TankReading_75 = 230;  // 75% capacity
unsigned int  TankReading_50 = 200;  // 50% capacity
unsigned int  TankReading_25 = 140;  // 25% capacity
unsigned int  TankReading_0 = 30;     // empty tank
unsigned int  TankCapacity = 82;     //Tank capacity 82 litres

// liner finction coefficients
float a,b,k;



float R2 = 1000; // ballast resistor in the fuel sensor circuit. 1k is good starting point
float R1 = 0;

    R1 = TankLevel1*R2/(1023-TankLevel1);

//  uncomment the two lines below to get the resistance reading at different volumes of fuel in the tank
//  Serial.print("Fuel sensor resistance = ");
//  Serial.println(R1);

//  Serial.print("TankLevel1 = ");
//  Serial.println(TankLevel1);

if (R1 < TankReading_0) {
  k = 0;
} else if (R1 < TankReading_25) { // between 0 and 25% of the tank capacity
  a = 0.25/(TankReading_25 - TankReading_0);
  b = 0.25 - a * TankReading_25;
  k = a * R1 + b;
} else if (R1 < TankReading_50) {
  a = 0.25/(TankReading_50 - TankReading_25);
  b = 0.5 - a * TankReading_50;
  k = a * R1 + b;
} else if (R1 < TankReading_75) {
  a = 0.25/(TankReading_75 - TankReading_50);
  b = 0.75 - a * TankReading_75;
  k = a * R1 + b;
} else if (R1 < TankReading_100) {
  a = 0.25/(TankReading_100 - TankReading_75);
  b = 1 - a * TankReading_100;
  k = a * R1 + b;
} else if (R1 >= TankReading_100) {
  k = 1;
}

  k = a * R1 + b;

  TankLevel1 = TankCapacity * k;
  
}
