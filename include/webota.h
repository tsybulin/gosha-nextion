#ifndef WEBOTA_H
#define WEBOTA_H

#include <Arduino.h>
#include <ESP8266WebServer.h>

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
		String get_ota_html() ;
		long max_sketch_size() ;
} ;

#endif
