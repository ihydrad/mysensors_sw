#include <Arduino.h>
#include <ArduinoOTA.h>
#include <BlynkSimpleEsp8266.h>

//#define DEBUG
//#define WALL
//#define MAIN
#define HOLL
#define SWPIN             12
#define CHECK_INTERVAL    1800000L // 30min
//*****************************************************************************
#ifdef WALL
  char auth[] = "25ffd92d34b54e6d88145c9c9fdc22a3";//wall
  IPAddress ip(192, 168, 0, 163); //wall
  char conn[] = "SW_BED_WALL_CONNECTED\n";
  char state_on[] = "SW_WALL_ON\n";
  char state_off[] = "SW_WALL_OFF\n";
  char host_name[] = "Blynk_bedr_wall";
#else
  #ifdef MAIN
    char auth[] = "a2b463124aac40ca87f2e00a2ea6551a";//main
    IPAddress ip(192, 168, 0, 162); //main
    char conn[] = "SW_BED_CONNECTED\n";
    char state_on[] = "SW_BED_ON\n";
    char state_off[] = "SW_BED_OFF\n";
    char host_name[] = "Blynk_bedr";
  #endif
  #ifdef HOLL
    char auth[] = "a6a115ee102841e7a359010d841ec8fa";//holl
    IPAddress ip(192, 168, 0, 164); //main
    char conn[] = "SW_HOLL_CONNECTED\n";
    char state_on[] = "SW_HOLL_ON\n";
    char state_off[] = "SW_HOLL_OFF\n";
    char host_name[] = "Blynk_holl";
  #endif
#endif
//*****************************************************************************
char token_getway[] = "e9f5d5d304ea488797066f87080e9864";
char domain[] = "192.168.0.103";
uint16_t port = 8080;
char ssid[] = "tenda";
char pass[] = "89174600028";

IPAddress     gateway(192,168,0,1);
IPAddress     subnet(255,255,255,0);
BlynkTimer    timer;
WidgetBridge  bridge_to_getway(V1);

void setup(){
  #ifdef DEBUG
    Serial.begin(9600);
    Serial.println("Started!");
  #endif
  pinMode(SWPIN, OUTPUT);
  digitalWrite(SWPIN, 1); // switch ON after power release
  ArduinoOTA.begin();
  Blynk.begin(auth, ssid, pass, domain, port);
  WiFi.config(ip, gateway, subnet);
  WiFi.hostname(host_name);
  Blynk.virtualWrite(V0, 1); //send to Blynk serv after connect "switch state ON"
  timer.setInterval(CHECK_INTERVAL, myTimerEvent);
}

void refresh_state(){
  if(digitalRead(SWPIN)){
      Blynk.virtualWrite(V0, 1);
      bridge_to_getway.virtualWrite(V1, state_on); //в терминал
  }
  else{
      Blynk.virtualWrite(V0, 0);
      bridge_to_getway.virtualWrite(V1, state_off); //в терминал
  }
}

BLYNK_CONNECTED(){
  bridge_to_getway.setAuthToken(token_getway);
  bridge_to_getway.virtualWrite(V1, conn);
  refresh_state();
}

void myTimerEvent(){
  refresh_state();
}

BLYNK_WRITE(V0){
  if(param.asInt() == 101){
    if(digitalRead(SWPIN)){
      digitalWrite(SWPIN, 0); //switch power OFF
      Blynk.virtualWrite(V0, 0);
      bridge_to_getway.virtualWrite(V1, state_off);
    }else{
      digitalWrite(SWPIN, 1); //switch power ON
      Blynk.virtualWrite(V0, 1);
      bridge_to_getway.virtualWrite(V1, state_on);
    }
  }else{
    if(param.asInt() && !digitalRead(SWPIN)){
      digitalWrite(SWPIN, 1); //switch power ON
      bridge_to_getway.virtualWrite(V1, state_on);
    }

    else if(!param.asInt() && digitalRead(SWPIN)){
      digitalWrite(SWPIN, 0); //switch power OFF
      bridge_to_getway.virtualWrite(V1, state_off);
    }
  }
}

void loop() {
  ArduinoOTA.handle();
  Blynk.run();
  timer.run();
}
