#include "ysa.h"

void print_v(vector<double> v)
{
    for (auto &i : v)
        cout << i << " ";
    cout << endl;
}

double identity(double input, enum DirectionNetwork direct)
{
    double ret;

    if (direct == FORWARD)
        ret = input;

    if (direct == BACKWARD)
        ret = 1;

    return ret;
}

double sigmoid(double input, enum DirectionNetwork direct)
{
    double ret;

    if (direct == FORWARD)
        ret = 1 / (1 + exp(-input));

    if (direct == BACKWARD)
        ret = input * (1 - input);

    return ret;
}

double relu(double input, enum DirectionNetwork direct)
{
    double ret;
    if (direct == FORWARD)
    {
        if (input >= 0)
            ret = input;
        else
            ret = 0;
    }

    if (direct == BACKWARD)
    {
        if (input >= 0)
            ret = 1;
        else
            ret = 0;
    }

    return ret;
}

double tanh(double input, enum DirectionNetwork direct)
{
    double ret;
    if (direct == FORWARD)
        ret = (2 / (1 + exp(-2 * input))) - 1;

    if (direct == BACKWARD)
        ret = 1 - pow(input, 2);

    return ret;
}

Node::Node(int input_size, enum Activation act, int layer)
{
    activation = act;
    srand(time(NULL));
    for (int j = 0; j < input_size; j++)
    {
        double random_w;
        if (layer == 0)
            random_w = 1;
        else
            random_w = (double)(rand() % 100) / 100;

        //cout<<layer<<" "<<j<<" "<<random_w<<endl;
        weights.push_back(random_w);
    }
}

Node::~Node()
{
}

double Node::actvation_calc(double input)
{
    double ret;
    switch (activation)
    {
    case SIGMOID:
        ret = sigmoid(input, FORWARD);
        break;
    case TANH:
        ret = tanh(input, FORWARD);
        break;
    case RELU:
        ret = relu(input, FORWARD);
        break;
    case IDENTITY:
        ret = identity(input, FORWARD);
        break;
    default:
        ret = sigmoid(input, FORWARD);
        break;
    }
    return ret;
}

double Node::derivative_calc()
{
    double ret;
    switch (activation)
    {
    case SIGMOID:
        ret = sigmoid(output, BACKWARD);
        break;
    case TANH:
        ret = tanh(output, BACKWARD);
        break;
    case RELU:
        ret = relu(output, BACKWARD);
        break;
    case IDENTITY:
        ret = identity(output, BACKWARD);
        break;
    default:
        ret = sigmoid(output, BACKWARD);
        break;
    }
    return ret;
}

void Node::run(vector<double> input)
{
    assert(input.size() == weights.size());
    double total = 0;

    for (int i = 0; i < weights.size(); i++)
    {
        total += input[i] * weights[i];
    }

    output = actvation_calc(total);
}

Network::Network()
{
}

Network::~Network()
{
}

void Network::train(vector<double> input, int action, double target, double error)
{
    double total = 999999;
    //while(total>error){
    predict(input);
    total = backward(action, target,error);
    update();
    cout << "Error : " << total << " Acton : " << action << " Target : " << target << endl;
    // }
}

void Network::update()
{
    for (int x = net.size() - 1; x > 0; x--)
    {
        for (int y = 0; y < net[x].size(); y++)
        {
            for (int k = 0; k < net[x][y].weights.size(); k++)
            {
                net[x][y].weights[k] += 0.7 * net[x][y].delta * net[x - 1][k].output;
                //cout<<net[x][y].weights[k]<<" ";
            }
            // cout<<endl<<"--------------------------------------------------"<<endl;
        }
    }
    save();
    //print();
}

double Network::cost(int id, double target)
{
    double total = 0;
    int output_layer_id = net.size() - 1;

    for (int i = 0; i < net[output_layer_id].size(); i++)
    {
        double t;

        if (target == 0 && id == i)
            t = 0;

        if (target == 0 && id != i)
            t = 1;

        if (target != 0 && id == i)
            t = target;

        if (target != 0 && id != i)
            t = 0;

        total += 0.5 * pow(t - net[output_layer_id][i].output, 2);
        net[output_layer_id][i].delta = (t - net[output_layer_id][i].output); //*net[output_layer_id][i].derivative_calc();
        //net[output_layer_id][i].delta =net[output_layer_id][i].output- (id == i ? target : !target);
        //cout << "Output error " << i << " : "<<t<<" - " << net[output_layer_id][i].delta << endl;
    }
    return total;
}

double Network::backward(int id, double target,double error_limit)
{
    int start_layer_id = net.size() - 1;

    double error = cost(id, target);

    //cout << "ID : " << id << " | Target : " << target << " | Error : " << error << endl;

    if(error < error_limit) return error;

    for (int i = start_layer_id; i > 0; i--)
    {
        for (int j = 0; j < net[i].size(); j++)
        {
            double total = 0;
            for (int k = 0; k < net[i][j].weights.size(); k++)
            {
                total += net[i][j].weights[k] * net[i][j].delta;
            }
            net[i - 1][j].delta = total * net[i][j].derivative_calc();
        }
    }

    return error;
}

