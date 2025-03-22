#include <OTAWebPage.h>

#include <WiFiClient.h>
#include <WebServer.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <ESPmDNS.h>
#include <Update.h>


extern const char* ssid;

extern WebServer server;

const char* host = "esp32";

const char* loginIndex =
  "<Body>"
  "<h2>GPAD_API  </h2>"
  "<form name='loginForm'>"
  "<table width='20%' bgcolor='A09F9F' align='center'>"
  "<tr>"
  "<td colspan=2>"
  "<center><font size=4><b>ESP32 Login Page</b></font></center>"
  "<br>"
  "</td>"
  "<br>"
  "<br>"
  "</tr>"
  "<td>Username:</td>"
  "<td><input type='text' size=25 name='userid'><br></td>"
  "</tr>"
  "<br>"
  "<br>"
  "<tr>"
  "<td>Password:</td>"
  "<td><input type='Password' size=25 name='pwd'><br></td>"
  "<br>"
  "<br>"
  "</tr>"
  "<tr>"
  "<td><input type='submit' onclick='check(this.form)' value='Login'></td>"
  "</tr>"
  "</table>"
  "</form>"
  "<script>"
  "function check(form)"
  "{"
  "if(form.userid.value=='admin' && form.pwd.value=='admin')"
  "{"
  "window.open('/serverIndex')"
  "}"
  "else"
  "{"
  " alert('Error Password or Username')/*displays error message*/"
  "}"
  "}"
  "</script>"
  "</Body>";

/*
 * Server Index Page
 */

const char* serverIndex =
  "<Body>"
  "<h2> OTA two </h2>"
  "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
  "<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
  "<input type='file' name='update'>"
  "<input type='submit' value='Update'>"
  "</form>"
  "<div id='prg'>progress: 0%</div>"
  "<script>"
  "$('form').submit(function(e){"
  "e.preventDefault();"
  "var form = $('#upload_form')[0];"
  "var data = new FormData(form);"
  " $.ajax({"
  "url: '/update',"
  "type: 'POST',"
  "data: data,"
  "contentType: false,"
  "processData:false,"
  "xhr: function() {"
  "var xhr = new window.XMLHttpRequest();"
  "xhr.upload.addEventListener('progress', function(evt) {"
  "if (evt.lengthComputable) {"
  "var per = evt.loaded / evt.total;"
  "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
  "}"
  "}, false);"
  "return xhr;"
  "},"
  "success:function(d, s) {"
  "console.log('success!')"
  "},"
  "error: function (a, b, c) {"
  "}"
  "});"
  "});"
  "</script>"
  "</Body>";

/*
 * setup function
 */
void setupfunction(Stream *serialport) {
  // pinMode(led, OUTPUT);

  // Serial.begin(115200);

  // // Connect to WiFi network
  // WiFi.begin(ssid, password);
  // Serial.println("");

  // // Wait for connection
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(500);
  //   Serial.print(".");
  // }
  serialport->println("");
  serialport->print("Connected to ");
  serialport->println(ssid);
  serialport->print("IP address: ");
  serialport->println(WiFi.localIP());

  /*use mdns for host name resolution*/
  if (!MDNS.begin(host)) {  //http://esp32.local
    serialport->println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  serialport->println("mDNS responder started");
  /*return index page which is stored in serverIndex */
  server.on("/", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", loginIndex);
  });
  server.on("/serverIndex", HTTP_GET, []() {
    server.sendHeader("Connection", "close");
    server.send(200, "text/html", serverIndex);
  });
  /*handling uploading firmware file */
  server.on(
    "/update", HTTP_POST, []() {
      server.sendHeader("Connection", "close");
      server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
      ESP.restart();
    },
    [serialport]() {
      HTTPUpload& upload = server.upload();
      if (upload.status == UPLOAD_FILE_START) {
        serialport->printf("Update: %s\n", upload.filename.c_str());
        if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {  //start with max available size
          Update.printError(*serialport);
        }
      } else if (upload.status == UPLOAD_FILE_WRITE) {
        /* flashing firmware to ESP*/
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
          Update.printError(*serialport);
        }
      } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) {  //true to set the size to the current progress
          serialport->printf("Update Success: %u\nRebooting...\n", upload.totalSize);
        } else {
          Update.printError(*serialport);
        }
      }
    });
  server.begin();
}