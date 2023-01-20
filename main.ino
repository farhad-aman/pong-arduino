// Pong game in arduino

#include <stdlib.h>

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
    int is_up;
    int is_down;
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
} Map;

Ball ball;
Paddle paddle1;
Paddle paddle2;
Map map;

bool view_map[8][8];

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
            map.tiles[i][j].is_blocked = false;
        }
    }
    map.tiles[paddle1.x][paddle1.y].is_blocked = true;
    map.tiles[paddle1.x + 1][paddle1.y].is_blocked = true;
    map.tiles[paddle1.x + 2][paddle1.y].is_blocked = true;
    map.tiles[paddle2.x][paddle2.y].is_blocked = true;
    map.tiles[paddle2.x + 1][paddle2.y].is_blocked = true;
    map.tiles[paddle2.x + 2][paddle2.y].is_blocked = true;

    // Update View Map
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            view_map[i][j] = map.tiles[i][j].is_blocked;
        }
    }
    view_map[ball.x][ball.y] = true;
}

void game_tick()
{
    // Check Finish
    if (ball.y == 0)
    {
        // Player 2 wins

    }
    else if (ball.y == 7)
    {
        // Player 1 wins
    }

    // Reset Map
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            map.tiles[i][j].is_blocked = false;
        }
    }

    // Move Paddles
    if (paddle1.is_up)
    {
        paddle1.x -= 1;
    }
    else if (paddle1.is_down)
    {
        paddle1.x += 1;
    }
    paddle1.is_up = false;
    paddle1.is_down = false;
    map.tiles[paddle1.x][paddle1.y].is_blocked = true;
    map.tiles[paddle1.x + 1][paddle1.y].is_blocked = true;
    map.tiles[paddle1.x + 2][paddle1.y].is_blocked = true;

    if (paddle2.is_up)
    {
        paddle2.x -= 1;
    }
    else if (paddle2.is_down)
    {
        paddle2.x += 1;
    }
    paddle2.is_up = false;
    paddle2.is_down = false;
    map.tiles[paddle2.x][paddle2.y].is_blocked = true;
    map.tiles[paddle2.x + 1][paddle2.y].is_blocked = true;
    map.tiles[paddle2.x + 2][paddle2.y].is_blocked = true;

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
        if (map.tiles[new_x][new_y].is_blocked)
        {
            ball.ySpeed = -ball.ySpeed;
            new_y = 2;
        }
    }
    else if (new_y == 7)
    {
        if (map.tiles[new_x][new_y].is_blocked)
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
            view_map[i][j] = map.tiles[i][j].is_blocked;
        }
    }
    view_map[ball.x][ball.y] = true;
}

void setup()
{

}

void loop()
{

}