//MKZ4_tank stand alone


#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>


/* Set these to your desired credentials. */
const char *ssid = "MKZ4_Tank";
const char *password = "";

ESP8266WebServer server(8080);
ESP8266WebServer server_8080(8080);

//DRV8835 port assign
#define AIN1 14
#define AIN2 13
#define BIN1 4
#define BIN2 5

//Fire LED assign
#define fire_LED_ON       (digitalWrite( 16, LOW ))
#define fire_LED_OFF       (digitalWrite( 16, HIGH ))

//unsigned char drive_mode = 0;
int pwm[]={512,700,1023};
int t_wait[]={2000,1000,500};
unsigned char s = 1;
unsigned char drive_mode = 0;
boolean action = 0;

//http://192.168.4.1:8080
//HTML User interface
String form ="<html>"
"<head><meta name=viewport content=width=device-width></head>"
"<head>"
"<div style=background-color:#0097C1;>"
"</style>"
"</head>"
"<center>"
"<form action=forward><input type= submit  value=forward  target=tif></form>"
"<form action=back><input type=submit value=back target=tif></form>"
"<form action=left><input type= submit  value=left  target=tif></form>"
"<form action=right><input type= submit  value=right  target=tif></form>"
"<form action=stop><input type=submit value=stop target=tif></form>"
"<form action=fire><input type= submit  value=fire  target=tif></form>"
"<form action=turn><input type=submit value=turn target=tif></form>"
"<form action=fast><input type= submit  value=fast  target=tif></form>"
"<form action=slow><input type=submit value=slow target=tif></form>"
"</center>"
"</div></html>";

//Motor DriverDRV8835 Control
/*
    drive mode0 : stop
    drive mode1 : forward
    drive mode2 : back
    drive mode3 : R left turn
    drive mode4 : R right turn
    drive mode5 : left turn
    drive mode6 : right turn
*/

void DRV8835_Control(){
    if (drive_mode == 0){
      analogWrite(AIN1,pwm[s]);
      analogWrite(AIN2,pwm[s]);
      analogWrite(BIN1,pwm[s]);
      analogWrite(BIN2,pwm[s]);
      delay(100);
    }
    if (drive_mode == 1){
      analogWrite(AIN1,pwm[s]);
      analogWrite(AIN2,0);
      analogWrite(BIN1,pwm[s]);
      analogWrite(BIN2,0);
      delay(100);
    }
    if (drive_mode == 2){
      analogWrite(AIN1,0);
      analogWrite(AIN2,pwm[s]);
      analogWrite(BIN1,0);
      analogWrite(BIN2,pwm[s]);
      delay(100);
    }
    if (drive_mode == 3){
      analogWrite(AIN1,0);
      analogWrite(AIN2,pwm[s]);
      analogWrite(BIN1,pwm[s]);
      analogWrite(BIN2,0);
      delay(100);
    }
    if (drive_mode == 4){
      analogWrite(AIN1,pwm[s]);
      analogWrite(AIN2,0);
      analogWrite(BIN1,0);
      analogWrite(BIN2,pwm[s]);
      delay(100);      
    }
    if (drive_mode == 5){
      analogWrite(AIN1,0);
      analogWrite(AIN2,0);
      analogWrite(BIN1,pwm[s]);
      analogWrite(BIN2,0);
      delay(100);      
    }
    if (drive_mode == 6){
      analogWrite(AIN1,pwm[s]);
      analogWrite(AIN2,0);
      analogWrite(BIN1,0);
      analogWrite(BIN2,0);
      delay(100);
    }


}

/* Just a little test message.  Go to http://192.168.4.1:8080 in a web browser
 * connected to this access point to see it.
 */
void setup() {

//port define
  pinMode(16,OUTPUT);
  fire_LED_ON;
  
  delay(1000);
  fire_LED_OFF;

//Serial setup
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");

  
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

//UI control    
  server.on("/", handleRoot);
  server.on("/stop", handle_stop);
  server.on("/forward", handle_forward);
  server.on("/back", handle_back);
  server.on("/left", handle_left);
  server.on("/right", handle_right);
  server.on("/turn", handle_turn);
  server.on("/fire", handle_fire);
  server.on("/fast", handle_fast);
  server.on("/slow", handle_slow);
 
  server.begin();
  server_8080.begin();
   
  Serial.println("HTTP server started");
}

//main loop
void loop() {
  server.handleClient();
}

//No control
void handleRoot() {
  action = 0;
  server.send(200, "text/html", form);
}

//Tank stop
void handle_stop() {
  Serial.println("stop");
  drive_mode = 0;
  DRV8835_Control();
  action = 0;
  server.send(200, "text/html", form);
}

//Tank forward
void handle_forward() {
  Serial.println("forward");
  drive_mode = 1;
  DRV8835_Control();
  
  server.send(200, "text/html", form);
}

//Tank Back
void handle_back() {
  Serial.println("back");
  drive_mode = 2;
  DRV8835_Control();  

  server.send(200, "text/html", form);
}

//Tank left turn
void handle_left() {
  action=1;  
  if(action) {  
    Serial.println("left");
    drive_mode = 5;
    DRV8835_Control();
      delay(t_wait[s]);
    handle_stop();
  }
  server.send(200, "text/html", form);
}

//Tank right turn
void handle_right() {
  action=1;  
  if(action) {  
    Serial.println("right");
    drive_mode = 6;
    DRV8835_Control();
      delay(t_wait[s]);
    handle_stop(); 
  }
  server.send(200, "text/html", form);
 
}

//Tank rapid turn
void handle_turn() {
  Serial.println("turn");
  drive_mode = 4;
  DRV8835_Control();
  
  server.send(200, "text/html", form);
}

//Tank fire action
void handle_fire() {
  action=1;  
  if(action) {  
    Serial.println("fire");

    handle_back() ;
      delay(50);
    handle_stop();

      delay(300);      
    fire_LED_ON;
    handle_forward() ;
      delay(50);
    handle_stop();        
    fire_LED_OFF;
      delay(10);
  }
  server.send(200, "text/html", form);
}

//Tank speed up(DRV8835 PWM change)
void handle_fast() {
  action=1;  
  if(action) {    
    Serial.println(s);   
    s++;
    Serial.println(s);   
    if (s>=2){s=2;}else{s=1;}
    Serial.print("fast:");  
    Serial.println(s);  
    handle_stop();
  } 
  server.send(200, "text/html", form);
}

//Tank speed down(DRV8835 PWM change)
void handle_slow() {
  action=1;
  if(action) {      
    Serial.println(s);    
    s--;
    Serial.println(s);
    if (s<=1){s=1;}else{s=2;}
    Serial.print("slow:");  
    Serial.println(s);
    handle_stop();
  }   
  server.send(200, "text/html", form);
  }
