#ifndef __COMS_H__
#define __COMS_H__

#include <Arduino.h>
#include <definitions.h>
#include <alerts.h>
#include <recovery.h>
#include <data_recorder.h>
#include <pyro.h>
#include <indicator.h>


// TODO: descriptions

void update_communication();
void _execute_command_from_list(int command);

#endif