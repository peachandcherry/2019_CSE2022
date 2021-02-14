
#include <Time.h>
#include <TimeLib.h>


#include <Adafruit_NeoPixel.h>

#define LEDPIN 6
#define NUM_LEDS 138
#define BUTTONPIN_T 10
#define BUTTONPIN_B 11
#define BUTTONPIN_A 9
#define MOTIONPIN 13


#define tmode1 normalmode(20,40,228,228,228)//독서 cylonBounce(30,30,30, 3,20,20)
#define tmode2 normalmode(20,40,22,55,20)//암기
#define tmode3 normalmode(20,40,0,20,34)//진정
#define tmode4 Fire(125,20,15)
#define tmode_h timer()

#define bmode1 normalmode(116,140,168,130,30)//휴식
#define bmode2 normalmode(116,140,0,20,34)//진정
#define bmode3 normalmode(116,140,228,228,228)//독서
#define bmode4 normalmode(116,140,22,55,20)//암기
#define bmode_h normalmode(116,140,35,120,38)

#define amode1 humidmode();
#define amode2 clockmode();
#define amode3
#define amode4
#define amode_h

int tmode = 0;
int bmode = 0;
int amode = 0;


//======================================

//<<timer>>

/*시작 상태*/
/*status = 불을 끄고 키는 것을 관리*/
/*new status = 타이머를 끄고 키는 것을 관리*/
boolean status = false;
boolean newStatus = false;
int sec = 0;
int currentSec = 0;
/*책상 위치 LED 시작 번호*/
int sensorValue = 0;

//=================

//<clock>
//int whole_second = 60*60*24;
int now_second;
int delay_val = 500;
int lednum;

//========================
//<welcome>
int in_out = 0;
int last_in_out = 0;
int pvalue;
//=========================
// Button timing variables

int rebound = 20; // ms rebound period to prevent flickering when pressing or releasing the button
int DCgap = 250; // max ms between clicks for a double click event
int holdTime = 1000; // ms hold period: how long to wait for press+hold event


// table button variables
boolean buttonVal_trex = HIGH;
boolean buttonLast_trex = HIGH;
boolean DCwaiting_trex = false;
boolean DConUp_trex = false;
boolean singleOK_trex = true;
long downTime_trex = -1;
long upTime_trex = -1;
boolean ignoreUp_trex = false;
boolean waitForUp_trex = false;
boolean holdEventPast_trex = false;

// bed button variables
boolean buttonVal_brex = HIGH;
boolean buttonLast_brex = HIGH;
boolean DCwaiting_brex = false;
boolean DConUp_brex = false;
boolean singleOK_brex = true;
long downTime_brex = -1;
long upTime_brex = -1;
boolean ignoreUp_brex = false;
boolean waitForUp_brex = false;
boolean holdEventPast_brex = false;

// all button variables
boolean buttonVal_arex = HIGH;
boolean buttonLast_arex = HIGH;
boolean DCwaiting_arex = false;
boolean DConUp_arex = false;
boolean singleOK_arex = true;
long downTime_arex = -1;
long upTime_arex = -1;
boolean ignoreUp_arex = false;
boolean waitForUp_arex = false;
boolean holdEventPast_arex = false;

// strip medthods prestatement

void showStrip();
void setAll(byte red, byte green, byte blue);
void setAllC(uint32_t);
void setPixel(int Pixel, byte red, byte green, byte blue);
void buttonInput();
void normalmode(int f, int l, byte r, byte g, byte b);
void timer();
void meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay);
void fadeToBlack(int ledNo, byte fadeValue);
void Fire(int Cooling, int Sparking, int SpeedDelay);
void setPixelHeatColor (int Pixel, byte temperature);
//=================================================

// state object
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LEDPIN, NEO_GRB + NEO_KHZ800);

//==================================================

//<<color list>

