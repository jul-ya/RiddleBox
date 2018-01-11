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

const int leds[] = {39,41,43,45,47,49,51,53}; 
const int btns[] = {52, 50, 48, 46, 44, 42, 40, 38}; // watch out! polarity inverted!
const int lamps[] = {37,35,33,31,29};

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte colPins[COLS] = {30, 32, 34, 36};
byte rowPins[ROWS] = {22, 24, 26, 28};
Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS); 

void setup() {
  Serial.begin(9600);
  randomSeed(analogRead(8));
  
  // lcd setup
  lcd.begin(16, 2);

  // led and switch setup
  for(int i = 0; i < 8; i++){
    pinMode(leds[i], OUTPUT);
    pinMode(btns[i], INPUT_PULLUP);
  }
  
  for(int i = 0; i < 5; i++){
    pinMode(lamps[i], OUTPUT);
  }
  
  // simple display test
  //lcd.noDisplay();
  //lcd.display();
  
  // display intro
  // todo: second row
  for(int i = 0; i < 16; i++){
    lcd.clear();
    String text = "";
    byte lampNr = i%5;
    
    for(int n = 0; n < 32; n++){
      byte randomValue = random(0, 37);
      char letter = randomValue + 'a';
      if(randomValue > 26)
        letter = (randomValue - 26) + '0';
      text += letter;
    }
    lcd.print(text);
    lcd.setCursor(0, 1);
    lcd.print(text.substring(16));

    digitalWrite(leds[i%8],HIGH);
    delay(20);
    digitalWrite(leds[i%8],LOW);
  }

  
  peep();

  k1_solved = true;
  k2_solved = true;
  t1_solved = true;
  t2_solved = true;

  k1->addTransition(&transitionk1k2,k2);
  k2->addTransition(&transitionk2t1,t1);
  t1->addTransition(&transitiont1t2,t2);
  t2->addTransition(&transitiont2p,p);
  p->addTransition(&transitionps,s);
}

void loop() {
  machine.run();

  // LIGHT UP LAMPS
  if(!p_solved){
    if(k1_solved) digitalWrite(lamps[0],HIGH);
    if(k2_solved) digitalWrite(lamps[1],HIGH);
    if(t1_solved) digitalWrite(lamps[2],HIGH);
    if(t2_solved) digitalWrite(lamps[3],HIGH);
    if(p_solved) digitalWrite(lamps[4],HIGH);
  }
}

void keypad1(){
  // print the pressed key
  char key = kpd.getKey();
  if(key){
        Serial.println(key);
  }
}

byte k2_pos;
byte k2_keypos[6];

bool transitionk1k2(){
  if(k1_solved){
    // -- INITIALIZE RIDDLE NO 2 --
    k2_pos = 0;
    
    // generate random keypad positions
    for(int i = 0; i < 6; i++){
      k2_keypos[i] = random(1,5);
    }

    // print keypad positions
    lcd.clear();
    lcd.print(String(k2_keypos[0]) + "/" + String(k2_keypos[1]) + " " + 
              String(k2_keypos[2]) + "/" + String(k2_keypos[3]) + " " +
              String(k2_keypos[4]) + "/" + String(k2_keypos[5]));
    
    
    return true;
  }else
    return false;
}

// -- RIDDLE NR 2 --
// :: enter random keypad coordinates
void keypad2(){
  // print the pressed key
  char key = kpd.getKey();

  if(key != NO_KEY){
    peep();

    // search for pressed key in keymap
    int row = 0, col = 0;
    boolean found = false;
    for(row = 0; row < ROWS; row++){
      for(col = 0; col < COLS; col++){
        found = key == keys[row][col];
        if(found){
          break;
        }
      }        
      if(found){
        break;
      }
    }
    
    if(k2_keypos[k2_pos*2] == col+1 && k2_keypos[k2_pos*2 + 1] == row+1){
      playPositiveSound();
      k2_pos++;
      if(k2_pos >= 3){
        playSolvedSound();
        k2_solved = true;
      }
    }else{
      k2_pos = 0;
      playNegativeSound();
    }
  }
}

byte t1_randomNr;

bool transitionk2t1(){
  if(k2_solved){
    // -- INITIALIZE RIDDLE NO 3 --
    t1_randomNr = random(); // generate 8bit nr
    if(t1_randomNr < 30){
      t1_randomNr += 30;
    }
    lcd.clear();
    lcd.print(t1_randomNr);
    
    return true;
  }
  else
    return false;
}

// -- RIDDLE NR 3 --
// :: enter random keypad coordinates
void toggle1(){
  // leds are turned on with switches
  for(int i = 0; i < 8; i++){
    if(digitalRead(btns[i])){
      digitalWrite(leds[i], HIGH);
    }else{
      digitalWrite(leds[i], LOW);
    }
  }

  // check switches
  boolean correct = true;
  byte switchValue = 0;
  for(int bitNr = 0; bitNr < 8; bitNr++){
    boolean bitState = (1 << bitNr) & t1_randomNr;
    if(digitalRead(btns[bitNr]))
      bitSet(switchValue, bitNr);
    if(bitState != digitalRead(btns[bitNr])){
      correct = false;
    }
  }

  lcd.clear();
  lcd.print(t1_randomNr);
  lcd.setCursor(0, 1);
  lcd.print(switchValue, HEX);
  
  if(correct){
    playSolvedSound();
    t1_solved = true;
  }

  delay(20);
}

