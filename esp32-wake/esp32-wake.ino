#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_TinyUSB.h>

// Define Wi-Fi credentials
const char* ssid = "It Hertz When IP";
const char* password = "Fortnite123";

// Define the HTTP server
WebServer server(80);

// HID device instance
Adafruit_USBD_HID usb_hid;

// Report descriptor for a mouse
uint8_t const desc_hid_report[] = {
  TUD_HID_REPORT_DESC_MOUSE()
};

void setup() {
  if (!TinyUSBDevice.isInitialized()) {
    TinyUSBDevice.begin(0);
  }

  // Initialize serial for debugging
  Serial.begin(115200);

  // Start Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.println(WiFi.localIP());

  // Set up HID
  usb_hid.setBootProtocol(HID_ITF_PROTOCOL_MOUSE);
  usb_hid.setPollInterval(2); // Set polling interval
  usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  usb_hid.setStringDescriptor("TinyUSB Mouse");
  usb_hid.begin();

  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }

  // Configure HTTP server
  server.on("/wake", handle_WakePC);

  // Start the server
  server.begin();
  Serial.println("HTTP server started");

}

void loop() {
  #ifdef TINYUSB_NEED_POLLING_TASK
  // Manual call tud_task since it isn't called by Core's background
  TinyUSBDevice.task();
  #endif

    // not enumerated()/mounted() yet: nothing to do
  if (!TinyUSBDevice.mounted()) {
    return;
  }

  // Handle client requests
  server.handleClient();
}

// Function to emulate mouse movement to wake PC
void wakePC() {
  Serial.println("Sending wake signal...");

  // Remote wakeup
  if (TinyUSBDevice.suspended()) {
    // Wake up host if we are in suspend mode
    // and REMOTE_WAKEUP feature is enabled by host
    TinyUSBDevice.remoteWakeup();
  }

  if (usb_hid.ready()) {
    uint8_t const report_id = 0; // no ID
    int8_t const delta = 5;
    usb_hid.mouseMove(report_id, delta, delta); // right + down
  }
}

void handle_WakePC() {
    Serial.println("Waking PC");
    wakePC();
    server.send(200, "text/plain", "Wake signal sent!");
}


// #include <WiFi.h>
// #include <WebServer.h>

// /*Put your SSID & Password*/
// const char* ssid = "It Hertz When IP";  // Enter SSID here
// const char* password = "Fortnite123";  //Enter Password here

// WebServer server(80);

// uint8_t LED1pin = 4;
// bool LED1status = LOW;

// uint8_t LED2pin = 5;
// bool LED2status = LOW;

// void setup() {
//   Serial.begin(115200);
//   delay(100);
//   pinMode(LED1pin, OUTPUT);
//   pinMode(LED2pin, OUTPUT);

//   Serial.println("Connecting to ");
//   Serial.println(ssid);

//   //connect to your local wi-fi network
//   WiFi.begin(ssid, password);

//   //check wi-fi is connected to wi-fi network
//   while (WiFi.status() != WL_CONNECTED) {
//   delay(1000);
//   Serial.print(".");
//   }
//   Serial.println("");
//   Serial.println("WiFi connected..!");
//   Serial.print("Got IP: ");  Serial.println(WiFi.localIP());

//   server.on("/", handle_OnConnect);
//   server.on("/led1on", handle_led1on);
//   server.on("/led1off", handle_led1off);
//   server.on("/led2on", handle_led2on);
//   server.on("/led2off", handle_led2off);
//   server.onNotFound(handle_NotFound);

//   server.begin();
//   Serial.println("HTTP server started");
// }
// void loop() {
//   server.handleClient();
//   if(LED1status)
//   {digitalWrite(LED1pin, HIGH);}
//   else
//   {digitalWrite(LED1pin, LOW);}
  
//   if(LED2status)
//   {digitalWrite(LED2pin, HIGH);}
//   else
//   {digitalWrite(LED2pin, LOW);}
// }

// void handle_OnConnect() {
//   LED1status = LOW;
//   LED2status = LOW;
//   Serial.println("GPIO4 Status: OFF | GPIO5 Status: OFF");
//   server.send(200, "text/html", SendHTML(LED1status,LED2status)); 
// }

// void handle_led1on() {
//   LED1status = HIGH;
//   Serial.println("GPIO4 Status: ON");
//   server.send(200, "text/html", SendHTML(true,LED2status)); 
// }

// void handle_led1off() {
//   LED1status = LOW;
//   Serial.println("GPIO4 Status: OFF");
//   server.send(200, "text/html", SendHTML(false,LED2status)); 
// }

// void handle_led2on() {
//   LED2status = HIGH;
//   Serial.println("GPIO5 Status: ON");
//   server.send(200, "text/html", SendHTML(LED1status,true)); 
// }

// void handle_led2off() {
//   LED2status = LOW;
//   Serial.println("GPIO5 Status: OFF");
//   server.send(200, "text/html", SendHTML(LED1status,false)); 
// }

// void handle_NotFound(){
//   server.send(404, "text/plain", "Not found");
// }

// String SendHTML(uint8_t led1stat,uint8_t led2stat){
//   String ptr = "<!DOCTYPE html> <html>\n";
//   ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
//   ptr +="<title>LED Control</title>\n";
//   ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
//   ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
//   ptr +=".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
//   ptr +=".button-on {background-color: #3498db;}\n";
//   ptr +=".button-on:active {background-color: #2980b9;}\n";
//   ptr +=".button-off {background-color: #34495e;}\n";
//   ptr +=".button-off:active {background-color: #2c3e50;}\n";
//   ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
//   ptr +="</style>\n";
//   ptr +="</head>\n";
//   ptr +="<body>\n";
//   ptr +="<h1>ESP32 Web Server</h1>\n";
//     ptr +="<h3>Using Station(STA) Mode</h3>\n";
  
//    if(led1stat)
//   {ptr +="<p>LED1 Status: ON</p><a class=\"button button-off\" href=\"/led1off\">OFF</a>\n";}
//   else
//   {ptr +="<p>LED1 Status: OFF</p><a class=\"button button-on\" href=\"/led1on\">ON</a>\n";}

//   if(led2stat)
//   {ptr +="<p>LED2 Status: ON</p><a class=\"button button-off\" href=\"/led2off\">OFF</a>\n";}
//   else
//   {ptr +="<p>LED2 Status: OFF</p><a class=\"button button-on\" href=\"/led2on\">ON</a>\n";}

//   ptr +="</body>\n";
//   ptr +="</html>\n";
//   return ptr;
// }