#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <array>
#include <cmath>
#include <thread>
#include <assert.h>

using namespace std;

void print_v(vector<double> v);

struct max
{
    int id;
    double value;
};

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

enum DirectionNetwork
{
    FORWARD,
    BACKWARD
};

enum Activation
{
    RELU,
    LEAKY_RELU,
    SIGMOID,
    TANH,
    STEP,
    LINEAR,
    SWISH,
    IDENTITY
};

class Node
{
public:
    vector<double> weights = {};
    double output;

    enum Activation activation;
    double value = 0;
    double delta = 0;

    Node(int input_size, enum Activation act, int layer);
    ~Node();
    void run(vector<double> input);
    void backward(double target);

    double actvation_calc(double input);
    double derivative_calc();
};

class Network
{

private:
    vector<vector<Node>> net;

    double act_sigmoid(double input, enum DirectionNetwork direct);
    double act_relu(double input, enum DirectionNetwork direct);
    double act_tanh(double input, enum DirectionNetwork direct);

    double activation(Node node, enum Activation act);
    double derivative(Node node, enum Activation act);
    double cost(int id, double target);

public:
    Network();
    ~Network();
    void save();
    bool load();
    double backward(int id, double target,double error_limit);
    void createLayer(int node_size, enum Activation activation);
    struct max predict(vector<double> input);
    void train(vector<double> input, int id, double target);
    void update();
    void train(vector<double> input, int action, double target, double error);
    void print();
};