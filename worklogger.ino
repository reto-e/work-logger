
/*
Reads values from a potentiometer and displays them in serial monitor

*/

const int potPin = A1; // eingang Poti
int val = 0;
int current_position = 20; // current position
int new_position;         // position read from poti
int stable_count = 0;         // counts how many times current and new position are the same;

String jobs[10] = { 
  "Admin", 
  "Coordination", 
  "Backlog Grooming", 
  "Analysis",
  "Testing",
  "Documentation",
  "I10000",
  "I15000",
  "Pause"
};


/*
reads voltage from the poti and returns the position of the pointer
*/
int calculate_position(int val) {
  int position = 0;
  if(val > 45 && val <= 166) position = 1;
  if(val > 166 && val <= 278) position = 2;
  if(val > 278 && val <= 383) position = 3;
  if(val > 383 && val <= 524) position = 4;
  if(val > 524 && val <= 647) position = 5;
  if(val > 647 && val <= 769) position = 6;
  if(val > 769 && val <= 885) position = 7;
  if(val > 885 && val <= 981) position = 8;
  if(val > 981) position = 9;
  return position;
}

/*
Defines job strings for sending to backend
*/
String storeWork(int pos) {
  Serial.print("storing job ");
  Serial.println(jobs[pos]);
  return "ok";
  
}

void setup() {
  pinMode(potPin, INPUT);
  Serial.begin(9600);
  current_position = -1;
    
}

void loop() {
  int val = analogRead(potPin); 
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
