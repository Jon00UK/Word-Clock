/*
  This code requires the following libraries:
  - RTClib https://github.com/adafruit/RTClib
  - Adafruit_GFX https://github.com/adafruit/Adafruit-GFX-Library
  - Adafruit_NeoPixel https://github.com/adafruit/Adafruit_NeoPixel
  - Adafruit_NeoMatrix https://github.com/adafruit/Adafruit_NeoMatrix
  grid pattern
   I T S H I S T H A L F B R T E N
   F L Q U A R T E R S T W E N T Y
   F I V E F E M I N U T E S S T O
   W P A S T C O N E S T H R E E W
   S T W O V F O U R G H E I G H T
   E L E V E N O T E N T I F I V E
   K N I N E R V S E V E N E S I X
   T W E L V E T S O R C L O C K S
*/

// include the library code:
#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>


// define pins
#define NEOPIN 6  // connect to DIN on NeoMatrix 8x8 via a resistor
#define RTCGND A2 // use this as  ground
#define RTCPWR A3 // use this as  power

RTC_DS1307 RTC; // Establish clock object
DateTime theTime; // Holds current clock time

int dst; // holds Europe/UK DST flag (1 = DST, 0 = Off)

// *** SETTINGS 1 ****

int dstMode = 0; // 0=Auto DST (EU/UK), 1 = Auto DST (USA/Canada), 2 = DST Disabled, 3 = DST Enabled

int updateTime = 0; // Set to 1 if you wish to reset your RTC time (from PC) to be compatible with this sketch, compile/upload, then set to 0 and compile/upload again.

// ******************


// configure for 8x8 neopixel matrix
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, NEOPIN,
                            NEO_MATRIX_TOP  + NEO_MATRIX_LEFT +
                            NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
                            NEO_GRB         + NEO_KHZ800);


//array to store if LEDs are lit or not
int ledMatrix[8][8] = {
  {1, 1, 1, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}
};

int ledCount = 64;  // the 8x8 grid of the Neomatrix

void setup() {
  // put your setup code here, to run once:

  // used to manually adjust the RTC via buttons in =/- 2 seconds during first setup
  pinMode(10, INPUT_PULLUP);
  pinMode(11, INPUT_PULLUP);

  //Serial for debugging
  Serial.begin(9600);

  // set pinmodes
  pinMode(NEOPIN, OUTPUT);

  // start clock
  Wire.begin();  // Begin I2C
  RTC.begin();   // begin clock

  if (! RTC.isrunning()) updateTime = 1;
  
  if (updateTime == 1) {
  Serial.println("Setting RTC from PC ...");
  RTC.adjust(DateTime(__DATE__, __TIME__));
    dst = dstFLAG(dstMode);
    theTime = RTC.now();
    if (dst == 1) {
      Serial.println("DST in operation - setting RTC back by one hour");
      theTime = theTime.unixtime() - 3600;
      RTC.adjust(DateTime(theTime.year(), theTime.month(), theTime.day(), theTime.hour(), theTime.minute(), theTime.second()));
      theTime = RTC.now();
      displayTime();
    }
  }

  matrix.begin();

  // Test matrix (all on)

  // Set all pixels to white
  for (int i = 0; i < 8; i++) {
  for (int j = 0; j < 8; j++) {
      matrix.drawPixel(i, j, matrix.Color(255, 255, 255));
    }
  }
  matrix.setBrightness(20);
  matrix.show();
  delay(5000);



  matrix.setBrightness(100);
  matrix.fillScreen(0); // Initialize all pixels to 'off'
  matrix.show();



}