//for clock
uint32_t skylist[46] = {
  strip.Color(2, 0, 6), strip.Color(4, 2, 10), strip.Color(6, 3, 14), strip.Color(8, 4, 14), strip.Color(10, 5, 15), strip.Color(12, 4, 16), strip.Color(14, 4, 17),
  strip.Color(16, 8, 23), strip.Color(18, 8, 25), strip.Color(20, 8, 28), strip.Color(22, 9, 29), strip.Color(24, 10, 30), strip.Color(26, 12, 33),
  strip.Color(28, 15, 35), strip.Color(30, 17, 36), strip.Color(32, 20, 41), strip.Color(44, 25, 42), strip.Color(57, 31, 43), strip.Color(74, 37, 45), strip.Color(91, 43, 48),
  strip.Color(108, 45, 45), strip.Color(125, 50, 42), strip.Color(142, 55, 39), strip.Color(159, 60, 36), strip.Color(176, 70, 33), strip.Color(191, 80, 36),
  strip.Color(206, 92, 42), strip.Color(221, 105, 50), strip.Color(227, 120, 55), strip.Color(233, 134, 61), strip.Color(242, 149, 57), strip.Color(254, 173, 66), strip.Color(250, 177, 83),
  strip.Color(246, 181, 101), strip.Color(243, 193, 131), strip.Color(241, 196, 140), strip.Color(231, 198, 159), strip.Color(219, 195, 166), strip.Color(221, 200, 180), strip.Color(203, 192, 182),
  strip.Color(181, 200, 186), strip.Color(165, 211, 188), strip.Color(138, 201, 174), strip.Color(99, 156, 188), strip.Color(52, 107, 203),
  strip.Color(20, 60, 203),
};

//humid
uint32_t humidity[4] = {
  strip.Color(183, 198, 50),
  strip.Color(153, 194, 56),
  strip.Color(122, 190, 63),
  strip.Color(97, 187, 70)
};

//===============================================================================

void setup()
{
  // Set button input pin
  pinMode(A0, INPUT);
  pinMode(BUTTONPIN_T, INPUT);
  pinMode(BUTTONPIN_B, INPUT);
  pinMode(BUTTONPIN_A, INPUT);
  pinMode(MOTIONPIN, INPUT);
  pinMode(A0, INPUT);
  digitalWrite(BUTTONPIN_T, HIGH );
  digitalWrite(BUTTONPIN_B, HIGH );
  digitalWrite(BUTTONPIN_A, HIGH );
  //set time
  setTime(0, 0, 0, 1, 1, 1970);
  // Set neopixel
  strip.begin();
  strip.show();
  Serial.begin(9600);
}

//=================================================================================

void loop()
{
  Serial.println(analogRead(A0));
  buttonInput();
  pvalue = analogRead(A0);
  if(pvalue>300){
    if(last_in_out==0){
      in();
      tmode = 1;
      last_in_out = 1;
    }else{
      amode = tmode = bmode = 0;
      setAll(0,0,0);
      showStrip();
      last_in_out = 0;
      delay(300);
    }
  }
  //  Serial.println(tmode);
  //  Serial.println(bmode);
  //  Serial.println(amode);
  switch (tmode) {
    case 1: tmode1; break;
    case 2: tmode2; break;
    case 3: tmode3; break;
    case 4: tmode4; break;
    case 100: tmode_h; break;
  }
  switch (bmode) {
    case 1: bmode1; break;
    case 2: bmode2; break;
    case 3: bmode3; break;
    case 4: bmode4; break;
    case 100: bmode_h; break;
  }
  switch (amode) {
    case 1: amode1; break;
    case 2: amode2; break;
    case 3: amode3; break;
    case 4: amode4; break;
    case 100: amode_h; break;
  }
}

//==========================================================================
void in() {
  meteorRain(0xff, 0xff, 0xff, 5, 64, true, 15);
  amode = 1;
  tmode = 0;
  bmode = 0;
}
//======================================================================

void humidmode() {

  sensorValue = analogRead(A0);
  if (sensorValue >= 0 && sensorValue < 25) {
    for (int i = 0; i < NUM_LEDS; i++) {
      strip.setPixelColor(i, humidity[3]);
    }
  }
  else if (sensorValue >= 25 && sensorValue < 50) {
    for (int i = 0; i < NUM_LEDS; i++) {
      strip.setPixelColor(i, humidity[2]);
    }
  }
  else if (sensorValue >= 50 && sensorValue < 75) {
    for (int i = 0; i < NUM_LEDS; i++) {
      strip.setPixelColor(i, humidity[1]);
    }
  }
  else if (sensorValue >= 75 && sensorValue <= 100) {
    for (int i = 0; i < NUM_LEDS; i++) {
      strip.setPixelColor(i, humidity[0]);
    }
  }
  strip.show();
}

