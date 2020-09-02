void OneStep(bool enabled, bool dir){  // Stepper motor takes one step in specified direction, if enabled
  
  if (enabled){
    digitalWrite(steering_stepper_enable_pin, LOW);
  }
  else{
    digitalWrite(steering_stepper_enable_pin, HIGH);
  }  

  if (dir){
    digitalWrite(steering_stepper_direction_pin, HIGH);
  }
  else{
    digitalWrite(steering_stepper_direction_pin, LOW);
  }

  if (enabled){
    digitalWrite(steering_stepper_step_pin, HIGH);
    digitalWrite(steering_stepper_step_pin, LOW);
  }
}