void loop() {
  // put your main code here, to run repeatedly:

  theTime = RTC.now();
  dst = dstFLAG(dstMode);
  if (dst == 1) theTime = theTime.unixtime() + 3600;

  // connect NANO D10 via switch to ground to adjust RTC by + 2 seconds on each poll. Keep switch pressed until desired time correction is shown via serial print!
  if (digitalRead(10) == LOW) {
    Serial.println("RTC + 2 Sec");
    theTime = theTime.unixtime() + 2;
    if (dst == 1) theTime = theTime.unixtime() - 3600; // Needed to keep RTC on standard time
    RTC.adjust(DateTime(theTime.year(), theTime.month(), theTime.day(), theTime.hour(), theTime.minute(), theTime.second()));
  }

  // connect NANO D11 via switch to ground to adjust RTC by - 2 seconds on each poll. Keep switch pressed until desired time correction is shown via serial print!
  if (digitalRead(11) == LOW) {
    Serial.println("RTC - 2 Sec");
    theTime = theTime.unixtime() - 2;
    if (dst == 1) theTime = theTime.unixtime() - 3600; // Needed to keep RTC on standard time
    RTC.adjust(DateTime(theTime.year(), theTime.month(), theTime.day(), theTime.hour(), theTime.minute(), theTime.second()));
  }

  // *** SETTINGS 2 ***

  //Display Minutes LEDs.
  int m = 1;  // 0=OFF, 1=ON

  //Display o'clock LEDs on the hour.
  int oc = 1;  // 0=OFF, 1=ON

  //Display a different color on 'it is' for each minute between clock change (learn the colors and you can tell the time to the minute).
  int mc = 1;  // 0=OFF, 1=ON

  //Display AM & PM in a different color.
  int ampmColor = 1; // 0=OFF, 1=ON

  // *******************

  //write 'it is' everytime:
  ledOn(0, 0);
  ledOn(0, 1);
  // ledOn(0, 2);


  if ((theTime.minute() > 4) && (theTime.minute() < 10)) {
    ledOn(2, 0);
    ledOn(2, 1);
    ledOn(2, 2);
    Serial.print("mfive ");
    minutes(m);
  }
  if ((theTime.minute() > 9) && (theTime.minute() < 15)) {
    ledOn(0, 6);
    ledOn(0, 7);
    Serial.print("mten ");
    minutes(m);
  }
  if ((theTime.minute() > 14) && (theTime.minute() < 20)) {
    ledOn(1, 0);
    ledOn(1, 1);
    ledOn(1, 2);
    ledOn(1, 3);
    Serial.print("mquarter ");
  }
  if ((theTime.minute() > 19) && (theTime.minute() < 25)) {
    ledOn(1, 4);
    ledOn(1, 5);
    ledOn(1, 6);
    ledOn(1, 7);
    Serial.print("mtwenty ");
    minutes(m);
  }
  if ((theTime.minute() > 24) && (theTime.minute() < 30)) {
    ledOn(1, 4);
    ledOn(1, 5);
    ledOn(1, 6);
    ledOn(1, 7);
    ledOn(2, 0);
    ledOn(2, 1);
    ledOn(2, 2);
    Serial.print("mtwenty five ");
    minutes(m);
  }
  if ((theTime.minute() > 29) && (theTime.minute() < 35)) {
    ledOn(0, 3);
    ledOn(0, 4);
    ledOn(0, 5);
    Serial.print("mhalf ");
  }
  if ((theTime.minute() > 34) && (theTime.minute() < 40)) {
    ledOn(1, 4);
    ledOn(1, 5);
    ledOn(1, 6);
    ledOn(1, 7);
    ledOn(2, 0);
    ledOn(2, 1);
    ledOn(2, 2);
    Serial.print("mtwenty five ");
    minutes(m);
  }
  if ((theTime.minute() > 39) && (theTime.minute() < 45)) {
    ledOn(1, 4);
    ledOn(1, 5);
    ledOn(1, 6);
    ledOn(1, 7);
    Serial.print("mtwenty ");
    minutes(m);
  }
  if ((theTime.minute() > 44) && (theTime.minute() < 50)) {
    ledOn(1, 0);
    ledOn(1, 1);
    ledOn(1, 2);
    ledOn(1, 3);
    Serial.print("mquarter ");
  }
  if ((theTime.minute() > 49) && (theTime.minute() < 55)) {
    ledOn(0, 6);
    ledOn(0, 7);
    Serial.print("mten");
    minutes(m);
  }
  if (theTime.minute() > 54) {
    ledOn(2, 0);
    ledOn(2, 1);
    ledOn(2, 2);
    Serial.print("mfive ");
    minutes(m);
  }


  if ((theTime.minute() < 5))
  {
    switch (theTime.hour()) {
      case 1:
      case 13:
        ledOn(3, 3);
        ledOn(3, 4);
        oclock(oc);
        Serial.print("hone ");
        break;
      case 2:
      case 14:
        ledOn(4, 0);
        ledOn(4, 1);
        oclock(oc);
        Serial.print("htwo ");
        break;
      case 3:
      case 15:
        ledOn(3, 6);
        ledOn(3, 7);
        oclock(oc);
        Serial.print("hthree ");
        break;
      case 4:
      case 16:
        ledOn(4, 2);
        ledOn(4, 3);
        ledOn(4, 4);
        oclock(oc);
        Serial.print("hfour ");
        break;
      case 5:
      case 17:
        ledOn(5, 6);
        ledOn(5, 7);
        oclock(oc);
        Serial.print("hfive ");
        break;
      case 6:
      case 18:
        ledOn(6, 6);
        ledOn(6, 7);
        oclock(oc);
        Serial.print("hsix ");
        break;
      case 7:
      case 19:
        ledOn(6, 3);
        ledOn(6, 4);
        ledOn(6, 5);
        oclock(oc);
        Serial.print("hseven ");
        break;
      case 8:
      case 20:
        ledOn(4, 5);
        ledOn(4, 6);
        ledOn(4, 7);
        oclock(oc);
        Serial.print("height ");
        break;
      case 9:
      case 21:
        ledOn(6, 0);
        ledOn(6, 1);
        ledOn(6, 2);
        oclock(oc);
        Serial.print("hnine ");
        break;
      case 10:
      case 22:
        ledOn(5, 4);
        ledOn(5, 5);
        oclock(oc);
        Serial.print("hten ");
        break;
      case 11:
      case 23:
        ledOn(5, 0);
        ledOn(5, 1);
        ledOn(5, 2);
        ledOn(5, 3);
        oclock(oc);
        Serial.print("heleven ");
        break;
      case 0:
      case 12:
        ledOn(7, 0);
        ledOn(7, 1);
        ledOn(7, 2);
        ledOn(7, 3);
        oclock(oc);
        Serial.print("htwelve ");
        break;
    }

  }
  else if ((theTime.minute() < 35) && (theTime.minute() > 4))
  {
    ledOn(3, 0);
    ledOn(3, 1);
    ledOn(3, 2);
    Serial.print("past");
    switch (theTime.hour()) {
      case 1:
      case 13:
        ledOn(3, 3);
        ledOn(3, 4);
        Serial.print("one ");
        break;
      case 2:
      case 14:
        ledOn(4, 0);
        ledOn(4, 1);
        Serial.print("two ");
        break;
      case 3:
      case 15:
        ledOn(3, 7);
        ledOn(3, 6);
        Serial.print("three ");
        break;
      case 4:
      case 16:
        ledOn(4, 2);
        ledOn(4, 3);
        ledOn(4, 4);
        Serial.print("four ");
        break;
      case 5:
      case 17:
        ledOn(5, 6);
        ledOn(5, 7);
        Serial.print("five ");
        break;
      case 6:
      case 18:
        ledOn(6, 6);
        ledOn(6, 7);
        Serial.print("six ");
        break;
      case 7:
      case 19:
        ledOn(6, 3);
        ledOn(6, 4);
        ledOn(6, 5);
        Serial.print("seven ");
        break;
      case 8:
      case 20:
        ledOn(4, 5);
        ledOn(4, 6);
        ledOn(4, 7);
        Serial.print("eight ");
        break;
      case 9:
      case 21:
        ledOn(6, 0);
        ledOn(6, 1);
        ledOn(6, 2);
        Serial.print("nine ");
        break;
      case 10:
      case 22:
        ledOn(5, 4);
        ledOn(5, 5);
        Serial.print("ten ");
        break;
      case 11:
      case 23:
        ledOn(5, 0);
        ledOn(5, 1);
        ledOn(5, 2);
        ledOn(5, 3);
        Serial.print("eleven ");
        break;
      case 0:
      case 12:
        ledOn(7, 0);
        ledOn(7, 1);
        ledOn(7, 2);
        ledOn(7, 3);
        Serial.print("twelve ");
        break;
    }
  }
  else
  {
    // if we are greater than 34 minutes past the hour then display
    // the next hour, as we will be displaying a 'to' sign
    ledOn(2, 6);
    ledOn(2, 7);
    Serial.print("to ");
    //Serial.print(" to ");
    switch (theTime.hour()) {
      case 1:
      case 13:
        ledOn(4, 0);
        ledOn(4, 1);
        Serial.print("two ");
        break;
      case 14:
      case 2:
        ledOn(3, 6);
        ledOn(3, 7);
        Serial.print("three ");
        break;
      case 15:
      case 3:
        ledOn(4, 2);
        ledOn(4, 3);
        ledOn(4, 4);
        Serial.print("four ");
        break;
      case 4:
      case 16:
        ledOn(5, 6);
        ledOn(5, 7);
        Serial.print("five ");
        break;
      case 5:
      case 17:
        ledOn(6, 6);
        ledOn(6, 7);
        Serial.print("six ");
        break;
      case 6:
      case 18:
        ledOn(6, 3);
        ledOn(6, 4);
        ledOn(6, 5);
        Serial.print("seven ");
        break;
      case 7:
      case 19:
        ledOn(4, 5);
        ledOn(4, 6);
        ledOn(4, 7);
        Serial.print("eight ");
        break;
      case 8:
      case 20:
        ledOn(6, 0);
        ledOn(6, 1);
        ledOn(6, 2);
        Serial.print("nine ");
        break;
      case 9:
      case 21:
        ledOn(5, 4);
        ledOn(5, 5);
        Serial.print("ten ");
        break;
      case 10:
      case 22:
        ledOn(5, 0);
        ledOn(5, 1);
        ledOn(5, 2);
        ledOn(5, 3);
        Serial.print("eleven ");
        break;
      case 11:
      case 23:
        ledOn(7, 0);
        ledOn(7, 1);
        ledOn(7, 2);
        ledOn(7, 3);
        Serial.print("twelve ");
        break;
      case 0:
      case 12:
        ledOn(3, 3);
        ledOn(3, 4);
        Serial.print("one ");
        break;
    }
  }

  //=======================================================
  //=======================================================


  //it's show time....
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      if (ledMatrix[i][j] == 1) {

        // Set normal color
        matrix.drawPixel(i, j, matrix.Color(255, 255, 0));

        if (ampmColor = 1) {
          // Serial.println("AMPM flag set");
          if (theTime.hour() < 12) {
            // Set color for AM
            //Serial.println("AMPM AM");
            matrix.drawPixel(i, j, matrix.Color(255, 255, 0));
          }
          else {
            // Set color for PM
            //Serial.println("AMPM PM");
            matrix.drawPixel(i, j, matrix.Color(20, 80, 200));
          }

        }

      }
      else {
        // Set black (off)
        matrix.drawPixel(i, j, matrix.Color(0, 0, 0));
      }
    }
    Serial.println();
  }

  //====================================================

  itisColor(mc);
  displayTime();
  checkArray();
  matrix.show();

  delay(2000);
  resetArray();

}

