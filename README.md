# ADXL345_WE
Arduino Library for the ADXL345 accelerometer. 

I have tried to create a library for the ADXL345 which is easy to use for people who don't want to deal with all the registers. Therefore I have added lots example sketches which will enable you to deal even with the more complex features such as the FIFO modes. Howerever I still recommend to have a look into the data sheet to get a deeper understanding. 

You can use both I2C and SPI. If you want to find out how to use SPI, look at the ADXL345_SPI_basic_data.ino. 

A detailed tutorial is available: 

https://wolles-elektronikkiste.de/adxl345-teil-1  (German)

https://wolles-elektronikkiste.de/en/adxl345-the-universal-accelerometer-part-1  (English) 

If you are not so much experienced with the ADXL345, I recommend to work through the examples in the following order:

1) ADXL345_basic_data
2) ADXL345_SPI_basic_data
3) ADXL345_calibration
4) ADXL345_angles_orientation
5) ADXL345_pitch_roll_corrected_angles
6) ADXL345_sleep
7) ADXL345_free_fall_interrupt
8) ADXL345_data_ready_interrupt
9) ADXL345_activity_inactivity_interrupt
10) ADXL345_auto_sleep.ino
11) ADXL345_single_tap
12) ADXL345_double_tap
13) ADXL345_fifo_fifo
14) ADXL345_fifo_stream
15) ADXL345_fifo_trigger

To develop this library I have worked with a ADXL345 module. It should also work with the bare ADXL345 IC. For the module I have noticed that the power consumption is much higher than mentioned in the data sheet. I think the issue is the voltage converter on the module. You can reduce the power consumption by choosing 3.3 volts instead of 5 volts. At least this worked with my module. 

If you like my library please give it a star. If you don't like it I would be happy to get feedback. And if you find bugs I will try to eliminate them as quickly as possible. 

<h2>If SPI does not work</h2>

My library has implemented SPI 4-Wire. Some modules have SDO connected GND via an 0 ohm resistor. With this resistor only SPI 3-Wire would work. Your options are:

1) Remove the 0 ohm resistor connected to SDO (R4 on my module)
2) Replace the 0 ohm resistor connected to SDO by an internal or external pull-down resistor of 4.7 kohm or 10 kohm
3) Find another library which supports SPI 3-Wire

![ADXL345_hack](https://github.com/wollewald/ADXL345_WE/assets/41305162/2fc39482-70f7-4de1-ac0b-2e27f28ac15e)

Another SPI issue might occur when using ESP8266 boards like the WEMOS D1 mini or NodeMCU. The standard CS Pin (e.g. D8/GPIO15 on a WEMOS D1 mini or NodeMCU) might not work since the CS Pin on most ADXL345 modules has a pull-up resistor. And if D8 is high at reset, the ESP8266 will not boot. In that case choose a different ESP8266 pin as CS!  

