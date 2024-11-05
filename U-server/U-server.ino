#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <TimeLib.h>
#include <uptime_formatter.h>
#include <FS.h>        // File System for Web Server Files
#include <LittleFS.h>  // This file system is used.

#include "secrets.h"  // add WLAN Credentials in here

// mark parameters not used in example
#define UNUSED __attribute__((unused))

// TRACE output simplified, can be deactivated here
#define TRACE(...) Serial.printf(__VA_ARGS__)

// name of the server. You reach it using http://webserver
#define HOSTNAME "u-server1"

// need a WebServer for http access on port 80.
ESP8266WebServer server(80);

// переменные для мигания светодоиодом
unsigned long previousMillis = 0;  // хранит время последнего обновления светодиода
const long interval = 1000;        // интервал мигания (миллисекунды)
int ledState = LOW;                // ledState используется для установки состояния светодиода

#include "builtinfiles.h" // The text of builtin files are in this header file

// ===== Simple functions used to answer simple GET requests =====

// This function is called when the WebServer was requested without giving a filename.
// This will redirect to the file index.htm when it is existing otherwise to the built-in $upload.htm page
void handleRedirect() {
  TRACE("Redirect...");
  String url = "/index.html";

  if (!LittleFS.exists(url)) { url = "/$notfound"; }

  server.sendHeader("Location", url, true);
  server.send(302);
}

void handleFormatStart() {
  LittleFS.format();
  server.sendHeader("Cache-Control", "no-cache");
  server.send(200, "text/html", FPSTR(formatStart));
}

// ===== Request Handler class used to answer more complex requests =====

// The FileServerHandler is registered to the web server to support DELETE and UPLOAD of files into the filesystem.
class FileServerHandler : public RequestHandler {
public:
  // @brief Construct a new File Server Handler object
  // @param fs The file system to be used.
  // @param path Path to the root folder in the file system that is used for serving static data down and upload.
  // @param cache_header Cache Header to be used in replies.
  FileServerHandler() {
    TRACE("FileServerHandler is registered\n");
  }


  // @brief check incoming request. Can handle POST for uploads and DELETE.
  // @param requestMethod method of the http request line.
  // @param requestUri request ressource from the http request line.
  // @return true when method can be handled.
  bool canHandle(HTTPMethod requestMethod, const String UNUSED &_uri) override {
    return ((requestMethod == HTTP_POST) || (requestMethod == HTTP_DELETE));
  }  // canHandle()


  bool canUpload(const String &uri) override {
    // only allow upload on root fs level.
    return (uri == "/");
  }  // canUpload()


  bool handle(ESP8266WebServer &server, HTTPMethod requestMethod, const String &requestUri) override {
    // ensure that filename starts with '/'
    String fName = requestUri;
    if (!fName.startsWith("/")) { fName = "/" + fName; }

    if (requestMethod == HTTP_POST) {
      // all done in upload. no other forms.

    } else if (requestMethod == HTTP_DELETE) {
      if (LittleFS.exists(fName)) { LittleFS.remove(fName); }
    }  // if

    server.send(200);  // all done.
    return (true);
  }  // handle()


  // uploading process
  void upload(ESP8266WebServer UNUSED &server, const String UNUSED &_requestUri, HTTPUpload &upload) override {
    // ensure that filename starts with '/'
    String fName = upload.filename;
    if (!fName.startsWith("/")) { fName = "/" + fName; }

    if (upload.status == UPLOAD_FILE_START) {
      // Open the file
      if (LittleFS.exists(fName)) { LittleFS.remove(fName); }  // if
      _fsUploadFile = LittleFS.open(fName, "w");

    } else if (upload.status == UPLOAD_FILE_WRITE) {
      // Write received bytes
      if (_fsUploadFile) { _fsUploadFile.write(upload.buf, upload.currentSize); }

    } else if (upload.status == UPLOAD_FILE_END) {
      // Close the file
      if (_fsUploadFile) { _fsUploadFile.close(); }
    }  // if
  }    // upload()

protected:
  File _fsUploadFile;
};


// Setup everything to make the webserver work.
void setup(void) {

  pinMode(LED_BUILTIN, OUTPUT);

  // Аутентификация по умолчанию
  ArduinoOTA.setPassword(passwordOTA);

  delay(3000);  // wait for serial monitor to start completely.

  // Use Serial port for some trace information from the example
  Serial.begin(115200);
  Serial.setDebugOutput(false);

  TRACE("Starting WebServer example...\n");

  TRACE("Mounting the filesystem...\n");
  if (!LittleFS.begin()) {
    TRACE("could not mount the filesystem...\n");
    delay(2000);
    ESP.restart();
  }

  // start WiFI
  WiFi.mode(WIFI_STA);
  WiFi.hostname(newHostName.c_str());
  if (strlen(ssid) == 0) {
    WiFi.begin();
  } else {
    WiFi.begin(ssid, passPhrase);
  }

  // allow to address the device by the given name e.g. http://webserver
  MDNS.begin(newDnsName);

  TRACE("Connect to WiFi...\n");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    TRACE(".");
  }
  TRACE("connected.\n");

  // Ask for the current time using NTP request builtin into ESP firmware.
  TRACE("Setup ntp...\n");
  //const char *TZstr = "CST+6CDT,M3.2.0/2,M11.1.0/2";
  const char *TZstr = "CET";
  configTime (TZstr, "pool.ntp.org");
  //configTime(TIMEZONE, "pool.ntp.org");
  //configTime(3600, 0, "pool.ntp.org");

  TRACE("Register service handlers...\n");

  server.on("/", HTTP_GET, handleRedirect);
  server.on("/$universalwebserver", []() { server.send(200, "text/html", FPSTR(universalWebServer)); });
  server.on("/$list", HTTP_GET, handleListFiles);
  server.on("/$systeminfo", HTTP_GET, handleSysInfo);
  server.on("/$format", []() { server.send(200, "text/html", FPSTR(formatContent)); });
  server.on("/$formatstart", HTTP_GET, handleFormatStart);
  server.on("/$upload", []() { server.send(200, "text/html", FPSTR(uploadContent)); });

  // register a redirect handler when only domain name is given.

  // UPLOAD and DELETE of files in the file system using a request handler.
  server.addHandler(new FileServerHandler());

  // enable CORS header in webserver results
  server.enableCORS(true);

  // enable ETAG header in webserver results from serveStatic handler
  server.enableETag(true);

  // serve all static files
  server.serveStatic("/", LittleFS, "/");

  server.onNotFound([]() { server.send(404, "text/html", FPSTR(notFoundContent)); });

  ArduinoOTA.begin();

  server.begin();
  TRACE("ip = %s\n", WiFi.localIP());
  TRACE("hostname = %s\n", WiFi.getHostname());
  MDNS.addService("http","tcp",80);
}

void loop(void) {
  server.handleClient();
  ArduinoOTA.handle();
  MDNS.update();

  unsigned long currentMillis = millis(); // цикл для мигания без использования задержек
  if (currentMillis - previousMillis >= interval) 
  {
    previousMillis = currentMillis; // сохранить последнее внемя, когда мигнули светодиодом
    ledState = not(ledState); // если светодиод погашен, включить его, и наоборот
    digitalWrite(LED_BUILTIN,  ledState); // установить состояние светодиода в соответствии с переменной
  }
}
