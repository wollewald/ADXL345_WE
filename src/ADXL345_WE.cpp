/********************************************************************
* This is a library for the ADXL345 / ADXL343 accelerometer.
*
* You'll find an example which should enable you to use the library. 
*
* You are free to use it, change it or build on it. In case you like 
* it, it would be cool if you give it a star.
* 
* If you find bugs, please inform me!
* 
* Written by Wolfgang (Wolle) Ewald
* https://wolles-elektronikkiste.de/adxl345-teil-1 (German)
* https://wolles-elektronikkiste.de/en/adxl345-the-universal-accelerometer-part-1 (English)
*
*********************************************************************/

#include "ADXL345_WE.h"

/************ Basic settings ************/
    
bool ADXL345_WE::init(){    
    if(useSPI){
        if(mosiPin == 999){
            _spi->begin();
        }
#ifdef ESP32
        else{
            _spi->begin(sckPin, misoPin, mosiPin, csPin);
        }
#endif
#ifdef ARDUINO_ARCH_STM32
    else {
       _spi->setMISO(misoPin);
       _spi->setMOSI(mosiPin);
       _spi->setSCLK(sckPin);
       _spi->begin();
    }
#endif
        setSPIClockSpeed(spiClock);
        pinMode(csPin, OUTPUT);
        digitalWrite(csPin, HIGH);
    }
    writeRegister(ADXL345_POWER_CTL, 0);
    writeRegister(ADXL345_POWER_CTL, 16);   
    setMeasureMode(true);
    rangeFactor = 1.0;
    corrFact = {1.0, 1.0, 1.0};
    offsetVal = {0.0, 0.0, 0.0};
    angleOffsetVal = {0.0, 0.0, 0.0};
    writeRegister(ADXL345_DATA_FORMAT,0); 
    setFullRes(true); 
    uint8_t ctrlVal; 
    bool ok = readRegister8(ADXL345_DATA_FORMAT, &ctrlVal);
    if(!ok || ctrlVal != 0b1000){
        return false;
    }
    writeRegister(ADXL345_INT_ENABLE, 0);
    writeRegister(ADXL345_INT_MAP,0);
    writeRegister(ADXL345_TIME_INACT, 0);
    writeRegister(ADXL345_THRESH_INACT,0);
    writeRegister(ADXL345_ACT_INACT_CTL, 0);
    writeRegister(ADXL345_DUR,0);
    writeRegister(ADXL345_LATENT,0);
    writeRegister(ADXL345_THRESH_TAP,0);
    writeRegister(ADXL345_TAP_AXES,0);
    writeRegister(ADXL345_WINDOW, 0);
    readAndClearInterrupts();
    writeRegister(ADXL345_FIFO_CTL,0);
    writeRegister(ADXL345_FIFO_STATUS,0);
     
    return true;
}

void ADXL345_WE::setSPIClockSpeed(unsigned long clock = 5000000){
    spiClock = clock;
    mySPISettings = SPISettings(spiClock, MSBFIRST, SPI_MODE3);
}

void ADXL345_WE::setCorrFactors(float xMin, float xMax, float yMin, float yMax, float zMin, float zMax){
    corrFact.x = UNITS_PER_G / (0.5 * (xMax - xMin));
    corrFact.y = UNITS_PER_G / (0.5 * (yMax - yMin));
    corrFact.z = UNITS_PER_G / (0.5 * (zMax - zMin));
    offsetVal.x = (xMax + xMin) * 0.5;
    offsetVal.y = (yMax + yMin) * 0.5;
    offsetVal.z = (zMax + zMin) * 0.5;
}

bool ADXL345_WE::setDataRate(adxl345_dataRate rate){
    if (!readRegister8(ADXL345_BW_RATE, &regVal) || rate == ADXL345_DATA_RATE_ERROR) {
        return false;
    }
    regVal &= 0xF0;
    regVal |= rate;
    writeRegister(ADXL345_BW_RATE, regVal);
    return true;
}
    
adxl345_dataRate ADXL345_WE::getDataRate(){
    if (!readRegister8(ADXL345_BW_RATE, &regVal)) {
        return ADXL345_DATA_RATE_ERROR;
    }
    return (adxl345_dataRate)(regVal & 0x0F);
}


