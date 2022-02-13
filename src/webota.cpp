#include "webota.h"
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include "indexhtml.h"
#include "webotahtml.h"
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

long WebOTA::max_sketch_size() {
	return (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000 ;
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

	// Upload firmware page
	server->on(path, HTTP_GET, [server,this]() {
        String s = WEBOTA_HTML ;
        server->send(200, "text/html", s) ;
	});

	// Handling uploading firmware file
	server->on(path, HTTP_POST, [server,this]() {
		server->send(200, "text/plain", (Update.hasError()) ? "Update: fail\n" : "Update: OK!\n");
		delay(500);
		ESP.restart();
	}, [server,this]() {
		HTTPUpload& upload = server->upload() ;

		if (upload.status == UPLOAD_FILE_START) {

			//uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000 ;
			uint32_t maxSketchSpace = this->max_sketch_size() ;

			if (!Update.begin(maxSketchSpace)) {
#ifdef NEXX_DEBUG_ENABLE
			    Update.printError(Serial) ;
#endif
			}
		} else if (upload.status == UPLOAD_FILE_WRITE) {
			if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
#ifdef NEXX_DEBUG_ENABLE
			    Update.printError(Serial) ;
#endif
			}

			// Store the next milestone to output
			uint16_t chunk_size  = 51200 ;
			static uint32_t next = 51200 ;

			// Check if we need to output a milestone (100k 200k 300k)
			if (upload.totalSize >= next) {
				next += chunk_size;
			}
		} else if (upload.status == UPLOAD_FILE_END) {
			if (Update.end(true)) { //true to set the size to the current progress
				Debug_println("Update end") ;
			} else {
#ifdef NEXX_DEBUG_ENABLE
			    Update.printError(Serial) ;
#endif
			}
		}
	});

	server->begin() ;

	return 1 ;
}
