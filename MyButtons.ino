#define BUT1 6
#include "my_buttons.h"

MyButton but1(BUT1,0);
MyButton but2(17,1);

ISR (TIMER0_COMPA_vect)
{
  static uint16_t cnt = 0;
  ++cnt;
 // if (cnt > 800) 
 {
    cnt = 0;
    DoAllMyButtons();
  }
}
void setup() {
  Serial.begin(115200);
  Serial.println("Setup");
//  pinMode(BUT1, INPUT_PULLUP);
 // pinMode(17, INPUT_PULLUP);

  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);
}

void loop() {
 // uint8_t s = but1.WaitForButton();
 if (but1.Pushed())
  Serial.println((String) (millis () & 0xffff) + "  s1=" + but1.GetState());
 if (but2.Pushed())
  Serial.println((String) (millis () & 0xffff) + "  s2=" + but2.GetState());

}
