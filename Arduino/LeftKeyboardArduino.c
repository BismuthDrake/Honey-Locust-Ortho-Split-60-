/* Title: LeftKeyboardHalf (Primary) - S00241117 Project
Code Details: Chip Driver for Pro Micro ATmega32U4 based Left Keyboard Half 
Start Date: 15/01/2024

pin D1: N/A 	-|	 usbc	|-	RAW
pin D0: N/A		-|			|-	GND
		GND		-|			|-	RST
		GND		-|   top	|-	VCC
pin D2: I2C SDA -|			|-	pin A3:  col 0
pin D3: I2C SCL -|			|-	pin A2:  col 1
pin D4: row 0   -|			|-	pin A1:  col 2
pin D5: row 1   -|			|-	pin A0:  col 3
pin D6: row 2   -|			|-	pin D15: col 4
pin D7: row 3  	-|			|-	pin D14: col 5
pin D8: row 4	-|			|-	pin D16: N/A 
pin D9: LEDSTR	-|__________|-	pin D10: N/A
*/

///// Libraries /////
#include <Keyboard.h>  			//Arduino Keyboard Library
#include <Wire.h> 				//I2C handler  
#include <Adafruit_NeoPixel.h>	//LED Strip Library

///// Global Variables /////
const int SCND_ADDY = 8;
const int ROWS = 5;
const int COLS = 6;
const int RIGHTCOLS = 8;
const int rowpin[ROWS] = {4, 5, 6, 7, 8};
const int colpin[COLS] = {A3, A2, A1, A0, 15, 14};
const int LEDPIN = 9;
const int NUMPIXELS = 6;

int byteCount = 0; 						//Counter to handle Secondary I2C reception
int initialDelay = 60; 					//No. of Cycles to delay if key held after first press, before repeating key input   
int repeatDelay = 6; 					//No. of Cycles to delay if key held continously, before repeating key input 

byte bytes[5] = {0, 0, 0, 0, 0};  		//Byte Array for Primary input handling
byte bytesRight[5] = {0, 0, 0, 0, 0};  	//Byte Array for Secondary input handling

unsigned int keyHoldCount [ROWS][COLS]; //Tracker for held Keys on Primary
bool keyLongHold [ROWS][COLS]; 			//Tracker for continously held keys on Primary

unsigned int keyHoldCountRight [ROWS][RIGHTCOLS]; 	//Tracker for held Keys on Secondary
bool keyLongHoldRight [ROWS][RIGHTCOLS]; 			//Tracker for continously held keys on Secondary

////// Keymap for Primary & Secondary //////

char keyMap[ROWS][COLS] = {
  {177 , '1' , '2' , '3' , '4' , '5'},	//esc [...]
  {179,  'q' , 'w' , 'e' , 'r' , 't'},  //tab [...]
  {193,  'a' , 's' , 'd' , 'f' , 'g'},  //caps lock [...]
  {129,  'z' , 'x' , 'c' , 'v' , 'b'},  //shift [...]
  {128,  '`',  131,  130,  ' ' , '*'},  //ctrl, [...] win, alt, [...], not used
};

char keyMapRight[ROWS][RIGHTCOLS] = {
  {'6' , '7' , '8' , '9' , '0' , '-' , '=', 178 }, //[...] backspace
  {'y' , 'u' , 'i' , 'o' , 'p' , '[' , ']', '\\'},
  {'h' , 'j' , 'k' , 'l' , ';' , '\'', 176, '*' }, //[...] enter, not used
  {'n', 'm' , ',' , '.' , '/' , 133 , 218, 212  }, //[...] shift, up, del
  {'*', ' ',  134,  237,  132,  216 , 217, 215  }, //not used [...] alt, menu, ctrl, left, down, right 
};

///// function prototypes /////
void keyPressed(int row, int col);
void keyReset(int row, int col);
void keyPressedRight(int row, int col);
void keyResetRight(int row, int col);
void printByteArray(const unsigned char* array, size_t length);
Adafruit_NeoPixel strip = Adafruit_NeoPixel (NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);

///// Arduino setup on power on /////
void setup() {
  Wire.begin();								//startup I2C handler
  Serial.begin(9600);						//Serial fucntion for debugging
  strip.begin();							//startup LED handler
  Keyboard.begin(); 						//startup Keyboard HID handler
  
  for (int r = 0; r < ROWS; r++) {			//Set all Rows as Output (HIGH)
    pinMode(rowpin[r], OUTPUT);
  }

  for (int c = 0; c < COLS; c++) {			//Set all Colums as Pull-Up Input
    pinMode(colpin[c], INPUT_PULLUP);
  }		
	
  strip.clear();							//reset all LEDs

  for(int p=0; p<NUMPIXELS; p++) {			//repeat for each LED Pixel
    strip.setPixelColor(p, 200, 120, 0);	//set LED Pixel RGB color value (orange) 
  }
  strip.show();								//Display RGB colors on current LEDs
}

