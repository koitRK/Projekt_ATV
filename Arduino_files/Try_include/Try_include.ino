// Using Multichannel_read.ino functions: start_receiver(), read_rc().
// Channel values are stored in ch[i], ch[1] == channel 1

// Channels: 1: R-hor,  2: R-ver,  3: L-ver,  4: L-hor,  5: custom, 6: custom.
// All channel values range from 0-1000

int target_pos = 500;
int current_pos = 500;
unsigned long int a,b,c;
int x[15],ch1[15],ch[7],i;
bool dir = false;
bool enabled = true;
int counter = 0;
int magnet_value; // analog readings
int brake_off_value = 440;
int brake_on_value = 520;
int target_brake_value;
int current_brake_value;

float input_brake_percent;
float output_brake_percent;
int throttle;

void setup() {
  start_receiver();      // Start receiving channels' values

  pinMode(A0, INPUT);    // Magnet sensor pin
  pinMode(6, OUTPUT);    // Motor step pin
  pinMode(7, OUTPUT);     // Motor direction pin
  pinMode(8, OUTPUT);    // Motor enabled pin
  pinMode(9, OUTPUT);     // Linear enabled pin
  pinMode(13, OUTPUT);    // Linear direction pin

  pinMode(5, OUTPUT);     // Throttle pin

  Serial.begin(9600);
}

void loop() {



  read_rc();    //  Write channel' values to list ch[i]

  magnet_value = analogRead(A0);

  // START - BRAKE
  //Serial.println(magnet_value);
  
  //new
  if (ch[3] < 490){
    analogWrite(5, 40);  // To stop motor from running
    input_brake_percent = ch[3] / 490.0; // 1 -> 0, 1 is off, 0 is fully on
    output_brake_percent = (brake_on_value - magnet_value) / float(brake_on_value - brake_off_value); // over 1 is over off, 1 is off, 0 is full on,
    //Serial.println(magnet_value);
    if (input_brake_percent < output_brake_percent){
      digitalWrite(9, HIGH);
      digitalWrite(13, LOW);
      }
    else if (input_brake_percent > output_brake_percent){
      digitalWrite(9, HIGH);
      digitalWrite(13, HIGH);
      }
    }
  else if (magnet_value > brake_off_value){
    analogWrite(5, 40);  // To stop motor from running
    digitalWrite(9, HIGH);
    digitalWrite(13, HIGH);
    }
  else{
    digitalWrite(9, LOW); // To stop brake from going over off position
    if (990 > ch[3] && ch[3] > 550){
      throttle = map(ch[3], 520, 980, 40, 240);
      analogWrite(5, throttle);
      }
    else{
      //IDLE - Handbrake maybe
      }
    }
      
    
  //new
  /* ORG
  if (ch[3] < 490){
    target_brake_value = map(ch[3], 0, 490, 100, 0);
    current_brake_value = map(magnet_value, brake_off_value, brake_on_value, 0, 100);
    
    if (current_brake_value < target_brake_value){
      digitalWrite(9, HIGH);
      digitalWrite(13, LOW);
    }
    else if (current_brake_value > target_brake_value){
      digitalWrite(9, HIGH);
      digitalWrite(13, HIGH);
    }
    else{
      digitalWrite(9, LOW);
    }
  }
  else if (current_brake_value < 0){
  digitalWrite(9, HIGH);
  digitalWrite(13, LOW);
  }
  else if (current_brake_value > 0){
  digitalWrite(9, HIGH);
  digitalWrite(13, HIGH);
  }
  ORG */
  // END - BRAKE
  // START - MOTOR

  counter += 1;
  /*
  if (counter >= 50){
    Serial.print(ch[1]);
    Serial.print("    ");
    Serial.print(current_pos);
    Serial.print("\n");
    counter = 0;
  }
  */

  /*
  target_pos = ch[1] * 10;
  if (abs(current_pos - target_pos) > 80){
    if (current_pos > target_pos){
      dir = true;
      enabled = true;
    }
    else if (current_pos < target_pos){
      dir = false;
      enabled = true;
    }
    else{
      enabled = false;
    }
  
    one_step(enabled, dir, 70);
  }
  */

  // END - MOTOR

}