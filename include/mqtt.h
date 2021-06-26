#ifndef MQTT_H
#define MQTT_H

#include <Arduino.h>
#include "entity.h"

void mqtt_setup() ;
bool mqtt_connected() ;
void mqtt_reconnect() ;
boolean mqtt_loop() ;
Entity* mqtt_get_entity(uint8) ;
void mqtt_change_entity(Entity*) ;

#endif
