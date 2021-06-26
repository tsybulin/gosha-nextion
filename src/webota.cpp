#include "webota.h"
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include "indexhtml.h"
#include "webotahtml.h"


ESP8266WebServer OTAServer(9999) ;

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
    String s = INDEX_HTML ;
    server->send(200, "text/html", s) ;
}


int WebOTA::add_http_routes(ESP8266WebServer *server, const char *path) {
	// Index page
	server->on("/", HTTP_GET, [server]() {
		handleIndex(server) ;
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
				//start with max available size
				// Update.printError(Serial) ;
			}
		} else if (upload.status == UPLOAD_FILE_WRITE) {
			/* flashing firmware to ESP*/
			if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
				// Update.printError(Serial) ;
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
				//
			} else {
				//
			}
		}
	});

	server->begin() ;

	return 1 ;
}
