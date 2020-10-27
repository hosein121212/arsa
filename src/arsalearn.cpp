//#include "Arduino.h"
#include "arsalearn_v1.h"
// Based on the work by DFRobot

//#include "lcd_i2c.h"
#include <inttypes.h>

#include <analogWrite.h>
#if defined(ARDUINO) && ARDUINO >= 100

#include "Arduino.h"

#define printIIC(args)	Wire.write(args)
inline size_t lcd_i2c::write(uint8_t value) {
	send(value, Rs);
	return 1;
}

#else
#include "WProgram.h"

#define printIIC(args)	Wire.send(args)
inline void lcd_i2c::write(uint8_t value) {
	send(value, Rs);
}

#endif
#include "Wire.h"
#define CUSTOM_SETTINGS
#define INCLUDE_LEDCONTROL_MODULE

#define led  20
#define low  0
#define LOW  0
#define high  255
#define HIGH  255

#define ON  255
// #define on  255
#define OFF  0
#define f    440
#define r    330
#define u    550
#define d    660

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


// lcd_i2c::lcd_i2c();

// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set: 
//    DL = 1; 8-bit interface data 
//    N = 0; 1-line display 
//    F = 0; 5x8 dot character font 
// 3. Display on/off control: 
//    D = 0; Display off 
//    C = 0; Cursor off 
//    B = 0; Blinking off 
// 4. Entry mode set: 
//    I/D = 1; Increment by 1
//    S = 0; No shift 
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't asAVREe that its in that state when a sketch starts (and the
// Li_quidCrystal constructor is called).

lcd_i2c::lcd_i2c(uint8_t lcd_Addr,uint8_t lcd_cols,uint8_t lcd_rows)
{
  _Addr = lcd_Addr;
  _cols = lcd_cols;
  _rows = lcd_rows;
  _backlightval = LCD_NOBACKLIGHT;
  
 
}

void lcd_i2c::init(){
	init_priv();
}

void lcd_i2c::init_priv()
{
	Wire.begin();
	_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
	begin(_cols, _rows);  
}

void lcd_i2c::begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {
	if (lines > 1) {
		_displayfunction |= LCD_2LINE;
	}
	_numlines = lines;

	// for some 1 line displays you can select a 10 pixel high font
	if ((dotsize != 0) && (lines == 1)) {
		_displayfunction |= LCD_5x10DOTS;
	}

	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
	delay(50); 
  
	// Now we pull both RS and R/W low to begin commands
	expanderWrite(_backlightval);	// reset expanderand turn backlight off (Bit 8 =1)
	delay(1000);

  	//put the LCD into 4 bit mode
	// this is according to the hitachi HD44780 datasheet
	// figure 24, pg 46
	
	  // we start in 8bit mode, try to set 4 bit mode
   write4bits(0x03 << 4);
   delayMicroseconds(4500); // wait min 4.1ms
   
   // second try
   write4bits(0x03 << 4);
   delayMicroseconds(4500); // wait min 4.1ms
   
   // third go!
   write4bits(0x03 << 4); 
   delayMicroseconds(150);
   
   // finally, set to 4-bit interface
   write4bits(0x02 << 4); 


	// set # lines, font size, etc.
	command(LCD_FUNCTIONSET | _displayfunction);  
	
	// turn the display on with no cursor or blinking default
	_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	display();
	
	// clear it off
	clear();
	
	// Initialize to default text direction (for roman languages)
	_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	
	// set the entry mode
	command(LCD_ENTRYMODESET | _displaymode);
	
	home();
  
}

/********** high level commands, for the user! */
void lcd_i2c::clear(){
	command(LCD_CLEARDISPLAY);// clear display, set cursor position to zero
	delayMicroseconds(2000);  // this command takes a long time!
}

void lcd_i2c::home(){
	command(LCD_RETURNHOME);  // set cursor position to zero
	delayMicroseconds(2000);  // this command takes a long time!
}

