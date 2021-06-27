#ifndef INDEXHTML_H
#define INDEXHTML_H

#include <Arduino.h>

const char INDEX_HTMLX[] PROGMEM = R"=====(
<html><head> <meta charset="utf-8"> <meta name="viewport" content="width=device-width,initial-scale=1,user-scalable=no"> <title>NEXX - Main</title> <style> body { text-align: center; font-family: verdana, sans-serif; background: #252525; color: whitesmoke; padding: 0; margin: 0; } #content { min-width: 340px; display: inline-block; margin: auto; margin-top: 20px; /* border: 1px solid #666; border-radius: 2px; */ } .btn { border: 0; border-radius: 0.3rem; background: #1fa3ec; color: #faffff; line-height: 2.4rem; font-size: 1.2rem; width: 100%; -webkit-transition-duration: 0.4s; transition-duration: 0.4s; cursor: pointer; } .btn:hover { background: #0e70a4; } .bred { background: #d43535; } .bred:hover { background: #931f1f; } </style>
<script>
function bodyReady() {
var currentdate = new Date() ;
document.getElementById('year').setAttribute('value', currentdate.getFullYear()) ;
document.getElementById('month').setAttribute('value', currentdate.getMonth() +1) ;
document.getElementById('day').setAttribute('value', currentdate.getDate()) ;
document.getElementById('hour').setAttribute('value', currentdate.getHours()) ;
document.getElementById('mins').setAttribute('value', currentdate.getMinutes()) ;
document.getElementById('secs').setAttribute('value', currentdate.getSeconds()) ;
}
</script>
</head> <body onload="bodyReady()"> <div id="content"> <h1>NEXX</h1> <form action="/" method="GET"> <button class="btn">Main menu</button> </form> <form action="/webota" method="GET"> <button class="btn">Update firmware</button> </form>
<form action="/dt" method="GET">
<input type="hidden" name="year" id="year" value="">
<input type="hidden" name="month" id="month" value="">
<input type="hidden" name="day" id="day" value="">
<input type="hidden" name="hour" id="hour" value="">
<input type="hidden" name="mins" id="mins" value="">
<input type="hidden" name="secs" id="secs" value="">
<button class="btn">Set Date Time</button>
</form>
<form action="/reboot" method="GET"> <button class="btn bred">Reboot</button> </form> </div></body></html>
)=====";

#endif
