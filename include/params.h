#ifndef PARAMS_H
#define PARAMS_H

#define WIFI_SSID "panda"
#define WIFI_PASSWD "pandapandapan"
#define HOSTNAME "nexx"

#define MQTT_SERVER "172.16.102.251"
#define MQTT_CLIENT HOSTNAME "-123"
#define MQTT_USER "tasmota"
#define MQTT_PASSWD "123456"
#define MQTT_LWT_TOPIC "tele/" HOSTNAME "/LWT"
#define MQTT_LWT_PAYLOAD_OFFLINE "Offline"
#define MQTT_LWT_PAYLOAD_ONLINE "Online"

#endif
