// Pong game in arduino

#include <stdlib.h>
#include <SPI.h>

// Defining the chip select pin
// #define CS 7

// MAX7219 Control Registers
// Opcodes for the MAX7221 and MAX7219
const uint16_t OP_NOOP = 0;
const uint16_t OP_DIGIT0 = 1;
// note: all OP_DIGITn are +n offsets from OP_DIGIT0
const uint16_t OP_DIGIT1 = 2;
const uint16_t OP_DIGIT2 = 3;
const uint16_t OP_DIGIT3 = 4;
const uint16_t OP_DIGIT4 = 5;
const uint16_t OP_DIGIT5 = 6;
const uint16_t OP_DIGIT6 = 7;
const uint16_t OP_DIGIT7 = 8;
const uint16_t OP_DECODEMODE = 9;
const uint16_t OP_INTENSITY = 10;
const uint16_t OP_SCANLIMIT = 11;
const uint16_t OP_SHUTDOWN = 12;
const uint16_t OP_DISPLAYTEST = 15;

// Players paddle button pin number
const int p1UpPin = A5;
const int p1DownPin = A4;
const int p2UpPin = 0;
const int p2DownPin = 1;

// SPI pins
const int SSPin = 10;   // LOAD
const int MOSIPin = 11; // DIN
const int MISOPin = 12; // not used!
const int SCKPin = 13;  // CLOCK

// #define HARDWARE_TYPE MD_MAX72XX::FC16_HW
// #define MAX_DEVICES 1

// MD_Parola mapDisplay = MD_Parola(HARDWARE_TYPE, MOSIPin, SCKPin, SSPin, MAX_DEVICES);

typedef struct
{
  int x;
  int y;
  int xSpeed;
  int ySpeed;
} Ball;

typedef struct
{
  int x;
  int y;
} Paddle;

typedef struct
{
  int x;
  int y;
  bool is_blocked;
} Tile;

typedef struct
{
  Tile tiles[8][8];
} GameMap;

Ball ball;
Paddle paddle1;
Paddle paddle2;
GameMap game_map;

bool view_map[8][8];
int LED_matrix[8];

void send_data(uint16_t cmd, uint8_t data)
// Send a simple command to the MAX7219
// using the hardware SPI interface
{
  uint16_t x = (cmd << 8) | data;
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  digitalWrite(SSPin, LOW);
  SPI.transfer16(x);
  digitalWrite(SSPin, HIGH);
  SPI.endTransaction();
}

void update_led()
{
  int x = 0b00000000;
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      int map_value = view_map[i][j];
      x |= (map_value << (8 - j));
      // example: if i = 0, j = 0, map_value = true --> x = (00000000) | (10000000) = (10000000)
      // example: if i = 0, j = 1, map_value = true --> x = (10000000) | (01000000) = (11000000)
      // example: if i = 0, j = 2, map_value = false --> x = (11000000) | (00000000) = (11000000)
    }
    LED_matrix[i] = x;
    send_data(OP_DIGIT0 + i, LED_matrix[i]);
    // send_data(i + 1, LED_matrix[i]);
  }
}

int random(int min, int max)
{
  return min + rand() % (max - min);
}

void start_game()
{
  // Randomize ball position between (3, 3), (4, 3), (3, 4), (4, 4)
  ball.x = random(3, 5);
  ball.y = random(3, 5);

  // Randomize ball xSpeed and ySpeed
  ball.xSpeed = random(0, 2) == 0 ? -1 : 1;
  ball.ySpeed = random(0, 2) == 0 ? -1 : 1;

  // Randomize paddle1 position between (2, 0), (3, 0)
  paddle1.x = random(2, 4);
  paddle1.y = 0;

  // Randomize paddle2 position between (2, 7), (3, 7)
  paddle2.x = random(2, 4);
  paddle2.y = 7;

  // Reset Map
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      game_map.tiles[i][j].is_blocked = false;
    }
  }
  game_map.tiles[paddle1.x][paddle1.y].is_blocked = true;
  game_map.tiles[paddle1.x + 1][paddle1.y].is_blocked = true;
  game_map.tiles[paddle1.x + 2][paddle1.y].is_blocked = true;
  game_map.tiles[paddle2.x][paddle2.y].is_blocked = true;
  game_map.tiles[paddle2.x + 1][paddle2.y].is_blocked = true;
  game_map.tiles[paddle2.x + 2][paddle2.y].is_blocked = true;

  // Update View Map
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      view_map[i][j] = game_map.tiles[i][j].is_blocked;
    }
  }
  view_map[ball.x][ball.y] = true;

  // Update 8x8 LED Matrix using SPI
  update_led();
}

