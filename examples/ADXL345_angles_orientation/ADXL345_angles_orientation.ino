/***************************************************************************
* Example sketch for the ADXL345_WE library
*
* This sketch shows how to measure angles, corrected angles and orientation 
* of the ADXL345 
* It does not use the internal offset registers (OFSX, OFSY, OFSZ).
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
  Serial.begin(115200);
  Serial.println("ADXL345_Sketch");
  Serial.println();
  if(!myAcc.init()){
    Serial.println("ADXL345 not connected!");
  }
  
/* Add your min and max raw values you found with ADXL345_calibration.ino.  
 *  The order is: setCorrFactors(min x, max x, min y, max y, min z, max z).
 *  setCorrFactors calibrates the slope and assumes the zero point 
 *  is (min+max)/2. You have to add the setCorrFactors function to all sketches 
 *  in which you want to use it.
*/
//  myAcc.setCorrFactors(-266.0, 285.0, -268.0, 278.0, -291.0, 214.0);

/* In this next step the offset for angles is corrected to get quite precise corrected 
 *  angles for x and y up to ~60°. The additional offsetCorrection is only used for 
 *  corrected angles measurements. The procedure just ensures to start at 0°.
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
  xyzFloat raw, g, angle, corrAngle;
  myAcc.getRawValues(&raw);
  myAcc.getGValues(&g);
  myAcc.getAngles(&angle);
  myAcc.getCorrAngles(&corrAngle);
  
/* Still the uncorrected raw values!! */  
  Serial.print("Raw-x    = ");
  Serial.print(raw.x);
  Serial.print("  |  Raw-y    = ");
  Serial.print(raw.y);
  Serial.print("  |  Raw-z    = ");
  Serial.println(raw.z);

/* g values use the corrected raws */
  Serial.print("g-x     = ");
  Serial.print(g.x);
  Serial.print("  |  g-y     = ");
  Serial.print(g.y);
  Serial.print("  |  g-z     = ");
  Serial.println(g.z);

/* Angles use the corrected raws. Angles are simply calculated by
    angle = arcsin(g Value) */
  Serial.print("Angle x  = ");
  Serial.print(angle.x);
  Serial.print("  |  Angle y  = ");
  Serial.print(angle.y);
  Serial.print("  |  Angle z  = ");
  Serial.println(angle.z);

/* Corrected angles use corrected raws and an extra angle 
    offsets to ensure they start at 0° 
*/
  Serial.print("cAngle x = ");
  Serial.print(corrAngle.x);
  Serial.print("  |  cAngle y   = ");
  Serial.print(corrAngle.y);
  Serial.print("  |  cAngle z   = ");
  Serial.println(corrAngle.z);

  Serial.print("Orientation of the module: ");
  Serial.println(myAcc.getOrientationAsString());

  Serial.println();
  
  delay(1000);
  
}
