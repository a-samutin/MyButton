//#include <vector>
#define ON_DELAY   40
#define OFF_DELAY  40
#define LONG_DELAY  2000
#define RPT_DELAY   1000
#define PIN_NOT_DEFINED (0xFF)

#define _BTN_SHORT_ 1
#define _BTN_LONG_  2
#define _BTN_RPT_   4

#define _M_LONG_    0
#define _M_RPT_     1

class MyButton;

MyButton * pLastMyButton = NULL;


class MyButton
{
  public:
    MyButton(uint8_t pin, uint8_t rptMode = _M_RPT_, uint8_t active = LOW, uint8_t onDelay = ON_DELAY, uint8_t offDelay = OFF_DELAY);
    //    MyButton();
    uint8_t WaitForButton(); //blocking call to get button state
    uint8_t GetState();  //non blocking call to get latest state
    uint8_t Pushed(); //return 1 if resently pushed. non blocking
    uint8_t DoButton();
    uint8_t DoButton (uint8_t state);
    MyButton * m_prevButt;

  private:
    uint8_t DoDebouncing(uint8_t state);
    uint8_t  m_inputPin;
    uint8_t  m_active;
    uint8_t  m_rptMode; // 0 - SHORT by release, LONG by timeout;
    uint8_t  m_onDelay; //debouncing delays
    uint8_t  m_offDelay;
    uint8_t  m_onTimer;  //debouncing timers
    uint8_t  m_offTimer;
    uint8_t  m_prev;    //prev states; 1- On; 0 -Off
    uint8_t  m_waitForRelease;

    volatile uint8_t  m_usrState;  //State for user, reset when read
    uint16_t m_holdTime;  //pushed state counter

    // inline  static uint16_t longDelay=LONG_DELAY ;
    //    static uint16_t rptDelay;
};

MyButton:: MyButton(uint8_t pin, uint8_t rptMode, uint8_t active , uint8_t onDelay,  uint8_t offDelay):
  m_inputPin(pin), m_active(active), m_rptMode(rptMode), m_onDelay(onDelay), m_offDelay(offDelay),
  m_onTimer(0), m_offTimer(0), m_prev(0), m_waitForRelease(0)

{
  m_prevButt = pLastMyButton;
  pLastMyButton = this;
  if (active == LOW)
     pinMode(pin, INPUT_PULLUP);
  else  
     pinMode(pin, INPUT); 
}

uint8_t  MyButton::DoDebouncing(uint8_t state)
{
  if (m_onTimer)
  {
    if (--(m_onTimer)) return 1; //еще не истекла задержка после нажатия, считаем нажатой
  }
  if (m_offTimer)
  {
    if (--m_offTimer) return 0; //еще не истекла задержка после отпускания,считаем отпущеной
  }
  if (state != m_prev) // cocстояние изменилось
  {
    if (state)
      m_onTimer = m_onDelay;
    else
      m_offTimer = m_offDelay;
    m_prev = state;
  }
  return state;
}

uint8_t MyButton::DoButton (uint8_t state)
{

  state = DoDebouncing(state);
  // Serial.print((String) "S=" + state + " mode " + m_rptMode);
  if (m_rptMode == _M_LONG_) {
    //  Serial.print(" long ");
    if (state && !m_waitForRelease)
    { // нажата
      ++(m_holdTime);
      if (m_holdTime >= LONG_DELAY)
      {
        //нажата уже долго
        m_waitForRelease = 1;
        m_usrState = _BTN_LONG_ ;
        return m_usrState ;
      }
    }
    //отпущена
    if (state == 0) {
      m_waitForRelease = 0;
      if (m_holdTime && m_holdTime < LONG_DELAY)
      { // короткое нажатие.
        m_usrState = _BTN_SHORT_ ;
      }
      m_holdTime = 0;
    }
  }
  if (m_rptMode == _M_RPT_) {
    if (state) { //pushed
      if (!m_waitForRelease) //just recently
      {
        m_waitForRelease = 1;
        m_usrState = _BTN_SHORT_;
      } else {
        ++(m_holdTime);
        if (m_holdTime >= RPT_DELAY)
        {
          m_holdTime = RPT_DELAY >> 1; //halving next intervals
          m_usrState = _BTN_RPT_;
        }
      }
    } else //released
    {
      m_waitForRelease = 0;
      m_holdTime = 0;
      //    m_usrState = 0;
    }
  }
  return m_usrState ;
}

uint8_t MyButton::DoButton ()
{
  uint8_t ret = DoButton(digitalRead(m_inputPin) == m_active);
  //  Serial.println((String) "  ret" + ret);
  return  ret;
}

uint8_t MyButton::WaitForButton() {
  while (!m_usrState); //waiting
  uint8_t ret = m_usrState;
  m_usrState = 0;
  return ret;
}

uint8_t MyButton::Pushed()
{
  return (m_usrState != 0);
}

uint8_t MyButton::GetState() {
  uint8_t ret = m_usrState;
  if (ret) m_usrState = 0;
  return ret;
}

void DoAllMyButtons()
{
  MyButton * curr = pLastMyButton;
  while (curr) {
    curr->DoButton();
    curr = curr->m_prevButt;
  }
}