byte t2_numbers[8];

bool transitiont1t2(){
  if(t1_solved){
    // -- INITIALIZE RIDDLE NO 4 --

    // generate random numbers
    for(int i = 0; i < 8; i++){
      t2_numbers[i] = random(1,255); // nr between 1 and 254 inklusive
    }

    // choose 3 positions
    byte pos1,pos2,pos3;
    pos1 = random(0,8);
    do{ pos2 = random(0,8); }while(pos2 == pos1);
    do{ pos3 = random(0,8); }while(pos3 == pos1 || pos3 == pos2);
    // set 3rd number to add up to 255 with 1st and 2nd number

    // make 3 numbers sum to 255
    t2_numbers[pos2] = random(1,255-t2_numbers[pos1]);
    t2_numbers[pos3] = 255 - (t2_numbers[pos1] + t2_numbers[pos2]);

    Serial.println(t2_numbers[pos1]); 
    Serial.println(t2_numbers[pos2]);
    Serial.println(t2_numbers[pos3]);

    // BUBBLESORT
    bool isSorted = false;
    do{
      isSorted = true;
      for(byte i = 0; i < 7; i++){
        if(t2_numbers[i] > t2_numbers[i+1]){
          byte temp = t2_numbers[i];
          t2_numbers[i] = t2_numbers[i+1];
          t2_numbers[i+1] = temp;
          isSorted = false;
        }
      }
    }while(!isSorted);
    
    
    return true;
  }else
    return false;
}

// -- RIDDLE NR 4 --
// :: add random numbers
void toggle2(){
  int sum = 0;
  
  // add activated numbers
  for(int i = 0; i < 8; i++){
    if(digitalRead(btns[i])){
      sum += t2_numbers[i];
    }
  }

  // print sum to display
  lcd.clear();
  lcd.print(sum);

  boolean correct = true;
  
  // light up leds according to sum
  for(int bitNr = 0; bitNr < 8; bitNr++){
    boolean bitState = (1 << bitNr) & (byte)sum;
    digitalWrite(leds[bitNr], bitState);
    if(!bitState){
      correct = false;
    }
  }

  if(correct){    
    delay(500);
    for(int i = 0; i < 8; i++){
      digitalWrite(leds[7-i], LOW);
      delay(50);
    }
    lcd.clear();
    playSolvedSound();
    
    t2_solved = true;
  }

  delay(20);
}

bool p_rythm[8];
int p_note1Freq = 440;
int p_note2Freq = 494;
int p_T = 200;
byte p_index = 0;

bool transitiont2p(){
  if(t2_solved){
    // -- INITIALIZE RIDDLE NO 5 --
    
    Serial.println("TUNE");
    // generate rythm
    for(int i = 0; i < 4; i++){
      byte pos;
      do{
        pos = random(0,8);
      }while(p_rythm[pos]);
      
      p_rythm[pos] = true;
    }
    
    Serial.println("TUNE");
    
    return true;
  }else
    return false;
}
boolean p_correct = false;
int notes[] = {440,466,493,523,554,587,622,659,698,739,783,830,880,932,987,1046};

// -- RIDDLE NR 5 --
// :: play tune
void piezo(){
  // handle peeps
  if(digitalRead(btns[p_index])){
    tone(piezoPin, p_note1Freq, 25);
  }
  delay(25);
  if(p_rythm[p_index]){
    tone(piezoPin, p_note2Freq, 25);
  }

  
  for(int i = 0; i < 8; i++){
    // handle leds
    if(digitalRead(btns[i])^(p_index==i)){
      digitalWrite(leds[i], HIGH);
    }else{
      digitalWrite(leds[i], LOW);
    }

    // check if everything is alright, is alright
    if(digitalRead(btns[i]) != p_rythm[i]){
      p_correct = false;
    }
  }
  
  delay(p_T);

  if(p_correct){
    p_solved = true;
    delay(500);
    
    for(int i = 0; i < 8; i++){
      tone(piezoPin, notes[i]);
      digitalWrite(leds[7-i], HIGH);
      delay(50);
      digitalWrite(leds[7-i], LOW);
    }
    noTone(piezoPin);
    
    lcd.clear();
    lcd.print("the journey ends");
    lcd.setCursor(0, 1);
    lcd.print("no more thought");
    playSolvedSound();
    digitalWrite(lamps[4],HIGH);
  }
  
  if(p_index < 7){
    p_index++;
  }else{
    p_index = 0;
    p_correct = true;
  }
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
  delay(150);
  tone(piezoPin, 523.26);
  delay(150);
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

void peep(){
  tone(piezoPin, 880, 50);
}

bool transitionps(){
  if(p_solved)
    return true;
  else
    return false;
}



void solved(){

  char key = kpd.getKey();
  if(key != NO_KEY){
    // search for pressed key in keymap
    int row = 0, col = 0;
    boolean found = false;
    for(row = 0; row < ROWS; row++){
      for(col = 0; col < COLS; col++){
        found = key == keys[row][col];
        if(found){
          break;
        }
      }        
      if(found){
        break;
      }
    }

    for(int i = 0; i < 5; i++){
      boolean r = (int)random(2) == 1;
      digitalWrite(lamps[i],r);
    }

    tone(piezoPin, notes[row+col*4],100);
  }else{
    //noTone(piezoPin);

  }
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
