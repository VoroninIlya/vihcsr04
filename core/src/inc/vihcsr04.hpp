/**
 * @file vihcsr04.hpp
 * @author Ilia Voronin (www.linkedin.com/in/ilia-voronin-7a169122a)
 * @brief Header file of HC-SR04 ultrasonic distance sensor control driver (c++ realisation)
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

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string>
#include <vector>
#include <map>
#include <climits>

namespace vihcsr04 {

  typedef enum {
    DEBUG_DISABLED = 0,  
    DEBUG_INFO
  } DebugLvl_t;

  typedef enum {
    ONESHOT_MEASURE = 0,  
    CONTINUOUS_MEASURE
  } MeasureMode_t;

  typedef uint64_t (*PulseIn_t)(const void* gpio, uint16_t port, 
    uint8_t state, uint64_t maxDurationTreshold, const void* context);
  typedef void (*TriggerPort_t) (const void* gpio, uint16_t port, 
    uint8_t state, uint64_t pulseDuration, const void* context);
  typedef void (*Distance_t) (float distance, const void* context);
  typedef int (*Printf_t) (const char *__format, ...);

  class Hcsr04Sensor 
  {
  public:
    Hcsr04Sensor() = delete;

    /**
     * @brief Construct a new Hcsr04 Sensor object
     * 
     * @param pulseInCb      Call-back funktion to count pulse duration (for example Arduino.pulseIn)
     * @param triggerPortCb  Call-back funktion to trigger a pulse on port
     */
    Hcsr04Sensor(PulseIn_t pulseInCb, TriggerPort_t triggerPortCb);

    ~Hcsr04Sensor();

    /**
     * @brief Create/register a new sensor handler to containter
     * 
     * @param name Unique name of sensor
     * @param triggerPort Pointer to GPIO structur for trigger pin
     * @param triggerPin A pin number, to which is trigger pin connected
     * @param echoPort Pointer to GPIO structur for trigger pin
     * @param echoPin A pin number, to which is trigger pin connected
     * @return true if sensor created successfull
     * @return false if any error occurred through creation
     */
    bool AddSensor(const std::string& name, 
      const void* triggerPort, uint16_t triggerPin, 
      const void* echoPort, uint16_t echoPin);

    /**
     * @brief Delete a sensor if exist from container
     * 
     * @param name 
     * @return true 
     * @return false 
     */
    bool DeleteSensor(const std::string& name);

    /**
     * @brief Start async distance mesurement
     * 
     * @param name Unique name of sensor
     * @param temperature Current environment temperature
     * @param maxDistanceCm Maximal measured distance
     * @param distanceMesuredCb Call-back funktion will me called if meassurement is done
     * @param context user context that will be returned by calling distanceMesuredCb
     * @return true if measurement started successfull
     * @return false if any error occurred while starting
     */
    bool MeasureDistanceAsync(const std::string& name, 
      const MeasureMode_t mode, float temperature, uint16_t maxDistanceCm,
      const Distance_t distanceMesuredCb, const void* context);

    /**
     * @brief Stop continuous distance mesurement
     * 
     * @param name Unique name of sensor.
     */
    void StopContinuousMeasure(const std::string& name);

    float MeasureDistance(const std::string& name, 
      float temperature, uint16_t maxDistanceCm);

    /**
     * @brief Driver runtime, should be placed in main loop or in a task loop
     * 
     */
    void Runtime(void);

    /**
     * @brief Set printf callback.
     *   This callback can be used to get debug info from driver
     * 
     * @param printfCb Callback of prontf fuktion
     */
    void SetPrintfCb(const Printf_t printfCb);

    /**
     * @brief Set debug info level
     * 
     * @param lvl Debug info level
     */
    void SetDebugLvl(const DebugLvl_t lvl);

  private:
    typedef struct
    {
      std::string name;                      /*!< unique name of sensor */
      const void* triggerPort{nullptr};      /*!< pointer to the physical port, to witch the trigger pin of sensor is connected*/
      uint16_t triggerPin{USHRT_MAX};        /*!< pin number, to witch the the trigger pin of sensor is connected*/
      const void* echoPort{nullptr};         /*!< pointer to the physical port, to witch the echo pin of sensor is connected*/
      uint16_t echoPin{USHRT_MAX};           /*!< pin number, to witch the echo pin of sensor is connected*/
      bool enabled{false};                   /*!< flag to enabled/disable if messurement */
      MeasureMode_t mode{};                  /*!< messurement mode */
      float temperature{};                   /*!< current environment temperature */
      uint16_t maxDistanceCm{};              /*!< maximal measured distance */
      const void* userContext{nullptr};      /*!< user context that is returned by calling distCb */
      Distance_t distCb{nullptr};            /*!< call-back funktion will be called if meassurement is done */

      float Runtime(const PulseIn_t pulseInCb, 
        const TriggerPort_t triggerPortCb, const Printf_t printfCb, DebugLvl_t debugLvl) 
      {
        if(!enabled)
          return -1;

        if(DEBUG_INFO <= debugLvl && nullptr != printfCb)
          printfCb("Sensor \"%s\": measurement startet\r\n", name.c_str());

        //float speedOfSoundInCmPerMicroSec = 0.03313 + 0.0000606 * sensor->temperature; // Cair ≈ (331.3 + 0.606 ⋅ ϑ) m/s
        uint64_t speadOfSound = 33130000000 + 60600000 * temperature;

        // Compute max delay based on max distance with 25% margin in microseconds
        //unsigned long maxDistanceDurationMicroSec = 2.5 * sensor->maxDistanceCm / speedOfSoundInCmPerMicroSec;
        uint64_t maxDistance = 2500000000000 / speadOfSound * maxDistanceCm;

        // Hold trigger for 10 microseconds, which is signal for sensor to measure distance.
        triggerPortCb(triggerPort, triggerPin, 1, 10, userContext);

        // Measure the length of echo signal, which is equal to the time needed for sound to go there and back.
        uint64_t durationMicroSec = pulseInCb(echoPort, echoPin, 1, maxDistance*1000, userContext); // can't measure beyond max distance

        //float distanceCm = durationMicroSec / 2.0 * speedOfSoundInCmPerMicroSec;
        float distanceCm = (float)speadOfSound / 2000000000000 * durationMicroSec ;

        //float distanceCm = durationMicroSec / 

        if (distanceCm == 0 || distanceCm > maxDistanceCm) {
            distanceCm = -1.0 ;
        } 

        if(DEBUG_INFO <= debugLvl && nullptr != printfCb)
          printfCb("Sensor \"%s\": measured distance %f\r\n", name.c_str(), distanceCm);

        if (distCb)
          distCb(distanceCm, userContext);

        if (mode == ONESHOT_MEASURE)
          enabled = false;

        return distanceCm;
      }

    } Sensor_t;

    bool m_isInitialized{false};
    uint32_t m_currentSnsr{};
    PulseIn_t m_pulseInCb{nullptr};
    TriggerPort_t m_triggerPortCb{nullptr};
    std::map<std::string, Sensor_t> m_sensors{};
    std::vector<Sensor_t*> m_sensorsPtr{};
    DebugLvl_t m_debugLvl{};
    Printf_t m_printfCb{};
  };
}