void itisColor(int status) {
  // Defines the color for 'IT IS' per minute

  if (status == 1) {
    switch (theTime.minute()) {
      case 1: case 6: case 11: case 16: case 21: case 26:
      case 31: case 36: case 41: case 46: case 51: case 56:
        // Minute 1 color

        matrix.drawPixel(0, 0, matrix.Color(255, 0, 0));
        matrix.drawPixel(0, 1, matrix.Color(255, 0, 0));
        //matrix.drawPixel(0, 2, matrix.Color(255, 0, 0));
        Serial.println("'it is' showing color 1 ");
        break;

      case 2: case 7: case 12: case 17: case 22: case 27:
      case 32: case 37: case 42: case 47: case 52: case 57:
        // Minute 2 color

        matrix.drawPixel(0, 0, matrix.Color(0, 255, 0));
        matrix.drawPixel(0, 1, matrix.Color(0, 255, 0));
        //matrix.drawPixel(0, 2, matrix.Color(0, 255, 0));
        Serial.println("'it is' showing color 2 ");
        break;

      case 3: case 8: case 13: case 18: case 23: case 28:
      case 33: case 38: case 43: case 48: case 53: case 58:
        // Minute 3 color

        matrix.drawPixel(0, 0, matrix.Color(255, 0, 255));
        matrix.drawPixel(0, 1, matrix.Color(255, 0, 255));
        //matrix.drawPixel(0, 2, matrix.Color(255, 0, 255));
        Serial.println("'it is' showing color 3 ");
        break;

      case 4: case 9: case 14: case 19: case 24: case 29:
      case 34: case 39: case 44: case 49: case 54: case 59:
        // Minute 4 color

        matrix.drawPixel(0, 0, matrix.Color(0, 255, 255));
        matrix.drawPixel(0, 1, matrix.Color(0, 255, 255));
        //matrix.drawPixel(0, 2, matrix.Color(0, 255, 255));
        Serial.println("'it is' showing color 4 ");
        break;
    }
  }
}

