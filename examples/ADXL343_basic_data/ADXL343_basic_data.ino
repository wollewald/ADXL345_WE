/***************************************************************************
* Example sketch for the ADXL345_WE / ADXL343 library
*
* This sketch shows how to get the basic raw and g values from the ADXL343. 
*  
* Further information can be found on:
* https://wolles-elektronikkiste.de/adxl345-teil-1 (German)
* https://wolles-elektronikkiste.de/en/adxl345-the-universal-accelerometer-part-1 (English)
* 
***************************************************************************/

#include<Wire.h>
#include<ADXL343_WE.h>
#define ADXL343_I2CADDR 0x53 // 0x1D if SDO = HIGH

/* There are several ways to create your ADXL343 object:
 * ADXL343_WE myAcc = ADXL343_WE()                -> uses Wire / I2C Address = 0x53
 * ADXL343_WE myAcc = ADXL343_WE(ADXL343_I2CADDR) -> uses Wire / ADXL343_I2CADDR
 * ADXL343_WE myAcc = ADXL343_WE(&wire2)          -> uses the TwoWire object wire2 / ADXL343_I2CADDR
 * ADXL343_WE myAcc = ADXL343_WE(&wire2, ADXL343_I2CADDR) -> all together
 */
ADXL343_WE myAcc = ADXL343_WE(ADXL343_I2CADDR);

void setup(){
  Wire.begin();
  Serial.begin(115200);
  Serial.println("ADXL343_Sketch - Basic Data");
  Serial.println();
  if(!myAcc.init()){
    Serial.println("ADXL343 not connected!");
  }
   
/* Choose the data rate         Hz
    ADXL343_DATA_RATE_3200    3200
    ADXL343_DATA_RATE_1600    1600
    ADXL343_DATA_RATE_800      800
    ADXL343_DATA_RATE_400      400
    ADXL343_DATA_RATE_200      200
    ADXL343_DATA_RATE_100      100
    ADXL343_DATA_RATE_50        50
    ADXL343_DATA_RATE_25        25
    ADXL343_DATA_RATE_12_5      12.5  
    ADXL343_DATA_RATE_6_25       6.25
    ADXL343_DATA_RATE_3_13       3.13
    ADXL343_DATA_RATE_1_56       1.56
    ADXL343_DATA_RATE_0_78       0.78
    ADXL343_DATA_RATE_0_39       0.39
    ADXL343_DATA_RATE_0_20       0.20
    ADXL343_DATA_RATE_0_10       0.10
*/
  myAcc.setDataRate(ADXL343_DATA_RATE_50);
  delay(100);
  Serial.print("Data rate: ");
  Serial.print(myAcc.getDataRateAsString());

/* In full resolution the size of the raw values depend on the range:
    2g = 10 bit; 4g = 11 bit; 8g = 12 bit; 16g =13 bit;
    uncomment to change to 10 bit for all ranges. 
 */
  // myAcc.setFullRes(false);

/* Choose the measurement range
    ADXL343_RANGE_16G    16g     
    ADXL343_RANGE_8G      8g     
    ADXL343_RANGE_4G      4g   
    ADXL343_RANGE_2G      2g
*/ 
  myAcc.setRange(ADXL343_RANGE_4G);
  Serial.print("  /  g-Range: ");
  Serial.println(myAcc.getRangeAsString());
  Serial.println();

/* Uncomment to enable Low Power Mode. It saves power but slightly
    increases noise. LowPower only affetcs Data Rates 12.5 Hz to 400 Hz.
*/
  // myAcc.setLowPower(true);
}

/* The LSB of the Data registers is 3.9 mg (milli-g, not milligramm).
    This value is used calculating g from raw. However, this is an ideal
    value which you might want to calibrate. 
*/

void loop() {
  xyzFloat raw;
  xyzFloat g;
  myAcc.getRawValues(&raw);
  myAcc.getGValues(&g);
  
  Serial.print("Raw-x = ");
  Serial.print(raw.x);
  Serial.print("  |  Raw-y = ");
  Serial.print(raw.y);
  Serial.print("  |  Raw-z = ");
  Serial.println(raw.z);

  Serial.print("g-x   = ");
  Serial.print(g.x);
  Serial.print("  |  g-y   = ");
  Serial.print(g.y);
  Serial.print("  |  g-z   = ");
  Serial.println(g.z);

  Serial.println();
  
  delay(1000);
  
}
