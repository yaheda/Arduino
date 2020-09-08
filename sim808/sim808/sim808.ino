#include <SIM808.h>
#include <ArduinoLog.h>

#include <SoftwareSerial.h>
#define SIM_SERIAL_TYPE  SoftwareSerial          ///< Type of variable that holds the Serial communication with SIM808
#define SIM_SERIAL    SIM_SERIAL_TYPE(10, 11) ///< Definition of the instance that holds the Serial communication with SIM808    

#define STRLCPY_P(s1, s2) strlcpy_P(s1, s2, BUFFER_SIZE)

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