String ADXL345_WE::getDataRateAsString(){
    adxl345_dataRate dataRate = getDataRate();    
    switch(dataRate) {
        case ADXL345_DATA_RATE_ERROR: return(F("ERROR")); break;
        case ADXL345_DATA_RATE_3200: return(F("3200 Hz")); break;
        case ADXL345_DATA_RATE_1600: return(F("1600 Hz")); break;
        case ADXL345_DATA_RATE_800:  return(F("800 Hz"));  break;
        case ADXL345_DATA_RATE_400:  return(F("400 Hz"));  break;
        case ADXL345_DATA_RATE_200:  return(F("200 Hz"));  break;
        case ADXL345_DATA_RATE_100:  return(F("100 Hz"));  break;
        case ADXL345_DATA_RATE_50:   return(F("50 Hz"));   break;
        case ADXL345_DATA_RATE_25:   return(F("25 Hz"));   break;
        case ADXL345_DATA_RATE_12_5: return(F("12.5 Hz")); break;
        case ADXL345_DATA_RATE_6_25: return(F("6.25 Hz")); break;
        case ADXL345_DATA_RATE_3_13: return(F("3.13 Hz")); break;
        case ADXL345_DATA_RATE_1_56: return(F("1.56 Hz")); break;
        case ADXL345_DATA_RATE_0_78: return(F("0.78 Hz")); break;
        case ADXL345_DATA_RATE_0_39: return(F("0.39 Hz")); break;
        case ADXL345_DATA_RATE_0_20: return(F("0.20 Hz")); break;
        case ADXL345_DATA_RATE_0_10: return(F("0.10 Hz")); break;
        default: return(F("unknown"));
    }
}

bool ADXL345_WE::setRange(adxl345_range range){
    if (!readRegister8(ADXL345_DATA_FORMAT, &regVal) || range == ADXL345_RANGE_ERROR) {
        return false;
    }
    if(adxl345_lowRes){
        switch(range){
            case ADXL345_RANGE_ERROR: return false; break; // Already handled, but avoids compiler warning
            case ADXL345_RANGE_2G:  rangeFactor = 1.0;  break;
            case ADXL345_RANGE_4G:  rangeFactor = 2.0;  break;
            case ADXL345_RANGE_8G:  rangeFactor = 4.0;  break;
            case ADXL345_RANGE_16G: rangeFactor = 8.0;  break;  
        }
    }
    else{
        rangeFactor = 1.0;
    }
    regVal &= 0b11111100;
    regVal |= range;
    writeRegister(ADXL345_DATA_FORMAT, regVal);
    return true;
}

adxl345_range ADXL345_WE::getRange(){
    if (!readRegister8(ADXL345_DATA_FORMAT, &regVal)) {
        return ADXL345_RANGE_ERROR;
    }
    regVal &= 0x03; 
    return adxl345_range(regVal);
}

bool ADXL345_WE::setFullRes(bool full){
    if (!readRegister8(ADXL345_DATA_FORMAT, &regVal)) {
        return false;
    }
    if(full){
        adxl345_lowRes = false;
        rangeFactor = 1.0;
        regVal |= (1<<ADXL345_FULL_RES);
    }
    else{
        adxl345_lowRes = true;
        regVal &= ~(1<<ADXL345_FULL_RES);
        if (!setRange(getRange())) {
            return false;
        }
    }
    writeRegister(ADXL345_DATA_FORMAT, regVal);
    return true;
}

String ADXL345_WE::getRangeAsString(){
    adxl345_range range = getRange();
    switch(range){
        case ADXL345_RANGE_ERROR: return(F("ERROR")); break;
        case ADXL345_RANGE_2G:  return(F("2g"));   break;
        case ADXL345_RANGE_4G:  return(F("4g"));   break;
        case ADXL345_RANGE_8G:  return(F("8g"));   break;
        case ADXL345_RANGE_16G: return(F("16g"));  break;
        default: return(F("unknown"));
        
    }
}

uint8_t ADXL345_WE::getDeviceID(){
    if (readRegister8(ADXL345_DEVID, &regVal)) {
        return regVal;
    } else {
        return 0;
    }
}

bool ADXL345_WE::isConnected(){
    if (getDeviceID() == ADXL345_DEVICE_ID_VAL){
        return true;
    } else {
        return false;
    }
}

/************ x,y,z results ************/

bool ADXL345_WE::getRawValues(xyzFloat *rawVal){
    uint8_t rawData[6]; 
    if (!readMultipleRegisters(ADXL345_DATAX0, 6, rawData)) {
        return false;
    }
    rawVal->x = (static_cast<int16_t>((rawData[1] << 8) | rawData[0])) * 1.0;
    rawVal->y = (static_cast<int16_t>((rawData[3] << 8) | rawData[2])) * 1.0;
    rawVal->z = (static_cast<int16_t>((rawData[5] << 8) | rawData[4])) * 1.0;
    return true;
}

