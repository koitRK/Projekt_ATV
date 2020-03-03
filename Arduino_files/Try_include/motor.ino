

void one_step(bool enabled, bool dir, float step_delay){
  
  if (enabled){
    digitalWrite(8, LOW);
  }
  else{
    digitalWrite(8, HIGH);
  }  

  if (dir){
    digitalWrite(7, HIGH);
  }
  else{
    digitalWrite(7, LOW);
  }

  if (enabled){
    delayMicroseconds(step_delay); 
    digitalWrite(6, HIGH);
    delayMicroseconds(step_delay); 
    digitalWrite(6, LOW);
  }


  if (enabled){
    if (dir){
      current_pos -= 1;
    }
    else{
      current_pos += 1;
    }
  }
}
