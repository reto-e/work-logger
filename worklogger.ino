#include <SPI.h>
#include<WiFiNINA.h>

// display includes
#include <fonts.h>
#include <epdpaint.h>
#include <epd2in13b.h>
#include <epdif.h>


// ----------------------------------------------------------------------------
// Variables
// ----------------------------------------------------------------------------

const int potPin = A1;          // eingang Poti
int pot_val = 0;                // Value des Poti, wird in Position umgerechnet
int pot_val_sum = 0;            // sum of 10 measures of pot_val
int current_position;           // current position
int new_position;               // position read from poti
int stable_count = 0;           // counts how many times current and new position are the same;
char ssid[] = "AndroidAP4750";    // your network SSID (name)
char pass[] = "aaf06d16f711";  // your network password (use for WPA, or use as key for WEP)
char server[] = "docs.google.com"; // google forms server
int status = WL_IDLE_STATUS;    // 
WiFiSSLClient client;           // client object which does the calls

//
unsigned long start_time;       // stores millis, used to update progress bar
char progress_bar[9] = "[......]"; // progress bar
int progress_bar_pos = 1;

#define COLORED   0
#define UNCOLORED 1

unsigned char image[1024];
Paint paint(image, 28,192);    //width should be the multiple of 8 
Epd epd;

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
int calculate_position(float val) {
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
    // set start time for progress_bar timer
    start_time = millis();

    // reset progress Bar
    for (int i = 1; i < 7; i++) {
      progress_bar[i] = '.';
      }
    progress_bar_pos = 1;

      
     
    drawString(jobdata);

    // drawProgressBar(); //TODO: uncomment if new display is super fast updating
    return "ok";
}

/*
 * Draw String on EPD
 */
void drawString(String jobdata) {
  // replace %20
  jobdata.replace("%20", " ");
  
  // convert String to char Array
  char jobchars[30];
  jobdata.toCharArray(jobchars,30);
  
  epd.ClearFrame();
  paint.SetRotate(ROTATE_90);

  paint.Clear(UNCOLORED);
  paint.DrawStringAt(8, 2, jobchars, &Font16, COLORED);
  epd.SetPartialWindowBlack(paint.GetImage(), 50, 16, paint.GetWidth(), paint.GetHeight());

  epd.DisplayFrame();
};

/*
 * draw progress bar on EPD
 */
void drawProgressBar() {
  paint.Clear(UNCOLORED);
  paint.DrawStringAt(8,2, progress_bar, &Font16, COLORED);
  epd.SetPartialWindowBlack(paint.GetImage(), 20, 16, paint.GetWidth(), paint.GetHeight());
  epd.DisplayFrame();
}

/*
 * update progress_bar string
 */
 void updateProgressBar() {
  // calculate time_past
  if (millis() - start_time > 300000) { //do stuff every 5 minutes
    if (progress_bar_pos < 7) {
      progress_bar[progress_bar_pos] = '=';
      progress_bar_pos++;
      drawProgressBar();
      }
    start_time = millis();  // reset time to current
    
    }
  
 }

// ----------------------------------------------------------------------------
// Setup
// ----------------------------------------------------------------------------
void setup() {

 
  pinMode(potPin, INPUT);
  Serial.begin(9600);
  delay(1000);

Serial.println("init Display");
  // initilize Display
 
 if (epd.Init() != 0) {
    Serial.print("e-Paper init failed");
    return;
  }
    
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

  // init display
  // initDisplay();
    
}

// ----------------------------------------------------------------------------
// Loop
// ----------------------------------------------------------------------------

void loop() {
  //val = analogRead(potPin); 
  for (int i = 0; i < 10; i++) {
    pot_val = analogRead(potPin);
    pot_val_sum = pot_val_sum + pot_val;
  }
  float flat_pot_val = pot_val_sum/10;

  pot_val_sum = 0; // reset to 0 for next loop.

  
  Serial.println(flat_pot_val);
  new_position = calculate_position(flat_pot_val);

  if (new_position != current_position) {
    stable_count = 0;
    
  }
  if (stable_count == 1) {
    storeWork(new_position);
  }
  current_position = new_position; 
  stable_count++;

  // Debug block
//  Serial.print(new_position);
//  Serial.print("\t");
//  Serial.print(current_position);
//  Serial.print("\t");
//  Serial.println(stable_count);

  // update progress bar
  updateProgressBar();

  delay(1000);
}