bool ADXL345_WE::getCorrectedRawValues(xyzFloat *rawVal){
    if (!getRawValues(rawVal)) {
        return false;
    }
    rawVal->x -= (offsetVal.x / rangeFactor);
    rawVal->y -= (offsetVal.y / rangeFactor);
    rawVal->z -= (offsetVal.z / rangeFactor);
    return true;
}

bool ADXL345_WE::getGValues(xyzFloat *gVal){
    if (!getCorrectedRawValues(gVal)) {
        return false;
    }
    *gVal *= corrFact * MILLI_G_PER_LSB * rangeFactor / 1000.0; 
    return true;
}

/************ Angles and Orientation ************/ 

bool ADXL345_WE::getAngles(xyzFloat *angleVal){
    xyzFloat gVal;
    if (!getGValues(&gVal)) {
        return false;
    }
    if(gVal.x > 1){
        gVal.x = 1;
    }
    else if(gVal.x < -1){
        gVal.x = -1;
    }
    angleVal->x = (asin(gVal.x)) * 57.296;
    
    if(gVal.y > 1){
        gVal.y = 1;
    }
    else if(gVal.y < -1){
        gVal.y = -1;
    }
    angleVal->y = (asin(gVal.y)) * 57.296;
    
    if(gVal.z > 1){
        gVal.z = 1;
    }
    else if(gVal.z < -1){
        gVal.z = -1;
    }
    angleVal->z = (asin(gVal.z)) * 57.296;
    return true;
}

bool ADXL345_WE::getCorrAngles(xyzFloat *corrAngleVal){
    if (!getAngles(corrAngleVal)) {
        return false;
    }
    *corrAngleVal -= angleOffsetVal;
    return true;
}

bool ADXL345_WE::measureAngleOffsets(){
    return getAngles(&angleOffsetVal);
}

xyzFloat ADXL345_WE::getAngleOffsets(){
    return angleOffsetVal;
}

void ADXL345_WE::setAngleOffsets(const xyzFloat aos){
    angleOffsetVal = aos;
}

adxl345_orientation ADXL345_WE::getOrientation(){
    adxl345_orientation orientation = FLAT;
    xyzFloat angleVal;
    if (!getAngles(&angleVal)) {
        return ADXL345_ORIENTATION_ERROR;
    }
    if(abs(angleVal.x) < 45){      // |x| < 45
        if(abs(angleVal.y) < 45){      // |y| < 45
            if(angleVal.z > 0){          //  z  > 0
                orientation = FLAT;
            }
            else{                        //  z  < 0
                orientation = FLAT_1;
            }
        }
        else{                         // |y| > 45 
            if(angleVal.y > 0){         //  y  > 0
                orientation = XY;
            }
            else{                       //  y  < 0
                orientation = XY_1;   
            }
        }
    }
    else{                           // |x| >= 45
        if(angleVal.x > 0){           //  x  >  0
            orientation = YX;       
        }
        else{                       //  x  <  0
            orientation = YX_1;
        }
    }
    return orientation;
}

String ADXL345_WE::getOrientationAsString(){
    adxl345_orientation orientation = getOrientation();
    String orientationAsString = "";
    switch(orientation){
        case ADXL345_ORIENTATION_ERROR: orientationAsString = "ERROR"; break;
        case FLAT:      orientationAsString = "z up";   break;
        case FLAT_1:    orientationAsString = "z down"; break;
        case XY:        orientationAsString = "y up";   break;
        case XY_1:      orientationAsString = "y down"; break;
        case YX:        orientationAsString = "x up";   break;
        case YX_1:      orientationAsString = "x down"; break;
    }
    return orientationAsString;
}

float ADXL345_WE::getPitch(){
    xyzFloat gVal;
    if (!getGValues(&gVal)) {
        return ADXL345_FLOAT_ERROR;
    }
    float pitch = (atan2(-gVal.x, sqrt(abs((gVal.y*gVal.y + gVal.z*gVal.z))))*180.0)/M_PI;
    return pitch;
}
    
float ADXL345_WE::getRoll(){
    xyzFloat gVal;
    if (!getGValues(&gVal)) {
        return ADXL345_FLOAT_ERROR;
    }
    float roll = (atan2(gVal.y, gVal.z)*180.0)/M_PI;
    return roll;
}

