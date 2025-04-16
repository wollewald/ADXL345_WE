/***************************************************************************
* Example sketch for the ADXL345_WE / ADXL343_WE library
*
* This sketch shows how use two devices with one SPI interface. 
*  
* Further information can be found on:
* https://wolles-elektronikkiste.de/adxl345-teil-1 (German)
* https://wolles-elektronikkiste.de/en/adxl345-the-universal-accelerometer-part-1 (English)
* 
***************************************************************************/

#include<ADXL345_WE.h>
#include<SPI.h>
#define CS_PIN_1 7
#define CS_PIN_2 6 
// const int sensorID_1 = 42;  // needed for Arduino UNO R4 Minima or WIFI
// const int sensorID_2 = 4242;  // needed for Arduino UNO R4 Minima or WIFI
bool spi = true;    // flag indicating that SPI shall be used

ADXL345_WE myAcc_1 = ADXL345_WE(&SPI, CS_PIN_1, spi);
ADXL345_WE myAcc_2 = ADXL345_WE(&SPI, CS_PIN_2, spi);

/* For Arduino UNO R4 boards choose these constructors */
// ADXL345_WE myAcc_1 = ADXL345_WE(&SPI, CS_PIN_1, spi, sensorID_1);
// ADXL345_WE myAcc_2 = ADXL345_WE(&SPI, CS_PIN_2, spi, sensorID_2);

void setup(){
  Serial.begin(115200);
  /* You can set the SPI clock speed. Default is 5 MHz. */
//   myAcc_1.setSPIClockSpeed(5000000);
//   myAcc_2.setSPIClockSpeed(5000000); 
  
  pinMode(CS_PIN_1, OUTPUT);
  digitalWrite(CS_PIN_1, HIGH);
  pinMode(CS_PIN_2, OUTPUT);
  digitalWrite(CS_PIN_2, HIGH);
  Serial.println("ADXL345_Sketch - Basic Data");
  if(!myAcc_1.init()){
    Serial.println("ADXL345 1 not connected!");
  }
  else{
    Serial.println("ADXL345 1: success!!");
  }
  delay(100);

  if(!myAcc_2.init()){
    Serial.println("ADXL345 2 not connected!");
  }
  else{
    Serial.println("ADXL345 2: success!!");
}
  delay(100);


   
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
  /*myAcc_1.setDataRate(ADXL345_DATA_RATE_12_5);
  delay(100);
  Serial.print("Data rate: ");
  Serial.print(myAcc_1.getDataRateAsString());*/

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
  /*myAcc_1.setRange(ADXL345_RANGE_4G);
  Serial.print("  /  g-Range: ");
  Serial.println(myAcc_1.getRangeAsString());
  Serial.println();*/

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
   xyzFloat g;
   myAcc_1.getGValues(&g);

  Serial.print("g-x   = ");
  Serial.print(g.x);
  Serial.print("  |  g-y   = ");
  Serial.print(g.y);
  Serial.print("  |  g-z   = ");
  Serial.println(g.z);

  myAcc_2.getGValues(&g);

  Serial.print("g-x   = ");
  Serial.print(g.x);
  Serial.print("  |  g-y   = ");
  Serial.print(g.y);
  Serial.print("  |  g-z   = ");
  Serial.println(g.z);

  Serial.println("*********************");
  
  delay(1000);
  
}
