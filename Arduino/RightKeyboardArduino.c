/* Title: RightKeyboardHalf (Secondary) - S00241117 Project
Code Details: Chip Driver for Pro Micro ATmega32U4 based Right Keyboard Half 
Start Date: 15/01/2024

pin D1: N/A		-|	 usbc	|-	RAW
pin D0: N/A		-|			|-	GND
		GND		-|			|-	RST
		GND		-|   top	|-	VCC
pin D2: I2C SDA -|			|-	pin A3:  col 0
pin D3: I2C SCL -|			|-	pin A2:  col 1
pin D4: row 0   -|			|-	pin A1:  col 2
pin D5: row 1   -|			|-	pin A0:  col 3
pin D6: row 2   -|			|-	pin D15: col 4
pin D7: row 3  	-|			|-	pin D14: col 5
pin D8: row 4	-|			|-	pin D16: col 6 
pin D9: LEDSTR	-|__________|-	pin D10: col 7
*/

///// Libraries /////
#include <Wire.h> 				//I2C handler  
#include <Adafruit_NeoPixel.h>	//LED Strip Library

///// Global Variables /////
const int SCND_ADDY = 8;
const int ROWS = 5;
const int COLS = 8;
const int LEDPIN = 9;
const int NUMPIXELS = 8;

const int rowpin[ROWS] = {4, 5, 6, 7, 8};
const int colpin[COLS] = {A3, A2, A1, A0, 15, 14, 16, 10};

byte keyStatus[ROWS] = {
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000,
  0b00000000
};

///// Keymap, not used on secondary (right) Keypad/////
/* char keyMap[ROWS][COLS] = {
	{'6' , '7' , '8' , '9' , '0' , '-' , '=', 178 }, //[...] backspace
	{'y' , 'u' , 'i' , 'o' , 'p' , '[' , ']', '\\'},
	{'h' , 'j' , 'k' , 'l' , ';' , '\'', 176, '*' }, //[...] enter, not used
	{'n', 'm' , ',' , '.' , '/' , 133 , 218, 212  }, //[...] shift, up, del
	{'*', ' ',  134,  237,  132,  216 , 217, 215  }, //not used [...] alt, menu, ctrl, left, down, right 
};*/

///// function prototypes /////
void printByteArray(const unsigned char* array, size_t length);
Adafruit_NeoPixel strip = Adafruit_NeoPixel (NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);

///// Arduino setup on power on /////
void setup() {
  Wire.begin(SCND_ADDY);			//startup I2C handler
  strip.begin();					//startup LED handler
  
  for (int r = 0; r < ROWS; r++) {	//Set all Rows as Output (HIGH)
    pinMode(rowpin[r], OUTPUT);
  }

  for (int c = 0; c < COLS; c++) {	//Set all Colums as Pull-Up Input
    pinMode(colpin[c], INPUT_PULLUP);
  }
  
  Wire.onRequest(requestEvent);		//define I2C request action
  
  strip.clear();					//reset all LEDs

  for(int p=0; p<NUMPIXELS; p++) {	//repeat for each LED Pixel
    strip.setPixelColor(p, 200, 120, 0);	// set LED Pixel RGB color value (orange) 
  }
  strip.show();						//Display RGB colors on current LEDs
}

///// Arduino infinite loop function while powered on /////

void loop() {

  for (int r = 0; r < ROWS; r++) {			//repeat per Row 
    
    digitalWrite(rowpin[r], LOW);			//Set current Row as LOW
    delayMicroseconds(10);					//Delay to stabilize new state

    for (int c = 0; c < COLS; c++) {		//repeat per Colum
      if(digitalRead(colpin[c]) == LOW) {	//if LOW detected 
        keyStatus[r] |= (1 << c); 			//set byte in Key status array
      }
      
      else {								//otherwise 
        keyStatus[r] &= ~(1 << c); 			//clear byte, to handle key releases
      }
    }
    

    digitalWrite(rowpin[i], HIGH);  		//reset Row Pin to High
    delayMicroseconds(100);  				//delay to stabilize new state 
  }
  
  delay(1);    								//delay to stabilize sytem befor next repeat

///// Function to handle I2C request Event /////

void requestEvent() {
  for(int i = 0; i<5; i++){
    Wire.write(keyStatus[i]);	//Send each Byte of the current KeyStatus Array (up to 5)
  }
}