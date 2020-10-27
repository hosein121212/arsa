#ifndef arsalearn_v1_h
#define arsalearn_v1_h

#include <inttypes.h>
#include "Print.h" 
#include <Wire.h>



#include <analogWrite.h>

#define led  20
#define low  0
#define LOW  0
#define high  255
#define HIGH  255
// #define on  255
#define ON  255
// #define off  0
#define r    330
#define f    440
#define u    550
#define d    660
#define OFF  0

#define key  0
#define key1  32
#define key2  4
#define key3  15
#define key4  14
#define key5  12
#define key6  33


#define T1  33
#define T2  4
#define T3  15
#define T4  14
#define T5  12
#define T6  32


#define t1  33
#define t2  4
#define t3  15
#define t4  14
#define t5  12
#define t6  32

#define led1  211
#define led2  212
#define led3  213
#define led4  214
#define led5  215
#define led6  216


#define D1  21
#define D2  22
#define D3  23
#define D4  24
#define D5  25
#define D6  26

#define A1  211
#define A2  212
#define A3  213
#define A4  214
#define A5  215
#define A6  216
//==========================================

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define En B00000100  // Enable bit
#define Rw B00000010  // Read/Write bit
#define Rs B00000001  // Register select bit

int pin (int pin_number , int Value);
int pin (int pin_number);
int pin();
// boolean press(int a, int b);
boolean press(int a, int b=1500);

void run();
class lcd_i2c : public Print {
public:
  lcd_i2c(uint8_t lcd_Addr,uint8_t lcd_cols,uint8_t lcd_rows);
  void begin(uint8_t cols, uint8_t rows, uint8_t charsize = LCD_5x8DOTS );
  void clear();
  void home();
  void noDisplay();
  void display();
  void noBlink();
  void blink();
  void noCursor();
  void cursor();
  void scrollDisplayLeft();
  void scrollDisplayRight();
  void printLeft();
  void printRight();
  void leftToRight();
  void rightToLeft();
  void shiftIncrement();
  void shiftDecrement();
  void noBacklight();
  void backlight();
  void autoscroll();
  void noAutoscroll(); 
  void createChar(uint8_t, uint8_t[]);
  void setCursor(uint8_t, uint8_t); 
#if defined(ARDUINO) && ARDUINO >= 100
  virtual size_t write(uint8_t);
#else
  virtual void write(uint8_t);
#endif
  void command(uint8_t);
  void init();

////compatibility API function aliases
void blink_on();						// alias for blink()
void blink_off();       					// alias for noBlink()
void cursor_on();      	 					// alias for cursor()
void cursor_off();      					// alias for noCursor()
void setBacklight(uint8_t new_val);				// alias for backlight() and nobacklight()
void load_custom_character(uint8_t char_num, uint8_t *rows);	// alias for createChar()
void printstr(const char[]);

////Unsupported API functions (not implemented in this library)
uint8_t status();
void setContrast(uint8_t new_val);
uint8_t keypad();
void setDelay(int,int);
void on();
void off();
uint8_t init_bargraph(uint8_t graphtype);
void draw_horizontal_graph(uint8_t row, uint8_t column, uint8_t len,  uint8_t pixel_col_end);
void draw_vertical_graph(uint8_t row, uint8_t column, uint8_t len,  uint8_t pixel_col_end);
	 

private:
  void init_priv();
  void send(uint8_t, uint8_t);
  void write4bits(uint8_t);
  void expanderWrite(uint8_t);
  void pulseEnable(uint8_t);
  uint8_t _Addr;
  uint8_t _displayfunction;
  uint8_t _displaycontrol;
  uint8_t _displaymode;
  uint8_t _numlines;
  uint8_t _cols;
  uint8_t _rows;
  uint8_t _backlightval;
 
	// long duration, distanceCm, distanceIn;

};

#endif
//!================pir=====================
// class pir
// {
// public:
//   pir(int mode);
//   bool read();

// private:
//   int _pin;
// };

// //!===================ultrasonic=======================

// class ultrasonic {
	
// 	public:
// 		ultrasonic(int mode);
// 		int getCm();
// 		int getIn();
// 	private:
// 		int _trig;
// 		int _echo;
// };

// //!==========================hall============================
// class hall
// {
// public:
//   hall(int mode);
//   bool read();

// private:
//   int _pin;
// };

// //!==========================hmy============================
// class hmy
// {
// public:
//   hmy(int mode);
//   int read(char a);

// private:
//   int _pin;
//   char _a;
// };


// //!========================bred bord==============================
// // class bred
// // {
// //   public:
// //  bred();
// // // int pin();
// // // int pin (char pin_number);
// // int pin (char pin_number , char Value);

// //   private:
// // int _pin_number;
// // int _Value;

// // };

// //!=========================buzzer=============================
// class buzzer
// {
// public:
//   buzzer(int mode);
//   void write(char a);

// private:
//   int _pin;
//   char _a;
//   char _b;
// };
// //!=========================relay=============================
// class relay
// {
// public:
//   relay(int mode);
//   void write(char a , char b);

// private:
//   int _pin;
//   int _pinn;
//   char _a;
//   char _b;
// };

//!======================================================


//!======================================================