/**
 * @file vihcsr04.h
 * @author Ilia Voronin (www.linkedin.com/in/ilia-voronin-7a169122a)
 * @brief Header file of HC-SR04 ultrasonic distance sensor control driver
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

#ifndef VIHCSR04_H
#define VIHCSR04_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/** 
 * @brief Maximal length of a sensor name.
 *   All names longer this limit will be cutted until this limit
 * */ 
#if !defined(VIHCSR04_NAME_LEN)
  #define VIHCSR04_NAME_LEN 15     
#endif

/** 
 * @brief Maximal number of allowed to create sensors ih the system.
 *   For this number of sensors will be static memory reserved 
 * */
#if !defined(VIHCSR04_MAX_SENSORS)
  #define VIHCSR04_MAX_SENSORS 1   
#endif

/**
 * @brief Debug level
 * 
 */
typedef enum {
  VIHCSR04_DEBUG_DISABLED = 0,  
  VIHCSR04_DEBUG_INFO
} VIHCSR04_DebugLvl_t;

typedef enum {
  VIHCSR04_ONESHOT_MEASURE = 0,  
  VIHCSR04_CONTINUOUS_MEASURE
} VIHCSR04_MeasureMode_t;

typedef uint64_t (*VIHCSR04_PulseIn_t)(
  const void* gpio, uint16_t port, uint8_t state, 
  uint64_t maxDurationTreshold, const void* context);

typedef void (*VIHCSR04_TriggerPort_t) (
  const void* gpio, uint16_t port, uint8_t state, 
  uint64_t pulseDuration, const void* context);

typedef void (*VIHCSR04_Distance_t) (float distance, const void* context);

typedef int (*VIHCSR04_Printf_t) (const char *__format, ...);

/**
 * @brief Initialization of HC-SR04 sensors control driver
 * 
 * @param pulseInCb Call-back funktion to count pulse duration (for example Arduino.pulseIn)
 * @param triggerPortCb Call-back funktion to trigger a pulse on port
 * @return true if driver initialized successfull
 * @return false if any error occurred while initialization
 */

bool VIHCSR04_Init(
  VIHCSR04_PulseIn_t pulseInCb,
  VIHCSR04_TriggerPort_t triggerPortCb
);

/**
 * @brief Create/register a new sensor handler
 * 
 * @param name Unique name of sensor. Care about VIHCSR04_NAME_LEN
 * @param triggerPort Pointer to GPIO structur for trigger pin
 * @param triggerPin A pin number, to which is trigger pin connected
 * @param echoPort Pointer to GPIO structur for trigger pin
 * @param echoPin A pin number, to which is trigger pin connected
 * @return true if sensor created successfull
 * @return false if any error occurred through creation
 */
bool VIHCSR04_Create(const char* name, 
  const void* triggerPort, uint16_t triggerPin, 
  const void* echoPort, uint16_t echoPin);

/**
 * @brief Start async distance mesurement
 * 
 * @param name Unique name of sensor. Care about VIHCSR04_NAME_LEN
 * @param temperature Current environment temperature
 * @param maxDistanceCm Maximal measured distance
 * @param distanceMesuredCb Call-back funktion will me called if meassurement is done
 * @param context user context that will be returned by calling distanceMesuredCb
 * @return true if measurement started successfull
 * @return false if any error occurred while starting
 */
bool VIHCSR04_MeasureDistanceAsync(const char* name, 
  const VIHCSR04_MeasureMode_t mode, float temperature, uint16_t maxDistanceCm,
  const VIHCSR04_Distance_t distanceMesuredCb, const void* context);

/**
 * @brief Stop continuous distance mesurement
 * 
 * @param name Unique name of sensor.
 */
void VIHCSR04_StopContinuousMeasure(const char* name);

/**
 * @brief Sync distance mesurement
 * 
 * @param name Unique name of sensor. Care about VIHCSR04_NAME_LEN
 * @param temperature Current environment temperature
 * @return distance to the object in cm
 */
float VIHCSR04_MeasureDistance(const char* name,
  float temperature, uint16_t maxDistanceCm);

/**
 * @brief Driver runtime, should be placed in main loop or in a task loop
 * 
 */
void VIHCSR04_Runtime(void);

/**
 * @brief Set printf callback.
 *   This callback can be used to get debug info from driver
 * 
 * @param printfCb Callback of prontf fuktion
 */
void VIHCSR04_SetPrintfCb(const VIHCSR04_Printf_t printfCb);

/**
 * @brief Set debug info level
 * 
 * @param lvl Debug info level
 */
void VIHCSR04_SetDebugLvl(const VIHCSR04_DebugLvl_t lvl);

#ifdef __cplusplus
}
#endif

#endif // VIHCSR04_H