int CLK = 10; //Digital Pin 10
int DO  = 11; //Digital Pin 11
int CSn = 12; //Digital Pin 12

unsigned int reading;

void setup() {                
  Serial.begin(115200);

  pinMode(CSn, OUTPUT);   // Chip select
  pinMode(CLK, OUTPUT);   // Serial clock
  pinMode(DO, INPUT);   // Serial data IN/OUT   
  digitalWrite(CLK, HIGH);    
}

void loop() {
  int degree;

  ReadSSI();
  degree = map(reading, 0, 4095, 0, 359);
  Serial.print(reading,DEC); Serial.print("\t"); Serial.println(degree);

  analogWrite(5, map(reading, 0, 4095, 0, 255));
  
  delay(10);
}

void ReadSSI(void)
{
  int i;
  char Resolution = 12;
  unsigned int bitStart = 0x0800;

   reading = 0;
   digitalWrite(CSn, LOW);
   delayMicroseconds(1);
   digitalWrite(CLK, LOW);

   for(i=(Resolution-1);i>=0;i--)
   {
     digitalWrite(CLK, HIGH);
     delayMicroseconds(1);
     //Serial.println(digitalRead(DO));
     //Serial.println();
     if (digitalRead(DO)) reading |= bitStart;   
     digitalWrite(CLK, LOW);
     bitStart = bitStart >> 1;

   if (i == 0) 
   {
     digitalWrite(CLK, HIGH);
     if (digitalRead(DO)) reading |= bitStart;   
     }
   }
   digitalWrite(CSn, HIGH);
   //Serial.println(reading);
}
