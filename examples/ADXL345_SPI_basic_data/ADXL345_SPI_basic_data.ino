/***************************************************************************
* Example sketch for the ADXL345_WE library
*
* This sketch shows how use SPI for the basic data sketch. Please apply the 
* same steps in the other sketches if you want to use SPI.  
*  
* Further information can be found on:
* https://wolles-elektronikkiste.de/adxl345-teil-1 (German)
* https://wolles-elektronikkiste.de/en/adxl345-the-universal-accelerometer-part-1 (English)
* 
***************************************************************************/

#include<ADXL345_WE.h>
#include<SPI.h>
#define CS_PIN 5   // Chip Select Pin
/* In case you want to change the default SPI pins (e.g. for ESP32), uncomment and adjust:
  #define MOSI_PIN 22
  #define MISO_PIN 17
  #define SCK_PIN 16
*/
bool spi = true;    // flag indicating that SPI shall be used

/* There are three ways to create your ADXL345 object in SPI mode:
 * ADXL345_WE myAcc = ADXL345_WE(CS_PIN, spi)     -> uses SPI, spi is just a flag, see SPI example
 * ADXL345_WE myAcc = ADXL345_WE(&SPI, CS_PIN, spi) -> uses SPI / passes the SPI object, spi is just a flag, see SPI example
 * ADXL345_WE myAcc = ADXL345_WE(&SPI, CS_PIN, spi, MOSI_PIN, MISO_PIN, SCK_PIN) -> like the latter, but also changes the SPI pins  
 */

ADXL345_WE myAcc = ADXL345_WE(CS_PIN, spi);

void setup(){
  Serial.begin(9600);
  Serial.println("ADXL345_Sketch - Basic Data");
  if(!myAcc.init()){
    Serial.println("ADXL345 not connected!");
  }

/* You can set the SPI clock speed. Default is 8 MHz. */
//  myAcc.setSPIClockSpeed(4000000);
   
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
  myAcc.setDataRate(ADXL345_DATA_RATE_12_5);
  delay(100);
  Serial.print("Data rate: ");
  Serial.print(myAcc.getDataRateAsString());

/* In full resolution the size of the raw values depend on the
    range: 2g = 10 bit; 4g = 11 bit; 8g = 12 bit; 16g =13 bit;
    uncomment to change to 10 bit for all ranges. 
 */
  // myAcc.setFullRes(false);

/* Choose the measurement range
    ADXL345_RANGE_16G    16g     
    ADXL345_RANGE_8G      8g     
    ADXL345_RANGE_4G      4g   
    ADXL345_RANGE_2G      2g
*/ 
  myAcc.setRange(ADXL345_RANGE_4G);
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
  xyzFloat raw = myAcc.getRawValues();
  xyzFloat g = myAcc.getGValues();
  
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