/************ Power, Sleep, Standby ************/ 

bool ADXL345_WE::setMeasureMode(bool measure){
    if (!readRegister8(ADXL345_POWER_CTL, &regVal)) {
        return false;
    }
    if(measure){
        regVal |= (1<<ADXL345_MEASURE);
    }
    else{
        regVal &= ~(1<<ADXL345_MEASURE);
    }
    writeRegister(ADXL345_POWER_CTL, regVal);
    return true;
}

bool ADXL345_WE::setSleep(bool sleep, adxl345_wUpFreq freq){
    if (!readRegister8(ADXL345_POWER_CTL, &regVal)) {
        return false;
    }
    if (freq != ADXL345_WUP_FQ_UNSET) {
        regVal &= 0b11111100;
        regVal |= freq;
    }
    if(sleep){
        regVal |= (1<<ADXL345_SLEEP);
    }
    else{
        // it is recommended to enter Stand Mode when clearing the Sleep Bit!
        if (!setMeasureMode(false)) {
            return false;
        }
        regVal &= ~(1<<ADXL345_SLEEP);
        regVal &= ~(1<<ADXL345_MEASURE);
    }
    writeRegister(ADXL345_POWER_CTL, regVal);
    if(!sleep){
        setMeasureMode(true); // No return check here as the setting has been changed already
    }
    return true;
}
    
bool ADXL345_WE::setAutoSleep(bool autoSleep, adxl345_wUpFreq freq){
    if (!readRegister8(ADXL345_POWER_CTL, &regVal)) {
        return false;
    }
    if(autoSleep){
        // Both AUTO_SLEEP and LINK bits must be set
        regVal |= (1<<ADXL345_AUTO_SLEEP) | (1<<ADXL345_LINK);
    } else {
        // The AUTO_SLEEP bit is cleared, but leave the LINK bit alone in case set by something else
        regVal &= ~(1<<ADXL345_AUTO_SLEEP);
    }
    if (freq != ADXL345_WUP_FQ_UNSET) {
        regVal &= 0b11111100;
        regVal |= freq;
    }
    writeRegister(ADXL345_POWER_CTL, regVal);
    return true;
}
        
bool ADXL345_WE::isAsleep(){
    if (!readRegister8(ADXL345_ACT_TAP_STATUS, &regVal)) {
        return false; // Not ideal
    }
    return regVal & (1<<ADXL345_ASLEEP);
}

bool ADXL345_WE::setLowPower(bool lowpwr){
    if (!readRegister8(ADXL345_BW_RATE, &regVal)) {
        return false;
    }
    if(lowpwr){
        regVal |= (1<<ADXL345_LOW_POWER);
    }
    else{
        regVal &= ~(1<<ADXL345_LOW_POWER);
    }
    writeRegister(ADXL345_BW_RATE, regVal);
    return true;
}

bool ADXL345_WE::isLowPower(){
    if (!readRegister8(ADXL345_BW_RATE, &regVal)) {
        return false; // Not ideal
    }
    return regVal & (1<<ADXL345_LOW_POWER);
}
            
/************ Interrupts ************/


bool ADXL345_WE::setInterrupt(adxl345_int type, uint8_t pin){
    if (!readRegister8(ADXL345_INT_ENABLE, &regVal)) {
        return false;
    }
    regVal |= (1<<type);
    writeRegister(ADXL345_INT_ENABLE, regVal);
    if (!readRegister8(ADXL345_INT_MAP, &regVal)) {
        return false;
    }
    if(pin == INT_PIN_1){
        regVal &= ~(1<<type);
    }
    else {
        regVal |= (1<<type);
    }
    writeRegister(ADXL345_INT_MAP, regVal);
    return true;
}

bool ADXL345_WE::setInterruptPolarity(uint8_t pol){
    if (!readRegister8(ADXL345_DATA_FORMAT, &regVal)) {
        return false;
    }
    if(pol == ADXL345_ACT_HIGH){
        regVal &= ~(0b00100000);
    }
    else if(pol == ADXL345_ACT_LOW){
        regVal |= 0b00100000;
    }
    writeRegister(ADXL345_DATA_FORMAT, regVal);
    return true;
}

bool ADXL345_WE::deleteInterrupt(adxl345_int type){
    if (!readRegister8(ADXL345_INT_ENABLE, &regVal)) {
        return false;
    }
    regVal &= ~(1<<type);
    writeRegister(ADXL345_INT_ENABLE, regVal);
    return true;
}

