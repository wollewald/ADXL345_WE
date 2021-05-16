/***************************************************************************
* Example sketch for the ADXL345_WE library
*
* This sketch shows how to measure pitch and roll angles of the ADXL345
* vs. the corrected angle method.
* It does not use the internal offset registers.
* Please note the ADXL345 "forgets" calibration data when reset or repowered.
* You might consider to save them to the EEPROM
*  
* Further information can be found on: 
* https://wolles-elektronikkiste.de/adxl345-teil-1 (German)
* https://wolles-elektronikkiste.de/en/adxl345-the-universal-accelerometer-part-1 (English)
* 
***************************************************************************/

#include<Wire.h>
#include<ADXL345_WE.h>
#define ADXL345_I2CADDR 0x53 // 0x1D if SDO = HIGH

/* There are several ways to create your ADXL345 object:
 * ADXL345_WE myAcc = ADXL345_WE()                -> uses Wire / I2C Address = 0x53
 * ADXL345_WE myAcc = ADXL345_WE(ADXL345_I2CADDR) -> uses Wire / ADXL345_I2CADDR
 * ADXL345_WE myAcc = ADXL345_WE(&wire2)          -> uses the TwoWire object wire2 / ADXL345_I2CADDR
 * ADXL345_WE myAcc = ADXL345_WE(&wire2, ADXL345_I2CADDR) -> all together
 */
ADXL345_WE myAcc = ADXL345_WE(ADXL345_I2CADDR);

void setup(){
  Wire.begin();
  Serial.begin(9600);
  Serial.println("ADXL345 Sketch - Pitch and Roll vs. Corrected Angles");
  Serial.println();
  if(!myAcc.init()){
    Serial.println("ADXL345 not connected!");
  }
  
/* Add your min and max raw values you found with ADXL345_calibration.ino.  
    The order is: setCorrFactors(min x, max x, min y, max y, min z, max z).
    setCorrFactors calibrates the slope and assumes the zero point 
    is (min+max)/2. You have to add the setCorrFactors function to all sketches 
    in which you want to use it.
*/
  myAcc.setCorrFactors(-266.0, 285.0, -268.0, 278.0, -291.0, 214.0);

/* In this next step the offset for angles is corrected to get quite precise corrected 
    angles x and y up to ~60°. The additional offsetCorrection is only for corrected 
    angle measurements, not for pitch and roll. The procedure just ensures to start at 0°.
*/
  Serial.println("Position your ADXL345 flat and don't move it");
  delay(2000);
  myAcc.measureAngleOffsets();
  Serial.println("....done");
   
/* Choose the data rate         Hz
    ADXL345_DATA_RATE_3200    3200
    ADXL345_DATA_RATE_1600    1600
    ADXL345_DATA_RATE_800      800
    ADXL345_DATA_RATE_400      400
    ADXL345_DATA_RATE_200      200
    ADXL345_DATA_RATE_100      100
    ADXL345_DATA_RATE_50        50
    ADXL345_DATA_RATE_25        25
    ADXL345_DATA_RATE_12_5      12.5  
    ADXL345_DATA_RATE_6_25       6.25
    ADXL345_DATA_RATE_3_13       3.13
    ADXL345_DATA_RATE_1_56       1.56
    ADXL345_DATA_RATE_0_78       0.78
    ADXL345_DATA_RATE_0_39       0.39
    ADXL345_DATA_RATE_0_20       0.20
    ADXL345_DATA_RATE_0_10       0.10
*/
  myAcc.setDataRate(ADXL345_DATA_RATE_50);
  Serial.print("Data rate: ");
  Serial.print(myAcc.getDataRateAsString());

/* Choose the measurement range
    ADXL345_RANGE_16G    16g     
    ADXL345_RANGE_8G      8g     
    ADXL345_RANGE_4G      4g   
    ADXL345_RANGE_2G      2g
*/
  myAcc.setRange(ADXL345_RANGE_2G);
  Serial.print("  /  g-Range: ");
  Serial.println(myAcc.getRangeAsString());
  Serial.println();  
}

void loop() {
  //xyzFloat g = myAcc.getGValues();
  xyzFloat corrAngles = myAcc.getCorrAngles();
  
/* Corrected angles use corrected raws and an extra angle
    offset. The method provides quite precise values for x/y 
    angles up 60°.
*/
  Serial.print("Angle x = ");
  Serial.print(corrAngles.x);
  Serial.print("  |  Angle y = ");
  Serial.print(corrAngles.y);
  Serial.print("  |  Angle z = ");
  Serial.println(corrAngles.z);

/* Pitch and roll use corrected slope, but no additional offset. 
    All axes are considered for calculation. 
*/
  float pitch = myAcc.getPitch();
  float roll  = myAcc.getRoll();
  
  Serial.print("Pitch   = "); 
  Serial.print(pitch); 
  Serial.print("  |  Roll    = "); 
  Serial.println(roll); 
  
  Serial.println();
  
  delay(1000);
  
}
