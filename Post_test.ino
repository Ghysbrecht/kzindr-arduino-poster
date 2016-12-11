#include <UIPEthernet.h>
#include <SoftwareSerial.h>

#define HS_PIN 6                            //Handshaking to the RFID unit

EthernetClient client;
SoftwareSerial mySerial(3, 2);              //Software serial to receive the UID (RX, TX)(TX not used)
char server[] = "www.kzindr.herokuapp.com";
char dataString[] = "{\"message\": \"Arduino TEST\",\"api_info\": \"Arduino TGHwebRFID\",\"add_time\": \"+30 minutes\"}";
char codeString[17];                        //Char array to store the UID (16 character + NULL)
int i;
void setup() {

  Serial.begin(9600);
  mySerial.begin(9600);
  
  uint8_t mac[6] = {0x00,0x01,0x02,0x03,0x04,0x05}; //Mac address
  Ethernet.begin(mac);                              //Init ethernet

  //Serial print some stuff to verify that we have a connection...
  Serial.print("localIP: ");
  Serial.println(Ethernet.localIP());
  Serial.print("subnetMask: ");
  Serial.println(Ethernet.subnetMask());
  Serial.print("gatewayIP: ");
  Serial.println(Ethernet.gatewayIP());
  Serial.print("dnsServerIP: ");
  Serial.println(Ethernet.dnsServerIP());
  delay(1000);
}

void loop() {
  digitalWrite(HS_PIN, HIGH);            //Tell the RFID arduino that we are waiting...
  if (mySerial.available()) {            //Data received!
    delay(500);
    digitalWrite(HS_PIN, LOW);           //Thanks for the data, now shut up for a moment...
    for(i=0;i<16;i++){              
      codeString[i] = mySerial.read();   //Read all the 16 bytes
    }
    codeString[17] = 0;                  //Add the end of string NULL
    Serial.println(codeString);          
    postData();                          //Post all data to the server
    digitalWrite(HS_PIN, HIGH);          //I'm done! A new UID can be sent
  }
}

void postData(){
if (client.connect(server, 80)) {
    Serial.println("Connected to the server!...");
    // Make a HTTP request:
    client.println("POST /api/checkin HTTP/1.1");
    client.println("Host: kzindr.herokuapp.com");
    client.println("User-Agent: Arduino/1.0");
    client.println("Connection: close");
    client.println("Content-Type: application/json");
    client.println("Accept: application/json");
    client.print("uid-key: ");
    client.println(codeString);
    client.print("Content-Length: ");
    client.println(sizeof(dataString));
    client.println();
    client.println(dataString);
    client.println();

    //For debugging purposes only...
    Serial.println("Data that has been sent:");
    Serial.println("POST /api/checkin HTTP/1.1");
    Serial.println("Host: kzindr.herokuapp.com");
    Serial.println("User-Agent: Arduino/1.0");
    Serial.println("Connection: close");
    Serial.println("Content-Type: application/json");
    Serial.println("Accept: application/json");
    Serial.print("uid-key: ");
    Serial.println(codeString);
    Serial.print("Content-Length: ");
    Serial.println(sizeof(dataString));
    Serial.println();
    Serial.println(dataString);
    Serial.println();
    
    Serial.println("All data sent!");
  } else {
    Serial.println("connection failed");
  }

  //If the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }
}
