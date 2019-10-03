/* 
Wemos D1 mini

L298N Motor Controller

 */

int ENA = 4; //D2
int IN1 = 0; //D3
int IN2 = 2; //D4

void setup() {
// set all the motor control pins to outputs
pinMode(ENA, OUTPUT);
pinMode(IN1, OUTPUT);
pinMode(IN2, OUTPUT);

Serial.begin(115200);
}


void testTwo() {
// turn on motor
digitalWrite(ENA, HIGH);
digitalWrite(IN1, HIGH);
digitalWrite(IN2, LOW);

delay(5000); // now change motor directions

digitalWrite(IN1, LOW);
digitalWrite(IN2, HIGH);

delay(5000); // now turn off motors

digitalWrite(IN1, LOW);
digitalWrite(IN2, LOW);
}

void testOne() {

// turn on motors
digitalWrite(IN1, HIGH);
digitalWrite(IN2, LOW);

// accelerate from zero to maximum speed
for (int i = 0; i < 1000; i+=50)
  {
    analogWrite(ENA, i);
    Serial.println(i);
    delay(1000);
   }

analogWrite(ENA, 1000);
Serial.println("Max Power");
delay(3000);

// decelerate from maximum speed to zero
for (int i = 1000; i >= 0; i-=10)
    {
      analogWrite(ENA, i);
      Serial.println(i);
      delay(20);
     }

// now turn off motors
digitalWrite(IN1, LOW);
digitalWrite(IN2, LOW);
}


void loop() {
  Serial.println("Test One");
  testOne();   
  delay(1000);   
  Serial.println("Test Two");
  testTwo();   
  delay(5000);
}
