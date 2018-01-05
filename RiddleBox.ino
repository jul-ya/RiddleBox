#include <LiquidCrystal.h>
#include <Keypad.h>
#include <StateMachine.h>

StateMachine machine = StateMachine();
State* k1 = machine.addState(&keypad1); // keypad riddle 1
State* k2 = machine.addState(&keypad2); // keypad riddle 2
State* t1 = machine.addState(&toggle1); // toggle switch 1
State* t2 = machine.addState(&toggle2); // toggle switch 2
State* p = machine.addState(&piezo);  // final state
State* s = machine.addState(&solved); // final state

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
const int lamps[] = {0,0,0,0,0};

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

  // display intro
  for(int i = 0; i < 10; i++){
    String text = "";
    for(int n = 0; n < 16; n++){
      byte randomValue = random(0, 37);
      char letter = randomValue + 'a';
      if(randomValue > 26)
        letter = (randomValue - 26) + '0';
      text += letter;
    }
   
    delay(100);
    lcd.clear();
  }

  // led and switch setup
  for(int i = 0; i < 8; i++){
    pinMode(leds[i], OUTPUT);
    pinMode(btns[i], INPUT_PULLUP);
  }

  for(int i = 0; i < 5; i++){
    //pinMode(lamps[i], OUTPUT);
  }

  k1->addTransition(&transitionk1k2,k2);
  k2->addTransition(&transitionk2t1,t1);
  t1->addTransition(&transitiont1t2,t2);
  t2->addTransition(&transitiont2p,p);
  p->addTransition(&transitionps,s);
}

void loop() {
  machine.run();

  // LIGHT UP LAMPS
  /*
  if(k1_solved) digitalWrite(lamp[0],HIGH);
  if(k2_solved) digitalWrite(lamp[1],HIGH);
  if(t1_solved) digitalWrite(lamp[2],HIGH);
  if(t2_solved) digitalWrite(lamp[3],HIGH);
  if(p_solved) digitalWrite(lamp[4],HIGH);
  */
  
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

byte k2_pos;
byte k2_key[2];

bool transitionk1k2(){
  if(k1_solved){
    // -- INITIALIZE RIDDLE NO 2 --
    k2_pos = 0;
    
    // generate random keypad position
    k2_key[0] = random(0,3);
    k2_key[1] = random(0,3);

    // print keypad positions
    lcd.clear();
    lcd.print(" " + String(k2_key[0]) + " " + String(k2_key[1]));
    
    return true;
  }else
    return false;
}

// -- RIDDLE NR 2 --
void keypad2(){
  // print the pressed key
  char key = kpd.getKey();
  if(key != NO_KEY){
    if(k2_key[k2_pos] == key){
      playPositiveSound();
      k2_pos++;
      if(k2_pos >= 2){
        playSolvedSound();
        k2_solved = true;
      }
    }else{
      k2_pos = 0;
      playNegativeSound();
    }
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

void playPositiveSound(){
  tone(piezoPin, 440);
  delay(300);
  tone(piezoPin, 523.26);
  delay(300);
  noTone(piezoPin);
}

void playSolvedSound(){
  tone(piezoPin, 440);
  delay(300);
  tone(piezoPin, 523.26);
  delay(300);
  tone(piezoPin, 783.99);
  delay(300);
  noTone(piezoPin);
}

void playNegativeSound(){
  tone(piezoPin, 440);
  delay(100);
  tone(piezoPin, 415);
  delay(100);
  noTone(piezoPin);
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

/*
Display und Tastenfeld:
Rechenoperationen werden angezeigt, man muss immer die Zwischenergebnisse eingeben
Eingabe hat nichts mit den Tasten zu tun, zB ¾ (3 nach rechts, 4 nach unten)

Binärcode in LEDS und Kippschalter (8 pins):
Zufallszahl auf Display durch Kippschalter eingeben
Jeder Kippschalter bekommt eine Zufallszahl (irgendwelche müssen zusammen aber 255 ergeben) und dann muss man addieren bis alle leuchten, das LCD Display zeigt “score all” an

Piezo Element:
Einen Rhythmus mit den Kippschaltern kreieren, den einer der Oszillatoren vorspielt und als letzter 9ter Ton muss der Stern am Tastenfeld gedrückt werden
 */
