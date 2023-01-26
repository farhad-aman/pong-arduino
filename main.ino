// Pong game in arduino

#include <stdlib.h>
#include <SPI.h>

// Defining the chip select pin
// #define CS 7

// MAX7219 Control Registers
#define DECODE_MODE 9
#define INTENSITY 10
#define SCAN_LIMIT 11
#define SHUTDOWN 12
#define DISPLAY_TEST 16

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
byte LED_matrix[8] = {B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000};

void send_data(uint8_t address, uint8_t value)
{
    digitalWrite(SSPin, LOW);  // start the transfer of data
    SPI.transfer(address);     // send the address
    SPI.transfer(value);       // send the value of data
    digitalWrite(SSPin, HIGH); // end the transfer of data
}

void update_led()
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            int map_value = int(view_map[i][j]);
            bitWrite(LED_matrix[i], j, map_value);
        }
        send_data(i + 1, LED_matrix[i]);
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
    Serial.begin(9600);
    pinMode(SSPin, OUTPUT);
    SPI.setBitOrder(MSBFIRST);     // MSB first
    SPI.begin();                   // Start SPI
    send_data(DISPLAY_TEST, 0x01); // Run a test - All LED segments lit
    delay(1000);
    send_data(DISPLAY_TEST, 0x00); // Finish test mode
    send_data(DECODE_MODE, 0x00);  // Disable BCD mode
    send_data(INTENSITY, 0x0e);    // Use lowest intensity
    send_data(SCAN_LIMIT, 0x0f);   // Scan all digits
    send_data(SHUTDOWN, 0x01);     // Turn on the chip

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
