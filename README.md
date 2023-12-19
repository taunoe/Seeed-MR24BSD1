# Library for Seeed MR24BSD1 radar sensor

Arduino library for Seeed MR24BSD1 sensor.

## Example

Tested on Raspberry Pi Pico.

```c++
#include <Arduino.h>
#include <Seeed_MR24BSD1.h>

const uint RX_PIN = 17;
const uint TX_PIN = 16;

Seeed_MR24BSD1 radar;

uint64_t prev_millis = 0;


void setup() {
  Serial.begin(115200);

  radar.begin(RX_PIN, TX_PIN);

 // while (!Serial) {
    // wait
  //}

  Serial.println("Ready");
}

void loop() {
  uint64_t current_millis = millis();

  // radar.loop(PRINT_RAW);
  radar.loop();
  if ((current_millis - prev_millis) >= 1000) {
    prev_millis = current_millis;

    /*
    Serial.print("Status ");
    Serial.println(radar.get_status());
    */
    Serial.print("Signs ");
    Serial.println(radar.get_signs_parameter());
    /*
    Serial.print("Threshold ");
    Serial.println(radar.get_threshold());
    Serial.print("Distance ");
    Serial.println(radar.get_distance());
    
    Serial.print("Breathing rate ");
    Serial.println(radar.get_breathing_rate());
    
    Serial.print("Breathing status ");
    Serial.println(radar.get_breathing_status());
    Serial.print("Bed status ");
    Serial.println(radar.get_bed_status());
    Serial.print("Sleep state ");
    Serial.println(radar.get_sleep_state());
    Serial.print("Wakefulness ");
    Serial.println(radar.get_duration_of_wakefulness());
    Serial.print("Lighr sleep ");
    Serial.println(radar.get_light_sleep_lenght());
    Serial.print("Deep sleep ");
    Serial.println(radar.get_deep_sleep_lenght());
    Serial.print("Sleep quality ");
    Serial.println(radar.get_sleep_quality());
    
    Serial.print("Heart rate ");
    Serial.println(radar.get_heart_rate());
    */
    radar.get_device_id();
    //radar.get_software_ver();
  }
}
```

____
Tauno Erik 2023
