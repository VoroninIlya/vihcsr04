/**
 * @file vihcsr04_private.h
 * @author Ilia Voronin (www.linkedin.com/in/ilia-voronin-7a169122a)
 * @brief Private header file of HC-SR04 ultrasonic distance sensor control driver
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

#ifndef VIHCSR04_PRIVATE_H
#define VIHCSR04_PRIVATE_H

#include "vihcsr04.h"

/**
 * @brief Sensor control type
 * 
 */
typedef struct
{
  char name[VIHCSR04_NAME_LEN]; /*!< unique name of sensor */
  const void* triggerPort;      /*!< pointer to the physical port, to witch the trigger pin of sensor is connected*/
  uint16_t triggerPin;          /*!< pin number, to witch the the trigger pin of sensor is connected*/
  const void* echoPort;         /*!< pointer to the physical port, to witch the echo pin of sensor is connected*/
  uint16_t echoPin;             /*!< pin number, to witch the echo pin of sensor is connected*/
  bool enabled;                 /*!< flag to enabled/disable if messurement */
  VIHCSR04_MeasureMode_t mode;  /*!< messurement mode */
  float temperature;            /*!< current environment temperature */
  uint16_t maxDistanceCm;       /*!< maximal measured distance */
  const void* userContext;      /*!< user context that is returned by calling distCb */
  VIHCSR04_Distance_t distCb;   /*!< call-back funktion will be called if meassurement is done */
} Sensor_t;

/**
 * @brief Initialize a semsor handler
 * 
 * @param button Pointer to a sensor control structur
 * @param name Unique name of a new sensor
 * @param triggerPort Pointer to a GPIO structur to witch the trigger pin of sensor is connected
 * @param triggerPin A pin number, to witch the the trigger pin of sensor is connected
 * @param echoPort Pointer to a GPIO structur, to witch the echo pin of sensor is connected
 * @param echoPin A pin number, to witch the echo pin of sensor is connected
 * @return true if initialization is successful
 * @return false if initialization is failed
 */
static bool Init(Sensor_t* sensor, const char* name, 
  const void* triggerPort, uint16_t triggerPin, 
  const void* echoPort, uint16_t echoPin);

/**
 * @brief Find sensor by name in array of initialized sensors
 * 
 * @param name Sensor name to search
 * @return int32_t index of found sensor, if no button found returns -1
 */
static int32_t FindSensorByName(const char* name);

/**
 * @brief Button runtime
 * 
 * @param button Pointer to a sensor control structur
 */
static float Runtime(Sensor_t* sensor);

#endif // VIHCSR04_PRIVATE_H
