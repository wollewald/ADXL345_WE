/***************************************************************************
* Example sketch for the ADXL345_WE library
*
* This sketch shows how to use sleep mode of the ADXL345 and its effect
*   
* Further information can be found on: 
* https://wolles-elektronikkiste.de/adxl345-teil-1 (German)
* https://wolles-elektronikkiste.de/en/adxl345-the-universal-accelerometer-part-1 (English)
* 
***************************************************************************/

#include<Wire.h>
#include<ADXL345_WE.h>
#define ADXL345_I2CADDR 0x53  // 0x1D if SDO = HIGH

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
  Serial.println("ADXL345_Sketch - Sleep");
  Serial.println();
  if(!myAcc.init()){
    Serial.println("ADXL345 not connected!");
  }

/* Insert your data from ADXL345_calibration.ino and uncomment for more precise results */
 // myAcc.setCorrFactors(-266.0, 285.0, -268.0, 278.0, -291.0, 214.0);
   
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

void loop(){
/* Switch on (true) or switch off (false) sleep mode. Choose the wake up 
    frequency:
    ADXL345_WUP_FQ_1  =  1 Hz
    ADXL345_WUP_FQ_2  =  2 Hz
    ADXL345_WUP_FQ_4  =  4 Hz 
    ADXL345_WUP_FQ_8  =  8 Hz

    In this specific example you will see (provided you move the ADXL345) that
    in sleep mode the values are updated every third to fourth request. In awake
    mode the values are updated steadily. Sleep Mode saves significant power.  
*/
  myAcc.setSleep(true, ADXL345_WUP_FQ_1);
  Serial.println("Measure in Sleep Mode:");
  doMeasurements();
  
  myAcc.setSleep(false);
  Serial.println("Measure in Normal Mode:");
  doMeasurements();
}
  

void doMeasurements(){
  for(int i=0; i<10; i++){
    xyzFloat g = myAcc.getGValues();
    
    Serial.print("g-x   = ");
    Serial.print(g.x);
    Serial.print("  |  g-y   = ");
    Serial.print(g.y);
    Serial.print("  |  g-z   = ");
    Serial.println(g.z);
    
    delay(300);
  }
}
