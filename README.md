# Engine Management System

Arduino software for the EMS sensor board

Details are here: http://experimentalavionics.com/engine-management-system/

Unzip libraries.zip to the "libraries" folder in your Arduino environment.

Important notes:

Originally the EMS was built for 6 cylinder Jabiru engine but over time the direction changed towards 4 cylinder Jabiru and lately towards Rotax 912. The module should still support engines other than Rotax and the code still have parts that might need to be inspected and uncommented where required. 

## Release Notes: ##

### 2021-07-16 ###
* Increased the size of the Moving Average filters for Fuel Pressure and Oil Pressure. Reason - suppress vibration-induced noise

### 2021-06-15 ###
* General code clean-up
* Fuel tank calibration data changed to match my fuel tank :)
* Added Return flow correction for the fuel flow for the engines with return line (Rotax-912). It is hardcoded for now. For my Rotax 912ULS it is about 6.7 L/HR.
* Added Movin Average filters for Oil and Fuel pressure sensors to stabilize the readings.
* Steinhart-Hart coefficients changed for the oil temperature sensor to match VDO/Rotax sensor.

