#include "raylib.h"
#include <iostream>
#include <deque>
#include <raymath.h>

#define TargetFPS 60
#define snakeMoveSpeed 0.15
using namespace std;

Color red = {255, 0, 0, 255};
Color green = {173, 204, 96, 255};
Color darkgreen = {43, 51, 24, 255};

int cellSize = 40;
int cellCount = 25;
int offset = 50; // in px
int res = cellCount * cellSize;
int score = 0;

double lastupTime = 0.0;
bool snakeSpeed(double interval){
    double currentTime = GetTime();
    if(currentTime - lastupTime >= interval)
    {
        lastupTime = currentTime;
        return true;
    }

    return false;
}

bool ElementInDeque(Vector2 element, deque<Vector2> deque){
    for(int i = 0; i < deque.size(); i++){
        if(Vector2Equals(deque[i], element)){
            return true;
        }
    }
    return false;
}

class Snake
{
    public:
        deque<Vector2> body = {Vector2{6, 5}, Vector2{5, 5}, Vector2{4, 5}};
        Vector2 direction = {1, 0};
        bool addSegment = false;

        void Draw(){
            for(int i = 0; i < body.size(); i++){
                int x = body[i].x;
                int y = body[i].y;
                DrawRectangle(offset + x * cellSize, offset + y * cellSize, cellSize, cellSize, darkgreen);
            }
        }

        void Update(){

            if (addSegment == true){
                //add a block to body
                body.push_front(Vector2Add(body[0], direction));
                addSegment = false;
            }
            else {
                // just move the snake
                body.pop_back();
                body.push_front(Vector2Add(body[0], direction));
            }
        }

        void reset(){
            body = {Vector2{6, 5}, Vector2{5, 5}, Vector2{4, 5}};
            direction = {1, 0};
        }
};

class Food
{
    public:
        Vector2 position;
        Texture2D texture;

        Food(deque<Vector2> snakebody){
            Image image = LoadImage("foodimg.jpg");
            texture = LoadTextureFromImage(image);
            UnloadImage(image);
            position = GenerateRandomPos(snakebody);

        }

        ~Food(){
            UnloadTexture(texture);
        }

        void Draw(){
            DrawTexture(texture, offset + position.x * cellSize, offset + position.y * cellSize, WHITE);
        }


        Vector2 GenerateRandomCell(){
            float x = GetRandomValue(0, cellCount - 1);
            float y = GetRandomValue(0, cellCount - 1);
            return Vector2{x, y};
        }

        Vector2 GenerateRandomPos(deque<Vector2> snakeBody){
            Vector2 position = GenerateRandomCell();
            while (ElementInDeque(position, snakeBody)) {
                position = GenerateRandomCell();
            }
            return position;
        }
};

class Game
{
    public:
        Snake snake = Snake();
        Food food = Food(snake.body);
        bool running = true;

        void Draw(){
            food.Draw();
            snake.Draw();
        }

        void Update(){
            if(running){
                snake.Update();
                CheckCollisionWithFood();
                checkCollisionWithEdges();
                checkCollisionWithBody();
            }
        }

        void CheckCollisionWithFood(){
            if (Vector2Equals(snake.body[0], food.position)){
                food.position = food.GenerateRandomPos(snake.body);
                snake.addSegment = true;
                score++;
            }
        }

        void checkCollisionWithEdges(){
            if(snake.body[0].x == cellCount || snake.body[0].x == -1){
                GameOver();
            }
            if(snake.body[0].y == cellCount || snake.body[0].y == -1){
                GameOver();
            }
        }

        void checkCollisionWithBody(){
            deque<Vector2> headlessbody = snake.body;
            headlessbody.pop_front();

            if(ElementInDeque(snake.body[0], headlessbody)){
                GameOver();
            }
        }

        void GameOver(){
            cout << "gameover" << endl;
            snake.reset();
            food.position = food.GenerateRandomPos(snake.body);
            running = false;
            score = 0;
        }
};

int main(void) {
    cout << "Starting..." << endl;
    InitWindow(2 * offset + res, 2 * offset + res, "Snake-Game");
    SetTargetFPS(TargetFPS);

    Game game = Game();

    while(WindowShouldClose() == false){

        BeginDrawing();



        if(snakeSpeed(snakeMoveSpeed))
        {
            game.Update();
        }

        ClearBackground(green);

        DrawRectangleLinesEx(Rectangle{(float)offset - 5, (float)offset - 5, (float)res + 10, (float)res + 10}, 5.0f, Color{0, 0, 0, 255});

        DrawText("Snake-Game", 50, offset - 40, 40, Color{0, 0, 0, 255});
        DrawText(TextFormat("%d", score), res - offset, offset - 40, 40, red);

        game.Draw();

        EndDrawing();

        // Input to move the Snake
        if (IsKeyPressed(KEY_UP) && game.snake.direction.y != 1){
            game.snake.direction = {0, -1};
            game.running = true;
        }
        if (IsKeyPressed(KEY_DOWN) && game.snake.direction.y != -1) {
            game.snake.direction = {0, 1};
            game.running = true;
        }
        if (IsKeyPressed(KEY_LEFT) && game.snake.direction.x != 1) {
            game.snake.direction = {-1, 0};
            game.running = true;
        }
        if(IsKeyPressed(KEY_RIGHT) && game.snake.direction.x != -1){
            game.snake.direction = {1, 0};
            game.running = true;
        }
    }

    CloseWindow();
    return 0;
}