void Network::createLayer(int node_size, enum Activation act)
{

    int node_input_count = 1;
    vector<Node> layer;

    if (net.size() > 0)
        node_input_count = net[net.size() - 1].size();

    for (int i = 0; i < node_size; i++)
    {
        Node node(node_input_count, act, net.size());
        layer.push_back(node);
    }

    net.push_back(layer);
}

struct max Network::predict(vector<double> input)
{

    assert(input.size() == net[0].size());

    cout << "Input :";
    print_v(input);

    vector<double> output;

    for (int j = 0; j < net[0].size(); j++)
    {
        net[0][j].run({input[j]});
        output.push_back(net[0][j].output);
    }

    for (int i = 1; i < net.size(); i++)
    {
        input = output;
        output.clear();
        for (int j = 0; j < net[i].size(); j++)
        {
            net[i][j].run(input);
            output.push_back(net[i][j].output);
        }
    }

    struct max m_max;

    m_max.id = 0;
    m_max.value = 0;

    for (int i = 0; i < output.size(); i++)
    {
        if (output[i] > m_max.value)
        {
            m_max.value = output[i];
            m_max.id = i;
        }
    }

    int random_act = rand() % 100;
    int try_ratio = (100 - (m_max.value * 100));

    if (random_act < try_ratio)
    {
        int r = random_act * 4 / 100;
        m_max.value = output[r];
        m_max.id = r;
    }
    else
        random_act = 0;

    cout << "Try Ratio : " << try_ratio << endl;
    cout << "Random : " << random_act << endl;
    cout << "Output :";
    print_v(output);
    return m_max;
}

void Network::print()
{
    for (int x = 0; x < net.size(); x++)
    {
        for (int y = 0; y < net[x].size(); y++)
        {
            cout << "[A: " << net[x][y].activation << ", D:" << net[x][y].delta << "] ";
        }

        cout << endl;
        for (int y = 0; y < net[x].size(); y++)
        {
            cout << "[[ ";
            for (int k = 0; k < net[x][y].weights.size(); k++)
            {
                cout << net[x][y].weights[k] << " ";
            }
            cout << "]]"
                 << " ";
        }
        cout << endl;
    }
}

void Network::save()
{

    FILE *fp = fopen("weights", "wb");

    int size = net.size();

    fwrite(&size, 1, sizeof(int), fp);

    for (int i = 0; i < size; i++)
    {
        int node_count = net[i].size();
        int act = net[i][0].activation;
        fwrite(&node_count, 1, sizeof(int), fp);
        fwrite(&act, 1, sizeof(int), fp);
    }

    for (int i = 0; i < net.size(); i++)
    {
        for (int j = 0; j < net[i].size(); j++)
        {
            for (int k = 0; k < net[i][j].weights.size(); k++)
            {
                fwrite(&net[i][j].weights[k], 1, sizeof(double), fp);
            }
        }
    }

    cout << "Saved" << endl;
    fclose(fp);
}

bool Network::load()
{

    FILE *fp = fopen("weights", "rb");
    if (fp == NULL)
        return false;

    int size;

    fread(&size, 1, sizeof(int), fp);

    for (int i = 0; i < size; i++)
    {
        int node_count;
        int act;
        fread(&node_count, 1, sizeof(int), fp);
        fread(&act, 1, sizeof(int), fp);
        createLayer(node_count, (enum Activation)act);
    }
    
    for (int i = 0; i < net.size(); i++)
    {
        for (int j = 0; j < net[i].size(); j++)
        {
            for (int k = 0; k < net[i][j].weights.size(); k++)
            {
                double w;
                fread(&w, 1, sizeof(double), fp);
                net[i][j].weights[k] = w;
            }
        }
    }

    fclose(fp);
    cout << "Loaded" << endl;
    return true;
}

