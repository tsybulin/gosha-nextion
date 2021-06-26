#ifndef INDEXHTML_H
#define INDEXHTML_H

#include <Arduino.h>

const char INDEX_HTML[] PROGMEM = R"=====(
<html><head> <meta charset="utf-8"> <meta name="viewport" content="width=device-width,initial-scale=1,user-scalable=no"> <title>NEXX - Main</title> <style> body { text-align: center; font-family: verdana, sans-serif; background: #252525; color: whitesmoke; padding: 0; margin: 0; } #content { min-width: 340px; display: inline-block; margin: auto; margin-top: 20px; /* border: 1px solid #666; border-radius: 2px; */ } .btn { border: 0; border-radius: 0.3rem; background: #1fa3ec; color: #faffff; line-height: 2.4rem; font-size: 1.2rem; width: 100%; -webkit-transition-duration: 0.4s; transition-duration: 0.4s; cursor: pointer; } .btn:hover { background: #0e70a4; } .bred { background: #d43535; } .bred:hover { background: #931f1f; } </style></head><body> <div id="content"> <h1>NEXX</h1> <form action="/" method="GET"> <button class="btn">Main menu</button> </form> <form action="/webota" method="GET"> <button class="btn">Update firmware</button> </form> <form action="/reboot" method="GET"> <button class="btn bred">Reboot</button> </form> </div></body></html>
)=====";

#endif
