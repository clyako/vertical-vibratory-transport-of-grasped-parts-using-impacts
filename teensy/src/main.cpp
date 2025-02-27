#include <Arduino.h>
#include <Bounce.h>
#include "CarltonHapticMotor.h"

/**
 * @brief This function checks the status of the rocker switch. The rocker switch determines when the
  sawtooth waveform is sent to the motor.
*/
void check_switch();

///< Initialize the motor.
CarltonHapticMotor motor = CarltonHapticMotor();

///< Rocker switch params to control device on / off state.
const int rocker_pin = A14;
int debounce_time = 5; // ms
Bounce rocker_switch = Bounce(rocker_pin, debounce_time);
bool run_device;

void setup()
{
  Serial.begin(115200);
  pinMode(rocker_pin, INPUT_PULLUP);
  delay(1000);
  run_device = !digitalRead(rocker_pin);
}

void loop()
{
  check_switch();
  if (run_device)
  {
    motor.sawtooth();
  }
  else
  {
    motor.zero();
  }
}

void check_switch()
{
  if (rocker_switch.update())
  {
    if (rocker_switch.fallingEdge())
    {
      run_device = true;
    }
    else if (rocker_switch.risingEdge())
    {
      run_device = false;
    }
  }
}
