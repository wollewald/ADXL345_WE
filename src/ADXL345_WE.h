/******************************************************************************
 *
 * This is a library for the ADXL345 accelerometer.
 *
 * You'll find several example sketches which should enable you to use the library. 
 *
 * You are free to use it, change it or build on it. In case you like it, it would
 * be cool if you give it a star.
 *
 * If you find bugs, please inform me!
 * 
 * Written by Wolfgang (Wolle) Ewald
 * https://wolles-elektronikkiste.de/adxl345-teil-1 (German)
 * https://wolles-elektronikkiste.de/en/adxl345-the-universal-accelerometer-part-1 (English)
 *
 * 
 ******************************************************************************/

#ifndef ADXL345_WE_H_
#define ADXL345_WE_H_

#if (ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <Wire.h>

/* registers */
#define ADXL345_ADDRESS          0x53
#define ADXL345_DEVID            0x00
#define ADXL345_THRESH_TAP       0x1D 
#define ADXL345_OFSX             0x1E
#define ADXL345_OFSY             0x1F
#define ADXL345_OFSZ             0x20
#define ADXL345_DUR              0x21
#define ADXL345_LATENT           0x22 
#define ADXL345_WINDOW           0x23 
#define ADXL345_THRESH_ACT       0x24
#define ADXL345_THRESH_INACT     0x25 
#define ADXL345_TIME_INACT       0x26 
#define ADXL345_ACT_INACT_CTL    0x27
#define ADXL345_THRESH_FF        0x28
#define ADXL345_TIME_FF          0x29 
#define ADXL345_TAP_AXES         0x2A
#define ADXL345_ACT_TAP_STATUS   0x2B
#define ADXL345_BW_RATE          0x2C
#define ADXL345_POWER_CTL        0x2D
#define ADXL345_INT_ENABLE       0x2E
#define ADXL345_INT_MAP          0x2F
#define ADXL345_INT_SOURCE       0x30
#define ADXL345_DATA_FORMAT      0x31
#define ADXL345_DATAX0           0x32
#define ADXL345_DATAX1           0x33
#define ADXL345_DATAY0           0x34
#define ADXL345_DATAY1           0x35
#define ADXL345_DATAZ0           0x36
#define ADXL345_DATAZ1           0x37
#define ADXL345_FIFO_CTL         0x38
#define ADXL345_FIFO_STATUS      0x39

/* Register bits */
#define ADXL345_FULL_RES		0x03
#define ADXL345_SUPPRESS		0x03
#define ADXL345_LOW_POWER 		0x04

/* Others */
#define MILLI_G_PER_LSB			 3.9f
#define UNITS_PER_G			  256.41f  // = 1/0.0039
#define INT_PIN_1 					1
#define INT_PIN_2					2
#define ADXL345_ACT_LOW				1
#define ADXL345_ACT_HIGH			0


typedef enum ADXL345_PWR_CTL {
	ADXL345_WAKE_UP_0, ADXL_WAKE_UP_1, ADXL345_SLEEP, 
	ADXL345_MEASURE, ADXL345_AUTO_SLEEP, ADXL345_LINK
} adxl345_pwrCtl;

typedef enum ADXL345_WAKE_UP { // belongs to POWER CTL
	ADXL345_WAKE_UP_FREQ_8,
	ADXL345_WAKE_UP_FREQ_4,
	ADXL345_WAKE_UP_FREQ_2,
	ADXL345_WAKE_UP_FREQ_1,
} adxl345_wakeUpFreq;

typedef enum ADXL345_DATA_RATE {
    ADXL345_DATA_RATE_3200    = 0x0F,
    ADXL345_DATA_RATE_1600    = 0x0E,
    ADXL345_DATA_RATE_800     = 0x0D,
    ADXL345_DATA_RATE_400     = 0x0C,
    ADXL345_DATA_RATE_200     = 0x0B,
    ADXL345_DATA_RATE_100     = 0x0A,
    ADXL345_DATA_RATE_50      = 0x09,
    ADXL345_DATA_RATE_25      = 0x08,
    ADXL345_DATA_RATE_12_5    = 0x07,
    ADXL345_DATA_RATE_6_25    = 0x06,
    ADXL345_DATA_RATE_3_13    = 0x05,
    ADXL345_DATA_RATE_1_56    = 0x04,
    ADXL345_DATA_RATE_0_78    = 0x03,
    ADXL345_DATA_RATE_0_39    = 0x02,
    ADXL345_DATA_RATE_0_20    = 0x01,
    ADXL345_DATA_RATE_0_10    = 0x00
} adxl345_dataRate;

typedef enum ADXL345_RANGE {
    ADXL345_RANGE_16G          = 0b11,
    ADXL345_RANGE_8G           = 0b10,
    ADXL345_RANGE_4G           = 0b01,
    ADXL345_RANGE_2G           = 0b00
} adxl345_range;

typedef enum ADXL345_ORIENTATION {
  FLAT, FLAT_1, XY, XY_1, YX, YX_1
} adxl345_orientation;

