
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <StateMachine.h>

StateMachine machine = StateMachine();
State* k1 = machine.addState(&keypad1);
State* k2 = machine.addState(&keypad2);
State* t1 = machine.addState(&toggle1);
State* t2 = machine.addState(&toggle2);
State* p = machine.addState(&piezo);
State* s = machine.addState(&solved);

bool k1_solved = false;
bool k2_solved = false;
bool t1_solved = false;
bool t2_solved = false;
bool p_solved = false;

const int piezoPin = 23;

const int rs = 7, en = 6, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int leds[] = {53,51,49,47,45,43,41,39};
const int btns[] = {38,40,42,44,46,48,50,52};

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {36, 34, 32, 30};
byte colPins[COLS] = {28, 26, 24, 22};
Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS); 

void setup() {
  Serial.begin(9600);
  
  // lcd setup
  lcd.begin(16, 2);
  lcd.print("RiddleBox 3000");

  // led and switch setup
  for(int i = 0; i < 8; i++){
    pinMode(leds[i], OUTPUT);
    pinMode(btns[i], INPUT_PULLUP);
  }

  k1->addTransition(&transitionk1k2,k2);
  k2->addTransition(&transitionk2t1,t1);
  t1->addTransition(&transitiont1t2,t2);
  t2->addTransition(&transitiont2p,p);
  p->addTransition(&transitionps,s);
}

void loop() {
  machine.run();
  
  /* simple display test
  lcd.noDisplay();
  delay(500);
  lcd.display();
  delay(500);*/
}

void keypad1(){
  // print the pressed key
  char key = kpd.getKey();
  if(key){
        Serial.println(key);
  }
}

bool transitionk1k2(){
  if(k1_solved)
    return true;
  else
    return false;
}

void keypad2(){
  // print the pressed key
  char key = kpd.getKey();
  if(key){
        Serial.println(key);
  }
}

bool transitionk2t1(){
  if(k2_solved)
    return true;
  else
    return false;
}

void toggle1(){
  // leds are turned on with switches
  for(int i = 0; i < 8; i++){
    if(digitalRead(btns[i])){
      digitalWrite(leds[i], HIGH);
    }else{
      digitalWrite(leds[i], LOW);
    }
  }
}

bool transitiont1t2(){
  if(t1_solved)
    return true;
  else
    return false;
}

void toggle2(){
  // leds are turned on with switches
  for(int i = 0; i < 8; i++){
    if(digitalRead(btns[i])){
      digitalWrite(leds[i], HIGH);
    }else{
      digitalWrite(leds[i], LOW);
    }
  }
}

bool transitiont2p(){
  if(t2_solved)
    return true;
  else
    return false;
}

void piezo(){
  Serial.println("piezo");
}

bool transitionps(){
  if(p_solved)
    return true;
  else
    return false;
}

void solved(){
  Serial.println("!!! solved !!!");
}
