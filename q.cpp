#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "q.h"
#include <iostream>
#include <fstream>

void Q_network::print_status(int status)
{

    if (status == WALL_WEST)
        std::cout << "Batı duvar";
    if (status == WALL_EAST)
        std::cout << "Doğu duvar";
    if (status == WALL_NORTH)
        std::cout << "Kuzey duvar";
    if (status == WALL_SOUTH)
        std::cout << "Güney duvar";
    if (status == FOOD_NORTH)
        std::cout << "Kuzey yem";
    if (status == FOOD_SOUTH)
        std::cout << "Güney yem";
    if (status == FOOD_WEST)
        std::cout << "Batı yem";
    if (status == FOOD_EAST)
        std::cout << "Doğu yem";
    if (status == DIRECTION_EAST)
        std::cout << "Doğu yön";
    if (status == DIRECTION_NORTH)
        std::cout << "Kuzey yön";
    if (status == DIRECTION_SOUTH)
        std::cout << "Güney yön";
    if (status == DIRECTION_WEST)
        std::cout << "Batı yön";
}

Q_network::Q_network()
{
    int status_size = pow(2, STATUS_COUNT);

    for (int i = 0; i < status_size; i++)
    {
        std::vector<node> actions(ACTION_COUNT, {.r = -1, .q = 0});
        this->matris.push_back(actions);
    }

    set_r(WALL_EAST, East, -1);
    set_r(WALL_WEST, West, -1);
    set_r(WALL_NORTH, North, -1);
    set_r(WALL_SOUTH, South, -1);

    set_r(FOOD_NORTH, North, 100);
    set_r(FOOD_SOUTH, South, 100);
    set_r(FOOD_WEST, West, 100);
    set_r(FOOD_EAST, East, 100);

    set_r(DIRECTION_WEST, East, -1);
    set_r(DIRECTION_EAST, West, -1);
    set_r(DIRECTION_SOUTH, North, -1);
    set_r(DIRECTION_NORTH, South, -1);

    load();
}

Q_network::~Q_network()
{
}

void Q_network::set_learning_rate(float lr)
{
    this->learning_rate = lr;
}

void Q_network::set_discount_rate(float dr)
{
    this->discount_rate = dr;
}

int Q_network::get_max_action(int status)
{

    float max_value = -999999999;
    int id = -1;

    for (int i = 0; i < this->matris[0].size(); i++)
        if (this->matris[status][i].q > max_value)
        {
            id = i;
            max_value = this->matris[status][i].q;
        }

    cout << "Max Action" << endl;
    return id;
}

float Q_network::get_action_ratio(int status)
{
    float avg = 0;
    float max = 0;
    float min=9999;
    float ret=0;

    for (int i = 0; i < this->matris[0].size(); i++)
        if (this->matris[status][i].q > max)
            max = this->matris[status][i].q;

    for (int i = 0; i < this->matris[0].size(); i++)
        if (this->matris[status][i].q < min)
            min = this->matris[status][i].q;


    for (int i = 0; i<this->matris[0].size(); i++)
        avg = (avg + this->matris[status][i].q) / 2;
    

    ret = (avg *100)/(max-min);
    cout<<"ret "<<max<<"-"<<min<<" -> "<<avg<<"="<<ret<<endl;

    ret = (ret<0)?0:ret;
    return ret;
}

int Q_network::get_random_action(int status)
{
    srand(time(NULL));
    int r = rand() % this->matris[0].size();

    cout << "Random Action" << endl;
    return r;
}

int Q_network::get_action(int status)
{
    int ret;
    int rand_select = rand() % 100;
    float ratio = get_action_ratio(status);

    cout << "Select ratio : " << rand_select <<" Limit Ratio : "<<ratio<<endl;

    if (rand_select < ratio)
        ret = get_random_action(status);
    else
        ret = get_max_action(status);

    return ret;
}

int Q_network::train(int old_status, int old_action, int new_status, double reward)
{
    float old_status_q = get_q(old_status, old_action);
    float new_status_q = get_q(new_status, get_max_action(new_status));
    //int old_status_r  = get_r(old_status, old_action);

    float action_rate = this->discount_rate * new_status_q;

    float learning_value = (this->learning_rate) * (reward + action_rate - old_status_q);

    float new_q = old_status_q + learning_value;

    std::cout << "Update " << old_status << "-" << old_action << " : " << old_status_q << "->" << new_q << std::endl;

    set_q(old_status, old_action, new_q);

    save();
    write_file_r();
    write_file_q();
    return 0;
}