//----------------------------------------------------------

void clockmode() {
  int now_second = now();
  lednum = map(now_second, 0, 140, 0, 140);
  for (int i = 0; i < 46; i++) {
    strip.setPixelColor(3 * i, skylist[i]);
    strip.setPixelColor((3 * i) + 1, skylist[i]);
    strip.setPixelColor((3 * i) + 2, skylist[i]);
  }
  strip.setPixelColor(lednum, 30, 30, 30);
  strip.setPixelColor(lednum + 1, 200, 200, 200);
  strip.setPixelColor(lednum + 2, 30, 30, 30);
  strip.show();
}
//--------------------------------------------------------------
void normalmode(int f, int l, byte r, byte g, byte b) {
  for (int i = f; i < l; i++) {
    setPixel(i, r, g, b);
  }
  strip.show();
}
//---------------------------------------------------------------
void timer() {
  sensorValue = digitalRead(10);
  Serial.print(sensorValue);
  int i = 20;
  int t = 0;
  if ((sensorValue == LOW) /*&& (newStatus == true)*/) {
    //rtc.startRTC();
    while (i < 40) {
      delay(1000);
      sensorValue = digitalRead(10);
      Serial.print(sensorValue);
      if (sensorValue == LOW) {
        strip.setPixelColor(i, strip.Color(0, 150, 150));
        strip.show();
        Serial.println("1");
        i++;
        sec++;
      }
      else {
        newStatus = false;
        strip.clear();
        Serial.print(sec);
        break;
      }
    }
  }

  if ((sec > 0)/* && (newStatus == false)*/) {
    for (int p = 20; p < sec + 20; p++) {
      strip.setPixelColor(p, strip.Color(150, 0, 150));
      strip.show();
      delay(3000);
    }
    status = false;
    tmode=4;
    return;    
  }
  tmode = 0;
}
//================================================
void showStrip() {
  strip.show();
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
  strip.setPixelColor(Pixel, strip.Color(red, green, blue));
}

void setAll(byte red, byte green, byte blue) {
  for (int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue);
  }
}

void setAllC(uint32_t color) {
  for (int i = 0; i < NUM_LEDS; i++ ) {
    strip.setPixelColor(i, color);
  }
}

void meteorRain(byte red, byte green, byte blue, byte meteorSize, byte meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay) {
  setAll(0, 0, 0);

  for (int i = 0; i < NUM_LEDS + NUM_LEDS; i++) {


    // fade brightness all LEDs one step
    for (int j = 0; j < NUM_LEDS; j++) {
      if ( (!meteorRandomDecay) || (random(10) > 5) ) {
        fadeToBlack(j, meteorTrailDecay );
      }
    }

    // draw meteor
    for (int j = 0; j < meteorSize; j++) {
      if ( ( i - j < NUM_LEDS) && (i - j >= 0) ) {
        setPixel(i - j, red, green, blue);
      }
    }

    showStrip();
    delay(SpeedDelay);
  }
}

void fadeToBlack(int ledNo, byte fadeValue) {
#ifdef ADAFRUIT_NEOPIXEL_H
  // NeoPixel
  uint32_t oldColor;
  uint8_t r, g, b;
  int value;

  oldColor = strip.getPixelColor(ledNo);
  r = (oldColor & 0x00ff0000UL) >> 16;
  g = (oldColor & 0x0000ff00UL) >> 8;
  b = (oldColor & 0x000000ffUL);

  r = (r <= 10) ? 0 : (int) r - (r * fadeValue / 256);
  g = (g <= 10) ? 0 : (int) g - (g * fadeValue / 256);
  b = (b <= 10) ? 0 : (int) b - (b * fadeValue / 256);

  strip.setPixelColor(ledNo, r, g, b);
#endif
#ifndef ADAFRUIT_NEOPIXEL_H
  // FastLED
  leds[ledNo].fadeToBlackBy( fadeValue );
#endif
}

