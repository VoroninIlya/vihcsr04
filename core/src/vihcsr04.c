/**
 * @file vihcsr04.c
 * @author Ilia Voronin (www.linkedin.com/in/ilia-voronin-7a169122a)
 * @brief Source file of HC-SR04 ultrasonic distance sensor control driver
 *
 * @copyright Copyright (c) 2024 Ilia Voronin
 * 
 * This software is licensed under GNU GENERAL PUBLIC LICENSE 
 * The terms can be found in the LICENSE file in
 * the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS,
 * Without warranty of any kind, express or implied, 
 * including but not limited to the warranties of merchantability, 
 * fitness for a particular purpose and noninfringement. 
 * In no event shall the authors or copyright holders be liable for any claim, 
 * damages or other liability, whether in an action of contract, tort or otherwise, 
 * arising from, out of or in connection with the software 
 * or the use or other dealings in the software.
 * 
 */

#include "vihcsr04_private.h"
#include "string.h"

/**
 * @brief array of all sensors in system
 * 
 */
static struct {
  Sensor_t snsr[VIHCSR04_MAX_SENSORS];           /*!< array of all buttons */
  uint32_t currentSnsr;                          /*!< current handled sensor in runtime */
  uint32_t initializedNumber;                    /*!< number of initialized sensors in array*/
  VIHCSR04_PulseIn_t pulseInCb;                  /*!< call-back funktion to measure pulse duration*/                      
  VIHCSR04_TriggerPort_t triggerPortCb;          /*!< call-back funktion to trigger a pulse*/                        
  VIHCSR04_Printf_t printfCb;                    /*!< printf callback */
  VIHCSR04_DebugLvl_t debugLvl;                  /*!< debug level */
} sensors;

bool VIHCSR04_Init(
  VIHCSR04_PulseIn_t pulseInCb,
  VIHCSR04_TriggerPort_t triggerPortCb
) {

  if(NULL == pulseInCb || NULL == triggerPortCb)
    return false;
  
  sensors.pulseInCb = pulseInCb;
  sensors.triggerPortCb = triggerPortCb;
  sensors.currentSnsr = 0;
  sensors.initializedNumber = 0;

  for(uint32_t i = 0; i < VIHCSR04_MAX_SENSORS; i++) {
    Init(&sensors.snsr[i], NULL, NULL, 0, NULL, 0);
  }
  return true;
}

bool VIHCSR04_Create(const char* name, 
  const void* triggerPort, uint16_t triggerPin, 
  const void* echoPort, uint16_t echoPin) {

  if(NULL == name)
    return false;

  if(VIHCSR04_MAX_SENSORS <= sensors.initializedNumber)
    return false;

  int32_t sensorIndex = FindSensorByName(name);

  if(0 <= sensorIndex)
    return false;

  if(Init(&sensors.snsr[sensors.initializedNumber], name, 
    triggerPort, triggerPin, echoPort, echoPin)) {
    sensors.initializedNumber++;
    return true;
  }

  return true;
}

bool VIHCSR04_MeasureDistanceAsync(
  const char* name, const VIHCSR04_MeasureMode_t mode, 
  float temperature, uint16_t maxDistanceCm,
  const VIHCSR04_Distance_t distanceMesuredCb, const void* context) {

  if(NULL == name)
    return false;
  
  int32_t sensorIndex = FindSensorByName(name);

  if(0 > sensorIndex)
    return false;

  sensors.snsr[sensorIndex].mode = mode;
  sensors.snsr[sensorIndex].temperature = temperature;
  sensors.snsr[sensorIndex].maxDistanceCm = maxDistanceCm;
  sensors.snsr[sensorIndex].distCb = distanceMesuredCb;
  sensors.snsr[sensorIndex].userContext = context;
  sensors.snsr[sensorIndex].enabled = true;
 
  return true;
}

void VIHCSR04_StopContinuousMeasure(const char* name) {

  if(NULL == name)
    return;
  
  int32_t sensorIndex = FindSensorByName(name);

  if(0 > sensorIndex)
    return;

  sensors.snsr[sensorIndex].enabled = false;
}

float VIHCSR04_MeasureDistance(const char* name, 
  float temperature, uint16_t maxDistanceCm) {
  
  if(NULL == name)
    return -1;

  int32_t sensorIndex = FindSensorByName(name);

  if(0 > sensorIndex)
    return -1;

  Sensor_t tmpSensor = sensors.snsr[sensorIndex];

  sensors.snsr[sensorIndex].temperature = temperature;
  sensors.snsr[sensorIndex].maxDistanceCm = maxDistanceCm;
  sensors.snsr[sensorIndex].enabled = true;

  float res = Runtime(&sensors.snsr[sensorIndex]);

  sensors.snsr[sensorIndex] = tmpSensor;

  return res;
}

