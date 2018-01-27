#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

#define TRIGGER 16
#define ECHO    12
#define ESP8266_LED 5
#define KNOCK A0
#define BUTTON 0
#define SHEBANG 4
#define TWITCH 13
#define THRESHHOLD 100
#define DIST_THRESH 24
#define DIST_START 10

#define FIREBASE_HOST "jarjar-941c8.firebaseapp.comm"
#define FIREBASE_AUTH "ALBY3k9LtZB5JfroLesiWZzW25NMI6jTI5IgYVYp"
#define WIFI_SSID "TreatYoSelf"
#define WIFI_PASSWORD "iotdevfest18"
 
int state = HIGH;      // the current state of the output pin
int reading = 0;           // the current reading from the input pin
int previous = LOW;    // the previous reading from the input pin
long btnTime = 0;
long debounce = 200;

// NodeMCU Pin D1 > TRIGGER | Pin D2 > ECHO
 
void setup() {
 
  Serial.begin (9600);
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(ESP8266_LED, OUTPUT);
  pinMode(KNOCK,INPUT);
  pinMode(BUTTON, INPUT);

    // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  Firebase.set("pushbutton", 0);
  Firebase.set("distanceSensor", 0);
  Firebase.set("knockSensor", 0);
  
  Firebase.set("pushButtonEnabled", 1);
  Firebase.set("knockEnabled", 1);
  Firebase.set("distanceEnabled", 1);
}
void loop() 
{
  distanceSensor();
  knockSensor();
  readButtoner();
  if(Firebase.getInt("fullshebang") > 0){
    Firebase.set("fullshebang", 0);
    fullShebang();
  }
  if(Firebase.getInt("twitch") > 0){
    Firebase.set("twitch", 0);
    twitch();
  }
}

void distanceSensor(){
  long duration;
  int distance;
  digitalWrite(TRIGGER, LOW);  
  delayMicroseconds(2); 
  
  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10); 
  
  digitalWrite(TRIGGER, LOW);
  duration = pulseIn(ECHO, HIGH);
  distance = (duration/2) / 29.1;
  
  if(DIST_THRESH > distance){
    Firebase.set("distanceSensor",distance);
    int triggers = Firebase.getInt("distanceTriggers");
    Firebase.set("distanceTriggers", triggers++);
    if(Firebase.getInt("distanceEnabled") > 0){
      if (DIST_START > distance){
        fullShebang();
      } else {
        twitch();
      }
    }
  }
  delay(10);
}

void knockSensor(){
  int val = 0;
  val = analogRead(KNOCK);
  if(val > THRESHHOLD){
    //run it
     int triggers = Firebase.getInt("knockTriggers");
    Firebase.set("knockTriggers", triggers++);
    if(Firebase.getInt("knockEnabled") > 0){
      twitch();
    }
  }
}

void readButtoner(){
   reading = digitalRead(BUTTON);
  // if the input just went from LOW and HIGH and we've waited long enough
  // to ignore any noise on the circuit, toggle the output pin and remember
  // the time
  if (reading == HIGH && previous == LOW && millis() - btnTime > debounce) {
    if (state == HIGH)
      state = LOW;
      Firebase.set("pushButton", 1);
      int triggers = Firebase.getInt("buttonTriggers");
    Firebase.set("buttonTriggers", triggers++);
      if(Firebase.getInt("pushButtonEnabled") > 0){
        //run it
        fullShebang();
      }
    else
      state = HIGH;
    btnTime = millis();    
  }
}

void fullShebang(){
  digitalWrite(SHEBANG,HIGH);
  int triggers = Firebase.getInt("shebangTriggers");
    Firebase.set("shebangTriggers", triggers++);
    delay(200);
    digitalWrite(SHEBANG, LOW);
}

void twitch(){
  digitalWrite(TWITCH, LOW);
  delay(50);
  digitalWrite(TWITCH, HIGH);
  int triggers = Firebase.getInt("twitchTriggers");
   Firebase.set("twitchTriggers", triggers++);
}