uint8_t ADXL345_WE::readAndClearInterrupts(){
    if (!readRegister8(ADXL345_INT_SOURCE, &regVal)) {
        return 0; // Not ideal
    }
    return regVal;
}

bool ADXL345_WE::checkInterrupt(uint8_t source, adxl345_int type){
    return source & (1<<type);
}

bool ADXL345_WE::setLinkBit(bool link){
    if (!readRegister8(ADXL345_POWER_CTL, &regVal)) {
        return false;
    }
    if(link){
        regVal |= (1<<ADXL345_LINK);
    }
    else{
        regVal &= ~(1<<ADXL345_LINK);
    }
    writeRegister(ADXL345_POWER_CTL, regVal);
    return true;
}

void ADXL345_WE::setFreeFallThresholds(float ffg, float fft){
    regVal = static_cast<uint8_t>(round(ffg / 0.0625));
    if(regVal<1){
        regVal = 1;
    }
    writeRegister(ADXL345_THRESH_FF, regVal);
    regVal = static_cast<uint8_t>(round(fft / 5));
    if(regVal<1){
        regVal = 1;
    }
    writeRegister(ADXL345_TIME_FF, regVal);
}

bool ADXL345_WE::setActivityParameters(adxl345_dcAcMode mode, adxl345_actTapSet axes, float threshold){
    regVal = static_cast<uint8_t>(round(threshold / 0.0625));
    if(regVal<1){
        regVal = 1;
    }
    
    writeRegister(ADXL345_THRESH_ACT, regVal);

    if (!readRegister8(ADXL345_ACT_INACT_CTL, &regVal)) {
        return false;
    }
    regVal &= 0x0F;
    regVal |= (static_cast<uint8_t>(mode) + static_cast<uint8_t>(axes))<<4;
    writeRegister(ADXL345_ACT_INACT_CTL, regVal);
    return true;
}

bool ADXL345_WE::setInactivityParameters(adxl345_dcAcMode mode, adxl345_actTapSet axes, float threshold, uint8_t inactTime){
    regVal = static_cast<uint8_t>(round(threshold / 0.0625));
    if(regVal<1){
        regVal = 1;
    }
    writeRegister(ADXL345_THRESH_INACT, regVal);
    writeRegister(ADXL345_TIME_INACT, inactTime);

    if (!readRegister8(ADXL345_ACT_INACT_CTL, &regVal)) {
        return false;
    }
    regVal &= 0xF0;
    regVal |= static_cast<uint8_t>(mode) + static_cast<uint16_t>(axes);
    writeRegister(ADXL345_ACT_INACT_CTL, regVal);
    return true;
}

bool ADXL345_WE::setGeneralTapParameters(adxl345_actTapSet axes, float threshold, float duration, float latent){
    if (!readRegister8(ADXL345_TAP_AXES, &regVal)) {
        return false;
    }
    regVal &= 0b11111000;
    regVal |= static_cast<uint8_t>(axes);
    writeRegister(ADXL345_TAP_AXES, regVal);
    
    regVal = static_cast<uint8_t>(round(threshold / 0.0625));
    if(regVal<1){
        regVal = 1;
    }
    writeRegister(ADXL345_THRESH_TAP,regVal);
    
    regVal = static_cast<uint8_t>(round(duration / 0.625));
    if(regVal<1){
        regVal = 1;
    }
    writeRegister(ADXL345_DUR, regVal);
    
    regVal = static_cast<uint8_t>(round(latent / 1.25));
    if(regVal<1){
        regVal = 1;
    }
    writeRegister(ADXL345_LATENT, regVal);     
    return true; 
}

bool ADXL345_WE::setAdditionalDoubleTapParameters(bool suppress, float window){
    if (!readRegister8(ADXL345_TAP_AXES, &regVal)) {
        return false;
    }
    if(suppress){
        regVal |= (1<<ADXL345_SUPPRESS);
    }
    else{
        regVal &= ~(1<<ADXL345_SUPPRESS);
    }
    writeRegister(ADXL345_TAP_AXES, regVal);
    
    regVal = static_cast<uint8_t>(round(window / 1.25));
    writeRegister(ADXL345_WINDOW, regVal);
    return true;
}

uint8_t ADXL345_WE::getActTapStatus(){
    if (!readRegister8(ADXL345_ACT_TAP_STATUS, &regVal)) {
        return 0; // Not ideal
    }
    return regVal;
}

