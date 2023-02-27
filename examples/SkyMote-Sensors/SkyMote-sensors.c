#include "contiki.h"
#include "dev/sht11-sensor.h"
#include "dev/light-sensor.h"
#include "dev/leds.h"
#include <stdio.h>

/* 
  This module is used to print the sensor information contained in a TelosB mote.
  The sensors used are the light sensor, the temperature sensor and the humidity sensor.
  The information is printed in the serial port.

  The humidity sensor and the temperature sensor are part of the SHT11 sensor.

  To compile this module, use the following command in the terminal with the mote connected:
  
  1. Identify the USB port of your device
      make TARGET=sky SkyMote-sensors.c motelist

  let's say it is /dev/ttyUSB0

  2. Compile the code
      make TARGET=sky PORT=/dev/ttyUSB0 SkyMote-sensors.upload

  3. Open the serial port
      make TARGET=sky PORT=/dev/ttyUSB0 SkyMote-sensors.login

*/

//Declare the process
PROCESS(send_sensor_info_process, "Print the Sensors Information");

//Make the process start when the module is loaded
AUTOSTART_PROCESSES(&send_sensor_info_process);


static int get_light(void)
{
    return 10 * light_sensor.value(LIGHT_SENSOR_PHOTOSYNTHETIC) / 7;
}

static int get_temp(void)
{
    return ((sht11_sensor.value(SHT11_SENSOR_TEMP) / 10) - 396) / 10;
}

static int get_hum(void)
{
    int _val;                    // Raw humidity value returned from sensor
    float _linearHumidity;       // Humidity with linear correction applied
    float _correctedHumidity;    // Temperature-corrected humidity
    float _temperature;          // Raw temperature value

    // Conversion coefficients from SHT15 datasheet
    const float C1 = -4.0;       // for 12 Bit
    const float C2 =  0.0405;    // for 12 Bit
    const float C3 = -0.0000028; // for 12 Bit
    const float T1 =  0.01;      // for 14 Bit @ 5V
    const float T2 =  0.00008;   // for 14 Bit @ 5V
  
  
    _val = sht11_sensor.value(SHT11_SENSOR_HUMIDITY);

    _linearHumidity = C1 + C2 * _val + C3 * _val * _val; // Linear correction

    // Get current temperature for humidity correction
    _temperature = ((sht11_sensor.value(SHT11_SENSOR_TEMP) / 10) - 396) / 10;

    // Correct humidity value for current temperature
    _correctedHumidity = (_temperature - 25.0 ) * (T1 + T2 * _val) + _linearHumidity;
  
return (_correctedHumidity);
}


PROCESS_THREAD(send_sensor_info_process, ev, data) //PROCESS
{
    PROCESS_BEGIN();
    SENSORS_ACTIVATE(light_sensor);
    SENSORS_ACTIVATE(sht11_sensor);
    printf("Light: %d \n", get_light());
    printf("Temperature: %d \n", get_temp());
    printf("Humidity: %d \n",get_hum());

    PROCESS_END();
}
