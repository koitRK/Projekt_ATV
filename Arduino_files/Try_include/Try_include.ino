#include "PPMReader.h"


// PPMReader(pin, interrupt)
PPMReader ppmReader(2, 0, false);

// PINS
int magnet_sensor = A0;
int steering_stepper_step = 6;
int steering_stepper_direction = 7;
int steering_stepper_enable = 8;

int linear_enable = 4;
// pin 9 on vist atv mootori suund, ehk tagukäik
int reverse_gear_pin = 9;
int linear_direction = 13;
int throttle_pin = 5;
int encoder_CLK_pin = 10; //Digital Pin 10
int encoder_DO_pin  = 11; //Digital Pin 11
int encoder_CSn_pin = 12; //Digital Pin 12
int e_stop = 15;  //Digital Pin 15
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
int brake_off_value = 450; //440 original
int brake_on_value = 527;  //520 original
int target_brake_value;
int current_brake_value;
bool pause_steering = false;
bool handbrake_on = false;
bool reverse_gear = false;

float target_brake_percent;
float current_brake_percent;
int throttle;
int faultcount = 0;


int received_steering;
int received_throttle;
int received_gear;
int received_steering_lock;
int received_handbrake;

void setup() {
  
  //Serial.begin(9600);

  pinMode(encoder_CSn_pin, OUTPUT);   // Chip select
  pinMode(encoder_CLK_pin, OUTPUT);   // Serial clock
  pinMode(encoder_DO_pin, INPUT);   // Serial data IN/OUT   
  digitalWrite(encoder_CLK_pin, HIGH);  

  pinMode(e_stop, INPUT);  // E_Stop relay
  pinMode(magnet_sensor, INPUT);    // Magnet sensor pin
  pinMode(steering_stepper_step, OUTPUT);    // steering_stepper step pin
  pinMode(steering_stepper_direction, OUTPUT);     // steering_stepper direction pin
  pinMode(steering_stepper_enable, OUTPUT);    // steering_stepper enabled pin
  pinMode(linear_enable, OUTPUT);     // Linear enabled pin
  pinMode(linear_direction, OUTPUT);    // Linear direction pin

  pinMode(throttle_pin, OUTPUT);     // Throttle pin

}

void loop() {
  
  ReadSSI();
  encoder_value = map(encoder_reading, 0, 4095, 0, 360);

  int mySensVals[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  static int count;
  while (ppmReader.get(count) != 0) {
    mySensVals[count] = ppmReader.get(count);
    count++;
  }
  count = 0;

  received_throttle = mySensVals[1]; // values 1000 to 2000
  received_steering = mySensVals[0];
  received_steering_lock = mySensVals[5];
  received_gear = mySensVals[4];
  received_handbrake = mySensVals[6];
  //Serial.println(received_handbrake);

  
  if (received_steering_lock > 1500){
    pause_steering = true;
  }
  else{
    pause_steering = false;
  }

  if (received_gear > 1500){
    reverse_gear = true;
    faultcount += 1;
    //Serial.println((String) faultcount + "  ch5: " + ch[5] + "  ch6: " + ch[6]);

  }
  else{
    reverse_gear = false;
  }

  if (received_handbrake > 1100){
    handbrake_on = true;
  }
  else{
    handbrake_on = false;
  }




  // START - BRAKE(and throttle)  
  
  magnet_value = analogRead(magnet_sensor); 
  if (magnet_value < 527){
    //Serial.println(magnet_value);

  }
  

  current_brake_percent = (brake_on_value - magnet_value) / float(brake_on_value - brake_off_value); // over 1 is over off, 1 is off, 0 is full on,

  
  if (received_throttle < 1490 || digitalRead(e_stop) == LOW || handbrake_on){ //BRAKING

    analogWrite(throttle_pin, 40);  // To stop motor from running
    target_brake_percent = 0;
    if (target_brake_percent < current_brake_percent){ // Extends until brake is engaged
      digitalWrite(linear_enable, HIGH);
      digitalWrite(linear_direction, LOW);
      //Serial.println("EXTENDING");
    }
    else{
        digitalWrite(linear_enable, LOW); //Brake is in desired position, disables linear actuator
    }
  }
  else{ //NOT BRAKING
    target_brake_percent = 1;
    if (target_brake_percent > current_brake_percent ){  // Contracts until brake is disengaged
      //analogWrite(throttle_pin, 40);  // To stop motor from running
      digitalWrite(linear_enable, HIGH);
      digitalWrite(linear_direction, HIGH);
      //Serial.println("CONTRACTING");
    }
    else{
      digitalWrite(linear_enable, LOW); //Brake is in desired position, disables linear actuator
  
      // !!!%%%!!!   start MOTOR !!!%%%!!!

      if (reverse_gear){
        digitalWrite(reverse_gear_pin, HIGH);
      }
      else{
        digitalWrite(reverse_gear_pin, LOW);  
      }
      
      if (received_throttle > 1550){  // Starts throttle
        throttle = map(received_throttle, 1550, 2000, 40, 150);
        analogWrite(throttle_pin, throttle);  // Throttles motor
      }
      else{
        analogWrite(throttle_pin, 40);  // To stop motor from running
      }
        
      // !!!%%%!!!  end MOTOR !!!%%%!!!
    }
  }




  
  /*
  else if (magnet_value > brake_off_value){  // Takes off brake and stops motor
    analogWrite(throttle_pin, 40);
    digitalWrite(linear_enable, HIGH);
    digitalWrite(linear_direction, HIGH);
    }
  else{
    digitalWrite(linear_enable, LOW); // To stop brake from going over off position
    if (990 > ch[3] && ch[3] > 550){  // Starts throttle
      throttle = map(ch[3], 520, 980, 40, 240);
      analogWrite(throttle_pin, throttle);  // Throttles motor
      }
    else{
      //IDLE - Handbrake maybe
      }
    }
    */

  // END - BRAKE
  

  
  // START - STEERING
  
  target_pos = map(received_steering, 1000, 2000, encoder_left_value, encoder_right_value);

  if (not pause_steering){
    if (encoder_value > target_pos){ // >
      dir = false;
      enabled = true;
    }
    else if (encoder_value < target_pos){  // <
      dir = true;
      enabled = true;
    }
    else{
      enabled = false;
    }    
    one_step(enabled, dir, 6);
  }

  
  counter += 1;
  /*
  if (counter >= 50){
    Serial.println(encoder_value);
    Serial.print("------");
    Serial.print((String)ch[0] + "  " +  ch[1] +  "  " + ch[2] +  "  " + ch[3] +  "  " + ch[4] +  "  " + ch[5] +  "  " + ch[6] +  "  " + ch[7]);
    Serial.print("    ");
    Serial.print(target_pos);
    Serial.print("   ");
    Serial.print(encoder_value);
    Serial.print("\n");
    counter = 0;
  }*/
  
  
  // END - STEERING

}