String ADXL345_WE::getActTapStatusAsString(){
    if (!readRegister8(ADXL345_ACT_TAP_STATUS, &regVal)) {
        return String("ERROR");
    }
    String returnStr = "";
    if(regVal & (1<<ADXL345_TAP_Z)) { returnStr += "TAP-Z "; }
    if(regVal & (1<<ADXL345_TAP_Y)) { returnStr += "TAP-Y "; }
    if(regVal & (1<<ADXL345_TAP_X)) { returnStr += "TAP-X "; }
    if(regVal & (1<<ADXL345_ACT_Z)) { returnStr += "ACT-Z "; }
    if(regVal & (1<<ADXL345_ACT_Y)) { returnStr += "ACT-Y "; }
    if(regVal & (1<<ADXL345_ACT_X)) { returnStr += "ACT-X "; }
    
    return returnStr;
}

/************ FIFO ************/

bool ADXL345_WE::setFifoParameters(adxl345_triggerInt intNumber, uint8_t samples){
    if (!readRegister8(ADXL345_FIFO_CTL, &regVal)) {
        return false;
    }
    regVal &= 0b11000000;
    regVal |= (samples-1);
    if(intNumber == ADXL345_TRIGGER_INT_2){
        regVal |= 0x20;
    }
    writeRegister(ADXL345_FIFO_CTL, regVal);
    return true;
}

bool ADXL345_WE::setFifoMode(adxl345_fifoMode mode){
    if (!readRegister8(ADXL345_FIFO_CTL, &regVal)) {
        return false;
    }
    regVal &= 0b00111111;
    regVal |= (mode<<6);
    writeRegister(ADXL345_FIFO_CTL,regVal);
    return true;
}

uint8_t ADXL345_WE::getFifoStatus(){
    if (!readRegister8(ADXL345_FIFO_STATUS, &regVal)) {
        return 0; // Not ideal
    }
    return regVal;
}

bool ADXL345_WE::resetTrigger(){
    return setFifoMode(ADXL345_BYPASS) && setFifoMode(ADXL345_TRIGGER);
}


/************************************************ 
    private functions
*************************************************/

void ADXL345_WE::writeRegister(uint8_t reg, uint8_t val){
    if(!useSPI){
        _wire->beginTransmission(i2cAddress);
        _wire->write(reg);
        _wire->write(val);
        _wire->endTransmission();    
    }
    else{
        _spi->beginTransaction(mySPISettings);
        digitalWrite(csPin, LOW);
        delayMicroseconds(5);
        _spi->transfer(reg); 
        _spi->transfer(val);
        digitalWrite(csPin, HIGH);
        _spi->endTransaction();
    }
}
  
bool ADXL345_WE::readRegister8(uint8_t reg, uint8_t *val){
    if(!useSPI){    
        bool ok = true;
        _wire->beginTransmission(i2cAddress);
        ok &= (_wire->write(reg) == 1);
        ok &= (_wire->endTransmission(false) == 0);
        _wire->requestFrom(i2cAddress, static_cast<uint8_t>(1));
        if(ok && _wire->available()){
            // Success
            *val = _wire->read();
            return true;
        } 
        // No response
        return false;
    }
    else{
        reg |= 0x80;
        _spi->beginTransaction(mySPISettings);
        digitalWrite(csPin, LOW);
        delayMicroseconds(5);
        _spi->transfer(reg); 
        *val = _spi->transfer(0x00);
        digitalWrite(csPin, HIGH);
        _spi->endTransaction();
        return true; // Error checking is not possible with SPI
    }
}

bool ADXL345_WE::readMultipleRegisters(uint8_t reg, uint8_t count, uint8_t *buf){
    if(!useSPI){
        bool ok = true;
        _wire->beginTransmission(i2cAddress);
        ok &= (_wire->write(reg) == 1);
        ok &= (_wire->endTransmission(false) == 0);
        _wire->requestFrom(i2cAddress, count);
        if (ok && _wire->available() == count) {
            // Success
            for(int i=0; i<count; i++){
                buf[i] = _wire->read();
            }
            return true;
        }
        // No response, or incomplete response
        return false;
    }
    else{
        reg = reg | 0x80;
        reg = reg | 0x40;
        _spi->beginTransaction(mySPISettings);
        digitalWrite(csPin, LOW);
        delayMicroseconds(5);
        _spi->transfer(reg); 
        for(int i=0; i<count; i++){
            buf[i] = _spi->transfer(0x00);
        }
        digitalWrite(csPin, HIGH);
        _spi->endTransaction();
        return true; // Error checking is not possible with SPI
    }
}