/*

struct Network
{

    Network()
    {
    }

    float activation(float activation)
    {
        return 1 / (1 + exp(-activation));
    }
    float derivative(float out)
    {
        return out * (1 - out);
    }
    void print(vector<vector<Node>> &clone)
    {
        for (int x = 0; x < clone.size(); x++)
        {
            for (int y = 0; y < clone[x].size(); y++)
            {
                cout << "[A: " << clone[x][y].activation << ", D:" << clone[x][y].delta << "] ";
            }
            cout << endl;
            for (int y = 0; x != clone.size() - 1 && y < clone[x].size(); y++)
            {
                cout << "[[ ";
                for (int k = 0; k < clone[x][y].weights.size(); k++)
                {
                    cout << clone[x][y].weights[k] << " ";
                }
                cout << "]]"
                     << " ";
            }
            cout << endl;
        }
    }
    void init(vector<Sample> samples, vector<int> conf)
    {
        config = conf;
        config.insert(config.begin(), samples[0].input.size());
        config.push_back(classes.size());
        for (int x = 0; x < config.size(); x++)
        {
            model.push_back({});
            for (int y = 0; y < config[x]; y++)
            {
                model[x].push_back(Node());
                if (x != config.size() - 1)
                {
                    model[x][y].weights = vector<float>(config[x + 1]);
                    for (int k = 0; k < model[x][y].weights.size(); k++)
                    {
                        model[x][y].weights[k] = float(rand() % 200) / 100 - 1;
                    }
                }
            }
        }
        dataset = samples;
    }
    void propagate(vector<vector<Node>> &clone, Sample sample)
    {
        for (int y = 0; y < clone[0].size(); y++)
        {
            clone[0][y].value = sample.input[y];
        }
        for (int x = 0; x < clone.size(); x++)
        {
            for (int y = 0; y < clone[x].size(); y++)
            {
                clone[x][y].activation = activation(clone[x][y].value);
                for (int k = 0; k < clone[x][y].weights.size(); k++)
                {
                    clone[x + 1][k].value += clone[x][y].weights[k] * clone[x][y].activation;
                }
            }
        }
    }
    void backprop(vector<vector<Node>> &clone)
    {
        for (int x = clone.size() - 2; x >= 0; x--)
        {
            for (int y = 0; y < clone[x].size(); y++)
            {
                float total = 0;
                for (int k = 0; k < clone[x][y].weights.size(); k++)
                {
                    total += clone[x][y].weights[k] * clone[x + 1][k].delta;
                }
                clone[x][y].delta = total * derivative(clone[x][y].activation);
            }
        }
    }
    void update(vector<vector<Node>> &clone)
    {
        for (int x = clone.size() - 2; x >= 0; x--)
        {
            for (int y = 0; y < clone[x].size(); y++)
            {
                for (int k = 0; k < clone[x][y].weights.size(); k++)
                {
                    model[x][y].weights[k] += 0.01 * clone[x + 1][k].delta * clone[x][y].activation;
                }
            }
        }
    }
    float cost(vector<vector<Node>> &clone, Sample sample)
    {
        float total = 0;
        for (int i = 0; i < clone[clone.size() - 1].size(); i++)
        {
            total += 0.5 * pow((sample.expected == i ? 1 : 0) - clone[clone.size() - 1][i].activation, 2);
            clone[clone.size() - 1][i].delta = (sample.expected == i ? 1 : 0) - clone[clone.size() - 1][i].activation;
        }
        return total;
    }
    void train(float rate, float minError)
    {
        float totalError = INFINITY;
        int i;
        for (i = 0; minError < totalError; i++)
        {
            totalError = 0;
            for (int k = 0; k < dataset.size(); k++)
            {
                vector<vector<Node>> clone = model;
                propagate(clone, dataset[k]);
                float caseCost = cost(clone, dataset[k]);
                backprop(clone);
                update(clone);
                totalError += caseCost;
            }
            cout << "Epoch: " << i + 1 << ", Total Error: " << totalError << endl;
        }
        cout << "Finished training in " << i << " epochs" << endl;
    }

    void run(vector<float> input)
    {

        Sample _sample(input, 0);
        propagate(model, _sample);

        vector<Node> last_layer = model[model.size() - 1];

        for (auto node = last_layer.begin(); node < last_layer.end(); node++)
        {
            cout << node->value << " ";
        }

        cout << endl;
    }
};

/*
int main(){
    Network network=Network();
    vector<Sample> samples;
    set<int> classSet;

    samples.push_back(Sample({0.1,0.6,0.0,0.5,0.8},1));
    samples.push_back(Sample({0.2,0.7,0.9,0.4,0.8},2));
    samples.push_back(Sample({0.3,0.8,0.8,0.3,0.8},3));
    samples.push_back(Sample({0.4,0.9,0.7,0.2,0.8},4));
    samples.push_back(Sample({0.5,0.0,0.6,0.1,0.8},5));


    for(int i=0;i<samples.size();i++){
        classSet.insert(samples[i].expected);
    }
    classes=vector<int>(classSet.size());
    copy(classSet.begin(),classSet.end(),classes.begin());
    for(int i=0;i<samples.size();i++){
        samples[i].expected=distance(classes.begin(),find(classes.begin(),classes.end(),samples[i].expected));
    }
    cout<<"Ağ oluşturuluyor."<<endl;
    network.init(samples,{5,5});

    cout<<"Ağ eğitiliyor."<<endl;
    network.train(0.1,0.001);

    for(auto sample = samples.begin();sample < samples.end();sample++){
        cout<<sample->input[0]<<" "<<sample->input[1]<<" "<<sample->input[2]<<" "<<sample->input[3]<<" "<<sample->input[4]<<" "<<sample->expected<<endl;
    }

    cout<<"Ağ çalışıyor"<<endl;
    network.run({0.1,0.6,0.756,0.07656,0.8435});
    return 0;
}*/