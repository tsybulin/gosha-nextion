#ifndef WEBOTA_H
#define WEBOTA_H

#include <Arduino.h>
#include <ESP8266WebServer.h>

typedef void (*date_time_set_t)(int, int, int, int, int, int) ;

class WebOTA {
	public:
		unsigned int port ;
		String path = "" ;
		String mdns = "" ;

		int init(const unsigned int port, const char *path) ;
		int init(const unsigned int port) ;
		int init() ;
		void delay(unsigned int ms) ;

		int handle() ;
	private:
		bool init_has_run ;
		int add_http_routes(ESP8266WebServer *server, const char *path) ;
} ;

#endif