void VIHCSR04_Runtime(void) {
  if(0 >= sensors.initializedNumber)
    return;
  
  Runtime(&sensors.snsr[sensors.currentSnsr]);

  sensors.currentSnsr++;

  if(sensors.currentSnsr >= sensors.initializedNumber) {
    sensors.currentSnsr = 0;
  }
}

void VIHCSR04_SetPrintfCb(VIHCSR04_Printf_t printfCb) {
  sensors.printfCb = printfCb;
}

void VIHCSR04_SetDebugLvl(VIHCSR04_DebugLvl_t lvl) {
  sensors.debugLvl = lvl;
}

static bool Init(Sensor_t* sensor, const char* name, 
  const void* triggerPort, uint16_t triggerPin, 
  const void* echoPort, uint16_t echoPin) {

  if(NULL == sensor)
    return false;

  if(NULL == name)
    memset(sensor->name, 0, VIHCSR04_NAME_LEN);
  else
    strncpy(sensor->name, name, VIHCSR04_NAME_LEN);

  sensor->triggerPort = triggerPort;
  sensor->triggerPin = triggerPin;
  sensor->echoPort = echoPort;
  sensor->echoPin = echoPin;
  sensor->enabled = false;

  if(VIHCSR04_DEBUG_INFO <= sensors.debugLvl && 
     NULL != sensors.printfCb && 0 < strlen(sensor->name))
    sensors.printfCb("Sensor \"%s\": is initialized\r\n", sensor->name);

  return true;
}

static int32_t FindSensorByName(const char* name) {
  int32_t result = -1;

  if(NULL == name)
    return result;

  for(uint32_t i = 0; ((i < VIHCSR04_MAX_SENSORS) && 
      (i < sensors.initializedNumber)); i++) {
    if(0 == strncmp(sensors.snsr[i].name, name, VIHCSR04_NAME_LEN)) {
      result = i;
    }
  }
  return result;
}

static float Runtime(Sensor_t* sensor) {

  if(NULL == sensor || !sensor->enabled)
    return -1;

  if(VIHCSR04_DEBUG_INFO <= sensors.debugLvl && 
     NULL != sensors.printfCb)
    sensors.printfCb("Sensor \"%s\": measurement startet\r\n", sensor->name);

  //float speedOfSoundInCmPerMicroSec = 0.03313 + 0.0000606 * sensor->temperature; // Cair ≈ (331.3 + 0.606 ⋅ ϑ) m/s
  uint64_t speadOfSound = 33130000000 + 60600000 * sensor->temperature;

  // Compute max delay based on max distance with 25% margin in microseconds
  //unsigned long maxDistanceDurationMicroSec = 2.5 * sensor->maxDistanceCm / speedOfSoundInCmPerMicroSec;
  uint64_t maxDistance = 2500000000000 / speadOfSound * sensor->maxDistanceCm;

  // Hold trigger for 10 microseconds, which is signal for sensor to measure distance.
  sensors.triggerPortCb(sensor->triggerPort, sensor->triggerPin, 1, 10, sensor->userContext);

  // Measure the length of echo signal, which is equal to the time needed for sound to go there and back.
  uint64_t durationMicroSec = sensors.pulseInCb(
    sensor->echoPort, sensor->echoPin, 1, maxDistance*1000, sensor->userContext); 

  //float distanceCm = durationMicroSec / 2.0 * speedOfSoundInCmPerMicroSec;
  float distanceCm = (float)speadOfSound / 2000000000000 * durationMicroSec ;

  //float distanceCm = durationMicroSec / 

  if (distanceCm == 0 || distanceCm > sensor->maxDistanceCm) {
      distanceCm = -1.0 ;
  } 

  if(VIHCSR04_DEBUG_INFO <= sensors.debugLvl && 
     NULL != sensors.printfCb)
    sensors.printfCb("Sensor \"%s\": measured distance %f\r\n", sensor->name, distanceCm);

  if (sensor->distCb)
    sensor->distCb(distanceCm, sensor->userContext);

  if (sensor->mode == VIHCSR04_ONESHOT_MEASURE)
    sensor->enabled = false;

  return distanceCm;
}

