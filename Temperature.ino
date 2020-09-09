void Get_OilTemperature() {

int Vo;
float R2 = 12000;
float logR1, R1, T;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

  Vo = analogRead(5);
  R1 = Vo*R2/(1023-(float)Vo);
  logR1 = log(R1);
  T = (1.0 / (c1 + c2*logR1 + c3*logR1*logR1*logR1)); // https://en.wikipedia.org/wiki/Steinhart%E2%80%93Hart_equation
  T = T - 273.15;

  OIL_Temperature = T*10;
  
}
