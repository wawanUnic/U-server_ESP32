static const char uploadContent[] PROGMEM =
R"==(
<!doctype html>
<html lang='en'>

<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Upload</title>
</head>

<body style="width:300px">
  <h1>Upload files</h1>
  <div>
    <p><a href="/">Home</a><pre>
    <a href="/$list">List lifes</a><pre>
    <a href="/$sysinfo">SysInfo</a><pre>
    <a href="/$format">Format memory</a><pre>
    <a href="/$upload.htm">Upload files</a></p>
  </div>
  <hr>
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

static const char notFoundContent[] PROGMEM = R"==(
<html>
<head>
  <title>Ressource not found</title>
</head>
<body>
  <h1>Universal Web-server (only HTM, not PHP)</h1>
  <p>The ressource was not found (index.htm).</p>
  <p><a href="/">Start again</a><pre>
  <a href="/$list">List files</a><pre>
  <a href="/$sysinfo">SysInfo</a><pre>
  <a href="/$format">Format memory</a><pre>
  <a href="/$upload.htm">Upload files</a></p>
</body>
)==";

static const char formatContent[] PROGMEM = R"==(
<html>
<head>
  <title>Format memory</title>
</head>
<body>
  <h1>Format memory</h1>
  <p>Are you sure? All data will be destroyed!</p>
  <p><a href="/">Back</a><pre>
  <a href="/$formatstart">StartFormat...</a>  </p>
</body>
)==";

static const char formatStart[] PROGMEM = R"==(
<html>
<head>
  <title>Memory Erased!</title>
</head>
<body>
  <h1>Format memory</h1>
  <p>Memory clearing was successful!</p>
  <p><a href="/">Home</a><pre></p>
</body>
)==";