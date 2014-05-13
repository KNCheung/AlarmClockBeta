#include <dht22.h>
#include <DS3231.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <IRremoteTools.h>
#include <pt.h>
#include <pt_timer.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
DS3231 clock;
dht22 DHT22;
PT_TIMER pt_t_time, pt_t_temp, pt_t_tick;
static struct pt pt_time, pt_temp, pt_ir, pt_tick;
#define DHT22PIN 2

#define DIN  5
#define nCS  6
#define CLK  7

uint16_t Y, M, D, h, m, s;
bool h12;
bool C = false;
bool PM;

byte Celsius[8] = {
  B11000,
  B11000,
  B00111,
  B01000,
  B01000,
  B01000,
  B00111,
};

void pushMAX7219(unsigned int x)
{
  digitalWrite(nCS, LOW);
  shiftOut(DIN, CLK, MSBFIRST, (x >> 8 ) % 256);
  shiftOut(DIN, CLK, MSBFIRST, (x % 256));
  digitalWrite(nCS, HIGH);
}

static int updateTime(struct pt *pt)
{
  PT_BEGIN(pt);
  while (1)
  {
    Y = clock.getYear();
    M = clock.getMonth(C);
    D = clock.getDate();
    h = clock.getHour(h12, PM);
    m = clock.getMinute();
    s = clock.getSecond();
    lcd.setCursor(0, 0);
    lcd.print(20);
    lcd.print(Y); lcd.print("-");
    if (M < 10) lcd.print("0");
    lcd.print(M); lcd.print("-");
    if (D < 10) lcd.print("0");
    lcd.print(D); lcd.print(" ");
    if (h < 10) lcd.print("0");
    lcd.print(h); lcd.print(":");
    if (m < 10) lcd.print("0");
    lcd.print(m);
    pushMAX7219(0x0100 | (h / 10));
    pushMAX7219(0x0280 | (h % 10));
    pushMAX7219(0x0300 | (m / 10));
    pushMAX7219(0x0400 | (m % 10));
    PT_TIMER_DELAY(pt, pt_t_time, (60 - s) * 1000);
  }
  PT_END(pt);
}

static int updateTemp(struct pt *pt)
{
  PT_BEGIN(pt);
  while (1)
  {
    digitalWrite(13, HIGH);
    lcd.setCursor(0, 1);
    if (DHT22.read(DHT22PIN) == DHTLIB_OK)
    {
      lcd.print("T:");
      lcd.print(DHT22.temperature, 1);
      lcd.print((char)0);
      lcd.print("  H:");
      lcd.print(DHT22.humidity, 1);
      lcd.print("%");
    } else {
      lcd.print("  DHT22 ERROR   ");
    }
    digitalWrite(13, LOW);
    PT_TIMER_DELAY(pt, pt_t_temp, (60 - s) * 1000);
  }
  PT_END(pt);
}

static int tick(struct pt *pt)
{
  PT_BEGIN(pt);
  s = clock.getSecond();
  while (s == clock.getSecond())
    delay(10);
  while (1)
  {
    pushMAX7219(0x0280 | (h % 10));
    PT_TIMER_DELAY(pt, pt_t_tick, 500);
    pushMAX7219(0x0200 | (h % 10));
    PT_TIMER_DELAY(pt, pt_t_tick, 500);
  }
  PT_END(pt);
}

static int ir(struct pt *pt)
{
  static bool s_reg=0,s_lcd=0;
  PT_BEGIN(pt);
  while (1)
  {
    PT_WAIT_UNTIL(pt,IRrecived());
    switch ((getIRresult()>>8)%256)
    {
      case 0xA2: 
      if (s_lcd) 
      lcd.backlight();
      else
      lcd.noBacklight(); 
      s_lcd=!s_lcd;
      break;
      case 0x62: 
      if (s_reg)
      pushMAX7219(0x0C01);
      else
      pushMAX7219(0x0C00); 
      s_reg=!s_reg;
      break;
      default: break;
    }
    resumeIRremote();
  }
  PT_END(pt);
}

void setup()
{
  pinMode(DIN, OUTPUT);
  pinMode(nCS, OUTPUT);
  pinMode(CLK, OUTPUT);
  lcd.init();
  pushMAX7219(0x0C01);
  pinMode(10, OUTPUT);
  //  SD.begin(SDCS);
  lcd.createChar(0, Celsius);
  lcd.setCursor(0, 0);
  lcd.print("BOOTING...");
  delay(2000);
  lcd.backlight();
  pinMode(13, OUTPUT);
  if (clock.getSecond() == 0)
    delay(1000);
  pushMAX7219(0x0B0F);
  pushMAX7219(0x0A03);
  pushMAX7219(0x090F);
  beginIRremote(); 
  PT_INIT(&pt_time);
  PT_INIT(&pt_temp);
  PT_INIT(&pt_tick);
  PT_INIT(&pt_ir);
}

void loop()
{
  tick(&pt_tick);
  updateTemp(&pt_temp);
  updateTime(&pt_time);
  ir(&pt_ir);
}
