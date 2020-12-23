/***************************************************************************
* Example sketch for the ADXL345_WE library
*
* This sketch shows how to calibrate the ADXL345 
* It does not use the internal offset registers.
*  
* Further information can be found on:
* http://wolles-elektronikkiste.de
* 
***************************************************************************/
#include<Wire.h>
#include<ADXL345_WE.h>
#define ADXL345_I2CADDR 0x53

ADXL345_WE myAcc(ADXL345_I2CADDR);
// ADXL345_WE myAcc = ADXL345_WE(); // Alternative: sets default address 0x53

void setup(){
  Wire.begin();
  Serial.begin(9600);
  Serial.println("ADXL345_Sketch - Calibration");
  Serial.println();
  if(!myAcc.init()){
    Serial.println("ADXL345 not connected!");
  }
  Serial.println("Calibration procedure:");
  Serial.println(" - stay in full resolution");
  Serial.println(" - supply voltage has influence (at least for the modules)");
  Serial.println("        -> choose the same voltage you will use in your project!"); 
  Serial.println(" - turn your ADXL slowly (!) to find the min and max raw x,y and z values");
  Serial.println(" - deviations of one or two units don't matter much");
  Serial.println(" - the calibration changes the slope of g vs raw and assumes zero is (min+max)/2 ");
  Serial.println(" - write down the six values ");
  Serial.println(" - you can try the calibration values in ADXL345_angles_tilt_orientation.ino example sketch");
  Serial.println(" - ready to go? Then type in any key and send. ");
  while(!Serial.available());
  Serial.read();
  Serial.println(); Serial.println(); Serial.println();  
}

void loop() {
  xyzFloat raw = myAcc.getRawValues();
  Serial.print("Raw-x = ");
  Serial.print(raw.x);
  Serial.print("  |  Raw-y = ");
  Serial.print(raw.y);
  Serial.print("  |  Raw-z = ");
  Serial.println(raw.z);
  
  delay(1000);
  
}
