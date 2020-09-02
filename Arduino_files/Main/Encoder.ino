void ReadSSI(void)  // Reads steering encoder value, 12-bit
{
  int i;
  char Resolution = 12;
  unsigned int bitStart = 0x0800;

  encoder_reading = 0;
  digitalWrite(encoder_CS_pin, LOW);
  delayMicroseconds(1);
  digitalWrite(encoder_CLK_pin, LOW);

  for (i = (Resolution - 1); i >= 0; i--){
    digitalWrite(encoder_CLK_pin, HIGH);
    delayMicroseconds(1);

    if (digitalRead(encoder_DO_pin)) encoder_reading |= bitStart;
    digitalWrite(encoder_CLK_pin, LOW);
    bitStart = bitStart >> 1;

    if (i == 0){
      digitalWrite(encoder_CLK_pin, HIGH);
      if (digitalRead(encoder_DO_pin)) encoder_reading |= bitStart;
    }
  }
  digitalWrite(encoder_CS_pin, HIGH);
}