void Fire(int Cooling, int Sparking, int SpeedDelay) {
  static byte heat[NUM_LEDS];
  int cooldown;
  
  // Step 1.  Cool down every cell a little
  for( int i = 0; i < NUM_LEDS; i++) {
    cooldown = random(0, ((Cooling * 10) / NUM_LEDS) + 2);
    
    if(cooldown>heat[i]) {
      heat[i]=0;
    } else {
      heat[i]=heat[i]-cooldown;
    }
  }
  
  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for( int k= NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
  }
    
  // Step 3.  Randomly ignite new 'sparks' near the bottom
  if( random(255) < Sparking ) {
    int y = random(7);
    heat[y] = heat[y] + random(160,255);
    //heat[y] = random(160,255);
  }

  // Step 4.  Convert heat to LED colors
  for( int j = 0; j < NUM_LEDS; j++) {
    setPixelHeatColor(j, heat[j] );
  }

  showStrip();
  delay(SpeedDelay);
}

void setPixelHeatColor (int Pixel, byte temperature) {
  // Scale 'heat' down from 0-255 to 0-191
  byte t192 = round((temperature/255.0)*191);
 
  // calculate ramp up from
  byte heatramp = t192 & 0x3F; // 0..63
  heatramp <<= 2; // scale up to 0..252
 
  // figure out which third of the spectrum we're in:
  if( t192 > 0x80) {                     // hottest
    setPixel(Pixel, 255, 255, heatramp);
  } else if( t192 > 0x40 ) {             // middle
    setPixel(Pixel, 255, heatramp, 0);
  } else {                               // coolest
    setPixel(Pixel, heatramp, 0, 0);
  }
}

void rgbLoop() {
  for (int j = 0; j < 3; j++ ) {
    // Fade IN
    for (int k = 0; k < 256; k++) {
      switch (j) {
        case 0: setAll(k, 0, 0); break;
        case 1: setAll(0, k, 0); break;
        case 2: setAll(0, 0, k); break;
      }
      showStrip();
      delay(1);
    }
    // Fade OUT
    for (int k = 255; k >= 0; k--) {
      switch (j) {
        case 0: setAll(k, 0, 0); break;
        case 1: setAll(0, k, 0); break;
        case 2: setAll(0, 0, k); break;
      }
      showStrip();
      delay(1);
    }
  }
}

void fadeInOut(byte red, byte green, byte blue) {
  float r, g, b;
  for (int k = 0; k < 256; k++) {
    buttonInput();
    r = (k / 256.0) * red;
    g = (k / 256.0) * green;
    b = (k / 256.0) * blue;
    setAll(r, g, b);
    showStrip();
  }
  for (int k = 255; k >= 0; k--) {
    buttonInput();
    r = (k / 256.0) * red;
    g = (k / 256.0) * green;
    b = (k / 256.0) * blue;
    setAll(r, g, b);
    showStrip();
  }
}

void strobe(byte red, byte green, byte blue, int strobeCount, int FlashDelay, int EndPause) {
  for (int j = 0; j < strobeCount; j++) {
    setAll(red, green, blue);
    showStrip();
    delay(FlashDelay);
    setAll(0, 0, 0);
    showStrip();
    delay(FlashDelay);
  }
  delay(EndPause);
}

void cylonBounce(byte red, byte green, byte blue, int EyeSize, int SpeedDelay, int ReturnDelay) {

  for (int i = 0; i < NUM_LEDS - EyeSize - 2; i++) {
    setAll(0, 0, 0);
    setPixel(i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(i + j, red, green, blue);
    }
    setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
    showStrip();
    delay(SpeedDelay);
  }
  delay(ReturnDelay);
  for (int i = NUM_LEDS - EyeSize - 2; i > 0; i--) {
    setAll(0, 0, 0);
    setPixel(i, red / 10, green / 10, blue / 10);
    for (int j = 1; j <= EyeSize; j++) {
      setPixel(i + j, red, green, blue);
    }
    setPixel(i + EyeSize + 1, red / 10, green / 10, blue / 10);
    showStrip();
    delay(SpeedDelay);
  }
  setAll(0, 0, 0);
  showStrip();
  delay(ReturnDelay);
}

void runningLights(byte red, byte green, byte blue, int WaveDelay) {
  int Position = 0;
  for (int j = 0; j < NUM_LEDS * 2; j++)
  {
    Position++; // = 0; //Position + Rate;
    for (int i = 0; i < NUM_LEDS; i++) {
      setPixel(i, ((sin(i + Position) * 127 + 128) / 255)*red,
               ((sin(i + Position) * 127 + 128) / 255)*green,
               ((sin(i + Position) * 127 + 128) / 255)*blue);
    }
    showStrip();
    buttonInput();
    /*for(int i=0; WaveDelay; i++){
      delay(1);
      buttonInput();
      }*/
    for (int i = 0; i < 300000; i++) {
      buttonInput();
    }
  }
  setAll(0, 0, 0);
  showStrip();
}

