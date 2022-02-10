#include <stdio.h>
#include <string>
#include <iostream>
#include <cstdio>
#include <algorithm>
#include <unistd.h>
#include <bits/stdc++.h>
#include "input.h"
#include "snake.h"
#include "snake_map.h"
#include <utility>
#include "macros.h"
#include "ysa.h"
#include <stdlib.h>

#define STATUS_COUNT 12
#define ACTION_COUNT 4

using namespace std;

Snake *snake;
SnakeMap *snake_map;
Network net;

void initialize()
{
    input_init();
    input_enter_off();
}

void save_score()
{
    int max_score = 0;
    FILE *fp = fopen("max_score", "rb");
    if (fp != NULL)
    {
        fread(&max_score, 1, sizeof(int), fp);
        fclose(fp);
    }

    if (snake->length > max_score)
    {
        FILE *fp = fopen("max_score", "wb");
        fwrite(&snake->length, 1, sizeof(int), fp);
        fclose(fp);
        FILE *fpt = fopen("max_score.txt", "w");
        char s[5] = {0};
        sprintf(s, "%d", snake->length);
        fwrite(s, 1, 5, fpt);
        fclose(fpt);
    }
}

bool is_game_end()
{
    bool result = false;
    pair<int, int> snake_head = snake->snake_head;
    if (snake_head.first < 0 || snake_head.first >= MAP_WIDTH || snake_head.second < 0 || snake_head.second >= MAP_HEIGHT)
    {
        result = true;
    }
    if (snake->is_dead)
    {
        result = true;
    }
    return result;
}

void game_over()
{
    cout << "GAME IS OVER" << endl;
}

int is_snake_body(int snake_y, int snake_x, enum Direction direct)
{

    int ret = 0;
    int is_there = 0;

    if (direct == North)
    {
        for (int i = snake_y - 1; i >= 0; i--)
        {
            ret++;
            if (snake_map->map_array[i][snake_x] == SNAKE_CHAR)
                is_there = 1;
        }
    }

    if (direct == South)
    {
        for (int i = snake_y + 1; i < MAP_HEIGHT; i++)
        {
            ret++;
            if (snake_map->map_array[i][snake_x] == SNAKE_CHAR)
                is_there = 1;
        }
    }

    if (direct == West)
    {
        for (int i = snake_x - 1; i >= 0; i--)
        {
            ret++;
            if (snake_map->map_array[snake_y][i] == SNAKE_CHAR)
                is_there = 1;
        }
    }

    if (direct == East)
    {
        for (int i = snake_x + 1; i < MAP_WIDTH; i++)
        {
            ret++;
            if (snake_map->map_array[snake_y][i] == SNAKE_CHAR)
                is_there = 1;
        }
    }

    if (is_there != 1)
        ret = 0;

    return ret;
}

vector<double> get_status()
{
    vector<double> status(STATUS_COUNT, 0);

    pair<int, int> snake_head = snake->snake_head;
    pair<int, int> food = snake->snake_food;

    int snake_x = snake_head.second;
    int snake_y = snake_head.first;

    int food_x = food.second;
    int food_y = food.first;

    if (snake_y - 1 < 0 || is_snake_body(snake_y, snake_x, North) > 0) //snake_map->map_array[snake_y - 1][snake_x] == SNAKE_CHAR)
    {
        status[WALL_NORTH] = 1;
        //cout << "Wall North" << endl;
    }

    if (snake_y + 1 >= MAP_HEIGHT || is_snake_body(snake_y, snake_x, South) > 0) //snake_map->map_array[snake_y + 1][snake_x] == SNAKE_CHAR)
    {
        status[WALL_SOUTH] = 1;
        //cout << "Wall South" << endl;
    }

    if (snake_x - 1 < 0 || is_snake_body(snake_y, snake_x, West) > 0) //snake_map->map_array[snake_y][snake_x - 1] == SNAKE_CHAR)
    {
        status[WALL_WEST] = 1;
        //cout << "Wall West" << endl;
    }

    if (snake_x + 1 >= MAP_WIDTH || is_snake_body(snake_y, snake_x, East) > 0) //snake_map->map_array[snake_y][snake_x + 1] == SNAKE_CHAR)
    {
        status[WALL_EAST] = 1;
        //cout << "Wall East" << endl;
    }

    if (food_y < snake_y)
    {
        status[FOOD_NORTH] = 1;
        //cout << "Food North" << endl;
    }

    if (food_y > snake_y)
    {
        status[FOOD_SOUTH] = 1;
        //cout << "Food South" << endl;
    }

    if (food_x < snake_x)
    {
        status[FOOD_WEST] = 1;
        //cout << "Food West" << endl;
    }

    if (food_x > snake_x)
    {
        status[FOOD_EAST] = 1;
        //cout << "Food East" << endl;
    }

    if (snake->direction == East)
    {
        status[DIRECTION_EAST] = 1;
        //cout << "Direction East" << endl;
    }

    if (snake->direction == South)
    {
        status[DIRECTION_SOUTH] = 1;
        //cout << "Direction South" << endl;
    }

    if (snake->direction == West)
    {
        status[DIRECTION_WEST] = 1;
        //cout << "Direction West" << endl;
    }

    if (snake->direction == North)
    {
        status[DIRECTION_NORTH] = 1;
        //cout << "Direction North" << endl;
    }

    //status[LOCATION)+snake_y + snake_x;

    return status;
}

