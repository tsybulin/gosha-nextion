#include "webota.h"
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include "indexhtml.h"
#include "debugger.h"

ESP8266WebServer OTAServer(9999) ;

extern date_time_set_t date_time_set ;

int WebOTA::init(const unsigned int port, const char *path) {
    this->port = port ;
	this->path = path ;

	if (this->init_has_run) {
		return 0 ;
	}

	add_http_routes(&OTAServer, path) ;
	OTAServer.begin(port) ;
	this->init_has_run = true ;
	return 1 ;
}

int WebOTA::init(const unsigned int port) {
	return WebOTA::init(port, "/webota") ;
}

int WebOTA::init() {
	return WebOTA::init(8080, "/webota") ;
}

int WebOTA::handle() {
	if (!this->init_has_run) {
		WebOTA::init() ;
	}

	OTAServer.handleClient() ;
	MDNS.update() ;

	return 1 ;
}

void WebOTA::delay(unsigned int ms) {
	unsigned long last = millis() ;

	while ((millis() - last) < ms) {
		OTAServer.handleClient() ;
		::delay(5) ;
	}
}

void handleIndex(ESP8266WebServer *server) {
    String s = INDEX_HTMLX ;
    server->send(200, "text/html", s) ;
}


int WebOTA::add_http_routes(ESP8266WebServer *server, const char *path) {
	// Index page
	server->on("/", HTTP_GET, [server]() {
		handleIndex(server) ;
	}) ;

	server->on("/dt", HTTP_GET, [server,this]() {
		String year = server->arg("year") ;
		String month = server->arg("month") ;
		String day = server->arg("day") ;

		String hour = server->arg("hour") ;
		String mins = server->arg("mins") ;
		String secs = server->arg("secs") ;

		handleIndex(server) ;
		unsigned long now = millis() ;
		while (millis() - now < 1000) {
			server->handleClient() ;
		}

		if (date_time_set) {
			date_time_set(year.toInt(), month.toInt(), day.toInt(), hour.toInt(), mins.toInt(), secs.toInt()) ;
		}

	}) ;

	server->on("/reboot", HTTP_GET, [server]() {
		handleIndex(server) ;
		unsigned long now = millis() ;
		while (millis() - now < 1000) {
			server->handleClient() ;
		}
		ESP.restart() ;
	}) ;


	server->begin() ;

	return 1 ;
}