void colorWipe(byte red, byte green, byte blue, int SpeedDelay) {
  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    setPixel(i, red, green, blue);
    showStrip();
    delay(SpeedDelay);
  }
}
//=================================================

// Events to trigger by table button input

void tclickEvent() {

  Serial.println("t click");
  if (tmode == 0) {
    setAll(0, 0, 0);
    bmode = 0;
    amode = 0;
    tmode = 1;
  } else {
    tmode = 0;
    setAll(0, 0, 0);
    showStrip();
  }
}

void tdoubleClickEvent() {
  Serial.println("t doubleclick");
  if (tmode == 0) {
    return;
  }
  setAll(0, 0, 0);
  tmode = (tmode + 1) % 5;
  if (tmode == 0) tmode = 1;
}

void tholdEvent() {
  Serial.println("t hold");
  setAll(0, 0, 0);
  tmode = 100;
}

//----------------------------------------------------

// Events to trigger by bed button input

void bclickEvent() {
  Serial.println("b click");
  if (bmode == 0) {
    setAll(0, 0, 0);
    tmode = 0;
    amode = 0;
    bmode = 1;
  } else {
    bmode = 0;
    setAll(0, 0, 0);
    showStrip();
  }
}

void bdoubleClickEvent() {
  Serial.println("b doubleclick");
  if (bmode == 0) {
    return;
  }
  setAll(0, 0, 0);
  bmode = (bmode + 1) % 5;
  if (bmode == 0) bmode = 1;
}

void bholdEvent() {
  Serial.println("b hold");
  setAll(0, 0, 0);
  bmode = 100;
}

//----------------------------------------------------

// Events to trigger by all button input

void aclickEvent() {
  Serial.println("a click");
  if (amode == 0) {
    setAll(0, 0, 0);
    tmode = 0;
    bmode = 0;
    amode = 1;
  } else {
    amode = 0;
    setAll(0, 0, 0);
    showStrip();
  }
}

void adoubleClickEvent() {
  Serial.println("a doubleclick");
  if (amode == 0) {
    return;
  }
  setAll(0, 0, 0);
  amode = (amode + 1) % 5;
  if (amode == 0) amode = 1;
}

void aholdEvent() {
  Serial.println("a hold");
  setAll(0, 0, 0);
  amode = 100;
}

//=================================================================================
// <<button methods>>

void buttonInput() {
  int t = tcheckButton();
  int b = bcheckButton();
  int a = acheckButton();
  if (t == 1) tclickEvent();
  if (t == 2) tdoubleClickEvent();
  if (t == 3) tholdEvent();
  if (b == 1) bclickEvent();
  if (b == 2) bdoubleClickEvent();
  if (b == 3) bholdEvent();
  if (a == 1) aclickEvent();
  if (a == 2) adoubleClickEvent();
  if (a == 3) aholdEvent();
}

//table button

int tcheckButton()
{
  int event = 0;
  // Read the state of the button
  buttonVal_trex = digitalRead(BUTTONPIN_T);
  // Button pressed down
  if (buttonVal_trex == LOW && buttonLast_trex == HIGH && (millis() - upTime_trex) > rebound) {
    downTime_trex = millis();
    ignoreUp_trex = false;
    waitForUp_trex = false;
    singleOK_trex = true;
    holdEventPast_trex = false;
    if ((millis() - upTime_trex) < DCgap && DConUp_trex == false && DCwaiting_trex == true) DConUp_trex = true;
    else DConUp_trex = false;
    DCwaiting_trex = false;
  }
  // Button released
  else if (buttonVal_trex == HIGH && buttonLast_trex == LOW && (millis() - downTime_trex) > rebound) {
    if (not ignoreUp_trex) {
      upTime_trex = millis();
      if (DConUp_trex == false) DCwaiting_trex = true;
      else {
        event = 2;
        DConUp_trex = false;
        DCwaiting_trex = false;
        singleOK_trex = false;
      }
    }
  }
  // Test for normal click event: DCgap expired
  if ( buttonVal_trex == HIGH && (millis() - upTime_trex) >= DCgap && DCwaiting_trex == true && DConUp_trex == false && singleOK_trex == true) {
    event = 1;
    DCwaiting_trex = false;
  }
  // Test for hold
  if (buttonVal_trex == LOW && (millis() - downTime_trex) >= holdTime) {
    // Trigger "normal" hold
    if (not holdEventPast_trex) {
      event = 3;
      waitForUp_trex = true;
      ignoreUp_trex = true;
      DConUp_trex = false;
      DCwaiting_trex = false;
      //downTime = millis();
      holdEventPast_trex = true;
    }
  }
  buttonLast_trex = buttonVal_trex;
  return event;
}

