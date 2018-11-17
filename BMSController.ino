#include "HID-Project.h"
#define keypin 4
volatile extern NKROKeyboard_ nk=NKROKeyboard;
const int key[]={'e','z','s','x','d','c','f','v','r'};
const int disk[]={'q','w'};
volatile int value = 0;
volatile uint8_t prev = 0;
volatile int dir=0;
const int attack=5;
const int decay=100;
volatile int cntd=0;

void setup() {
  pinMode(2, INPUT_PULLUP); 
  pinMode(3, INPUT_PULLUP);
  for(int i=0;i<9;i++)
    pinMode(i+keypin,INPUT_PULLUP);
  
  attachInterrupt(0, updateEncoder, CHANGE);
  attachInterrupt(1, updateEncoder, CHANGE);
  int a = (PIND & _BV(0))>>0;
  int b = (PIND & _BV(1))>>1;
 
  prev = (a << 1) | b;
  nk.begin();
}

void updateEncoder() {
  int a = (PIND & _BV(0))>>0;
  int b = (PIND & _BV(1))>>1;
 
  uint8_t ab = (a << 1) | b;
  uint8_t encoded  = (prev << 2) | ab;

  if(encoded == 0b1101 || encoded == 0b0100 || encoded == 0b0010 || encoded == 0b1011){
    value ++;cntd=0;
  } else if(encoded == 0b1110 || encoded == 0b0111 || encoded == 0b0001 || encoded == 0b1000) {
    value --;cntd=0;
  } //from http://nn-hokuson.hatenablog.com/entry/2017/03/26/102145
  if(value>=attack){dir=1;cntd=0;value=0;nk.add(disk[0]);nk.remove(disk[1]);}
  else if(value<=-attack){dir=-1;cntd=0;value=0;nk.add(disk[1]);nk.remove(disk[0]);}
  prev = ab;
  delayMicroseconds(100);
}

int counter=0;

void loop() {
  for(int i=0;i<9;i++){
    if(!digitalRead(i+keypin))nk.add(key[i]);
    else nk.remove(key[i]);
  }
  if(cntd<decay)cntd++;
  if(cntd>=decay){dir=0;value=0;nk.remove(disk[0]);nk.remove(disk[1]);}
  nk.send();
}
