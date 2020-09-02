#include "PPMReader.h"      // Used to receive RC signal

// PINS
const int magnet_sensor_pin = A0;
const int e_stop_pin = A1;

const int rc_receiver_pin = 2;    // Must be D2 or D3 - external interrupt pin

const int linear_direction_pin = 3;
const int linear_enable_pin = 4;

const int throttle_pin = 5;
const int reverse_gear_pin = 6;

const int steering_stepper_step_pin = 7;
const int steering_stepper_direction_pin = 8;
const int steering_stepper_enable_pin = 9; 

const int encoder_CLK_pin = 10;
const int encoder_DO_pin  = 11;
const int encoder_CS_pin = 12;

/*
const int left_blinker = ;
const int right_blinker = ;
const int full_lights = ;
const int brake_light = ;
*/
// PINS

// RC CHANNELS
const int RC_steering_channel = 0;      // Right horizontal
const int RC_throttle_channel = 1;      // Left vertical
const int RC_gear_channel = 4;          // Switch A (leftmost)
const int RC_steering_lock_channel = 5; // Switch D (rightmost)
const int RC_handbrake_channel = 6;     // Switch C (mid right)
// RC CHANNELS

PPMReader ppmReader(rc_receiver_pin, 0, false); // Setting up RC receiver PPMReader


unsigned int encoder_reading;             // Encoder_reading is a 12-bit value with range 0-4095
int current_steering_value;               // current_steering_value is encoder_reading mapped to 0-360 degrees
const int steering_left_value = 310;     // Encoder value when ATV is steering left
const int steering_right_value = 143;    // Encoder value when ATV is steering right
int target_steering_value;

bool steering_stepper_direction;
bool steering_stepper_enabled;

int magnet_value;                 // Hall sensor value for measuring brake position
const int brake_off_value = 522;  // Hall sensor value when NOT BRAKING
const int brake_on_value = 532;   // Hall sensor value when BRAKING
int target_brake_value;
int current_brake_value;

bool lock_steering = false;
bool handbrake = false;
bool reverse_gear = false;

float target_brake_ratio;  // Target brake position: 0 = braking, 1 = not braking
float current_brake_ratio;

int throttle;  // Analog signal going to Kelly controller. 40/255 = motor off, 150?/255 = max power

int RC_channel_values[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int received_steering;
int received_throttle;
int received_gear;
int received_steering_lock;
int received_handbrake;

void setup() {
  
  Serial.begin(9600);

  pinMode(encoder_CS_pin, OUTPUT);     // Chip select
  pinMode(encoder_CLK_pin, OUTPUT);     // Serial clock
  pinMode(encoder_DO_pin, INPUT);       // Serial data OUT   
  digitalWrite(encoder_CLK_pin, HIGH);  
  pinMode(e_stop_pin, INPUT);
  pinMode(magnet_sensor_pin, INPUT);
  pinMode(steering_stepper_step_pin, OUTPUT);
  pinMode(steering_stepper_direction_pin, OUTPUT);
  pinMode(steering_stepper_enable_pin, OUTPUT);
  pinMode(linear_enable_pin, OUTPUT);
  pinMode(linear_direction_pin, OUTPUT);
  pinMode(throttle_pin, OUTPUT);     // Throttle pin

  //pinMode(left_blinker, OUTPUT);
  //pinMode(right_blinker, OUTPUT);
  //pinMode(full_lights, OUTPUT);
  //pinMode(brake_light, OUTPUT);
}
int temp;

void loop() { // Main loop
  
  ReadSSI();   // Reads steering encoder value, 12-bit
  
  current_steering_value = map(encoder_reading, 0, 4095, 0, 360);  // Maps steering encoder reading from 12-bit to 0-360 range
  
  ReadRCValues();   // Reads RC channels, ranging 1000-2000

  /*if (RC_channel_values[0] < 900){  // If RC transmitter is off, skip everything
    return;
  }*/

  received_steering = RC_channel_values[RC_steering_channel];
  received_throttle = RC_channel_values[RC_throttle_channel];
  received_gear = RC_channel_values[RC_gear_channel];
  received_steering_lock = RC_channel_values[RC_steering_lock_channel];
  received_handbrake = RC_channel_values[RC_handbrake_channel];  

  lock_steering = (received_steering_lock > 1500) ? true : false; 
  reverse_gear = (received_gear > 1500) ? true: false;
  handbrake = (received_handbrake > 1100) ? true: false;

  temp = map(received_steering, 1000, 2000, 0, 100);


  
  // Start BRAKING and THROTTLE
  
  magnet_value = analogRead(magnet_sensor_pin); 

  // VVV 0: braking,   1: not braking,   <0: braking too much,   >1: piston contracted too much
  current_brake_ratio = (brake_on_value - magnet_value) / float(brake_on_value - brake_off_value);

  
  if (received_throttle < 1490 || digitalRead(e_stop_pin) == LOW || handbrake){  // BRAKING
    analogWrite(throttle_pin, 40);  // Stopping ATV motor
    target_brake_ratio = 0;
    
    if (target_brake_ratio < current_brake_ratio){  // Extends until brake is engaged
      digitalWrite(linear_enable_pin, HIGH);
      digitalWrite(linear_direction_pin, LOW);
    }
    else{
        digitalWrite(linear_enable_pin, LOW); // Brake is in desired position, disables linear actuator
    }
  }
  else{ // NOT BRAKING
    target_brake_ratio = 1;
    
    if (target_brake_ratio > current_brake_ratio ){  // Contracts until brake is disengaged before allowing ATV to power motor
      digitalWrite(linear_enable_pin, HIGH);
      digitalWrite(linear_direction_pin, HIGH);
    }
    else{
      digitalWrite(linear_enable_pin, LOW); // Brake is in desired position, disables linear actuator
  
      digitalWrite(reverse_gear_pin, reverse_gear ? HIGH : LOW);  // Changes gear, if requested

      if (received_throttle > 1550){
        throttle = map(received_throttle, 1550, 2000, 40, 150);
        analogWrite(throttle_pin, throttle);  // Powering ATV motor
      }
      else{
        analogWrite(throttle_pin, 40);  // Stopping ATV motor
      }        
    }
  }
  

  // START - STEERING
  
  target_steering_value = map(received_steering, 1000, 2000, steering_left_value, steering_right_value);
  //Serial.println((String) encoder_reading + "  " + current_steering_value + " " + target_steering_value);  // PRINT PRINT PRINT PRINT PRINT PRINT PRINT PRINT PRINT PRINT PRINT PRINT PRINT
  if (not lock_steering){
    if (current_steering_value > target_steering_value){
      steering_stepper_direction = false;
      steering_stepper_enabled = true;
    }
    else if (current_steering_value < target_steering_value){
      steering_stepper_direction = true;
      steering_stepper_enabled = true;
    }
    else{
      steering_stepper_enabled = false;
    }    
    OneStep(steering_stepper_enabled, steering_stepper_direction);
  }

  // END - STEERING
}
