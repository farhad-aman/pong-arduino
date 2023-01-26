// Mario's Ideas
//MAX7219 - using SPI library to display 8x8 bitmap
#include   <SPI.h>
#define CS 10
const int buttonPinLeft_P1 = 3;
const int buttonPinRight_P1 = 4;
const int buttonPinLeft_P2 = 5;
const int buttonPinRight_P2 = 6;

int oldButtonLeftState_P1 = LOW;
int oldButtonRightState_P1 = LOW;
int oldButtonLeftState_P2 = LOW;
int oldButtonRightState_P2 = LOW;

byte sendAddress[9];

// MAX7219 Control registers

#define DECODE_MODE   9 
#define INTENSITY 0x0A
#define SCAN_LIMIT 0x0B 
#define SHUTDOWN 0x0C
#define   DISPLAY_TEST 0x0F

byte apple [8]={B00111000,B00000000,B00000000,B00010000,B00000000,B00000000,B00000000,B00111000};
byte backup_apple [8]={B00111000,B00000000,B00000000,B00010000,B00000000,B00000000,B00000000,B00111000};

class Father{
  public : 
    void display(){
        
    }
    void tick(){

    }
    void wall(){

    }
};

class Paddle : public Father {
  public : 
    int positionY [3]={2,3,4};
    int positionX;
    void display(){
        
    }
    void tick(int direction, int player){
      if(direction == 1)
      {
        if(player == 1){
          apple[0] = apple[0] << 1;
        }
        if(player == 2){
          apple[7] = apple[7] << 1;
        }
        positionY[0] -= 1;
        positionY[1] -= 1;
        positionY[2] -= 1;
      }
      else
      {
        if(player == 1){
          apple[0] = apple[0] >> 1;
        }
        if(player == 2){
          apple[7] = apple[7] >> 1;
        }
          positionY[0] += 1;
          positionY[1] += 1;
          positionY[2] += 1;
      }
    }
};

class Ball : public Father {
  public : 
    int directionY = 0; // 0 down, 1 up
    int directionX = 0; // 0 not active, 1 right, 2 left 
    int positionX = 3;
    int positionY = 4;
    void display(){
        apple[0] = B00000011;
    }
    void out_if(){
      if(positionX == 0 || positionX == 7)
      {
        for(int i=0; i<7; i++)
        {
          apple[i] = backup_apple[i];
        }
      }
    }
    void tick(){
      if(directionY == 0) // go down
      {
        if(directionX == 0)
        {
          if(bitRead(apple[positionX+1], positionY) == 0) // can go
          {
            bitWrite(apple[positionX], positionY, 0);
            positionX += 1;
            bitWrite(apple[positionX], positionY, 1);
          }
          else if(bitRead(apple[positionX+1], positionY+1) == 1) // middle of paddle
          {
            directionY = 1;
          }
          else if(bitRead(apple[positionX+1], positionY+1) == 0) // left corner
          {
            directionY = 1;
            directionX = 2;
          }
          else if(bitRead(apple[positionX+1], positionY-1) == 0) // right corner
          {
            directionY = 1;
            directionX = 1;
          }
        }
        else if(directionX == 2) // down left
        {
          if(bitRead(apple[positionX+1], positionY+1) == 0)
          {
            if(positionY == 7) // out
            {
              directionY = 0;
              directionX = 1;
            }
            else{
              bitWrite(apple[positionX], positionY, 0);
              positionX += 1;
              positionY += 1;
              bitWrite(apple[positionX], positionY, 1);
            }
          }
          else if(bitRead(apple[positionX+1], positionY) == 1 && bitRead(apple[positionX+1], positionY+2) == 1) // middle of paddle
          {
            directionY = 1;
          }
          else if(bitRead(apple[positionX+1], positionY) == 1 && bitRead(apple[positionX+1], positionY+2) == 0) // left corner
          {
            directionY = 1;
            directionX = 1;
          }
          else if(bitRead(apple[positionX+1], positionY) == 0) // right corner
          {
            directionY = 1;
            directionX = 1;
          }
          
        }
        else if(directionX == 1) // down right
        {
          if(bitRead(apple[positionX+1], positionY-1) == 0)
          {
            if(positionY == 0) // out
            {
              directionY = 0;
              directionX = 2;
            }
            else{
              bitWrite(apple[positionX], positionY, 0);
              positionX += 1;
              positionY -= 1;
              bitWrite(apple[positionX], positionY, 1);
            }
          }
          else if(bitRead(apple[positionX+1], positionY) == 1 && bitRead(apple[positionX+1], positionY-2) == 1) // middle of paddle
          {
            directionY = 1;
          }
          else if(bitRead(apple[positionX+1], positionY) == 1 && bitRead(apple[positionX+1], positionY-2) == 0) // right corner
          {
            directionY = 1;
            directionX = 2;
          }
          else if(bitRead(apple[positionX+1], positionY) == 0) // left corner
          {
            directionY = 1;
            directionX = 2;
          }
        }
      }
      else if(directionY == 1) // up
      {
        if(directionX == 0)
        {
          if(bitRead(apple[positionX-1], positionY) == 0) // can go
          {
            bitWrite(apple[positionX], positionY, 0);
            positionX -= 1;
            bitWrite(apple[positionX], positionY, 1);
          }
          else if(bitRead(apple[positionX-1], positionY+1) == 1) // middle of paddle
          {
            directionY = 0;
          }
          else if(bitRead(apple[positionX-1], positionY+1) == 0) // left corner
          {
            directionY = 0;
            directionX = 2;
          }
          else if(bitRead(apple[positionX-1], positionY-1) == 0) // right corner
          {
            directionY = 0;
            directionX = 1;
          }
        }
        else if(directionX == 2) // up left
        {
          if(bitRead(apple[positionX-1], positionY+1) == 0)
          {
            if(positionY == 7) // out
            {
              directionY = 1;
              directionX = 1;
            }
            else{
              bitWrite(apple[positionX], positionY, 0);
              positionX -= 1;
              positionY += 1;
              bitWrite(apple[positionX], positionY, 1);
            }
          }
          else if(bitRead(apple[positionX-1], positionY) == 1 && bitRead(apple[positionX+1], positionY+2) == 1) // middle of paddle
          {
            directionY = 0;
          }
          else if(bitRead(apple[positionX-1], positionY) == 1 && bitRead(apple[positionX+1], positionY+2) == 0) // left corner
          {
            directionY = 0;
            directionX = 1;
          }
          else if(bitRead(apple[positionX-1], positionY) == 0) // right corner
          {
            directionY = 0;
            directionX = 1;
          }
        }
        else if(directionX == 1) // up right
        {
          if(bitRead(apple[positionX-1], positionY-1) == 0)
          {
            if(positionY == 0) // out
            {
              directionY = 1;
              directionX = 2;
            }
            else{
              bitWrite(apple[positionX], positionY, 0);
              positionX -= 1;
              positionY -= 1;
              bitWrite(apple[positionX], positionY, 1);
            }
          }
          else if(bitRead(apple[positionX-1], positionY) == 1 && bitRead(apple[positionX+1], positionY-2) == 1) // middle of paddle
          {
            directionY = 0;
          }
          else if(bitRead(apple[positionX-1], positionY) == 1 && bitRead(apple[positionX+1], positionY-2) == 0) // right corner
          {
            directionY = 0;
            directionX = 2;
          }
          else if(bitRead(apple[positionX-1], positionY) == 0) // left corner
          {
            directionY = 0;
            directionX = 2;
          }
        }
     } 
    }
};