enum Direction ai2snake_action(int id)
{
    enum Direction result;
    switch (id)
    {
    case 0:
        result = West;
        break;
    case 1:
        result = North;
        break;
    case 2:
        result = East;
        break;
    case 3:
        result = South;
        break;
    default:
        result = Error;
        break;
    }
    return result;
}

double get_reward()
{
    double ret = 1;

    pair<int, int> snake_head = snake->snake_head;
    pair<int, int> food = snake->snake_food;

    int snake_x = snake_head.second;
    int snake_y = snake_head.first;

    int food_x = food.second;
    int food_y = food.first;

    if (food.first == snake_head.first && food.second == snake_head.second)
        ret += 10;

    if (snake->direction == East && food_x > snake_x)
        ret += 6;

    if (snake->direction == South && food_y > snake_y)
        ret += 6;

    if (snake->direction == West && food_x < snake_x)
        ret += 6;

    if (snake->direction == North && food_y < snake_y)
        ret += 6;

    if (snake->direction == East && food_y == snake_y && food_x > snake_x)
        ret += 8;

    if (snake->direction == South && food_x == snake_x && food_y > snake_y)
        ret += 8;

    if (snake->direction == West && food_y == snake_y && food_x < snake_x)
        ret += 8;

    if (snake->direction == North && food_x == snake_x && food_y < snake_y)
        ret += 8;

    cout << "in Reward : " << ret << endl;
    return (ret / 10);
}

void start_game()
{

    while (true)
    {
        cout << "--------------------------------" << endl;
        vector<double> status = get_status();

        struct max m_max = net.predict(status);
        int action = m_max.id;

        enum Direction action_d = ai2snake_action(action);
        snake->update_direction(action_d);

        snake->update_movement();

        vector<double> new_status = get_status();

        double reward = get_reward();
        struct max new_max = net.predict(new_status);

        if (is_game_end())
        {
            game_over();
            reward = 0;
            //cout << "Status:" << status << endl;
            cout << "Action:" << action << endl;
            cout << "Reward:" << reward << endl;
            cout << "X:" << snake->snake_head.first << " Y:" << snake->snake_head.second << endl;

            net.train(status, action, 0, 0.1);

            save_score();
           // sleep(1);
            break;
        }
        //ai.print();

        snake_map->redraw();

        //cout << "Old Status:" << status << endl;
        cout << "Action:" << action << endl;
        //cout << "New Status:" << new_status << endl;
        cout << "Reward:" << reward << endl;
        cout << "Food X : " << snake->snake_food.second << " Y : " << snake->snake_food.first << endl;

        net.train(status, action, reward, 0.1);

        //usleep(PAUSE_LENGTH);

        //snake->validate_direction();
    }
}

int main()
{
    if (!net.load())
    {
        net.createLayer(STATUS_COUNT, SIGMOID);
        net.createLayer(15, SIGMOID);
        net.createLayer(ACTION_COUNT, SIGMOID);
    }

    initialize();
    while (1)
    {
        snake = new Snake();
        snake_map = new SnakeMap(snake);
        start_game();
    }

    return 0;
}