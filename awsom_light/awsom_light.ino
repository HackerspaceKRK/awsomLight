// based on TwitterClient example from Arduino IDE
#include <SPI.h>
#include <Ethernet.h>
#include <avr/wdt.h>

int latchPin = 8;
int clockPin = 7;
int dataPin = 9;

byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x5D, 0xDD };
byte ip[] = { 192, 168, 1, 20 };
byte gw[] = { 192, 168, 1, 4 };
byte dnsAddr[] = { 8, 8, 8, 8 };
byte subnet[] = { 255, 255, 255, 0 };
byte server[] = { 192, 168, 1, 5 };

EthernetClient client;

const unsigned long requestInterval = 500;
unsigned long lastAttemptTime = 0;

String currentLine = "";  
String tweet = "";
boolean readingTweet = false;

void shiftOut(byte myDataOut) {
  int myDataPin = dataPin;
  int myClockPin = clockPin;
  int i=0;
  int pinState;
  pinMode(myClockPin, OUTPUT);
  pinMode(myDataPin, OUTPUT);

  digitalWrite(myDataPin, 0);
  digitalWrite(myClockPin, 0);

  for (i=0; i<8; i++)  {
    digitalWrite(myClockPin, 0);
    
    if ( myDataOut & (1<<i) ) {
      pinState= 1;
    }
    else {	
      pinState= 0;
    }

    digitalWrite(myDataPin, pinState);
    digitalWrite(myClockPin, 1);
    digitalWrite(myDataPin, 0);
  }

  //stop shifting
  digitalWrite(myClockPin, 0);
}

void setup() {
  Serial.begin(9600);
  
  pinMode(latchPin, OUTPUT);
  digitalWrite(latchPin, 0);
  shiftOut(0);
  shiftOut(0);
  digitalWrite(latchPin, 1);
  
  delay(1000);
  Ethernet.begin(mac, ip, dnsAddr, gw, subnet);
  Serial.print("My address:");
  Serial.println(Ethernet.localIP());
  wdt_enable (WDTO_4S);
}

void loop()
{
  if (client.connected()) {
    if (client.available()) {
      char inChar = client.read();
      currentLine += inChar; 

      if (inChar == '\n')
        currentLine = "";
      
      if (readingTweet) {
        if (inChar != '<') {
          tweet += inChar;
        } 
        else {
          readingTweet = false;
          Serial.println(tweet);
          pushState(tweet);  
          client.stop(); 
        }
      }
      if ( currentLine.endsWith("<text>")) {
        readingTweet = true; 
        tweet = "";
      }
    }else{
         currentLine = "";
    }   
  }else if (millis() - lastAttemptTime > requestInterval) {
    wdt_reset(); 
    connectToServer();
  }
}

void connectToServer() {
  Serial.println("connecting to server...");
  if (client.connect(server, 80)) {
    Serial.println("making HTTP request...");
    client.println("GET /light.txt HTTP/1.1");
    client.println("HOST: light");
    client.println();
  }
  lastAttemptTime = millis();
}   

void pushState(String state){
  int firstByte = 0;
  int secondByte = 0;
  for(int i = 0; i < 8; i++)
    if(state[i] == '1')
      firstByte |= 1 <<i;
  for(int i = 8; i < 16; i++)
    if(state[i] == '1')
      secondByte |= 1 <<i;
   digitalWrite(latchPin, 0);
   shiftOut(firstByte);
   shiftOut(secondByte);
   digitalWrite(latchPin, 1);
}


