#ifndef WEBOTAHTML_H
#define WEBOTAHTML_H

#include <Arduino.h>

const char WEBOTA_HTML[] PROGMEM = R"=====(
<html><head> <meta charset="utf-8"> <meta name="viewport" content="width=device-width,initial-scale=1,user-scalable=no"> <title>NEXX - Update</title> <style> body { text-align: center; font-family: verdana, sans-serif; background: #252525; color: whitesmoke; padding: 0; margin: 0; } #content { min-width: 340px; display: inline-block; margin: auto; margin-top: 20px; /* border: 1px solid #666; border-radius: 2px; */ } .btn { border: 0; border-radius: 0.3rem; background: #1fa3ec; color: #faffff; line-height: 2.4rem; font-size: 1.2rem; width: 100%; padding: 15px; -webkit-transition-duration: 0.4s; transition-duration: 0.4s; cursor: pointer; } .btn:hover { background: #0e70a4; } .bred { background: #d43535; } .bred:hover { background: #931f1f; } #prg_wrap { border: 0px solid; width: 100%; } #prg { text-shadow: 2px 2px 3px black; padding: 5px 0; display: none; border: 1px solid #008aff; background: #002180; text-align: center; color: white; } </style>
    <script>
        var domReady = function (callback) {
            document.readyState === "interactive" || document.readyState === "complete" ? callback() : document.addEventListener("DOMContentLoaded", callback);
        };

        domReady(function () {
            var myform = document.getElementById('upload_form');
            var filez = document.getElementById('file');

            myform.onsubmit = function (event) {
                event.preventDefault();

                var formData = new FormData();
                var file = filez.files[0];

                if (!file) { return false; }

                formData.append("files", file, file.name);

                var xhr = new XMLHttpRequest();
                xhr.upload.addEventListener("progress", function (evt) {
                    if (evt.lengthComputable) {
                        var per = Math.round((evt.loaded / evt.total) * 100);
                        var prg = document.getElementById('prg');

                        prg.innerHTML = per + "%"
                        prg.style.width = per + "%"
                        prg.style.display = "block"
                    }
                }, false);
                xhr.open('POST', location.href, true);

                // Set up a handler for when the request finishes.
                xhr.onload = function () {
                    if (xhr.status === 200) {
                        //alert('Success');
                    } else {
                        //alert('An error occurred!');
                    }
                };

                xhr.send(formData);
            }
        });
    </script>
</head><body> <div id="content"> <h1>NEXX</h1> <form method="POST" action="#" enctype="multipart/form-data" id="upload_form"> <input type="file" class="btn" name="update" id="file"> <input type="submit" class="btn bred" value="Update"> </form> <div id="prg_wrap"> <div id="prg"> </div> </div> <form action="/" method="GET"> <button class="btn">Main menu</button> </form> </div></body></html>
)=====";

#endif
