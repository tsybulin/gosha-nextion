#include "mqtt.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "params.h"
#include "debugger.h"

WiFiClient wiFiClient ;
PubSubClient mqttClient(wiFiClient) ;

extern entity_changed_t entity_changed ;

void switch_parser(Entity *e, byte* payload, unsigned int length) {
    if (strncmp((char *)payload, "ON", 2) == 0) {
        e->onff = true ;
    } else {
        e->onff = false ;
    }
}

void switch_json_parser(Entity *e, byte* payload, unsigned int length) {
    StaticJsonDocument<200> doc ;
    char json[length+1] ;
    strncpy(json, (char *)payload, length) ;
    json[length] = '\0' ;
    DeserializationError error = deserializeJson(doc, json) ;
    if (error) {
        Debug_print("deserializeJson() failed: ") ;
        Debug_println(error.f_str()) ;
        Debug_println(json) ;
        return ;
    }

    const char *onoff = doc[e->stateValueTemplate] ;
    if (onoff) {
        if (strncmp(onoff, "ON", 2) == 0) {
            e->onff = true ;
        } else {
            e->onff = false ;
        }
    }
}

void light_json_parser(Entity *e, byte* payload, unsigned int length) {
    StaticJsonDocument<200> doc ;
    char json[length+1] ;
    strncpy(json, (char *)payload, length) ;
    json[length] = '\0' ;
    DeserializationError error = deserializeJson(doc, json) ;
    if (error) {
        Debug_print("deserializeJson() failed: ") ;
        Debug_println(error.f_str()) ;
        Debug_println(json) ;
        return ;
    }

    const char *onoff = doc[e->stateValueTemplate] ;
    if (onoff) {
        if (strncmp(onoff, "ON", 2) == 0) {
            e->onff = true ;
        } else {
            e->onff = false ;
        }
    }

    JsonVariant brightness = doc[e->brightnessValueTemplate] ;
    if (!brightness.isNull()) {
        e->brightness = brightness.as<int>() ;
    }
}

void temperature_json_parser(Entity *e, byte* payload, unsigned int length) {
    StaticJsonDocument<200> doc ;
    char json[length+1] ;
    strncpy(json, (char *)payload, length) ;
    json[length] = '\0' ;
    DeserializationError error = deserializeJson(doc, json) ;
    if (error) {
        Debug_print("deserializeJson() failed: ") ;
        Debug_println(error.f_str()) ;
        Debug_println(json) ;
        return ;
    }

    float DS18B20_Temperature = doc["DS18B20"]["Temperature"] ;

    e->value = DS18B20_Temperature ;
}

void energy_json_parser(Entity *e, byte* payload, unsigned int length) {
    StaticJsonDocument<256> doc ;
    char json[length+1] ;
    strncpy(json, (char *)payload, length) ;
    json[length] = '\0' ;
    DeserializationError error = deserializeJson(doc, json) ;
    if (error) {
        Debug_print("deserializeJson() failed: ") ;
        Debug_println(error.f_str()) ;
        Debug_println(json) ;
        return ;
    }

    JsonObject energy = doc["ENERGY"] ;
    int value = energy[e->stateValueTemplate] ;
    e->value = value ;
}

void alarm_parser(Entity *e, byte* payload, unsigned int length) {
    strncpy(e->state, (char *)payload, length) ;
    e->state[length] = '\0' ;
}

