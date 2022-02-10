#ifndef Q_H
#define Q_H

#include <vector>
#include <stdlib.h>
#include <cmath>
#include "input.h"

using namespace std;

/*
struct status
{
    bool wall_east = false;
    bool wall_west = false;
    bool wall_north = false;
    bool wall_south = false;
    bool food_east = false;
    bool food_west = false;
    bool food_north = false;
    bool food_south = false;
    bool direction_east = false;
    bool direction_west = false;
    bool direction_north = false;
    bool direction_south = false;
};*/

#define STATUS_COUNT 12
#define ACTION_COUNT 4

enum status
{
    WALL_EAST = 0,
    WALL_WEST,
    WALL_NORTH,
    WALL_SOUTH,
    FOOD_EAST,
    FOOD_WEST,
    FOOD_NORTH,
    FOOD_SOUTH,
    DIRECTION_EAST,
    DIRECTION_WEST,
    DIRECTION_NORTH,
    DIRECTION_SOUTH
};


class Q_network
{

private:

    vector<bool> status;
    

    std::vector<std::vector<struct node>> matris;


public:
    Q_network();
    ~Q_network();

    void predict();
    void train();




    void set_learning_rate(float lr);
    void set_discount_rate(float dr);
    int get_max_action(int);
    int get_random_action(int status);
    int get_action(int);
    int select_target(int target);
    void print();
    int train(int old_status, int old_action, int new_status,double reward);
    int get_r(int status, int action);
    float get_q(int status, int action);
    void set_r(int status, int action, int value);
    void set_q(int status, int action, float value);
    void save();
    void load();
    void write_file_r();
    void write_file_q();
    void set_status(enum status);
    void reset_status();
    vector<enum status> get_status();
    void reset_danger();
    void print_status(int status);
    float get_action_ratio(int status);
};

#endif