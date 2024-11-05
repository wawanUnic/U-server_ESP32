// 404
static const char notFoundContent[] PROGMEM = R"==(
<html>
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Not Found</title>
</head>
<body>
  <h1>Page Not Found (404)</h1>
  <p><a href="/">Home</a><pre>
</body>
)==";

// Universal Web Server
static const char universalWebServer[] PROGMEM = R"==(
<html>
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Universal Web Server</title>
</head>
<body>
  <h1>Universal Web Server</h1>
  <h2>(only HTML, not PHP)</h2>
  <p><a href="/">Start index.html</a><pre>
  <a href="/$list">List files</a><pre>
  <a href="/$systeminfo">System Info</a><pre>
  <a href="/$format">Format memory</a><pre>
  <a href="/$upload">Upload files</a></p>
</body>
)==";

// This function is called when the WebServer was requested to list all existing files in the filesystem.
// a JSON array with file information is returned.
void handleListFiles() {
  Dir dir = LittleFS.openDir("/");
  String result = "<!doctype html>";
  unsigned int count = 0;
//  result += "<html lang='en'>";
  result += "<head>";
  result += "<meta charset=\"utf-8\">";
  result += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  result += "<title>List</title>";
  result += "</head>";
  result += "<body style=\"width:300px\">";
  result += "<h1>List files</h1>";
  result += "<p><a href=\"/$universalwebserver\">Back</a><pre>";
  while (dir.next()) {
    if (result.length() > 4) { result += "<p>"; }
    count += 1;
    result += String(count) + ". Name: \"" + dir.fileName() + "\", ";
    result += " size: " + String(dir.fileSize()) + ", ";
    result += "date/time: " + String(year(dir.fileTime())) + "/" + String(month(dir.fileTime())) + "/" + String(day(dir.fileTime()));
    result += " " + String(hour(dir.fileTime())) + ":" + String(minute(dir.fileTime())) + ":" + String(second(dir.fileTime()));
  }
  result += "</body>";
  server.sendHeader("Cache-Control", "no-cache");
  server.sendHeader("Refresh", "60");
  server.send(200, "text/html; charset=utf-8", result);
}

// This function is called when the sysInfo service was requested.
void handleSysInfo() {
  FSInfo fs_info;
  LittleFS.info(fs_info);
  String result = "<!doctype html>";
//  result += "<html lang='en'>";
  result += "<head>";
  result += "<meta charset=\"utf-8\">";
  result += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  result += "<title>System Info</title>";
  result += "</head>";
  result += "<body style=\"width:300px\">";
  result += "<h1>System Info</h1>";
  result += "<p><a href=\"/$universalwebserver\">Back</a><pre>";
  result += "<p>Flash Size: " + String(ESP.getFlashChipSize());
  result += "<p>Free Heap: " + String(ESP.getFreeHeap());
  result += "<p>FileSystem Total Bytes: " + String(fs_info.totalBytes);
  result += "<p>FileSystem Used Bytes: " + String(fs_info.usedBytes);
  result += "<hr>";
  result += "<p>Chip ID: " + String(ESP.getChipId());
  result += "<p>FlashChip ID: " + String(ESP.getFlashChipId());
  result += "<p>MAC Address: " + String(WiFi.macAddress());
  result += "<p>IP Address: " + WiFi.localIP().toString();
  result += "<p>Hostname: " + newHostName;
  result += "<p>mDNSname: " + newDnsName;
  result += "<hr>";
  result += "<p>Current Date-Time: " + String(year(time(NULL))) + "/" + String(month(time(NULL))) + "/" + String(day(time(NULL)));
  result += " " + String(hour(time(NULL))) + ":" + String(minute(time(NULL))) + ":" + String(second(time(NULL)));
  result += "<p>UpTime: " + uptime_formatter::getUptime();
  result += "</body>";
  server.sendHeader("Cache-Control", "no-cache");
  server.sendHeader("Refresh", "60");
  server.send(200, "text/html; charset=utf-8", result);
}

// format
static const char formatContent[] PROGMEM = R"==(
<html>
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Format memory</title>
</head>
<body>
  <h1>Format memory</h1>
  <p>Are you sure? All data will be destroyed!</p>
  <p><a href="/$universalwebserver">Back</a><pre>
  <a href="/$formatstart">StartFormat...</a>  </p>
</body>
)==";

// format start
static const char formatStart[] PROGMEM = R"==(
<html>
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Memory Erased!</title>
</head>
<body>
  <h1>Format memory</h1>
  <p><a href="/$universalwebserver">Back</a><pre>
  <p>Memory clearing was successful!</p>
</body>
)==";

// upload
static const char uploadContent[] PROGMEM =
R"==(
<!doctype html>
<!-- <html lang='en'> -->
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Upload</title>
</head>
<body style="width:300px">
  <h1>Upload files</h1>
  <div>
    <p><a href="/$universalwebserver">Back</a><pre>
  </div>
  <div id='zone' style='width:16em;height:12em;padding:10px;background-color:#ddd'>Drop files here...</div>
  <script>
    // allow drag&drop of file objects 
    function dragHelper(e) {
      e.stopPropagation();
      e.preventDefault();
    }
    // allow drag&drop of file objects 
    function dropped(e) {
      dragHelper(e);
      var fls = e.dataTransfer.files;
      var formData = new FormData();
      for (var i = 0; i < fls.length; i++) {
        formData.append('file', fls[i], '/' + fls[i].name);
      }
      fetch('/', { method: 'POST', body: formData }).then(function () {
        window.alert('File(s) upload. Done.');
      });
    }
    var z = document.getElementById('zone');
    z.addEventListener('dragenter', dragHelper, false);
    z.addEventListener('dragover', dragHelper, false);
    z.addEventListener('drop', dropped, false);
  </script>
</body>
)==";