void lcd_i2c::setCursor(uint8_t col, uint8_t row){
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if ( row > _numlines ) {
		row = _numlines-1;    // we count rows starting w/0
	}
	command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

// Turn the display on/off (_quickly)
void lcd_i2c::noDisplay() {
	_displaycontrol &= ~LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void lcd_i2c::display() {
	_displaycontrol |= LCD_DISPLAYON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turns the underline cursor on/off
void lcd_i2c::noCursor() {
	_displaycontrol &= ~LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void lcd_i2c::cursor() {
	_displaycontrol |= LCD_CURSORON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// Turn on and off the blinking cursor
void lcd_i2c::noBlink() {
	_displaycontrol &= ~LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}
void lcd_i2c::blink() {
	_displaycontrol |= LCD_BLINKON;
	command(LCD_DISPLAYCONTROL | _displaycontrol);
}

// These commands scroll the display without changing the RAM
void lcd_i2c::scrollDisplayLeft(void) {
	command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void lcd_i2c::scrollDisplayRight(void) {
	command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void lcd_i2c::leftToRight(void) {
	_displaymode |= LCD_ENTRYLEFT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This is for text that flows Right to Left
void lcd_i2c::rightToLeft(void) {
	_displaymode &= ~LCD_ENTRYLEFT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'right justify' text from the cursor
void lcd_i2c::autoscroll(void) {
	_displaymode |= LCD_ENTRYSHIFTINCREMENT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// This will 'left justify' text from the cursor
void lcd_i2c::noAutoscroll(void) {
	_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
	command(LCD_ENTRYMODESET | _displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void lcd_i2c::createChar(uint8_t location, uint8_t charmap[]) {
	location &= 0x7; // we only have 8 locations 0-7
	command(LCD_SETCGRAMADDR | (location << 3));
	for (int i=0; i<8; i++) {
		write(charmap[i]);
	}
}

// Turn the (optional) backlight off/on
void lcd_i2c::noBacklight(void) {
	_backlightval=LCD_NOBACKLIGHT;
	expanderWrite(0);
}

void lcd_i2c::backlight(void) {
	_backlightval=LCD_BACKLIGHT;
	expanderWrite(0);
}



/*********** mid level commands, for sending data/cmds */

inline void lcd_i2c::command(uint8_t value) {
	send(value, 0);
}


/************ low level data pushing commands **********/

// write either command or data
void lcd_i2c::send(uint8_t value, uint8_t mode) {
	uint8_t highnib=value&0xf0;
	uint8_t lownib=(value<<4)&0xf0;
       write4bits((highnib)|mode);
	write4bits((lownib)|mode); 
}

void lcd_i2c::write4bits(uint8_t value) {
	expanderWrite(value);
	pulseEnable(value);
}

void lcd_i2c::expanderWrite(uint8_t _data){                                        
	Wire.beginTransmission(_Addr);
	printIIC((int)(_data) | _backlightval);
	Wire.endTransmission();   
}

void lcd_i2c::pulseEnable(uint8_t _data){
	expanderWrite(_data | En);	// En high
	delayMicroseconds(1);		// enable pulse must be >450ns
	
	expanderWrite(_data & ~En);	// En low
	delayMicroseconds(50);		// commands need > 37us to settle
} 


// Alias functions

void lcd_i2c::cursor_on(){
	cursor();
}

void lcd_i2c::cursor_off(){
	noCursor();
}

void lcd_i2c::blink_on(){
	blink();
}

void lcd_i2c::blink_off(){
	noBlink();
}

void lcd_i2c::load_custom_character(uint8_t char_num, uint8_t *rows){
		createChar(char_num, rows);
}

void lcd_i2c::setBacklight(uint8_t new_val){
	if(new_val){
		backlight();		// turn backlight on
	}else{
		noBacklight();		// turn backlight off
	}
}

void lcd_i2c::printstr(const char c[]){
	//This function is not identical to the function used for "real" I2C displays
	//it's here so the user sketch doesn't have to be changed 
	print(c);
}


// unsupported API functions
void lcd_i2c::off(){}
void lcd_i2c::on(){}
void lcd_i2c::setDelay (int cmdDelay,int charDelay) {}
uint8_t lcd_i2c::status(){return 0;}
uint8_t lcd_i2c::keypad (){return 0;}
uint8_t lcd_i2c::init_bargraph(uint8_t graphtype){return 0;}
void lcd_i2c::draw_horizontal_graph(uint8_t row, uint8_t column, uint8_t len,  uint8_t pixel_col_end){}
void lcd_i2c::draw_vertical_graph(uint8_t row, uint8_t column, uint8_t len,  uint8_t pixel_row_end){}
void lcd_i2c::setContrast(uint8_t new_val){}

	
//!================pir=====================
// pir::pir(int mode)
// {
//   pinMode(14,INPUT_PULLUP);
  
//   if (digitalRead(14)==0)
//   {
//    switch (mode)
//    {
//    case 88:
//    _pin =  1;
//      pinMode(_pin, INPUT);
//        break;
       

//        case 22:
//    _pin =  0;
//      pinMode(_pin, INPUT);
//        break;


//        case  33:
//    _pin =  3;
//      pinMode(_pin, INPUT);
//        break;


//        case 44:
//    _pin =  4;
//      pinMode(_pin, INPUT);
//        break;



//        case  55:
//    _pin =  5;
//      pinMode(_pin, INPUT);
//        break;


//        case  66:
//    _pin =  6;
//      pinMode(_pin, INPUT);
//        break;
//        case 70:
//    _pin =  1;
//      pinMode(_pin, INPUT);
//        break;
       

//        case 80:
//    _pin =  0;
//      pinMode(_pin, INPUT);
//        break;


//        case  90:
//    _pin =  3;
//      pinMode(_pin, INPUT);
//        break;


//        case 100:
//    _pin =  4;
//      pinMode(_pin, INPUT);
//        break;



//        case  110:
//    _pin =  5;
//      pinMode(_pin, INPUT);
//        break;


//        case  120:
//    _pin =  6;
//      pinMode(_pin, INPUT);
//        break;
//    }
//   }
// }

// bool pir::read()
// {
//     if (digitalRead(_pin) == LOW)
//     {
//         return 0;
//     }
//     else
//     {
//         return 1;
//     }
// }

// //!======================ultrasonic==========================

// ultrasonic::ultrasonic(int mode){
// pinMode(14,INPUT_PULLUP);
  
//   if (digitalRead(14)==0)
//   {

//    switch (mode)
//    {
//    case  22:
//     _trig=13 ;
// 	_echo=12;
// 	pinMode(_trig,OUTPUT);
// 	pinMode(_echo,INPUT);
//   // Serial.print("AVALY=");
//        break;
       

//        case  11:
//     _trig=3;
// 	_echo=2;
// 	pinMode(_trig,OUTPUT);
// 	pinMode(_echo,INPUT);
//        break;


//        case  33:
//     _trig= 3;
// 	_echo=1;
// 	pinMode(_trig,OUTPUT);
// 	pinMode(_echo,INPUT);
//        break;
//     default:
//   _trig= 3;
// 	_echo=2;
// 	pinMode(_trig,OUTPUT);
// 	pinMode(_echo,INPUT);
//    }
   
//   }
  
// }
// int ultrasonic::getCm(){
// 	long duration, distanceCm, distanceIn;

// 	digitalWrite(_trig, LOW);
// 	delayMicroseconds(2);
// 	digitalWrite(_trig, HIGH);
// 	delayMicroseconds(10);
// 	digitalWrite(_trig, LOW);
// 	duration = pulseIn(_echo,HIGH);

// 	distanceCm = duration / 29.1 / 2 ;

// 	return distanceCm;
// }
// int ultrasonic::getIn(){
// 	long duration, distanceCm, distanceIn;

// 	digitalWrite(_trig, LOW);
// 	delayMicroseconds(2);
// 	digitalWrite(_trig, HIGH);
// 	delayMicroseconds(10);
// 	digitalWrite(_trig, LOW);
// 	duration = pulseIn(_echo,HIGH);

// 	distanceIn = duration / 74 / 2;

// 	return distanceIn;
// }

// //!=============================hall=================================
// hall::hall(int mode)
// {
//   pinMode(14,INPUT_PULLUP);
  
//   if (digitalRead(14)==0)
//   {
//    switch (mode)
//    {
//    case 88:
//    _pin =  1;
//      pinMode(_pin, INPUT);
//        break;
       

//        case 22:
//    _pin =  0;
//      pinMode(_pin, INPUT);
//        break;


//        case  33:
//    _pin =  3;
//      pinMode(_pin, INPUT);
//        break;


//        case 44:
//    _pin =  4;
//      pinMode(_pin, INPUT);
//        break;



//        case  55:
//    _pin =  5;
//      pinMode(_pin, INPUT);
//        break;


//        case  66:
//    _pin =  6;
//      pinMode(_pin, INPUT);
//        break;
//        case 70:
//    _pin =  1;
//      pinMode(_pin, INPUT);
//        break;
       

//        case 80:
//    _pin =  0;
//      pinMode(_pin, INPUT);
//        break;


//        case  90:
//    _pin =  3;
//      pinMode(_pin, INPUT);
//        break;


//        case 100:
//    _pin =  4;
//      pinMode(_pin, INPUT);
//        break;



//        case  110:
//    _pin =  5;
//      pinMode(_pin, INPUT);
//        break;


//        case  120:
//    _pin =  6;
//      pinMode(_pin, INPUT);
//        break;
//    }
//   }
// }

// bool hall::read()
// {
//     if (digitalRead(_pin) == LOW)
//     {
//         return 1;
//     }
//     else
//     {
//         return 0;
//     }
// }


// //!==============================hmy================================
// hmy::hmy(int mode)
// {
//   pinMode(14,INPUT_PULLUP);
  
//   if (digitalRead(14)==0)
//   {
//    switch (mode)
//    {
//    case 88:
//    _pin =  1;
//      pinMode(_pin, INPUT);
//        break;
       

//        case 22:
//    _pin =  0;
//      pinMode(_pin, INPUT);
//        break;


//        case  33:
//    _pin =  3;
//      pinMode(_pin, INPUT);
//        break;


//        case 44:
//    _pin =  4;
//      pinMode(_pin, INPUT);
//        break;



//        case  55:
//    _pin =  5;
//      pinMode(_pin, INPUT);
//        break;


//        case  66:
//    _pin =  6;
//      pinMode(_pin, INPUT);
//        break;
//        case 70:
//    _pin =  1;
//      pinMode(_pin, INPUT);
//        break;
       

//        case 80:
//    _pin =  0;
//      pinMode(_pin, INPUT);
//        break;


//        case  90:
//    _pin =  3;
//      pinMode(_pin, INPUT);
//        break;


//        case 100:
//    _pin =  4;
//      pinMode(_pin, INPUT);
//        break;



//        case  110:
//    _pin =  5;
//      pinMode(_pin, INPUT);
//        break;


//        case  120:
//    _pin =  6;
//      pinMode(_pin, INPUT);
//        break;
//    }
//   }


// }


// int hmy::read(char a)
// {
//   _a=a;

//     if (_a==1)
//     {
//         return analogRead(_pin);
//     }
//     else
//     {
//        return digitalRead(_pin);
//     }
// }
//=====================================تابع پرس  ===========================================
boolean press(int a, int b)
{

  pinMode(a, INPUT_PULLUP);
  int cunnter_time = 0;
  while (1)
  {
    if (digitalRead(a) == 0)
    {
      cunnter_time++;
      delay(1);
    } else
    {
      cunnter_time = 0;
      return 0;
    }
    if (cunnter_time > b) return 1;
  }
  
}
//================================================================================

//================================================================================

// ========================================pin==============================================
int pin()
{
   Serial.println("بدون مقدار");
       Serial.println("براي خواندن ورودي شماره پايه را در پرانتز قرار دهيد");
     
       Serial.println("براي دستور دادن به پايه, شماره پايه و دستور را به صورت زير بنويسيد");
       Serial.println("(دستور , شماره پايه)");
}
int _q=0;
  int AVRE=0;
//============================================تابع خواندن مقدار پايه ها=============================================
int pin (int pin_number)
{
  switch (pin_number) {
    
    case D1:
     pinMode(32, INPUT_PULLUP);
        if(digitalRead(32)==0)
        {
          delay(1);
          if(digitalRead(32)==0)
          {
             delay(1);
            if(digitalRead(32)==0)
            {
            
              return 1;
            }
          }
        }
        else if (digitalRead(32)==1)
        {
         delay(1);
          if(digitalRead(32)==1)
          {
             delay(1);
            if(digitalRead(32)==1)
            {
             
              return 0;
            }
          }
        }
       
        
      break;
    case D2:
       pinMode(4, INPUT_PULLUP);
        if(digitalRead(4)==0)
        {
          delay(1);
          if(digitalRead(4)==0)
          {
             delay(1);
            if(digitalRead(4)==0)
            {
              
              return 1;
            }
          }
        }
        else if (digitalRead(4)==1)
        {
         delay(1);
          if(digitalRead(4)==1)
          {
             delay(1);
            if(digitalRead(4)==1)
            {
            
              return 0;
            }
          }
        }
       
      
      break;
    case D3:
      pinMode(15, INPUT_PULLUP);
        if(digitalRead(15)==0)
        {
          delay(1);
          if(digitalRead(15)==0)
          {
             delay(1);
            if(digitalRead(15)==0)
            {
             
              return 1;
            }
          }
        }
        else if (digitalRead(15)==1)
        {
         delay(1);
          if(digitalRead(15)==1)
          {
             delay(1);
            if(digitalRead(15)==1)
            {
             
              return 0;
            }
          }
        }
       
      break;
    case D4:
     pinMode(14, INPUT_PULLUP);
        if(digitalRead(14)==0)
        {
          delay(1);
          if(digitalRead(14)==0)
          {
             delay(1);
            if(digitalRead(14)==0)
            {
            
              return 1;
            }
          }
        }
        else if (digitalRead(14)==1)
        {
         delay(1);
          if(digitalRead(14)==1)
          {
             delay(1);
            if(digitalRead(14)==1)
            {
             
              return 0;
            }
          }
        }
       
      break;
    case D5:
    pinMode(12, INPUT_PULLUP);
        if(digitalRead(12)==0)
        {
          delay(1);
          if(digitalRead(12)==0)
          {
             delay(1);
            if(digitalRead(12)==0)
            {
             
              return 1;
            }
          }
        }
        else if (digitalRead(12)==1)
        {
         delay(1);
          if(digitalRead(12)==1)
          {
             delay(1);
            if(digitalRead(12)==1)
            {
              
              return 0;
            }
          }
        }
      
      break;
    case D6:
    //  delay(2);
        pinMode(33, INPUT_PULLUP);
        if(digitalRead(33)==0)
        {
          delay(1);
          if(digitalRead(33)==0)
          {
             delay(1);
            if(digitalRead(33)==0)
            {
            
              return 1;
            }
          }
        }
        else if (digitalRead(33)==1)
        {
         delay(1);
          if(digitalRead(33)==1)
          {
             delay(1);
            if(digitalRead(33)==1)
            {
              
              return 0;
            }
          }
        }
    
     break;
     
    case A1:
pinMode(32, INPUT);
   
	for (byte i = 0; i < 10; i++) {
		AVRE += analogRead(32);
		delay(1);
	}
	return AVRE / 10;
      
    case A2:
      
        pinMode(4, INPUT);
     
	for (byte i = 0; i < 10; i++) {
		AVRE += analogRead(4);
		delay(1);
	}
	return AVRE / 10;
      
      break;
    case A3:
     
        pinMode(15, INPUT);
 
	for (byte i = 0; i < 10; i++) {
		AVRE += analogRead(15);
		delay(1);
	}
	return AVRE / 10;
      
      
      break;
    case A4:
    
        pinMode(14, INPUT);
  
	for (byte i = 0; i < 10; i++) {
		AVRE += analogRead(14);
		delay(1);
	}
	return AVRE / 10;
      
    
      break;
    case A5:
   
        pinMode(12, INPUT);
     
	for (byte i = 0; i < 10; i++) {
		AVRE += analogRead(12);
		delay(1);
	}
	return AVRE / 10;
      
     
      break;
    case A6:
     
        pinMode(33, INPUT);

	for (byte i = 0; i < 10; i++) {
		AVRE += analogRead(33);
		delay(1);
	}
	return AVRE / 10;
      
     
      break;
      default:
      
	 
	
	return touchRead(pin_number);
      
  }
}
//=============================================تابع دستور دادن به پايه ها و خواندن کليد===========================================
int pin (int pin_number , int Value)
{
  

  switch (pin_number) {
    
    case D1:
  
        pinMode(32, OUTPUT);
        digitalWrite(32 , Value);
      
      break;
    case D2:
     
  
        pinMode(4, OUTPUT);
        digitalWrite(4 , Value);
      
      break;
    case D3:
    
        pinMode(15, OUTPUT);
        digitalWrite(15 , Value);
      
      break;
    case D4:
    
        pinMode(14, OUTPUT);
        digitalWrite(14 , Value);
      
      break;
    case D5:
     
        pinMode(12, OUTPUT);
        digitalWrite(12 , Value);
      
      break;
    case D6:
     
        pinMode(33, OUTPUT);
        digitalWrite(33, Value);
      
      break;
       case led:
  
        pinMode(2, OUTPUT);
        analogWrite(2 , Value);
      
      break;
        case A1:
  
        pinMode(32, OUTPUT);
        analogWrite(32 , Value);
      
      break;
    case A2:
     
  
        pinMode(4, OUTPUT);
       analogWrite(4 , Value);
      
      break;
    case A3:
    
        pinMode(15, OUTPUT);
        analogWrite(15 , Value);
      
      break;
    case A4:
    
        pinMode(14, OUTPUT);
        analogWrite(14 , Value);
      
      break;
    case A5:
     
        pinMode(12, OUTPUT);
       analogWrite(12 , Value);
      
      break;
    case A6:
     
        pinMode(33, OUTPUT);
        analogWrite(33, Value);
      
      break;
      

      }
      

      
//   // int mo = Value;
// // _pin_number=pin_number;
//   // char analog_pin = _pin_number;
//   //==============================================================================================
  
  //================================خواندن سطح صفر کليد=====================================
   if(Value==d)
  {
switch (pin_number) {
    
      case key:

       pinMode(0, INPUT_PULLUP);
        if(digitalRead(0)==1 )
        {
          delay(1);
          if(digitalRead(0)==1)
          {
             delay(1);
            if(digitalRead(0)==1)
            {
              
              return 1;
            }
          }
        }
        else if (digitalRead(0)==0)
        {
         delay(1);
          if(digitalRead(0)==0)
          {
             delay(1);
            if(digitalRead(0)==0)
            {
              
              return 0;
            }
          }
        }
       
        
      break;
       case key1:
     pinMode(32, INPUT_PULLUP);
        if(digitalRead(32)==1)
        {
          delay(1);
          if(digitalRead(32)==1)
          {
             delay(1);
            if(digitalRead(32)==1)
            {
              
              return 1;
            }
          }
        }
        else if (digitalRead(32)==0)
        {
         delay(1);
          if(digitalRead(32)==0)
          {
             delay(1);
            if(digitalRead(32)==0)
            {
             
              return 0;
            }
          }
        }
       
        
      break;
    case key2:
       pinMode(4, INPUT_PULLUP);
        if(digitalRead(4)==1)
        {
          delay(1);
          if(digitalRead(4)==1)
          {
             delay(1);
            if(digitalRead(4)==1)
            {
              
              return 1;
            }
          }
        }
        else if (digitalRead(4)==0)
        {
         delay(1);
          if(digitalRead(4)==0)
          {
             delay(1);
            if(digitalRead(4)==0)
            {
            
              return 0;
            }
          }
        }
       
      
      break;
    case key3:
      pinMode(15, INPUT_PULLUP);
        if(digitalRead(15)==1)
        {
          delay(1);
          if(digitalRead(15)==1)
          {
             delay(1);
            if(digitalRead(15)==1)
            {
             
              return 1;
            }
          }
        }
        else if (digitalRead(15)==0)
        {
         delay(1);
          if(digitalRead(15)==0)
          {
             delay(1);
            if(digitalRead(15)==0)
            {
             
              return 0;
            }
          }
        }
       
      break;
    case key4:
     pinMode(14, INPUT_PULLUP);
        if(digitalRead(14)==1)
        {
          delay(1);
          if(digitalRead(14)==1)
          {
             delay(1);
            if(digitalRead(14)==1)
            {
            
              return 1;
            }
          }
        }
        else if (digitalRead(14)==0)
        {
         delay(1);
          if(digitalRead(14)==0)
          {
             delay(1);
            if(digitalRead(14)==0)
            {
             
              return 0;
            }
          }
        }
       
      break;
    case key5:
    pinMode(12, INPUT_PULLUP);
        if(digitalRead(12)==1)
        {
          delay(1);
          if(digitalRead(12)==1)
          {
             delay(1);
            if(digitalRead(12)==1)
            {
             
              return 1;
            }
          }
        }
        else if (digitalRead(12)==0)
        {
         delay(1);
          if(digitalRead(12)==0)
          {
             delay(1);
            if(digitalRead(12)==0)
            {
              
              return 0;
            }
          }
        }
      
      break;
    case key6:
    //  delay(2);
        pinMode(33, INPUT_PULLUP);
        if(digitalRead(33)==1)
        {
          delay(1);
          if(digitalRead(33)==1)
          {
             delay(1);
            if(digitalRead(33)==1)
            {
            
              return 1;
            }
          }
        }
        else if (digitalRead(33)==0)
        {
         delay(1);
          if(digitalRead(33)==0)
          {
             delay(1);
            if(digitalRead(33)==0)
            {
              
              return 0;
            }
          }
        }
    
     break;
  }
  }
  
  //================================خواندن سطح يک کليد=====================================
   if(Value==u)
  {
switch (pin_number) {
    
      case key:

       pinMode(0, INPUT_PULLUP);
        if(digitalRead(0)==0 )
        {
          delay(1);
          if(digitalRead(0)==0)
          {
             delay(1);
            if(digitalRead(0)==0)
            {
              
              return 1;
            }
          }
        }
        else if (digitalRead(0)==1)
        {
         delay(1);
          if(digitalRead(0)==1)
          {
             delay(1);
            if(digitalRead(0)==1)
            {
              
              return 0;
            }
          }
        }
       
        
      break;
       case key1:
     pinMode(32, INPUT_PULLUP);
        if(digitalRead(32)==0)
        {
          delay(1);
          if(digitalRead(32)==0)
          {
             delay(1);
            if(digitalRead(32)==0)
            {
              
              return 1;
            }
          }
        }
        else if (digitalRead(32)==1)
        {
         delay(1);
          if(digitalRead(32)==1)
          {
             delay(1);
            if(digitalRead(32)==1)
            {
             
              return 0;
            }
          }
        }
       
        
      break;
    case key2:
       pinMode(4, INPUT_PULLUP);
        if(digitalRead(4)==0)
        {
          delay(1);
          if(digitalRead(4)==0)
          {
             delay(1);
            if(digitalRead(4)==0)
            {
              
              return 1;
            }
          }
        }
        else if (digitalRead(4)==1)
        {
         delay(1);
          if(digitalRead(4)==1)
          {
             delay(1);
            if(digitalRead(4)==1)
            {
            
              return 0;
            }
          }
        }
       
      
      break;
    case key3:
      pinMode(15, INPUT_PULLUP);
        if(digitalRead(15)==0)
        {
          delay(1);
          if(digitalRead(15)==0)
          {
             delay(1);
            if(digitalRead(15)==0)
            {
             
              return 1;
            }
          }
        }
        else if (digitalRead(15)==1)
        {
         delay(1);
          if(digitalRead(15)==1)
          {
             delay(1);
            if(digitalRead(15)==1)
            {
             
              return 0;
            }
          }
        }
       
      break;
    case key4:
     pinMode(14, INPUT_PULLUP);
        if(digitalRead(14)==0)
        {
          delay(1);
          if(digitalRead(14)==0)
          {
             delay(1);
            if(digitalRead(14)==0)
            {
            
              return 1;
            }
          }
        }
        else if (digitalRead(14)==1)
        {
         delay(1);
          if(digitalRead(14)==1)
          {
             delay(1);
            if(digitalRead(14)==1)
            {
             
              return 0;
            }
          }
        }
       
      break;
    case key5:
    pinMode(12, INPUT_PULLUP);
        if(digitalRead(12)==0)
        {
          delay(1);
          if(digitalRead(12)==0)
          {
             delay(1);
            if(digitalRead(12)==0)
            {
             
              return 1;
            }
          }
        }
        else if (digitalRead(12)==1)
        {
         delay(1);
          if(digitalRead(12)==1)
          {
             delay(1);
            if(digitalRead(12)==1)
            {
              
              return 0;
            }
          }
        }
      
      break;
    case key6:
    //  delay(2);
        pinMode(33, INPUT_PULLUP);
        if(digitalRead(33)==0)
        {
          delay(1);
          if(digitalRead(33)==0)
          {
             delay(1);
            if(digitalRead(33)==0)
            {
            
              return 1;
            }
          }
        }
        else if (digitalRead(33)==1)
        {
         delay(1);
          if(digitalRead(33)==1)
          {
             delay(1);
            if(digitalRead(33)==1)
            {
              
              return 0;
            }
          }
        }
    
     break;
  }
  }
  //=============================================خواندن لبه پايين رونده کليد==============================================
  if(Value==r)
  {
switch (pin_number) {
    
      case key:

       pinMode(0, INPUT_PULLUP);
        if(digitalRead(0)==1 && _q==1)
        {
          delay(1);
          if(digitalRead(0)==1 && _q==1)
          {
             delay(1);
            if(digitalRead(0)==1 && _q==1)
            {
              _q=0;
              return 1;
            }
          }
        }
        else if (digitalRead(0)==0)
        {
         delay(1);
          if(digitalRead(0)==0)
          {
             delay(1);
            if(digitalRead(0)==0)
            {
              _q=1;
              return 0;
            }
          }
        }
        else if(_q==0) return 0;
        
      break;
       case key1:
     pinMode(32, INPUT_PULLUP);
        if(digitalRead(32)==1 && _q==1)
        {
          delay(1);
          if(digitalRead(32)==1 && _q==1)
          {
             delay(1);
            if(digitalRead(32)==1 && _q==1)
            {
              _q=0;
              return 1;
            }
          }
        }
        else if (digitalRead(32)==0)
        {
         delay(1);
          if(digitalRead(32)==0)
          {
             delay(1);
            if(digitalRead(32)==0)
            {
              _q=1;
              return 0;
            }
          }
        }
        else if(_q==0) return 0;
        
      break;
    case key2:
       pinMode(4, INPUT_PULLUP);
        if(digitalRead(4)==1 && _q==0)
        {
          delay(1);
          if(digitalRead(4)==1 && _q==0)
          {
             delay(1);
            if(digitalRead(4)==1 && _q==0)
            {
              _q=0;
              return 1;
            }
          }
        }
        else if (digitalRead(4)==0)
        {
         delay(1);
          if(digitalRead(4)==0)
          {
             delay(1);
            if(digitalRead(4)==0)
            {
              _q=1;
              return 0;
            }
          }
        }
        else if(_q==0) return 0;
      
      break;
    case key3:
      pinMode(15, INPUT_PULLUP);
        if(digitalRead(15)==1 && _q==1)
        {
          delay(1);
          if(digitalRead(15)==1 && _q==1)
          {
             delay(1);
            if(digitalRead(15)==1 && _q==1)
            {
              _q=0;
              return 1;
            }
          }
        }
        else if (digitalRead(15)==0)
        {
         delay(1);
          if(digitalRead(15)==0)
          {
             delay(1);
            if(digitalRead(15)==0)
            {
              _q=1;
              return 0;
            }
          }
        }
        else if(_q==0) return 0;
      break;
    case key4:
     pinMode(14, INPUT_PULLUP);
        if(digitalRead(14)==1 && _q==1)
        {
          delay(1);
          if(digitalRead(14)==1 && _q==1)
          {
             delay(1);
            if(digitalRead(14)==1 && _q==1)
            {
              _q=0;
              return 1;
            }
          }
        }
        else if (digitalRead(14)==0)
        {
         delay(1);
          if(digitalRead(14)==0)
          {
             delay(1);
            if(digitalRead(14)==0)
            {
              _q=1;
              return 0;
            }
          }
        }
        else if(_q==0) return 0;
      break;
    case key5:
    pinMode(12, INPUT_PULLUP);
        if(digitalRead(12)==1 && _q==1)
        {
          delay(1);
          if(digitalRead(12)==1 && _q==1)
          {
             delay(1);
            if(digitalRead(12)==1 && _q==1)
            {
              _q=0;
              return 1;
            }
          }
        }
        else if (digitalRead(12)==0)
        {
         delay(1);
          if(digitalRead(12)==0)
          {
             delay(1);
            if(digitalRead(12)==0)
            {
              _q=1;
              return 0;
            }
          }
        }
        else if(_q==0) return 0;
      break;
    case key6:
    //  delay(2);
        pinMode(33, INPUT_PULLUP);
        if(digitalRead(33)==1 && _q==1)
        {
          delay(1);
          if(digitalRead(33)==1 && _q==1)
          {
             delay(1);
            if(digitalRead(33)==1 && _q==1)
            {
              _q=0;
              return 1;
            }
          }
        }
        else if (digitalRead(33)==0)
        {
         delay(1);
          if(digitalRead(33)==0)
          {
             delay(1);
            if(digitalRead(33)==0)
            {
              _q=1;
              return 0;
            }
          }
        }
           else if(_q==0) return 0;
     break;
  }
  }

  //==================================خواندن لبه بالا رونده کليد===================================
 
   if(Value==f)
  {
   switch (pin_number) {
    
      case key:
delay(2);
       pinMode(0, INPUT_PULLUP);
        if(digitalRead(0)==0 && _q==1)
        {
          delay(1);
          if(digitalRead(0)==0 && _q==1)
          {
             delay(1);
            if(digitalRead(0)==0 && _q==1)
            {
              _q=0;
              return 1;
            }
          }
        }
        else if (digitalRead(0)==1)
        {
         delay(1);
          if(digitalRead(0)==1)
          {
             delay(1);
            if(digitalRead(0)==1)
            {
              _q=1;
              return 0;
            }
          }
        }
     else if(_q==0) return 0;
        
      break;
       case key1:
     pinMode(32, INPUT_PULLUP);
        if(digitalRead(32)==0 && _q==1)
        {
          delay(1);
          if(digitalRead(32)==0 && _q==1)
          {
             delay(1);
            if(digitalRead(32)==0 && _q==1)
            {
              _q=0;
              return 1;
            }
          }
        }
        else if (digitalRead(32)==1)
        {
         delay(1);
          if(digitalRead(32)==1)
          {
             delay(1);
            if(digitalRead(32)==1)
            {
              _q=1;
              return 0;
            }
          }
        }
     
           else if(_q==0) return 0;
      break;
    case key2:
       pinMode(4, INPUT_PULLUP);
        if(digitalRead(4)==0 && _q==1)
        {
          delay(1);
          if(digitalRead(4)==0 && _q==1)
          {
             delay(1);
            if(digitalRead(4)==0 && _q==1)
            {
              _q=0;
              return 1;
            }
          }
        }
        else if (digitalRead(4)==1)
        {
         delay(1);
          if(digitalRead(4)==1)
          {
             delay(1);
            if(digitalRead(4)==1)
            {
              _q=1;
              return 0;
            }
          }
        }
     
         else if(_q==0) return 0;
      break;
    case key3:
      pinMode(15, INPUT_PULLUP);
        if(digitalRead(15)==0 && _q==1)
        {
          delay(1);
          if(digitalRead(15)==0 && _q==1)
          {
             delay(1);
            if(digitalRead(15)==0 && _q==1)
            {
              _q=0;
              return 1;
            }
          }
        }
        else if (digitalRead(15)==1)
        {
         delay(1);
          if(digitalRead(15)==1)
          {
             delay(1);
            if(digitalRead(15)==1)
            {
              _q=1;
              return 0;
            }
          }
        }
        else if(_q==0) return 0;
      break;
    case key4:
     pinMode(14, INPUT_PULLUP);
        if(digitalRead(14)==0 && _q==1)
        {
          delay(1);
          if(digitalRead(14)==0 && _q==1)
          {
             delay(1);
            if(digitalRead(14)==0 && _q==1)
            {
              _q=0;
              return 1;
            }
          }
        }
        else if (digitalRead(14)==1)
        {
         delay(1);
          if(digitalRead(14)==1)
          {
             delay(1);
            if(digitalRead(14)==1)
            {
              _q=1;
              return 0;
            }
          }
        }
        else if(_q==0) return 0;
      break;
    case key5:
    pinMode(12, INPUT_PULLUP);
        if(digitalRead(12)==0 && _q==1)
        {
          delay(1);
          if(digitalRead(12)==0 && _q==1)
          {
             delay(1);
            if(digitalRead(12)==0 && _q==1)
            {
              _q=0;
              return 1;
            }
          }
        }
        else if (digitalRead(12)==1)
        {
         delay(1);
          if(digitalRead(12)==1)
          {
             delay(1);
            if(digitalRead(12)==1)
            {
              _q=1;
              return 0;
            }
          }
        }
        else if(_q==0) return 0;
      break;
    case key6:
    //  delay(2);
        pinMode(33, INPUT_PULLUP);
        if(digitalRead(33)==0 && _q==1)
        {
          delay(1);
          if(digitalRead(33)==0 && _q==1)
          {
             delay(1);
            if(digitalRead(33)==0 && _q==1)
            {
              _q=0;
              return 1;
            }
          }
        }
        else if (digitalRead(33)==1)
        {
         delay(1);
          if(digitalRead(33)==1)
          {
             delay(1);
            if(digitalRead(33)==1)
            {
              _q=1;
              return 0;
            }
          }
        }
        else if(_q==0) return 0;
      break;
   }
  }
  

  }
  //=====================================================================
  

// //==========================================buzzer=============================================
// buzzer::buzzer(int mode)
// {
  
  
//    switch (mode)
//    {
//    case 88:
//    _pin =  1;
//      pinMode(_pin, OUTPUT);
//      //digitalWrite(32 , 0);
//        break;
       

//        case 22:
//    _pin =  0;
//     pinMode(_pin, OUTPUT);
//    //  digitalWrite(32 , 0);
//        break;


//        case  33:
//    _pin =  3;
//     pinMode(_pin, OUTPUT);
//      //digitalWrite(32 , 0);
//        break;


//        case 44:
//    _pin =  4;
//      pinMode(_pin, OUTPUT);
//     // digitalWrite(32 , 0);
//        break;



//        case  55:
//    _pin =  5;
//       pinMode(_pin, OUTPUT);
//      //digitalWrite(32 , 0);
//        break;


//        case  66:
//    _pin =  6;
//       pinMode(_pin, OUTPUT);
//      //digitalWrite(32 , 0);
//        break;
//        case 70:
//    _pin =  1;
//      pinMode(_pin, OUTPUT);
//      //digitalWrite(32 , 0);
//        break;
       

//        case 80:
//    _pin =  0;
//       pinMode(_pin, OUTPUT);
//     // digitalWrite(32 , 0);
//        break;


//        case  90:
//    _pin =  3;
//       pinMode(_pin, OUTPUT);
//     // digitalWrite(32 , 0);
//        break;


//        case 100:
//    _pin =  4;
//       pinMode(_pin, OUTPUT);
//     // digitalWrite(32 , 0);
//        break;



//        case  110:
//    _pin =  5;
//       pinMode(_pin, OUTPUT);
//     // digitalWrite(32 , 0);
//        break;


//        case  120:
//    _pin =  6;
//       pinMode(_pin, OUTPUT);
//      //digitalWrite(32 , 0);
//        break;
//    }
//   }
// void buzzer::write(char b)
// {
//   _b=b;
// digitalWrite(_pin , _b);
// }


// //============================================relay===========================================
// relay::relay(int mode)
// {
  
  
//    switch (mode)
//    {
//    case 88:
//    _pin =  1;
//     _pinn =  1;
//      pinMode(_pin, OUTPUT);
//      pinMode(_pinn, OUTPUT);
//     //  digitalWrite(32 , 0);
//     //  digitalWrite(32 , 0);
//        break;
       

//        case 22:
//    _pin =  0;
//    _pinn =  1;
//      pinMode(_pin, OUTPUT);
//      pinMode(_pinn, OUTPUT);
//     //  digitalWrite(32 , 0);
//     //  digitalWrite(32 , 1);
//        break;


//        case  33:
//    _pin =  3;
//    _pinn =  1;
//      pinMode(_pin, OUTPUT);
//      pinMode(_pinn, OUTPUT);
//     //  digitalWrite(32 , 1);
//     //  digitalWrite(32 , 0);
//        break;


//        case 44:
//    _pin =  4;
//     _pinn =  1;
//      pinMode(_pin, OUTPUT);
//      pinMode(_pinn, OUTPUT);
//     //  digitalWrite(32 , 1);
//     //  digitalWrite(32 , 1);
//        break;



//    }
//   }

// void relay::write(char a, char b)
// {
//   _b=b;
//   _a=a;
//   digitalWrite(_pin, _a);
//      digitalWrite(_pinn, _b);
// }
//=======================================================================================
void run()
{

  Serial.begin(115200);
  // init_priv();
  // backlight();
}
//=======================================================================================