typedef enum ADXL345_INT {
	ADXL345_OVERRUN, ADXL345_WATERMARK, ADXL345_FREEFALL, ADXL345_INACTIVITY, 
	ADXL345_ACTIVITY, ADXL345_DOUBLE_TAP, ADXL345_SINGLE_TAP, ADXL345_DATA_READY
} adxl345_int;

typedef enum ADXL345_ACT_TAP_SET {
	ADXL345_000, ADXL345_00Z, ADXL345_0Y0, ADXL345_0YZ,
	ADXL345_X00, ADXL345_X0Z, ADXL345_XY0, ADXL345_XYZ
} adxl345_actTapSet;

typedef enum ADXL345_DC_AC {
	ADXL345_DC_MODE = 0,
	ADXL345_AC_MODE = 0x08
} adxl345_dcAcMode;

typedef enum ADXL345_WAKE_UP_FREQ{
	ADXL345_WUP_FQ_8, ADXL345_WUP_FQ_4, ADXL345_WUP_FQ_2, ADXL345_WUP_FQ_1
} adxl345_wUpFreq;

typedef enum ADXL345_ACT_TAP {
	ADXL345_TAP_Z, ADXL345_TAP_Y, ADXL345_TAP_X, ADXL345_ASLEEP, ADXL345_ACT_Z, ADXL345_ACT_Y, ADXL345_ACT_X
} adxl345_actTap;

typedef enum ADXL345_FIFO_MODE {
	ADXL345_BYPASS, ADXL345_FIFO, ADXL345_STREAM, ADXL345_TRIGGER
} adxl345_fifoMode;

typedef enum ADXL345_TRIGGER_INT {
	ADXL345_TRIGGER_INT_1, ADXL345_TRIGGER_INT_2
} adxl345_triggerInt;

struct xyzFloat {
    float x;
    float y;
    float z;
};


class ADXL345_WE
{
public:	
	/* Constructors */
	
	ADXL345_WE(int addr);
	ADXL345_WE();			//sets default I2C Address 0x53
	
	/* Basic settings */
	
	bool init();
	void setCorrFactors(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax);
	void setDataRate(adxl345_dataRate rate);
	adxl345_dataRate getDataRate();
	String getDataRateAsString();
	uint8_t getPowerCtlReg();
	void setRange(adxl345_range range);
	adxl345_range getRange();
	void setFullRes(boolean full);
	String getRangeAsString();
	
	/* x,y,z results */
		
	xyzFloat getRawValues();
	xyzFloat getCorrectedRawValues();
	xyzFloat getGValues();
	xyzFloat getAngles();
	xyzFloat getCorrAngles();
		
	/* Angles and Orientation */ 
	
	void measureAngleOffsets();
	adxl345_orientation getOrientation();
	String getOrientationAsString();
	float getPitch();
	float getRoll();
	
	/* Power, Sleep, Standby */ 
	
	bool setMeasureMode(bool measure);
	void setSleep(bool sleep, adxl345_wUpFreq freq);
	void setSleep(bool sleep);
	void setAutoSleep(bool autoSleep, adxl345_wUpFreq freq);
	void setAutoSleep(bool autoSleep);
	bool isAsleep();
	void setLowPower(bool lowpwr);
	
	/* Interrupts */
	
	void setInterrupt(adxl345_int type, uint8_t pin);
	void setInterruptPolarity(uint8_t pol);
	void deleteInterrupt(adxl345_int type);
	uint8_t readAndClearInterrupts();
	bool checkInterrupt(uint8_t source, adxl345_int type);
	void setLinkBit(bool link);
	void setFreeFallThresholds(float ffg, float fft);
	void setActivityParameters(adxl345_dcAcMode mode, adxl345_actTapSet axes, float threshold);
	void setInactivityParameters(adxl345_dcAcMode mode, adxl345_actTapSet axes, float threshold, uint8_t inactTime);
	void setGeneralTapParameters(adxl345_actTapSet axes, float threshold, float duration, float latent);
	void setAdditionalDoubleTapParameters(bool suppress, float window);
	uint8_t getActTapStatus();
	String getActTapStatusAsString();
	
	/* FIFO */
	
	void setFifoParameters(adxl345_triggerInt intNumber, uint8_t samples);
	void setFifoMode(adxl345_fifoMode mode);
	uint8_t getFifoStatus();
	void resetTrigger();
	
	
private:
	int i2cAddress;
	uint8_t regVal;   // intermediate storage of register values
	xyzFloat rawVal;
	xyzFloat gVal;
	xyzFloat angleVal;
	xyzFloat offsetVal;
	xyzFloat angleOffsetVal;
	xyzFloat corrFact;
	int16_t rangeFactor;
	uint8_t writeRegister(uint8_t reg, uint8_t val);
	uint8_t readRegister8(uint8_t reg);
	int16_t readRegister16(uint8_t reg);
	bool adxl345_lowRes;
};

#endif


