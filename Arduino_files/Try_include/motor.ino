

void one_step(bool enabled, bool dir, float step_delay){
  
  if (enabled){
    digitalWrite(steering_stepper_enable, LOW);
  }
  else{
    digitalWrite(steering_stepper_enable, HIGH);
  }  

  if (dir){
    digitalWrite(steering_stepper_direction, HIGH);
  }
  else{
    digitalWrite(steering_stepper_direction, LOW);
  }

  if (enabled){
    //delayMicroseconds(step_delay); 
    digitalWrite(steering_stepper_step, HIGH);
    //delayMicroseconds(step_delay); 
    digitalWrite(steering_stepper_step, LOW);
  }

  /*
  if (enabled){
    if (dir){
      current_pos -= 1;
    }
    else{
      current_pos += 1;
    }
  }*/
}
