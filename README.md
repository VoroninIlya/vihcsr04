# HC-SR04 Ultrasonic distance sensor driver 

Driver is independent from system it should by compatible with raspberry py (linux) and with stm32 (no OS or RTOS). 
Only is necessery to provide some callback functions to control gpio on used system.

There are two versions: the first one is written in c and the second one in c++
For using c you need to include "vihcsr04.h", for cpp include "vihcsr04.hpp"

The driver is expandable and allows to register a few sensors. By using asyncron measurement 
only one sensor is handled on runtime one each program loop. A synchronous measurement is supported as well.

# Usage examples

```
static uint32_t startTick;
static uint32_t pulseDuration;

static void AlertCb(int gpio, int level, uint32_t tick)
{
    static bool measuring = false;

    if (level == 1 && !measuring) {
        // Rising edge, start measuring
        startTick = tick;
        measuring = true;
    } else if (level == 0 && measuring) {
        // Falling edge, stop measuring
        pulseDuration = tick - startTick;
        measuring = false;
        gpioSetAlertFunc(gpio, NULL); // Disable callback
    }
}

static uint64_t PulseIn(const void* gpio, uint16_t port, 
  uint8_t state, uint64_t timeoutUs, const void* context) {

  startTick = 0;
  pulseDuration = 0;  
  // Set the callback for state change
  gpioSetAlertFunc(port, AlertCb);  
  auto start_time = std::chrono::high_resolution_clock::now();  
  // Wait for the pulse to be measured or timeout
  while (pulseDuration == 0) {
      auto current_time = std::chrono::high_resolution_clock::now();
      auto elapsed = current_time - start_time;
      auto count = elapsed.count();
      if (count > timeoutUs) {
          gpioSetAlertFunc(port, NULL); // Disable callback  
          printf("Error %f %f\r\n", elapsed, timeoutUs);  
          return 0; // Timeout occurred
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }    
  return pulseDuration; // Return duration in microseconds  
}

void DelayMs(uint32_t ms) {
   struct timespec ts;
    ts.tv_sec = ms / 1000; 
    ts.tv_nsec = ms % 1000 * 1000000L;   

   nanosleep(&ts, nullptr);  // Sleep for 1 millisecond
}

static void TriggerPort(const void* gpio, uint16_t port, 
   uint8_t state, uint64_t pulseDuration, const void* context) {
   gpioTrigger(port, pulseDuration, state);
}

void Distance(float distance) {
   printf("Distance: %f\r\n", distance);
}

int main(void) {
  printf("Hallo world\r\n");

  if (gpioInitialise() < 0) {
    printf("Error by gpio initialization\r\n");
    return 0;
  }

  gpioSetMode(6, PI_OUTPUT);
  gpioSetPullUpDown(6, PI_PUD_DOWN);
  gpioSetMode(5, PI_INPUT);
  gpioSetPullUpDown(6, PI_PUD_DOWN);

  // c usage
  VIHCSR04_SetPrintfCb((VIHCSR04_Printf_t)printf);
  VIHCSR04_SetDebugLvl(VIHCSR04_DEBUG_INFO);
  VIHCSR04_Init(PulseIn, TriggerPort);
  VIHCSR04_Create("HC-SR04 1", nullptr, 6, nullptr, 5);
  
  VIHCSR04_MeasureDistanceAsync("HC-SR04 1", VIHCSR04_CONTINUOUS_MEASURE, 
   31.307f, 400, (VIHCSR04_Distance_t)Distance, nullptr);

  // c++ usage
  vihcsr04::Hcsr04Sensor sensor{PulseIn, TriggerPort};   
  sensor.AddSensor("HC-SR04 1", nullptr, 6, nullptr, 5);
  sensor.MeasureDistanceAsync("HC-SR04 1", vihcsr04::MeasureMode_t::CONTINUOUS_MEASURE, 
  31.307f, 400, (vihcsr04::Distance_t)Distance, nullptr);

  while(1) {

    VIHCSR04_Runtime(); // c runtime
    sensor.Runtime(); // c++ runtime

    DelayMs(1000);
  }

  gpioTerminate();

  return 0;
}

```