float Q_network::get_q(int status, int action)
{
    return this->matris[status][action].q;
}
int Q_network::get_r(int status, int action)
{
    return this->matris[status][action].r;
}

void Q_network::set_q(int status, int action, float value)
{
    this->matris[status][action].q = value;
}
void Q_network::set_r(int status, int action, int value)
{
    this->matris[status][action].r = value;
}

void Q_network::print()
{
    for (int s = 0; s < this->matris.size(); s++)
    {
        printf("%d - ", s);
        for (int a = 0; a < this->matris[s].size(); a++)
        {
            printf("%d ", this->matris[s][a].r);
        }
        printf("\n");
    }
}

void Q_network::save()
{
    std::ofstream out("weights", std::ofstream::binary);

    for (int s = 0; s < this->matris.size(); s++)
        for (int a = 0; a < this->matris[s].size(); a++)
            out.write((const char *)&this->matris[s][a].q, sizeof(float));
}

void Q_network::load()
{
    FILE *fp;
    fp = fopen("weights", "rb");

    if (fp == NULL)
        return;

    for (int s = 0; s < this->matris.size(); s++)
        for (int a = 0; a < this->matris[s].size(); a++)
            fread(&this->matris[s][a].q, 1, sizeof(float), fp);

    fclose(fp);
}

void Q_network::write_file_q()
{
    int i = 0;
    std::ofstream outFile("q.txt");
    for (const auto &e : this->matris)
    {
        outFile << i << "\t";
        for (const auto &d : e)
        {
            outFile << d.q << "\t";
        }
        i++;
        outFile << std::endl;
    }
}

void Q_network::write_file_r()
{
    int i = 0;
    std::ofstream outFile("r.txt");
    for (const auto &e : this->matris)
    {
        outFile << i << "-";
        for (const auto &d : e)
        {
            outFile << d.r << " ";
        }
        i++;
        outFile << std::endl;
    }
}

/*
void q_add_connect(struct network *net, int s, int a)
{
    net->r[s][a] = 0;
    printf("%d->%d connected \n", s, a);
}

void q_add_target(struct network *net, int target)
{
    for (int s = 0; s < net->size; s++)
    {
        if (net->r[s][target] == 0)
            net->r[s][target] = 100;
    }
    net->r[target][target] = 100;
}

struct network *q_network_create(int size)
{
    struct network *net = (struct network *)malloc(sizeof(struct network));
    net->size = size;
    net->r = (int **)malloc(sizeof(int *) * net->size);
    net->q = (float **)malloc(sizeof(float *) * net->size);

    for (int s = 0; s < net->size; s++)
    {
        net->r[s] = (int *)malloc(sizeof(int) * net->size);
        net->q[s] = (float *)malloc(sizeof(float) * net->size);

        for (int a = 0; a < net->size; a++)
        {
            net->r[s][a] = -1;
            net->q[s][a] = 0;
        }
    }

    printf("Network created %dx%d\n", net->size, net->size);
    return net;
}

void q_network_free(struct network *net)
{

    for (int s = 0; s < net->size; s++)
    {
        free(net->r[s]);
        free(net->q[s]);
    }

    free(net->r);
    free(net->q);
}

int q_random_action(struct network *net, int s)
{
    int count = 0;
    int list[net->size];

    for (int i = 0; i < net->size; i++)
    {
        if (net->r[s][i] >= 0)
        {
            list[count] = i;
            count++;
        }
    }

    assert(count != 0);

    srand(time(NULL));
    int r = rand() % count;
    return list[r];
}

void q_network_print_r(struct network *net)
{
    for (int s = 0; s < net->size; s++)
    {
        for (int a = 0; a < net->size; a++)
        {
            printf("%d ", net->r[s][a]);
        }
        printf("\n");
    }
}

void q_network_print_q(struct network *net)
{
    for (int s = 0; s < net->size; s++)
    {
        for (int a = 0; a < net->size; a++)
        {
            printf("%f ", net->q[s][a]);
        }
        printf("\n");
    }
}

struct node q_get_action(struct network *net, int s)
{
    float max_value = 0;
    struct node st = {.id = -1, .value = max_value};

    for (int i = 0; i < net->size; i++)
    {
        if (net->r[s][i] != -1 && net->q[s][i] > max_value)
        {
            st.value = net->q[s][i];
            st.id = i;
            max_value = st.value;
        }
    }

    if (st.id == -1)
    {
        st.id = q_random_action(net, s);
    }

    printf("%d -> %d max value : %f \n", s, st.id, st.value);
    return st;
}*/