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

void setup() {
    Debug_begin(57600) ;
    Debug_println() ;
    Debug_println("NEXX setup") ;

    nav.setup() ;

    nav.setBootProgress(10) ;

    wifi_connect() ;
    Debug_print("wifi connected ") ;
#ifdef NEXX_DEBUG_ENABLE
    WiFi.localIP().printTo(Serial) ;
#endif
    Debug_println() ;

    nav.setBootProgress(20) ;

    webota.init(80) ;
    if (MDNS.begin(HOSTNAME)) {
        webota.mdns = HOSTNAME ;
    }

    nav.setBootProgress(30) ;

    mqtt_setup() ;

    nav.setBootProgress(40) ;

    if (!mqtt_connected()) {
        mqtt_reconnect() ;
    }

    nav.setBootProgress(50) ;

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
