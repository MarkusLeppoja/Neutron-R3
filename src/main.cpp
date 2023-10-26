#include <Arduino.h>
#include <state_logic.h>


//TODO: Add option to format the entire device. ensure that formating closes all files and disables logging for a minute and restores it to default state after.


void setup() 
{
  set_mission_state(e_mission_state::startup);
}

void loop() 
{
  update_state_machine();
}


/* @todo Consider
void loop1()
void setup1()
*/
