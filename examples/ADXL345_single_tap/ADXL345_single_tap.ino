/***************************************************************************
* Example sketch for the ADXL345_WE library
*
* This sketch shows how to use the single tap function.
*   
* Further information can be found on: 
* https://wolles-elektronikkiste.de/adxl345-teil-1 (German)
* https://wolles-elektronikkiste.de/en/adxl345-the-universal-accelerometer-part-1 (English)
* 
***************************************************************************/
#include<Wire.h>
#include<ADXL345_WE.h>
#define ADXL345_I2CADDR 0x53 // 0x1D if SDO = HIGH
const int int2Pin = 2;
volatile bool tap = false;

/* There are several ways to create your ADXL345 object:
 * ADXL345_WE myAcc = ADXL345_WE()                -> uses Wire / I2C Address = 0x53
 * ADXL345_WE myAcc = ADXL345_WE(ADXL345_I2CADDR) -> uses Wire / ADXL345_I2CADDR
 * ADXL345_WE myAcc = ADXL345_WE(&wire2)          -> uses the TwoWire object wire2 / ADXL345_I2CADDR
 * ADXL345_WE myAcc = ADXL345_WE(&wire2, ADXL345_I2CADDR) -> all together
 */
ADXL345_WE myAcc = ADXL345_WE(ADXL345_I2CADDR);

void setup() {
  Wire.begin();
  Serial.begin(9600);
  pinMode(int2Pin, INPUT);
  Serial.println("ADXL345_Sketch - Single Tap Interrupt");
  Serial.println();
  if (!myAcc.init()) {
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
  myAcc.setDataRate(ADXL345_DATA_RATE_200);
  Serial.print("Data rate: ");
  Serial.print(myAcc.getDataRateAsString());

/* Choose the measurement range
    ADXL345_RANGE_16G    16g     
    ADXL345_RANGE_8G      8g     
    ADXL345_RANGE_4G      4g   
    ADXL345_RANGE_2G      2g
*/ 
  myAcc.setRange(ADXL345_RANGE_8G);
  Serial.print("  /  g-Range: ");
  Serial.println(myAcc.getRangeAsString());
  Serial.println();

  attachInterrupt(digitalPinToInterrupt(int2Pin), tapISR, RISING);
  
/* The following four parameters have to be set for tap application (single and double):
    1. Axes, that are considered:
        ADXL345_000  -  no axis (which makes no sense)
        ADXL345_00Z  -  z 
        ADXL345_0Y0  -  y
        ADXL345_0YZ  -  y,z
        ADXL345_X00  -  x
        ADXL345_X0Z  -  x,z
        ADXL345_XY0  -  x,y
        ADXL345_XYZ  -  all axes
    2. Threshold in g
        It is recommended to not choose the value to low. 3g is a good starting point. 
    3. Duration in milliseconds (max 159 ms): 
        maximum time that the acceleration must be over g threshold to be regarded as a single tap. If 
        the acceleration drops below the g threshold before the duration is exceeded an interrupt will be 
        triggered. If also double tap is active an interrupt will only be triggered after the double tap 
        conditions have been checked. Duration should be greater than 10. 
    4. Latency time in milliseconds (maximum: 318 ms): minimum time before the next tap can be detected.
        Starts at the end of duration or when the interrupt was triggered. Should be greater than 20 ms.  
*/
  myAcc.setGeneralTapParameters(ADXL345_XY0, 3.0, 30, 100.0);
  
/* You can choose the following interrupts:
     Variable name:             Triggered, if:
    ADXL345_OVERRUN      -   new data replaces unread data
    ADXL345_WATERMARK    -   the number of samples in FIFO equals the number defined in FIFO_CTL
    ADXL345_FREEFALL     -   acceleration values of all axes are below the threshold defined in THRESH_FF 
    ADXL345_INACTIVITY   -   acc. value of all included axes are < THRESH_INACT for period > TIME_INACT
    ADXL345_ACTIVITY     -   acc. value of included axes are > THRESH_ACT
    ADXL345_DOUBLE_TAP   -   double tap detected on one incl. axis and various defined conditions are met
    ADXL345_SINGLE_TAP   -   single tap detected on one incl. axis and various defined conditions are met
    ADXL345_DATA_READY   -   new data available

    Assign the interrupts to INT1 (INT_PIN_1) or INT2 (INT_PIN_2). Data ready, watermark and overrun are 
    always enabled. You can only change the assignment of these which is INT1 by default.

    You can delete interrupts with deleteInterrupt(type);
*/ 
  myAcc.setInterrupt(ADXL345_SINGLE_TAP, INT_PIN_2); 
  myAcc.readAndClearInterrupts();
}

/* In the main loop some checks are done:
    getActTapStatus() returns which axes are resposible activity interrupt as byte (code in library)
    getActTapStatusAsString() returns the axes that caused the interrupt as string
    readAndClearInterrupts(); return the interrupt type as byte (code in library) 
    checkInterrupt(intSource, type) returns if intSource is type as bool  
*/
void loop() {
  if ((millis() % 1000) == 1) {
    xyzFloat g = myAcc.getGValues();
    Serial.print("g-x   = ");
    Serial.print(g.x);
    Serial.print("  |  g-y   = ");
    Serial.print(g.y);
    Serial.print("  |  g-z   = ");
    Serial.println(g.z);
  }

  if(tap == true) {
      //byte actTapSource = myAcc.getActTapStatus();
      //Serial.println(actTapSource, BIN);
      String axes = myAcc.getActTapStatusAsString();
      byte intSource = myAcc.readAndClearInterrupts();
      if(myAcc.checkInterrupt(intSource, ADXL345_SINGLE_TAP)){
        Serial.print("TAP at: ");
        Serial.println(axes);
      }
    
    delay(1000);
    myAcc.readAndClearInterrupts();
    tap = false;
  }
}

void tapISR() {
  tap = true;
}