void checkArray() {
  //print out the array to check it:
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      Serial.print(ledMatrix[i][j]);
      Serial.print(",");
    }
    Serial.println();
  }
  Serial.println();
}


void resetArray() {
  //set all the array values to 0 (off):
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      ledMatrix[i][j] = 0;
    }
  }
  Serial.print("Array reset (All LEDs switched set to off)");
  Serial.println();
  Serial.println();
}

void displayTime() {

  // get time from the RTC
  DateTime theTime = RTC.now();
  if (dst == 1) {
    Serial.println("DST is in operation");
    theTime = theTime.unixtime() + 3600;
  }
  // serial print current time

  Serial.print(theTime.year(), DEC);
  Serial.print('/');
  Serial.print(theTime.month(), DEC);
  Serial.print('/');
  Serial.print(theTime.day(), DEC);
  Serial.print(' ');
  Serial.print(theTime.hour(), DEC);
  Serial.print(':');
  Serial.print(theTime.minute(), DEC);
  Serial.print(':');
  Serial.print(theTime.second(), DEC);
  Serial.println();
}

void ledOn(int x, int y) {
  //Function to make it easier & faster to edit matrix LED toggling.
  //Example: ledOn(5,3); instead of ledMatrix[5][3] = 1;
  ledMatrix[x][y] = 1;
}