//bed button

int bcheckButton()
{
  int event = 0;
  // Read the state of the button
  buttonVal_brex = digitalRead(BUTTONPIN_B);
  // Button pressed down
  if (buttonVal_brex == LOW && buttonLast_brex == HIGH && (millis() - upTime_brex) > rebound) {
    downTime_brex = millis();
    ignoreUp_brex = false;
    waitForUp_brex = false;
    singleOK_brex = true;
    holdEventPast_brex = false;
    if ((millis() - upTime_brex) < DCgap && DConUp_brex == false && DCwaiting_brex == true) DConUp_brex = true;
    else DConUp_brex = false;
    DCwaiting_brex = false;
  }
  // Button released
  else if (buttonVal_brex == HIGH && buttonLast_brex == LOW && (millis() - downTime_brex) > rebound) {
    if (not ignoreUp_brex) {
      upTime_brex = millis();
      if (DConUp_brex == false) DCwaiting_brex = true;
      else {
        event = 2;
        DConUp_brex = false;
        DCwaiting_brex = false;
        singleOK_brex = false;
      }
    }
  }
  // Test for normal click event: DCgap expired
  if ( buttonVal_brex == HIGH && (millis() - upTime_brex) >= DCgap && DCwaiting_brex == true && DConUp_brex == false && singleOK_brex == true) {
    event = 1;
    DCwaiting_brex = false;
  }
  // Test for hold
  if (buttonVal_brex == LOW && (millis() - downTime_brex) >= holdTime) {
    // Trigger "normal" hold
    if (not holdEventPast_brex) {
      event = 3;
      waitForUp_brex = true;
      ignoreUp_brex = true;
      DConUp_brex = false;
      DCwaiting_brex = false;
      //downTime = millis();
      holdEventPast_brex = true;
    }
  }
  buttonLast_brex = buttonVal_brex;
  return event;
}

//all button
int acheckButton()
{
  int event = 0;
  // Read the state of the button
  buttonVal_arex = digitalRead(BUTTONPIN_A);
  // Button pressed down
  if (buttonVal_arex == LOW && buttonLast_arex == HIGH && (millis() - upTime_arex) > rebound) {
    downTime_arex = millis();
    ignoreUp_arex = false;
    waitForUp_arex = false;
    singleOK_arex = true;
    holdEventPast_arex = false;
    if ((millis() - upTime_arex) < DCgap && DConUp_arex == false && DCwaiting_arex == true) DConUp_arex = true;
    else DConUp_arex = false;
    DCwaiting_arex = false;
  }
  // Button released
  else if (buttonVal_arex == HIGH && buttonLast_arex == LOW && (millis() - downTime_arex) > rebound) {
    if (not ignoreUp_arex) {
      upTime_arex = millis();
      if (DConUp_arex == false) DCwaiting_arex = true;
      else {
        event = 2;
        DConUp_arex = false;
        DCwaiting_arex = false;
        singleOK_arex = false;
      }
    }
  }
  // Test for normal click event: DCgap expired
  if ( buttonVal_arex == HIGH && (millis() - upTime_arex) >= DCgap && DCwaiting_arex == true && DConUp_arex == false && singleOK_arex == true) {
    event = 1;
    DCwaiting_arex = false;
  }
  // Test for hold
  if (buttonVal_arex == LOW && (millis() - downTime_arex) >= holdTime) {
    // Trigger "normal" hold
    if (not holdEventPast_arex) {
      event = 3;
      waitForUp_arex = true;
      ignoreUp_arex = true;
      DConUp_arex = false;
      DCwaiting_arex = false;
      //downTime = millis();
      holdEventPast_arex = true;
    }
  }
  buttonLast_arex = buttonVal_arex;
  return event;
}
