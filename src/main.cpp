#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include "wifi.h"
#include "webota.h"
#include "params.h"
#include "mqtt.h"
#include "navigator.h"
#include "entity.h"
#include "debugger.h"

WebOTA webota ;
Navigator nav ;

void entityChanged(Entity *entity) {
    nav.onEntityChanged(entity) ;
}

entity_changed_t entity_changed = entityChanged ;

Entity* getEntity(uint8 id) {
    return mqtt_get_entity(id) ;
}

entity_get_t entity_get = getEntity ;

void changeEntity(Entity *entity) {
    mqtt_change_entity(entity) ;
}

entity_changed_t change_entity = changeEntity ;

void nav_setDateTime(int year, int month, int day, int hour, int mins, int secs) {
    nav.setDateTime(year, month, day, hour, mins, secs) ;
}

date_time_set_t date_time_set = nav_setDateTime ;

void setup() {
    Debug_begin(57600) ;
    Debug_println() ;
    Debug_println("NEXX setup 1") ;

    nav.setup() ;

    nav.setBootProgress(20) ;

    wifi_connect() ;
    Debug_print("wifi connected ") ;
#ifdef NEXX_DEBUG_ENABLE
    WiFi.localIP().printTo(Serial) ;
#endif
    Debug_println() ;

    nav.setBootProgress(40) ;

    webota.init(80) ;
    if (MDNS.begin(HOSTNAME)) {
        webota.mdns = HOSTNAME ;
    }

    nav.setBootProgress(60) ;

    mqtt_setup() ;

    nav.setBootProgress(80) ;

    if (!mqtt_connected()) {
        mqtt_reconnect() ;
    }

    nav.setBootProgress(90) ;

    nav.showHomePage() ;
}

void loop() {
    if (!mqtt_connected()) {
        mqtt_reconnect() ;
    }
    mqtt_loop() ;
    nav.loop() ;
    webota.handle() ;
    yield() ;
}
