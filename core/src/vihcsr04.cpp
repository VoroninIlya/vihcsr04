/**
 * @file vihcsr04.c
 * @author Ilia Voronin (www.linkedin.com/in/ilia-voronin-7a169122a)
 * @brief Source file of HC-SR04 ultrasonic distance sensor control driver (c++ realisation)
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

#include "vihcsr04.hpp"

namespace vihcsr04 {

  Hcsr04Sensor::Hcsr04Sensor(PulseIn_t pulseInCb, TriggerPort_t triggerPortCb) {

    if(nullptr == pulseInCb || nullptr == triggerPortCb)
      return;

    m_pulseInCb = pulseInCb;
    m_triggerPortCb = triggerPortCb;

    m_isInitialized = true;
  }

  Hcsr04Sensor::~Hcsr04Sensor() {
    m_sensors.clear();
  }

  bool Hcsr04Sensor::AddSensor(const std::string& name, 
      const void* triggerPort, uint16_t triggerPin, 
      const void* echoPort, uint16_t echoPin) {
    
    if (!m_isInitialized || name.empty() || 
        m_sensors.contains(name))
      return false;

    m_sensors[name] = Sensor_t{
      .name = name,
      .triggerPort = triggerPort,
      .triggerPin = triggerPin,
      .echoPort = echoPort,
      .echoPin = echoPin
    };

    m_sensorsPtr.push_back(&m_sensors.at(name));
    if(DEBUG_INFO <= m_debugLvl && nullptr != m_printfCb)
      m_printfCb("Sensor \"%s\": registered\r\n", name.c_str());

    return true;
  }

  bool Hcsr04Sensor::DeleteSensor(const std::string& name) {

    if (!m_isInitialized || name.empty())
      return false;

    m_sensors.erase(name);

    m_sensorsPtr.clear();

    for (auto it = m_sensors.begin(); it != m_sensors.end(); ++it) {
      m_sensorsPtr.push_back(&it->second);
    }

    return true;
  }

  bool Hcsr04Sensor::MeasureDistanceAsync(const std::string& name, 
    const MeasureMode_t mode, float temperature, uint16_t maxDistanceCm,
    const Distance_t distanceMesuredCb, const void* context) {
    
    if (!m_isInitialized || name.empty() ||
        !m_sensors.contains(name))
      return false;

    auto& sensor = m_sensors[name];

    sensor.mode = mode;
    sensor.temperature = temperature;
    sensor.maxDistanceCm = maxDistanceCm;
    sensor.distCb = distanceMesuredCb;
    sensor.userContext = context;
    sensor.enabled = true;

    return true;
  }

  float Hcsr04Sensor::MeasureDistance(const std::string& name, 
      float temperature, uint16_t maxDistanceCm) {

    if (!m_isInitialized || name.empty() ||
        !m_sensors.contains(name))
      return -1;

    
    Sensor_t tmpSensor = m_sensors[name];
    Sensor_t& currSensor = m_sensors[name];

    currSensor.temperature = temperature;
    currSensor.maxDistanceCm = maxDistanceCm;
    currSensor.enabled = true;

    float res = currSensor.Runtime(m_pulseInCb, m_triggerPortCb, 
      m_printfCb, m_debugLvl);

    m_sensors[name] = tmpSensor;

    return res;
  }

  void Hcsr04Sensor::Runtime(void) {
    if(!m_isInitialized || m_sensorsPtr.empty())
      return;

    auto currSensor = m_sensorsPtr[m_currentSnsr];

    if(!currSensor)
      return;

    currSensor->Runtime(m_pulseInCb, m_triggerPortCb, 
      m_printfCb, m_debugLvl);

    m_currentSnsr++;

    if(m_currentSnsr >= m_sensorsPtr.size()) {
      m_currentSnsr = 0;
    }
  }

  void Hcsr04Sensor::SetPrintfCb(const Printf_t printfCb) {
    m_printfCb = printfCb;
  }

  void Hcsr04Sensor::SetDebugLvl(const DebugLvl_t lvl) {
    m_debugLvl = lvl;
  }

}

