#include <Arduino.h>
#include <state_logic.h>


void setup()
{
  set_mission_state(e_mission_state::startup);
}

void loop()
{
  update_state_machine();
}

