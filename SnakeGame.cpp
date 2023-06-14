#include "raylib.h"
#include <iostream>
#include <deque>


using namespace std;

//Define colors and its adjustment to use in game loop
Color green = { 173,204,96,255 };
Color darkGreen = { 43,51,24,255 };

//Declaring and initializing global variables 
int cellSize = 30;
int cellCount = 25;
int offset = 75;
double lastUpdateTime = 0;

// Function to check if a certain time interval has passed since the last event
bool eventTriggered(double interval)
{
    double currentTime = GetTime();
    if (currentTime-lastUpdateTime >= interval)
    {
        lastUpdateTime = currentTime;
        return true;
    }
    return false;
}

// Function to check if a Vector2 element exists in a deque
bool ElementInDeque(Vector2 element,deque<Vector2> deque)
{
    for (unsigned int i = 0; i <deque.size(); i++)
    {
        if (deque[i].x == element.x && deque[i].y == element.y)
        {
            return true;
        }
    }
    return false;
}

// Snake class
class Snake 
{
public:
    deque<Vector2> body = { Vector2{6,9},Vector2{5,9},Vector2{4,9} };

    Vector2 direction = { 1,0 };

    bool addSegment = false;

    // Function to draw the snake
    void DrawSnake()
    {
        for (unsigned int i = 0; i < body.size(); i++)
        {
            float x = body[i].x;
            float y = body[i].y;
            Rectangle segment = Rectangle{ offset + x * cellSize, offset + y * cellSize,(float)cellSize,(float)cellSize };
            DrawRectangleRounded(segment, 0.5, 6, darkGreen);
        }
    }

    // Function to update the snake's position
    void Update()
    {
        Vector2 newHead = { body[0].x + direction.x, body[0].y + direction.y }; // Calculate the position of the new head

        // Add the new head to the front of the body
        body.push_front(newHead);

        if (addSegment == true)
        {
            addSegment = false;
        }
        else
        {
            // Remove the last item from the body
            body.pop_back();
        }   
    }
    // Function to reset the snake's position and direction
    void Reset()
    {

        body = { Vector2{6,9},Vector2{5,9},Vector2{4,9} };
        direction = { 1,0 };

    }


};

//Food class
class Food 
{

public:
    Vector2 position;
    Texture2D texture;
     
    // Constructor of food class
    Food(deque<Vector2> snakeBody) 
    {

        
        Image image = LoadImage("Graphics/food.png");
        texture=LoadTextureFromImage(image);
        UnloadImage(image);
        position = GenerateRandomPos(snakeBody);

    }
    // Destructor of food class
   ~Food() {

        UnloadTexture(texture);
    }
    
   // Function to draw the food
    void DrawFood() {
        DrawTexture(texture, offset + position.x * cellSize, offset + position.y * cellSize, WHITE);
    }
    // Function to generate a random cell position
    Vector2 GenerateRandomCell()
    {
        float x = GetRandomValue(0, cellCount - 1);
        float y = GetRandomValue(0, cellCount - 1);
        return Vector2{ x,y };
    }
    // Function to generate a random position for the food, avoiding the snake's body
    Vector2 GenerateRandomPos(deque<Vector2> snakeBody)
    {
         
        Vector2 position = GenerateRandomCell();
        while (ElementInDeque(position, snakeBody))
        {
            position = GenerateRandomCell();
        }
        return position;
    }

};

// Game class
class Game
{
public:
    Snake snakeObj = Snake();
    Food foodObj = Food(snakeObj.body);
    bool running = true;
    int score = 0;
    Sound eatSound;
    Sound wallSound;

    // Constructor of game class
    Game()
    {
        InitAudioDevice();
        eatSound = LoadSound("Sounds/eat.mp3");
        wallSound = LoadSound("Sounds/wall.mp3");
    }
    // Destructor of game class
    ~Game()
    {
        UnloadSound(eatSound);
        UnloadSound(eatSound);
        CloseAudioDevice();
    }
    // Function to draw the game elements
    void Draw()
    {
        foodObj.DrawFood();
        snakeObj.DrawSnake();
    }
    // Function to update the game logic
    void Update()
    {
        if (running)
        {
            snakeObj.Update();
            CheckCollisionWithFood();
            CheckCollisionWithEdges();
            CheckCollisionWithTail();
        }
    }
    // Function to check collision with the food
    void CheckCollisionWithFood()
    {
        if (snakeObj.body[0].x == foodObj.position.x && snakeObj.body[0].y == foodObj.position.y)
        {
            cout << "Eating Food" << endl;
            foodObj.position = foodObj.GenerateRandomPos(snakeObj.body);
            snakeObj.addSegment = true;
            score++;
            PlaySound(eatSound);
        }
    }
    // Function to check collision with the edges of the screen
    void CheckCollisionWithEdges()
    {
        if (snakeObj.body[0].x == cellCount || snakeObj.body[0].x == -1)
        {
            GameOver();
        }
        if (snakeObj.body[0].y == cellCount || snakeObj.body[0].y == -1)
        {
            GameOver();
        }
    }
    // Function to handle game over
    void GameOver()
    {

        cout << "Game Over" << endl;
        snakeObj.Reset();
        foodObj.position = foodObj.GenerateRandomPos(snakeObj.body);
        running = false;
        score = 0;
        PlaySound(wallSound);
    }
    // Function to check collision with the snake's tail
    void CheckCollisionWithTail()
    {
        deque<Vector2> headlessBody = snakeObj.body;
        headlessBody.pop_front();
        if (ElementInDeque(snakeObj.body[0],headlessBody))
        {
            GameOver();
        }
    }
};




int main(void)
{

    cout << "Starting The Game..." << endl;

    InitWindow(2 * offset + cellSize * cellCount, 2 * offset + cellSize * cellCount, "Retro Snake!");

    SetTargetFPS(60); // Function to define a frame rate to make game runs at the same speed on every computer... 

    
    Game game = Game();

    while (!WindowShouldClose())// Execute the statements of control structure body as long as escape key or close icon is not pressed...
    {
        BeginDrawing(); // Function to create a blank canvas (drawing)...
        
     

        //Snake movement depends on conditions...
        if (IsKeyPressed(KEY_UP) && game.snakeObj.direction.y != 1)
        {
            game.snakeObj.direction = { 0,-1 };
            game.running = true;
        }
        else if (IsKeyPressed(KEY_DOWN) && game.snakeObj.direction.y != -1)
        {
            game.snakeObj.direction = { 0,1 };
            game.running = true;
        }
        else if (IsKeyPressed(KEY_LEFT) && game.snakeObj.direction.x != 1)
        {
            game.snakeObj.direction = { -1,0 };
            game.running = true;
        }
        else if (IsKeyPressed(KEY_RIGHT) && game.snakeObj.direction.x != -1)
        {
            game.snakeObj.direction = { 1,0 };
            game.running = true;
        }
        else 
        {
            if (eventTriggered(0.2))
            {
                game.Update();
            }
        }
        

        //drawing...
        ClearBackground(green);
        DrawRectangleLinesEx(Rectangle{(float)offset-5,(float) offset-5,(float) cellSize * cellCount + 10,
            (float) cellSize * cellCount + 10}, 5, darkGreen);
        
        DrawText("Retro Snake", offset - 5, 20, 40, darkGreen);
        DrawText(TextFormat("%i", game.score), offset - 5, offset + cellSize * cellCount + 10, 40, darkGreen);
        game.Draw();

        EndDrawing();// Function to end drawing...
    }

    CloseWindow();

    return 0;
}