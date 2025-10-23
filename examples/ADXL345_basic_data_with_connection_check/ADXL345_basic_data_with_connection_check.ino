/***************************************************************************
* Example sketch for the ADXL345_WE / ADXL343_WE library
*
* This sketch shows how to get the connection status. Several functions return
* a bool value that indicates whether the ADXL345 is connected. But this only works
* with I2C, because it is using the ACK/NACK Bit. For SPI you can use isConnected(),
* which is less efficient because it triggers an extra read operation.
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
  Serial.println("ADXL345_Sketch - Basic Data");
  Serial.println();
  if(!myAcc.init()){
    Serial.println("ADXL345 not connected!");
  }
   
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
  delay(100);
  Serial.print("Data rate: ");
  Serial.print(myAcc.getDataRateAsString());

/* In full resolution the size of the raw values depend on the range:
    2g = 10 bit; 4g = 11 bit; 8g = 12 bit; 16g =13 bit;
    uncomment to change to 10 bit for all ranges. 
 */
  // if(!myAcc.setFullRes(false)){
  //  errorMessage();
  //}

/* Choose the measurement range
    ADXL345_RANGE_16G    16g     
    ADXL345_RANGE_8G      8g     
    ADXL345_RANGE_4G      4g   
    ADXL345_RANGE_2G      2g
*/ 
  if(!myAcc.setRange(ADXL345_RANGE_4G)){
    errorMessage();
  }
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
/* When using SPI, you can check with isConnected() whether the ADXL345 is still connected.
    The function simply tries to read the Device ID. Of course, it also works for I2C but
    it is less efficient than the I2C-checks, because isConnected() requires an additional 
    register read. It may impact your ability to achieve the highest data rates.
*/ 
  // if (!myAcc.isConnected()) {
  //   errorMessage();
  // }
  
  xyzFloat raw;
  xyzFloat g;
  if(!myAcc.getRawValues(&raw)){
    errorMessage();
  }
  if(!myAcc.getGValues(&g)){
    errorMessage();
  }
  
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

void errorMessage() {
  Serial.println("Connection lost to the ADX345. Please check your circuit.");
  // while(1){;} // uncomment if the sketch shall be stopped in case of connection issues
}
