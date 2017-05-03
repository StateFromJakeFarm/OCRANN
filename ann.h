#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <utility>
#include <string>
#include <cmath>
#include <climits>
#include <cstdlib>

using namespace std;

class ANN {
    private:
    // alpha value
    long double alpha;
    // number of training iterations
    int k;

    // Neurons to compose the network
    struct Neuron {
        long double a;
        long double delta;
        vector<long double> weights;
    };

    // Actual ANN structure composed of neurons and weights
    vector< vector<Neuron> > layers;

    // Digit encodings
    vector< vector<long double> > encodings;

    // Train + test input/output
    vector< vector<long double> > trainIns;
    vector< vector<long double> > testIns;
    vector<int> trainOuts;
    vector<int> testOuts;

    // Construct ANN and get input/output data sets
    void constructLayers(char*);
    void getWeights(char*);
    void randWeights();
    void getCharEncodings(char*);
    void getIns(char*, vector< vector<long double> >&);
    void getOuts(char*, vector<int>&);

    // Print weights of first node to all nodes in next layer
    void printWeights();
    // Calculate the activation functions for all neurons
    void calcActivations(vector<long double>);
    // Run one iteration of back-propagation algo (updating weights)
    void backPropogate();
    // Compare output node values to digit encodings to classify the input
    void classify();

    // Run the ANN's main loop
    void main();

    public:
    // Constructor
    ANN(char*, char*, char*, char*, char*, char*, char*, long double, int);
};
