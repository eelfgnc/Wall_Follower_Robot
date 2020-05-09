#include <Servo.h>
#include <Adafruit_SSD1306.h>
Adafruit_SSD1306 display(4);

const int enA = 5;
const int enB = 3;
const int in1 = 2;
const int in2 = 4;     
const int in3 = 6;
const int in4 = 7;
int trigPin = A0;
int echoPin = A1;
const int s0 = 12; 
const int s1 = 13;
const int s2 = 8;
const int s3 = 11;
const int colorsensorOut = 10;

int red = 0;
int green = 0;
int blue = 0;
String color;
int i=0;
int j=0;
Servo myservo;
struct Data{
  int dis;
  String dir;
};
long times;
long distance=0;
int distance_straight, distance_left, distance_right;
unsigned long start_time = 0;
unsigned long finish_time = 0;
int first = 0, second = 0;

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  pinMode(colorsensorOut, INPUT);
  digitalWrite(s1,LOW); 
  digitalWrite(s0,HIGH);  
  Serial.begin(9600);
  pinMode(in1, OUTPUT); 
  pinMode(in2, OUTPUT);  
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin,INPUT); 
  Serial.begin(9600);
  myservo.attach(9);
  start_time = 0;
  finish_time = 0;
}

void loop() {
  if(first == 0){
    start_time = millis();
    first++;
  }
  readColor();
  if(color.equals("RED")){
     stops();
     if(second == 0){
       finish_time = millis();
       second++;      
     }
     String runtime =  convertingTime(finish_time - start_time);
     display.clearDisplay();
     display.setTextColor(WHITE);
     display.setCursor(15,13);
     display.println(runtime);
     display.display();
  }
  else if (color.equals("GREEN") && i<2){
      turnLeft();
      stops();
      myservo.write(90);
      delay(500);
      int fordistance=measure();
      while(fordistance>=17){
        forward();
        fordistance=measure();
      }
      stops();
      i++;
  }
  else if (color.equals("BLUE") && j%2==0){
      turnRight();
      turnRight();
      stops();
      myservo.write(90);
      delay(500);
      int fordistance=measure();
      while(fordistance>=17){
        forward();
        fordistance=measure();
      }
      stops();
      j++;
  }
  else 
  {
    j=0;
    Data data = getDistancesandDirection();
    if(data.dir.equals("right")){
      turnRight();
      stops();
      myservo.write(90);
      delay(500);
      int fordistance=measure();
      while(fordistance>=17){
        forward();
        fordistance=measure();
      }
      stops();
    }
    else if (data.dir.equals("left")){
      turnLeft();
      stops();
      myservo.write(90);
      delay(500);
      int fordistance=measure();
      while(fordistance>=13){
        forward();
        fordistance=measure();
      }
      stops();
    }
    else if (data.dir.equals("straight")){
      myservo.write(90);
      delay(500);
      int fordistance=measure();
      while(fordistance>=17){
        forward();
        fordistance=measure();
      }
      stops();
    } 
    else{
      stops();
    } 
  }
}
void forward(){
  analogWrite(enA,  90); 
  digitalWrite(in2, LOW );
  digitalWrite(in1, HIGH );  
  analogWrite(enB,  60);  
  digitalWrite(in3, HIGH );
  digitalWrite(in4, LOW);
  delay(328);   
}
void turnRight(){
  analogWrite(enA,  90); 
  digitalWrite(in2, HIGH );
  digitalWrite(in1, LOW );  
  analogWrite(enB,  90);  
  digitalWrite(in3, HIGH );
  digitalWrite(in4, LOW);
  delay(1450);   
}
void turnLeft(){
  analogWrite(enA,  90); 
  digitalWrite(in2, LOW );
  digitalWrite(in1, HIGH );  
  analogWrite(enB,  90);  
  digitalWrite(in3, LOW );
  digitalWrite(in4, HIGH);
  delay(1550);   
}
void stops(){
  analogWrite(enA,  0);
  digitalWrite(in1, LOW );
  digitalWrite(in2, LOW );  
  analogWrite(enB, 0);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  delay(500);
}

Data getDistancesandDirection(){
  for(int k=0; k<=180; k=k+90){
    for(int j=0;j<3;j++){
      myservo.write(k);
      delay(500);
      digitalWrite(trigPin, LOW);
      delayMicroseconds(10);
      digitalWrite(trigPin, HIGH); 
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);  
      times = pulseIn(echoPin, HIGH); 
      int x = times / 58.2;         
      if(x > 200){
        x = 200; 
      }
      distance += x;   
      delayMicroseconds(500);            
    }
    distance = distance / 3;
    if(k == 0) distance_right = distance;
    else if(k == 90) distance_straight = distance;
    else distance_left = distance;
    distance=0;
  }
  Data data;
  if(distance_right>distance_left && distance_right>distance_straight){
    data.dir = "right"; 
    data.dis = distance_right;
  }
  else if(distance_straight>distance_right && distance_straight>distance_left){
    data.dir = "straight"; 
    data.dis = distance_straight;
  }
  else{
    data.dir = "left"; 
    data.dis = distance_left;
  }
  return data;
}

int measure(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(10);
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);  
  times = pulseIn(echoPin, HIGH); 
  int x = times / 58.2;         
  if(x > 200){
    x = 200; 
  }
  return x;
}

void readColor(){  
  for (int t = 0; t < 10; t++)
  {
    digitalWrite(s2, LOW);
    digitalWrite(s3, LOW);
    red = red + pulseIn(colorsensorOut, LOW);
  
    digitalWrite(s2, HIGH);
    digitalWrite(s3, HIGH);
    green = green + pulseIn(colorsensorOut, LOW);
    
    digitalWrite(s2, LOW);
    digitalWrite(s3, HIGH);
    blue = blue + pulseIn(colorsensorOut, LOW);
  }
  red = red/10;
  green = green/10;
  blue = blue/10;
  if (red >= 75 && red <= 100  && green >= 55 && green <= 85 && blue >= 95 && blue <= 115) color = "GREEN";
  else if (red >= 45 && red <= 60 && green >= 125 && green <= 150 && blue >= 100 && blue <= 120) color = "RED";
  else if (red >= 95 && red <= 120 && green >= 65 && green <= 85 && blue >= 35 && blue <= 55) color = "BLUE";
  else  color = "NO_COLOR";
}
String convertingTime(unsigned long times){
  unsigned long allseconds = times/1000;
  int runMinutes = allseconds/60;
  int runSeconds = allseconds%60;
  String buf = "Run Time: " +  String(runMinutes, DEC) + ":" + String(runSeconds, DEC);
  return buf;
}