int game_tick()
{
  // Check Finish
  if (ball.y == 0)
  {
    return 2;
  }
  else if (ball.y == 7)
  {
    return 1;
  }

  // Reset Map
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      game_map.tiles[i][j].is_blocked = false;
    }
  }

  // Update 8x8 LED Matrix using SPI
  update_led();

  // Move Paddles
  if (digitalRead(p1UpPin) == HIGH && paddle1.x > 0)
  {
    paddle1.x -= 1;
  }
  else if (digitalRead(p1DownPin) == HIGH && paddle1.x < 7)
  {
    paddle1.x += 1;
  }
  game_map.tiles[paddle1.x][paddle1.y].is_blocked = true;
  game_map.tiles[paddle1.x + 1][paddle1.y].is_blocked = true;
  game_map.tiles[paddle1.x + 2][paddle1.y].is_blocked = true;

  if (digitalRead(p2UpPin) == HIGH && paddle2.x > 0)
  {
    paddle2.x -= 1;
  }
  else if (digitalRead(p2DownPin) == HIGH && paddle2.x < 7)
  {
    paddle2.x += 1;
  }
  game_map.tiles[paddle2.x][paddle2.y].is_blocked = true;
  game_map.tiles[paddle2.x + 1][paddle2.y].is_blocked = true;
  game_map.tiles[paddle2.x + 2][paddle2.y].is_blocked = true;

  // Move Ball
  int new_x = ball.x + ball.xSpeed;
  int new_y = ball.y + ball.ySpeed;

  if (new_x < 0)
  {
    ball.xSpeed = -ball.xSpeed;
    new_x = 1;
  }
  else if (new_x > 7)
  {
    ball.xSpeed = -ball.xSpeed;
    new_x = 6;
  }

  if (new_y == 0)
  {
    if (game_map.tiles[new_x][new_y].is_blocked)
    {
      ball.ySpeed = -ball.ySpeed;
      new_y = 2;
    }
  }
  else if (new_y == 7)
  {
    if (game_map.tiles[new_x][new_y].is_blocked)
    {
      ball.ySpeed = -ball.ySpeed;
      new_y = 5;
    }
  }

  ball.x = new_x;
  ball.y = new_y;

  // Update View Map
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      view_map[i][j] = game_map.tiles[i][j].is_blocked;
    }
  }
  view_map[ball.x][ball.y] = true;

  // Update 8x8 LED Matrix using SPI
  update_led();

  return 0;
}

void setup()
{
  // mapDisplay.setIntensity(0); // brightness (0, 15)
  // mapDisplay.displayClear();

  pinMode(SSPin, OUTPUT);
  SPI.setBitOrder(MSBFIRST); // MSB first
  SPI.begin();               // Start SPI
  // Set the MAX7219 parameters
  send_data(OP_SHUTDOWN, 1);
  send_data(OP_DECODEMODE, 0xff);
  send_data(OP_SCANLIMIT, 0x0f); // Scan all digits
  send_data(OP_INTENSITY, 7);

  // turn on all LED for short time
  send_data(OP_DISPLAYTEST, 1);
  delay(500);
  send_data(OP_DISPLAYTEST, 0);

  // initialize the display
  for (int i = 0; i < 8; i++)
  {
    LED_matrix[i] = 0b00000000;
  }
  update_led();

  pinMode(p1UpPin, INPUT);
  pinMode(p1DownPin, INPUT);
  pinMode(p2UpPin, INPUT);
  pinMode(p2DownPin, INPUT);

  start_game();
}

unsigned long elapsedTime, previousTime = 0;
const unsigned long tickTime = 500;

void loop()
{
  elapsedTime = millis() - previousTime;
  if (elapsedTime >= tickTime)
  {
    int result = game_tick();
    if (result == 1)
    {
      Serial.println("Player 1 Wins!");
      start_game();
    }
    else if (result == 2)
    {
      Serial.println("Player 2 Wins!");
      start_game();
    }
    previousTime = millis();
  }
}
