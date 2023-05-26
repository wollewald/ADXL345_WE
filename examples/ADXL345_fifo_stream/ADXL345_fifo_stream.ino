/***************************************************************************
* Example sketch for the ADXL345_WE library
*
* This sketch shows how to use the FIFO in stream mode.
*   
* Further information can be found on: 
* https://wolles-elektronikkiste.de/adxl345-teil-1 (German)
* https://wolles-elektronikkiste.de/en/adxl345-the-universal-accelerometer-part-1 (English)
* 
***************************************************************************/

#include<Wire.h>
#include<ADXL345_WE.h>
#define ADXL345_I2CADDR 0x53  // 0x1D if SDO = HIGH
const int int2Pin = 2;
volatile bool event = false;

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
  Serial.println("ADXL345_Sketch - FIFO - Stream Mode");
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
  myAcc.setDataRate(ADXL345_DATA_RATE_12_5);
  Serial.print("Data rate: ");
  Serial.print(myAcc.getDataRateAsString());

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

  attachInterrupt(digitalPinToInterrupt(int2Pin), eventISR, RISING);

 /* Three parameters have to be set for activity:
    1. DC / AC Mode:
        ADXL345_DC_MODE - Threshold is the defined one (parameter 3)
        ADXL345_AC_MODE - Threshold = starting acceleration + defined threshold
    2. Axes, that are considered:
        ADXL345_000  -  no axis (which makes no sense)
        ADXL345_00Z  -  z 
        ADXL345_0Y0  -  y
        ADXL345_0YZ  -  y,z
        ADXL345_X00  -  x
        ADXL345_X0Z  -  x,z
        ADXL345_XY0  -  x,y
        ADXL345_XYZ  -  all axes
    3. Threshold in g
*/
  myAcc.setActivityParameters(ADXL345_DC_MODE, ADXL345_XY0, 0.6);

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
  myAcc.setInterrupt(ADXL345_ACTIVITY, INT_PIN_2);

/* The following two FIFO parameters need to be set:
    1. Trigger Bit: not relevant for this FIFO mode.
    2. FIFO samples (max 32). Defines the size of the FIFO. One sample is an x,y,z triple.
*/
  myAcc.setFifoParameters(ADXL345_TRIGGER_INT_1, 32);
 
 /* You can choose the following FIFO modes:
    ADXL345_FIFO     -  you choose the start, ends when FIFO is full (at defined limit)
    ADXL345_STREAM   -  FIFO always filled with new data, old data replaced if FIFO is full; you choose the stop
    ADXL345_TRIGGER  -  FIFO always filled up to 32 samples; when the trigger event occurs only defined number of samples
                        is kept in the FIFO and further samples are taken after the event until FIFO is full again. 
    ADXL345_BYPASS   -  no FIFO
*/     
  myAcc.setFifoMode(ADXL345_STREAM);
  
  myAcc.readAndClearInterrupts();

  Serial.println("Waiting for Activity Event - I propose to slowly turn the ADXL345");
}

/* In the main loop some checks are done:
    getActTapStatus() returns which axes are responsible for activity interrupt as byte (code in library)
    getActTapStatusAsString() returns the axes that caused the interrupt as string
    readAndClearInterrupts(); returns the interrupt type as byte (code in library) 
    checkInterrupt(intSource, type) returns if intSource is type as bool  
*/

void loop() {
  if(event){
    myAcc.setMeasureMode(false);
    //byte actTapSource = myAcc.getActTapStatus();
    //Serial.println(actTapSource, BIN);
    String axes = myAcc.getActTapStatusAsString();
    byte intSource = myAcc.readAndClearInterrupts();
    if(myAcc.checkInterrupt(intSource, ADXL345_ACTIVITY)){
        Serial.print("ACT at: ");
        Serial.println(axes);
    }
    printFifo();
    Serial.println("For another series of measurements, enter any key and send");
    while(!(Serial.available())){}
    Serial.read();
    Serial.println();
    Serial.println("Waiting for Activity Event");
  
    event = false;
    myAcc.readAndClearInterrupts();
    myAcc.setMeasureMode(true);
  }
}

void eventISR() {
  event = true;
}

void printFifo(){
  for(int i=0; i<32; i++){
    xyzFloat g = myAcc.getGValues();
    
    Serial.print("g-x   = ");
    Serial.print(g.x);
    Serial.print("  |  g-y   = ");
    Serial.print(g.y);
    Serial.print("  |  g-z   = ");
    Serial.println(g.z);
  }
}
