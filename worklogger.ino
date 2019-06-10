#include <SPI.h>
#include<WiFiNINA.h>

// ----------------------------------------------------------------------------
// Variables
// ----------------------------------------------------------------------------

const int potPin = A1;          // eingang Poti
int val = 0;                    // Value des Poti, wird in Position umgerechnet
int current_position;           // current position
int new_position;               // position read from poti
int stable_count = 0;           // counts how many times current and new position are the same;
char ssid[] = "AndroidAP4750";    // your network SSID (name)
char pass[] = "aaf06d16f711";  // your network password (use for WPA, or use as key for WEP)
char server[] = "docs.google.com"; // google forms server
int status = WL_IDLE_STATUS;    // 
WiFiSSLClient client;           // client object which does the calls

String jobs[11] = { 
  "Con Admin", 
  "Con Testing", 
  "Con Analysis", 
  "Con Coord",
  "Z Admin",
  "Z Coord",
  "Z Analysis",
  "Z Testing",
  "I10000",
  "I15000",
  "Pause"
};

// ----------------------------------------------------------------------------
// Functions
// ----------------------------------------------------------------------------
/*
reads voltage from the poti and returns the position of the pointer
*/
int calculate_position(int val) {
  int position = 10;
  if(val < 20) position = 0;
  else if(val < 96) position = 1;
  else if(val < 213) position = 2;
  else if(val < 316) position = 3;
  else if(val < 458) position = 4;
  else if(val < 598) position = 5;
  else if(val < 721) position = 6;
  else if(val < 838) position = 7;
  else if(val < 936) position = 8;
  else if(val < 1010) position = 9;
  else position = 10;
  
  return position;
}


/*
Defines job strings for sending to backend
*/
String storeWork(int pos) {
  Serial.print("storing job ");
  Serial.println(jobs[pos]);
  String jobdata = jobs[pos];
  jobdata.replace(" ", "%20");
  if (client.connectSSL(server, 443)) {
    client.print("GET http://docs.google.com/forms/d/e/1FAIpQLSdo4mZ-P4LfauRNjesSDceLhdNDUcVl5QPq9StZG6vIGjwQng/formResponse?entry.1030855005=");
    client.print(jobdata);
    client.println(" HTTP/1.1");
    client.println("Host: docs.google.com");
    client.println("Cache-Control: no-cache");
    client.println("Connection: close");
    client.println();
    }

    return "ok";
}

// ----------------------------------------------------------------------------
// Setup
// ----------------------------------------------------------------------------
void setup() {
  pinMode(potPin, INPUT);
  Serial.begin(9600);
    
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(7000);
  }
  Serial.println("Connected to wifi");
  
  current_position = -1;
    
}

// ----------------------------------------------------------------------------
// Loop
// ----------------------------------------------------------------------------

void loop() {
  val = analogRead(potPin); 
  Serial.println(val);
  new_position = calculate_position(val);

  if (new_position != current_position) {
    stable_count = 0;
    
  }
  if (stable_count == 1) {
    storeWork(new_position);
  }
  current_position = new_position; 
  stable_count++;

  // Debug block
  Serial.print(new_position);
  Serial.print("\t");
  Serial.print(current_position);
  Serial.print("\t");
  Serial.println(stable_count);

  delay(1000);
}