Entity entities[] = {
    {1, "cabinet/stat/toplight/POWER", "cabinet/cmnd/toplight/POWER", switch_parser},
    {2, "cabinet/stat/tablelamp/POWER", "cabinet/cmnd/tablelamp/POWER", switch_parser},
    {3, "cabinet/stat/outlet1/POWER", "cabinet/cmnd/outlet1/POWER", switch_parser},
    {4, "cabinet/stat/backlight/RESULT", "POWER", "cabinet/cmnd/backlight/POWER", "cabinet/cmnd/backlight/DIMMER", "Dimmer", light_json_parser},

    {5, "hall/stat/outlet2/POWER", "hall/cmnd/outlet2/POWER", switch_parser},
    {6, "hall/stat/sofa/POWER", "hall/cmnd/sofa/POWER", switch_parser},
    {7, "hall/stat/nightlight/POWER", "hall/cmnd/nightlight/POWER", switch_parser},
    {8, "hall/stat/luster/POWER1", "hall/cmnd/luster/POWER1", switch_parser},
    {9, "hall/stat/luster/POWER2", "hall/cmnd/luster/POWER2", switch_parser},

    {10, "corridor/stat/switch12/POWER1", "corridor/cmnd/switch12/POWER1", switch_parser},
    {11, "corridor/stat/switch34/POWER1", "corridor/cmnd/switch34/POWER1", switch_parser},
    {12, "corridor/stat/wardrobe/RESULT", "POWER", "corridor/cmnd/wardrobe/POWER", "corridor/cmnd/wardrobe/DIMMER", "Dimmer", light_json_parser},
    {13, "corridor/stat/corrcharger/POWER", "corridor/cmnd/corrcharger/POWER", switch_parser},

    {14, "kitchen/stat/kitchswitch123/POWER1", "kitchen/cmnd/kitchswitch123/POWER1", switch_parser},
    {15, "kitchen/stat/kitchenlight/POWER", "kitchen/cmnd/kitchenlight/POWER", switch_parser},
    {16, "kitchen/stat/kitchworkzone/POWER", "kitchen/cmnd/kitchworkzone/POWER", switch_parser},

    {17, "bathroom/stat/bathlight/POWER", "bathroom/cmnd/bathlight/POWER", switch_parser},
    {18, "bathroom/stat/bathfan/POWER", "bathroom/cmnd/bathfan/POWER", switch_parser},
    {19, "bathroom/stat/killswitch/POWER", "bathroom/cmnd/killswitch/POWER", switch_parser},
    {20, "bathroom/stat/toiletsubscription/POWER", "bathroom/cmnd/toiletsubscription/POWER", switch_parser},

    {21, "hall/tele/nightlight/SENSOR", temperature_json_parser},
    {22, "bathroom/tele/bathboiler/SENSOR", energy_json_parser, "Voltage"},
    {23, "bathroom/tele/bathboiler/SENSOR", energy_json_parser, "Power"},
    {24, "bathroom/tele/bathwasher/SENSOR", energy_json_parser, "Power"},

    {25, "stat/sbu/STATE", "cmnd/sbu/STATE", alarm_parser},

    {26, "lady/stat/ladylight/POWER1", "lady/cmnd/ladylight/POWER1", switch_parser},
    {27, "lady/stat/ladylight/POWER2", "lady/cmnd/ladylight/POWER2", switch_parser},
    {28, "kitchen/stat/kitchenfan/POWER", "kitchen/cmnd/kitchenfan/POWER", switch_parser}
} ;

void mqttHandler(char* topic, byte* payload, unsigned int length) {
    for (int i = 0; i < (int)(sizeof(entities) / sizeof(Entity)); i++) {
        Entity *e = &entities[i] ;

        if (strcmp(topic, e->stateTopic) != 0) {
            continue ;
        }

        if (e->parser == NULL) {
            continue ;
        }

        e->parser(e, payload, length) ;
#ifdef NEXX_DEBUG_ENABLE
        Serial.printf("%d state: %d, brightness: %d, value: %3.1f\n", e->id, e->state, e->brightness, e->value) ;
#endif
        if (entity_changed != NULL) {
            entity_changed(e) ;
        }
    }
}

void mqtt_setup() {
    mqttClient.setServer(MQTT_SERVER, 1883) ;
    mqttClient.setCallback(mqttHandler) ;
}

bool mqtt_connected() {
    return mqttClient.connected() ;
}

void mqtt_reconnect() {
    while (!mqttClient.connected()) {
        if (mqttClient.connect(MQTT_CLIENT, MQTT_USER, MQTT_PASSWD, MQTT_LWT_TOPIC, 0, 1, MQTT_LWT_PAYLOAD_OFFLINE)) {
            mqttClient.publish(MQTT_LWT_TOPIC, MQTT_LWT_PAYLOAD_ONLINE, true) ;
            for (Entity e : entities) {
                Debug_print("Subscribe to ") ;
                Debug_print(e.id) ;
                Debug_print(" : ") ;
                Debug_println(e.stateTopic) ;
                mqttClient.subscribe(e.stateTopic) ;
            }

            for (Entity e : entities) {
                if (e.commandTopic == NULL) {
                    continue ;
                }

                mqttClient.publish(e.commandTopic, "", false) ;

                if (e.dimmable) {
                    mqttClient.publish(e.brightnessCommandTopic, "", false) ;
                }
            }
            
        } else {
            delay(5000) ;
        }
    }
}

Entity* mqtt_get_entity(uint8 id) {
    for (int i = 0; i < (int)(sizeof(entities) / sizeof(Entity)); i++) {
        if (entities[i].id == id) {
            return &entities[i] ;
        }
    }
    return NULL ;
}

void mqtt_change_entity(Entity *entity) {
    if (entity->state[0] != '\0') {
        mqttClient.publish(entity->commandTopic, entity->state, false) ;
        return ;
    }

    if (entity->dimmable && entity->brightnessCommandTopic) {
        char buffer[10] ;
        sprintf(buffer, "%d", entity->brightness) ;
        mqttClient.publish(entity->brightnessCommandTopic, buffer, false) ;
    }

    if (entity->commandTopic) {
        mqttClient.publish(entity->commandTopic, entity->onff ? "ON" : "OFF", false) ;
    }
}

boolean mqtt_loop() {
    return mqttClient.loop() ;
}
