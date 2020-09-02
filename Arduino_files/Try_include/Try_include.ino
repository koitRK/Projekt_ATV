// Using Multichannel_read.ino functions: start_receiver(), read_rc().
// Channel values are stored in ch[i], ch[1] == channel 1

// Channels: 1: R-hor,  2: R-ver,  3: L-ver,  4: L-hor,  5: custom, 6: custom.
// All channel values range from 0-1000

// PINS
int magnet_sensor = A0;
int motor_step = 6;
int motor_direction = 7;
int motor_enable = 8;
int linear_enable = 4;
// pin 9 on vist atv mootori suund, ehk tagukäik
int linear_direction = 13;
int throttle_pin = 5;
int encoder_CLK_pin = 10; //Digital Pin 10
int encoder_DO_pin  = 11; //Digital Pin 11
int encoder_CSn_pin = 12; //Digital Pin 12
// PINS


unsigned int encoder_reading;
int encoder_value;
int encoder_middle_value = 221;  //100 - 250 seda ei kasuta nikuinii
int encoder_left_value = 302;
int encoder_right_value = 130;
int target_pos = 2500;
int current_pos = 2500;
unsigned long int a,b,c;
int x[15],ch1[15],ch[7],i;
bool dir = false;
bool enabled = true;
int counter = 0;
int magnet_value; // analog readings
int brake_off_value = 440; //440 original
int brake_on_value = 520;  //520 original
int target_brake_value;
int current_brake_value;
bool pause_steering = false;

float input_brake_percent;
float output_brake_percent;
int throttle;

void setup() {
  start_receiver();      // Start receiving channels' values

  pinMode(encoder_CSn_pin, OUTPUT);   // Chip select
  pinMode(encoder_CLK_pin, OUTPUT);   // Serial clock
  pinMode(encoder_DO_pin, INPUT);   // Serial data IN/OUT   
  digitalWrite(encoder_CLK_pin, HIGH);  

  pinMode(magnet_sensor, INPUT);    // Magnet sensor pin
  pinMode(motor_step, OUTPUT);    // Motor step pin
  pinMode(motor_direction, OUTPUT);     // Motor direction pin
  pinMode(motor_enable, OUTPUT);    // Motor enabled pin
  pinMode(linear_enable, OUTPUT);     // Linear enabled pin
  pinMode(linear_direction, OUTPUT);    // Linear direction pin

  pinMode(throttle_pin, OUTPUT);     // Throttle pin

  Serial.begin(9600);
}

void loop() {

  ReadSSI();
  encoder_value = map(encoder_reading, 0, 4095, 0, 360);

  read_rc();    //  Write channel' values to list ch[i]

  if (ch[5] > 500){
    pause_steering = true;
  }
  else{
    pause_steering = false;
  }
  
  magnet_value = analogRead(magnet_sensor);


 
  // START - BRAKE(and throttle)
  //Serial.println(magnet_value);
  
  //new
  if (ch[3] < 490){
    analogWrite(throttle_pin, 40);  // To stop motor from running
    input_brake_percent = ch[3] / 490.0; // 1 -> 0, 1 is off, 0 is fully on
    output_brake_percent = (brake_on_value - magnet_value) / float(brake_on_value - brake_off_value); // over 1 is over off, 1 is off, 0 is full on,
    
    if (input_brake_percent < output_brake_percent){ // Should apply brake
      digitalWrite(linear_enable, HIGH);
      digitalWrite(linear_direction, LOW);
      Serial.println(777);
      }
    else if (input_brake_percent > output_brake_percent){  // Should disengage brake
      digitalWrite(linear_enable, HIGH);
      digitalWrite(linear_direction, HIGH);
      Serial.println("££###££");
      }
    }
  else if (magnet_value > brake_off_value){  // Takes off brake and stops motor
    analogWrite(throttle_pin, 40);
    digitalWrite(linear_enable, HIGH);
    digitalWrite(linear_direction, HIGH);
    }
  else{
    digitalWrite(linear_enable, LOW); // To stop brake from going over off position
    if (ch[3] > 550){  // Starts throttle
      throttle = map(ch[3], 520, 980, 40, 240);
      analogWrite(throttle_pin, throttle);  // Throttles motor
      }
    else{
      //IDLE - Handbrake maybe
      }
    }

  // END - BRAKE

  
  // START - STEERING
  
  target_pos = map(ch[1], 0, 1000, encoder_left_value, encoder_right_value);

  if (not pause_steering){
    if (encoder_value > target_pos){
      dir = false;
      enabled = true;
    }
    else if (encoder_value < target_pos){
      dir = true;
      enabled = true;
    }
    else{
      enabled = false;
    }    
    one_step(enabled, dir, 100);
  }

  
  counter += 1;
  
  if (counter >= 50){
    Serial.println(encoder_value);
    Serial.print("------");
    Serial.print(ch[3]);
    Serial.print("    ");
    Serial.print(target_pos);
    Serial.print("   ");
    Serial.print(encoder_value);
    Serial.print("\n");
    counter = 0;
  }

  // END - STEERING

}