Paddle p1;
Paddle p2;
Ball ball;


void movepaddle(int buttonState, int oldButtonState, int direction, int player){
  if (buttonState != oldButtonState &&
      buttonState == HIGH)
  {
    p1.tick(direction, player);
    delay(50);
  }
}  

void   SendData(uint8_t address, uint8_t value) {  
  digitalWrite(CS, LOW);   
   SPI.transfer(address);      // Send address.
  SPI.transfer(value);        //   Send the value.
  digitalWrite(CS, HIGH); // Finish transfer.
}
void setup()   {
  Serial.begin(9600);
  p1.positionX = 0;
  p2.positionX = 7;

  pinMode(CS, OUTPUT);  
  SPI.setBitOrder(MSBFIRST);   // Most significant   bit first 
  SPI.begin();                 // Start SPI
  SendData(DISPLAY_TEST,   0x01);       // Run test - All LED segments lit.
  delay(1000);
  SendData(DISPLAY_TEST,   0x00);           // Finish test mode.
  SendData(DECODE_MODE, 0x00);            //   Disable BCD mode. 
  SendData(INTENSITY, 0x0e);              // Use lowest intensity.   
  SendData(SCAN_LIMIT, 0x0f);             // Scan all digits.
  SendData(SHUTDOWN,   0x01);               // Turn on chip.
}
void loop()  {
  int buttonLeftState_P1 = digitalRead(buttonPinLeft_P1); // Take a reading
  int buttonRightState_P1 = digitalRead(buttonPinRight_P1);
  int buttonLeftState_P2 = digitalRead(buttonPinLeft_P2); // Take a reading
  int buttonRightState_P2 = digitalRead(buttonPinRight_P2);

  if(Serial.available())
  {
    if(Serial.read() == 0x44)
    {
      // delay(100);
      for(int i=0; i<8; i++)
      {
        apple[i] = Serial.read();
        // delay(100);
      }
    }
    delay(100);
  }

  
  for (int i=1;i<9;i++){
    SendData(i, apple[i-1]);
  }

  // ball.out_if();
  ball.tick();
  
   movepaddle(buttonLeftState_P2, oldButtonLeftState_P2, 1, 2);
  oldButtonLeftState_P2 = buttonLeftState_P2;
  movepaddle(buttonRightState_P2, oldButtonRightState_P2, 0, 2);
  oldButtonRightState_P2 = buttonRightState_P2;

  movepaddle(buttonLeftState_P1, oldButtonLeftState_P1, 1, 1);
  oldButtonLeftState_P1 = buttonLeftState_P1;
  movepaddle(buttonRightState_P1, oldButtonRightState_P1, 0, 1);
  oldButtonRightState_P1 = buttonRightState_P1;
  

  for(int i=0; i<9; i++){
      if(i == 0)
      {
        sendAddress[0] = 0x44;
      }
      else
      {
        sendAddress[i] = apple[i-1];
      }
    }
    
  Serial.write(sendAddress, 9);
  delay(500);

}