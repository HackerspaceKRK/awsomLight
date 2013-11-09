//based on IRremote receive example from Arduino IDE
#include <IRremote.h>
#include <SPI.h>
#include <Ethernet.h>

int RECV_PIN = 6;
IRrecv irrecv(RECV_PIN);

byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x0D, 0x5D, 0xDD };
IPAddress ip(192,168,1,21);
EthernetClient client;
byte server[] = { 192, 168, 1, 5 };

decode_results results;

void toggleChannel(int chan){
  if (client.connect(server, 80)) {
    Serial.println("making HTTP request...");
    char buf[50];
    sprintf(buf, "GET /api/v1/toggleByChannel/%d HTTP/1.1", chan);
    client.println(buf);
    client.println("HOST: light");
    client.println();
  }
}
void setup()
{
  irrecv.enableIRIn();
}

void loop() {
  if (irrecv.decode(&results)) {
    irrecv.resume(); 
    switch (results.value) {
      case 0xBCA:
       toggleChannel(1);
       break; 
      case 0x80BCA:
       toggleChannel(2);
       break;
      case 0x40BCA:
       toggleChannel(3);
       break; 
      case 0xC0BCA:
       toggleChannel(4);
       break;
      case 0x20BCA:
       toggleChannel(5);
       break;
      case 0xA0BCA:
       toggleChannel(6);
       break;
    }
  }
}
