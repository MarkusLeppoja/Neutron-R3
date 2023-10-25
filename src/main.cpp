#include <Arduino.h>
#include <state_logic.h>

// Create a debug_mode  boolean that if enabled

// Automatically build a .csv string based on the enables for ground control and flash.csv logging Im not going to rewrite the csv string layout every time
// Every time that something gets toggeled in datasave, we should regenerate the csv layout and print it to flash & serial 

//Neutron checklist: @YTFurys Ensure debug modes (@YTFurys What are debug modes?)
// TODO:
// Communication file (include state transition in there)
// State transition

//TODO: lAUNCH CHECKILIST: Ensure datalogging is happening (Get it to print the status of the file, if its open)
// TODO: Add option to format the entire device. ensure that formating closes all files and disables logging for a minute and restores it to default state after.

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
