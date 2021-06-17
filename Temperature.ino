void Get_OilTemperature() {

int Vo;
float R2 = 1000;
float logR1, R1, T;
// float c1 = 0.0012718948770798462, c2 = 0.0003326009074864466, c3 = -3.9023201666061627e-7;  // measured on aftermarket look-alike sensor
float c1 = 0.0016142582959906724, c2 = 0.00025957420166777926, c3 = -9.751302606784632e-8;  // calculated based on VDO datasheet
// Online coefficient calculator https://sanjit.wtf/Calibrator/webCalibrator.html
//                               https://rusefi.com/Steinhart-Hart.html

  Vo = analogRead(5);

  R1 = Vo*R2/(1023-(float)Vo);

// uncomment the two lines below to get the resistance reading at different temeperatures
//  Serial.print("Thermistor = ");
//  Serial.println(R1);
  
  logR1 = log(R1);
  T = (1.0 / (c1 + c2*logR1 + c3*logR1*logR1*logR1)); // https://en.wikipedia.org/wiki/Steinhart%E2%80%93Hart_equation
  T = T - 273.15;

  OIL_Temperature = T*10;
  
}



// Get CHT readings from thermistors normally installed on Rotax 912 engines
float Get_CHT_Rotax(float ThermistorReading) {

float logR1,  T;
float c1 = -0.013733951254788702, c2 = 0.003301937438448783, c3 = -0.00001760449381455601;
// Online coefficient calculator https://sanjit.wtf/Calibrator/webCalibrator.html
//                               https://rusefi.com/Steinhart-Hart.html
  
  logR1 = log(ThermistorReading);
  T = (1.0 / (c1 + c2*logR1 + c3*logR1*logR1*logR1)); // https://en.wikipedia.org/wiki/Steinhart%E2%80%93Hart_equation
  T = T - 273.15;

  return T;
  
}
