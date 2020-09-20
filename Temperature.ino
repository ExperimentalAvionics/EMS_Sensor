void Get_OilTemperature() {

int Vo;
float R2 = 1000;
float logR1, R1, T;
float c1 = 0.0012718948770798462, c2 = 0.0003326009074864466, c3 = -3.9023201666061627e-7;
// Online coefficient calculator https://sanjit.wtf/Calibrator/webCalibrator.html

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