///// Arduino infinite loop function while powered on /////
void loop() {
  Wire.requestFrom(SCND_ADDY, 5);    		//Send request to secondary for 5 bytes
  
  while (Wire.available()) { 				//When receiving, loop through received bytes 
    bytesRight[byteCount] = Wire.read();	//Transpose received Byte to bytesRight Array
    byteCount++;							//move to next byte received
  }
    
  byteCount = 0;  							//Reset byteCount for next request
    
  for (int r = 0; r < ROWS; r++) { 			//repeat per Row    
  
    digitalWrite(rowpin[r], LOW);  			//Set current Row as LOW
    delayMicroseconds(100);  				//Delay to stabilize new state
	
    for (int c = 0; c < COLS; c++) { 		//repeat per Colum 
      if(digitalRead(colpin[c]) == LOW) {	//if LOW detected
        bytes[r] |= (1 << c); 				//set bit in bytes array
      } 
      else{  								//otherwise
        bytes[r] &= ~(1 << c); 				//clear byte, to handle key releases
      }     
    }
    digitalWrite(rowpin[r], HIGH);  		//reset Row Pin to High
    delayMicroseconds(100);  				//delay to stabilize new state
  }
  
  for(int r = 0; r < ROWS; r++){    		//repeat per Row 
    for(int c = (COLS - 1); c >= 0; c--){	//repeat per Colum on Primary, in reverse
      if((bytes[r] >> c) & 1){  			//if position in bytes array is 1
        keyPressed(r,c);					//call keyPressed with current row and col
      }
      else{									//otherwise
        if(!((r == 3 && c == 0) || (r == 4 && c == 0) || (r == 4 && c == 2) || (r == 4 && c == 3))){  
											//if NOT modifier key
          keyReset(r,c);    				//call keyReset ith current row and col
        }
        else {								//otherwise
          Keyboard.release(keyMap[r][c]);  	//Keyboard Handler at current row and col set key released
        }
      }
    }
  }
  
  /*Due to transposed bit array for Secondary, Cols are inverted and need to be handled as such.
	Because of 0 indexing for 8 culums on the Secondary this will be handled via 7 - c (current colum) */
  for(int r = 0; r < ROWS; r++){ 			//repeat per Row
    for(int c = (RIGHTCOLS - 1); c >= 0; c--){ 	//repeat per Colum on Secondary, in reverse
      if((bytesRight[r] >> c) & 1){  		//if position in bytesRight array is 1
        keyPressedRight(r,(7-c));			//call keyPressedRight with current row and col
      }
      else{									//otherwise
        if(!((r == 3 && c == (7-5)) || (r == 4 && c == (7-2)) || (r == 4 && c == (7-4)))){ 
											//if NOT modifier key
          keyResetRight(r,(7-c));    		//call keyResetRight ith current row and col
        }
        else{								//otherwise
          Keyboard.release(keyMapRight[r][c]);  //Keyboard Handler at current Secondary row and col set key released
        }
      }
    }
  }
}

///// Called Functions /////
void keyPressed(int row, int col) {			//Key Pressed on Priary at row and col position
  if(!((row == 3 && col == 0) || (row == 4 && col == 0) || (row == 4 && col == 2) || (row == 4 && col == 3))){
											//if NOT modifier key
     if(keyHoldCount[row][col] == 0){		//if first call for key
      Keyboard.write(keyMap[row][col]);		//print key matching Primary keyMap
    }
    
    else if(keyLongHold[row][col] && keyHoldCount[row][col] > repeatDelay){ 
											//if key is held (detected again without release)
											//check if long hold is true and is Hold Count > repeatDelay 
      Keyboard.write(keyMap[row][col]);		//print key matching Primary keyMap
      keyHoldCount[row][col] = 1;  			//Reset Hold Count for key at current row and col
    }
    
    else if(keyHoldCount[row][col] > initialDelay){ //if key is held (detected again without release)
											//following initial press, check if Hold Count > initialDelay
      keyLongHold[row][col] = true;			//set long hold as true 
    }
  
    keyHoldCount[row][col]++;  				//increase Hold Count for key at current row and col
  }
  else{										//otherwise (Key at current row and col is modifier)
     Keyboard.press(keyMap[row][col]);		//register key matching Primary keyMap as pressed in Keyboard Handler
  }
  
}


void keyPressedRight(int row, int col) {	//Key Pressed on Secondary at row and col position
  if(!((row == 3 && col == 5) || (row == 4 && col == 2) || (row == 4 && col == 4))){
											//if NOT modifier key
     if(keyHoldCountRight[row][col] == 0){  //if first call for key
      Keyboard.write(keyMapRight[row][col]);//print key matching Secondary keyMap
      }
      
      else if(keyLongHoldRight[row][col] && keyHoldCountRight[row][col] > repeatDelay){ 
											//if key is held (detected again without release)
											//check if long hold is true and is Hold Count > repeatDelay 
		Keyboard.write(keyMapRight[row][col]);//print key matching Secondary keyMap
        keyHoldCountRight[row][col] = 1;  	//Reset Hold Count for key at current row and col
      
      else if(keyHoldCountRight[row][col] > initialDelay){ //if key is held (detected again without release)
											//following initial press, check if Hold Count > initialDelay
        keyLongHoldRight[row][col] = true;	//set long hold as true
      }
	  
      keyHoldCountRight[row][col]++;  		//increase Hold Count for key at current row and col
      
    }
    else{									//otherwise (Key at current row and col is modifier)
      Keyboard.press(keyMapRight[row][col]);//register key matching Secondary keyMap as pressed in Keyboard Handler
    }
}

void keyReset(int row, int col) {			//Key release handler at current row and col for Primary Prmiary
  keyHoldCount[row][col] = 0;				//reset  current row and col Hold Count 
  keyLongHold[row][col] = false;			//reset  current row and col long hold
}

void keyResetRight(int row, int col) {		//Key release handler at current row and col for Secondary Prmiary
  keyHoldCountRight[row][col] = 0;			//reset  current row and col Hold Count
  keyLongHoldRight[row][col] = false;		//reset  current row and col long hold
}	