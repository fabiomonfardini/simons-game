#include <LiquidCrystal.h>

const int PINS = 4; //numeber of LEDs and Sensors
const int MAX_VALUE = 100;

 #define NOTE_D4  294 
 #define NOTE_G4  392 
 #define NOTE_A4  440 
 #define NOTE_A5  880   

 //array tones
int tons[4] = { NOTE_A5, NOTE_A4, NOTE_G4, NOTE_D4 }; 

int buzzer = 11;
int startButton = 6;

int sensorRed    = 0;
int sensorGreen  = 1;
int sensorYellow = 2;
int sensorBlue   = 3;
int sensors[PINS] = {sensorRed, sensorGreen, sensorYellow, sensorBlue};

int ledRed    = 5;
int ledGreen  = 4;
int ledYellow = 3;
int ledBlue   = 2;
int leds[PINS] = {ledRed, ledGreen, ledYellow, ledBlue};

int sensorMinValues[PINS];
int sequence[MAX_VALUE];

int touchedSensor = 0; 
int currentTurn = 0; 
int currentRecord = 0;
int stepSequence = 0; //current step in the sequence
int velocity;

boolean gameStarted = false;
boolean youLose = false;
boolean newRecord = false;

//                BS  E  D4 D5  D6 D7
LiquidCrystal lcd(7, 8, 9, 10, 12, 13);

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  
  //initialize all LEDs OUTPUTS
  for(int i=0; i < sizeof(leds); i++){
    pinMode(i, OUTPUT);
  }

  pinMode(startButton, INPUT);
  pinMode(buzzer, OUTPUT);
  
  randomSeed(analogRead(0));
}
//----------------------------------------------------------------LOOP---------------------------------------------------------------
void loop() {
    if (start()){
      
      setRecord();
      nextTurn();
      displayStatus(); 
      showSequence();
      waitingPlayer();
      displayVictory();
     
      
      //one second between each turn
      delay(1000);

      //decrease velocity after each turn
      if (velocity > 100){
        velocity -= 50;
      }
    }  
}
//----------------------------------------------------------------LOOP---------------------------------------------------------------

void setRecord(){
  if (currentRecord <= currentTurn){
     currentRecord = currentTurn;
     newRecord = true;
  }
  else{
     newRecord = false;
  }
}
void showSequence() {   
  for (int i = 0; i < currentTurn; i++) { 
    tone(buzzer, tons[sequence[i]]);    
    digitalWrite(leds[sequence[i]], HIGH);     
    delay(velocity); 
    noTone(buzzer);         
    digitalWrite(leds[sequence[i]], LOW);     
    delay(velocity);   
  }   
}

void waitingPlayer() {   
  for (int i = 0; i < currentTurn; i++) {     
    getSequence();     
    checkSequence();       

    if (youLose) {     
      break;     
    }       
    stepSequence++;   
  }     

  // set to 0 after loop  
  stepSequence = 0;
} 

void checkSequence() {   
  if (sequence[stepSequence] != touchedSensor) {     
    // GAME OVER.                 
     failure();
  } 
}

void failure(){     
    for (int i = 0; i < PINS; i++) {
      tone(buzzer, tons[i]);       
      digitalWrite(leds[i], HIGH);       
      delay(200);       
      digitalWrite(leds[i], LOW);
      noTone(buzzer);
    }       

    tone(buzzer, tons[3]);
    
    for (int i = 0; i < PINS; i++) {       
      digitalWrite(leds[0], HIGH);       
      digitalWrite(leds[1], HIGH);       
      digitalWrite(leds[2], HIGH);       
      digitalWrite(leds[3], HIGH);       
      delay(100);       
      digitalWrite(leds[0], LOW);       
      digitalWrite(leds[1], LOW);       
      digitalWrite(leds[2], LOW);       
      digitalWrite(leds[3], LOW);       
      delay(100);     
     }

     noTone(buzzer);
     
   displayFailure();   
   youLose = true;
   gameStarted = false;
}

void getSequence() {   
  boolean played = false;
   
  while (!played) {     
  for (int i = 0; i < PINS; i++) {   
    if (isSensorTouched(sensors[i])){
      
      touchedSensor = i; //Which button was Touched
      do{
        tone(buzzer, tons[touchedSensor]);
        digitalWrite(leds[touchedSensor], HIGH);
      }while (isSensorTouched(sensors[i]));
      
      played = true;
    }
    digitalWrite(leds[i], LOW); 
    noTone(buzzer);
    delay(20);  
  }     
  delay(20);   
 } 

}

boolean isSensorTouched(int sensorPin){
  if (analogRead(sensors[sensorPin]) < sensorMinValues[sensorPin]) return true;
    else return false;
}

//Add a random number in the array
 void nextTurn() {   
  int randomNumber = random(4);   
  sequence[currentTurn++] = randomNumber; 
 }  

boolean start(){
  if (!gameStarted){
    while (digitalRead(startButton) == LOW){
      displayWelcome();
    }
    lcd.clear();
    initializeVariables();
    startSong();
    getSensorMinValues();
    gameStarted = true;
    delay(10);
  }
  return gameStarted;
}
void displayWelcome(){
  lcd.setCursor(0,0);
  lcd.print("____WELCOME!____");
  lcd.setCursor(0,1);
  lcd.print("Press the button to start");//25 char (25-16 = 9)
  for (int positionCounter = 0; positionCounter < 9; positionCounter++) {
    // scroll one position left:
    delay(300);
    lcd.scrollDisplayLeft(); 
  }
}

void displayStatus(){
  lcd.clear();
  lcd.setCursor(0,0);
  String strTurn = "Turn: ";
  strTurn += currentTurn;
  lcd.print(strTurn);

  lcd.setCursor(0,1);
  String strRecord = "Your Record: ";
  strRecord += currentRecord;
  lcd.print(strRecord);
}

void displayFailure(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(" Game Over!! :'(");
  
  lcd.setCursor(0,1);
  String strRecord = "    Record:";
  strRecord += currentRecord;
  lcd.print(strRecord);

  delay(5000);
  lcd.clear();
}

void initializeVariables(){
  int sequencia[MAX_VALUE] = {};     
  currentTurn  = 0;     
  stepSequence = 0;     
  youLose = false;
  velocity = 1000;
}

void displayVictory(){
  if (youLose == false){    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print(" You got it!! :)");

    if (newRecord){
      lcd.setCursor(0,1);
      lcd.print("  New record!!"); 
    }
  }
}

void startSong() {   
  tone(buzzer, tons[0]);   
  digitalWrite(leds[0], HIGH);   
  digitalWrite(leds[1], HIGH);   
  digitalWrite(leds[2], HIGH);   
  digitalWrite(leds[3], HIGH);   
  delay(500);   
  digitalWrite(leds[0], LOW);   
  digitalWrite(leds[1], LOW);   
  digitalWrite(leds[2], LOW);   
  digitalWrite(leds[3], LOW);   
  delay(500);   
  noTone(buzzer); 
 }

//With this function I know what is the sensor value and how much it needs to decrease to light up the LED
void getSensorMinValues(){
   for(int i=0; i < PINS; i++){
    
    int valSensor = analogRead(sensors[i]);
    valSensor -= 100;

    sensorMinValues[i] = valSensor;
   } 
}
