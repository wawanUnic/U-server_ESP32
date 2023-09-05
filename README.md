# U-server_ESP8266
ESP8266 - Uniwersal HTM(l) serwer.
No PHP, only HTM(l)


It features

* http access to the web server
* deliver all files from the file system
* deliver special built-in files
* implement services (list files, sysinfo)
* uploading files using drag & drop
* listing and deleting files using a SPA application
* Example of SPA and Web Service application 
* Only files in the root folder are supported for simplicity - no directories.

In the setup() function in the sketch file the following steps are implemented to make the webserver available on the local network.

* Create a webserver listening to port 80 for http requests.
* Initialize the access to the filesystem in the free flash memory (typically 2MByte).
* Connect to the local WiFi network. Here is only a straight-forward implementation hard-coding network name and passphrase. You may consider to use something like the WiFiManager library.
* Register the device in DNS using a known hostname.
* Starting the web server.

In the loop() function the web server will be given time to receive and send network packages by calling
`server.handleClient();`.

Registering function is the simplest integration mechanism available to add functionality. The server offers the `on(path, function)` methods that take the URL and the function as parameters.

There are 2 functions implemented that get registered to handle incoming GET requests for given URLs.

The JSON data format is used often for such services as it is the "natural" data format of the browser using javascript.

When the **handleSysInfo()** function is registered and a browser requests for <http://webserver/$sysinfo> the function will be called and can collect the requested information.

The result in this case is a JSON object that is assembled in the result String variable and the returned as a response to the client also giving the information about the data format.

The function **handleList()** is registered the same way to return the list of files in the file system also returning a JSON object including name, size and the last modification timestamp.

This is an example of registering a inline function in the web server.
The 2. parameter of the on() method is a so called CPP lamda function (without a name) 
that actually has only one line of functionality by sending a string as result to the client.

Here the text from a static String with html code is returned instead of a file from the filesystem. 
The content of this string can be found in the file `builtinfiles.h`. It contains a small html+javascript implementation
that allows uploading new files into the empty filesystem.

Just open <http://webserver/$upload.htm> and drag some files from the data folder on the drop area.

Often servers are addressed by using the base URL like <http://webserver/> where no further path details is given.
Of course we like the user to be redirected to something usable. 
The `handleRoot()` function checks the filesystem for the file named **/index.htm** and creates a redirect to this file when the file exists.
Otherwise the redirection goes to the built-in **/$upload.htm** web page.

The example also implements the class `FileServerHandler` derived from the class `RequestHandler` to plug in functionality
that can handle more complex requests without giving a fixed URL.
It implements uploading and deleting files in the file system that is not implemented by the standard server.serveStatic functionality.

This class has to implements several functions and works in a more detailed way:

* The `canHandle()` method can inspect the given http method and url to decide weather the RequestFileHandler can handle the incoming request or not.

  In this case the RequestFileHandler will return true when the request method is an POST for upload or a DELETE for deleting files.

  The regular GET requests will be ignored and therefore handled by the also registered server.serveStatic handler.

* The function `handle()` then implements the real deletion of the file.

* The `canUpload()`and `upload()` methods work similar while the `upload()` method is called multiple times to create, append data and close the new file.

Any other incoming request that was not handled by the registered plug-ins above can be detected by registering 

This allows sending back an "friendly" result for the browser. Here a sim ple html page is created from a static string.
You can easily change the html code in the file `builtinfiles.h`.
