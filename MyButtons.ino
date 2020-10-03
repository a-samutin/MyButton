
/*
   uint8_t *onTimer, uint8_t *offTimer, uint8_t  *prev, uint8_t curr, uint8_t onDelay, uint8_t offDelay

*/
//Buttin states for user
#define _BTN_SHORT_ 1
#define _BTN_LONG_  2
#define _BTN_RPT_   4
#define LONG_DELAY  1000
#define RPT_DELAY   500
#define _M_LONG_    0
#define _M_RPT_     1

typedef struct {
  uint8_t  onTimer;  //debouncing timers
  uint8_t  offTimer;
  uint8_t  prev;    //sprev tates; 1- On; 0 -Off
  uint8_t  waitForRelease;
  uint8_t  onDelay; //debouncing delays
  uint8_t  offDelay;
  uint8_t  usrState;  //State for user, reset when read
  uint16_t holdTime;  //pushed state counter
  uint8_t  rptMode; // 0 - SHORT by release, LONG by timeout;
  // 1 - SHORT by push, REP by repeat timeout ubtil released
} ButtonData;


uint8_t DoDebouncing(ButtonData* b, uint8_t state)
{
  if (b->onTimer)
  {
    if (--(b->onTimer)) return 1; //еще не истекла задержка после нажатия, считаем нажатой
  }
  if (b->offTimer)
  {
    if (--(b->offTimer)) return 0; //еще не истекла задержка после отпускания,считаем отпущеной
  }
  if (state != b->prev) // cocстояние изменилось
  {
    if (state)
      b->onTimer = b->onDelay;
    else
      b->offTimer = b->offDelay;
    b->prev = state;
  }
  return state;
}

uint8_t DoButton (ButtonData *b, uint8_t state)
{
  //  static uint8_t currentMode = 0;
  //  static uint16_t holdCnt = 0;
  state = DoDebouncing(b, state);
  //  static uint8_t waitForRelease = 0;
  if (b->rptMode == _M_LONG_) {
    if (state && !b->waitForRelease)
    { // нажата
      ++(b->holdTime);
      if (b->holdTime >= LONG_DELAY)
      {
        //нажата уже долго
        b->waitForRelease = 1;
        b->usrState = _BTN_LONG_ ;
        return b->usrState ;
      }
    }
    //отпущена
    if (state == 0) {
      b->waitForRelease = 0;
      if (b->holdTime && b->holdTime < LONG_DELAY)
      { // короткое нажатие.
        b->usrState = _BTN_SHORT_ ;
      } else
      {
        b->usrState = 0;
      }
      b->holdTime = 0;
    }
  }
  if (b->rptMode == _M_RPT_) {
    if (state) { //pushed
      if (!b->waitForRelease) //just recently
      {
        b->waitForRelease = 1;
        b->usrState = _BTN_SHORT_;
      } else {
        ++(b->holdTime);
        if (b->holdTime >= RPT_DELAY)
        {
          b->holdTime = RPT_DELAY>>1; //halving next intervals
          b->usrState = _BTN_RPT_;
        }
      }
    } else //released
    {
      b->waitForRelease = 0;
      b->holdTime = 0;
      b->usrState = 0;
    }
  }
  return b->usrState ;
}
* /
void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