void oclock(int status) {
  //Function to light up 'orclock'. Use tape on the letter grid to cover up most of the R letter to form a ' character so it displays o'clock
  if (status == 0) {
    Serial.print("");
  }
  if (status == 1) {
    ledOn(7, 4);
    ledOn(7, 5);
    ledOn(7, 6);
    ledOn(7, 7);
    Serial.print("o'clock ");
  }
}


void minutes(int status) {
  if (status == 0) {
    Serial.print("");
  }
  if (status == 1) {
    ledOn(2, 3);
    ledOn(2, 4);
    ledOn(2, 5);
    Serial.print("minutes ");
  }
}

int dstFLAG(int status) {
  //check for dst (EU/UK)
  DateTime rtcTime = RTC.now();
  int dst = 0;

  switch (status) {
    case 0:
      if (rtcTime.month() < 3 || rtcTime.month() > 10) dst = 0;
      if (rtcTime.month() > 3 && rtcTime.month() < 10) dst = 1;
      if (rtcTime.month() == 3 && rtcTime.day() < 25) dst = 0;
      if (rtcTime.month() == 10 && rtcTime.day() < 25) dst = 1;
      if (rtcTime.month() == 3 && (rtcTime.hour() + 24 * rtcTime.day()) >= (1 + 24 * (31 - (5 * rtcTime.year() / 4 + 4 % 7))) || rtcTime.month() == 10 && (rtcTime.hour() + 24 * rtcTime.day()) < (1 + 24 * (31 - (5 * rtcTime.year() / 4 + 1) % 7 ))) dst = 1;
      break;
    case 1:
      int y = rtcTime.year() - 2000;
      int x = (y + y / 4 + 2) % 7;

      if (rtcTime.month() == 3 && rtcTime.day() == (14 - x) && rtcTime.hour() >= 2) dst = 1;
      if (rtcTime.month() == 3 && rtcTime.day() > (14 - x) || rtcTime.month() > 3) dst = 1;
      if (rtcTime.month() == 11 && rtcTime.day() == (7 - x) && rtcTime.hour() >= 2) dst = 0;
      if (rtcTime.month() == 11 && rtcTime.day() > (7 - x) || rtcTime.month() > 11 || rtcTime.month() < 3) dst = 0;
      break;
    case 2:
      dst = 0;
      break;

    case 3:
      dst = 1;
      break;
  }

  return dst; // 1 = dst otherwise 0


}
