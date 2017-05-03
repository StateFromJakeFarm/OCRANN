/**
 * @file ann.cpp Definitions for ANN class defined in 'ann.h'
 *
 * @author Jacob Hansen
 * @date 4/14/17
 **/

#include "ann.h"

/**
 * Create the layers of the ANN to store all of our Neurons.
 *
 * @param fname A character pointer to the name of the 'structure' file.
 **/
void ANN::constructLayers(char* fname) {
    layers.resize(1);

    ifstream f(fname);

    int numNodes;
    int numLayers = 1;
    while(f >> numNodes) {
        ++numLayers;
        layers.resize(numLayers);
        layers[numLayers-1].resize(numNodes);
    }

    f.close();
}

/**
 * Set all the starting weights within the ANN.
 *
 * @param fname A character pointer to the name of the 'weights' file.
 **/
void ANN::getWeights(char* fname) {
    ifstream f(fname);

    int numNeurons = 0;
    long double curWeight;
    // loop through all layers except output layer
    for(unsigned int l=1; l<layers.size()-1; l++) {
        for(unsigned int n=0; n<layers[l].size(); n++) {
            // for each node in next layer, add weight to vector for
            // current node
            for(unsigned int i=0; i<layers[l+1].size(); i++) {
                f >> curWeight;
                layers[l][n].weights.push_back(curWeight);
            }

            ++numNeurons;
        }
    }

    f.close();

    // Add dummy weights
    layers[0].resize(1);
    for(int i=layers[1].size(); i<numNeurons+layers[layers.size()-1].size(); i++)
        layers[0][0].weights.push_back(0.01);
}

void ANN::randWeights() {
    int numNeurons = 0;
    long double curWeight;
    // loop through all layers except output layer
    for(unsigned int l=1; l<layers.size()-1; l++) {
        for(unsigned int n=0; n<layers[l].size(); n++) {
            // for each node in next layer, add weight to vector for
            // current node
            for(unsigned int i=0; i<layers[l+1].size(); i++) {
                curWeight = 0.01;  // CHANGE THIS //
                cout << curWeight << endl;
                layers[l][n].weights.push_back(curWeight);
            }

            ++numNeurons;
        }
    }

    // Add dummy weights
    layers[0].resize(1);
    for(int i=layers[1].size(); i<numNeurons+layers[layers.size()-1].size(); i++)
        layers[0][0].weights.push_back(0.01);
}

/**
 * Retrieve the 10-value digit encodings for all 10 digits.
 *
 * @param fname A character pointer to the name of the 'encodings' file.
 **/
void ANN::getDigitEncodings(char* fname) {
    ifstream f(fname);

    encodings.resize(10);

    long double curVal;
    for(int d=0; d<10; d++) {
        for(int i=0; i<10; i++) {
            f >> curVal;
            encodings[d].push_back(curVal);
        }
    }

    f.close();
}

/**
 * Retrieve the values for the input layer nodes from either the train or test
 * input files.  These values will serve as the activation values for the
 * first layer of neurons.
 *
 * @param fname A character pointer to the name of the 'inputs' file.
 * @param inputs A reference to a 2-dimensional vector of long double type
 *        where the values will be stored.
 **/
void ANN::getIns(char* fname, vector< vector<long double> > &inputs) {
    ifstream f(fname);

    int numInputNeurons = layers[1].size();

    int curLine = 0;
    string line;
    long double curVal;
    while(getline(f, line)) {
        ++curLine;
        inputs.resize(curLine);

        istringstream ss(line);

        for(int i=0; i<numInputNeurons; i++) {
            ss >> curVal;
            inputs[curLine-1].push_back(curVal);
        }
    }

    f.close();
}

/**
 * Retrieve the proper digit output values for each training or test input.
 *
 * @param fname A character pointer to the name of the outputs file.
 * @param outputs A vector of integer reference to contain the the correct
 *        digit classification for each input.
 **/
void ANN::getOuts(char* fname, vector<int> &outputs) {
    ifstream f(fname);

    long double curVal;
    while(f >> curVal)
        outputs.push_back(curVal);

    f.close();
}

/**
 * Print out the weights from the first nueron in the input layer to all
 * nuerons in the second (hidden) layer.
 **/
void ANN::printWeights() {
    for(unsigned int j=0; j<layers[2].size(); j++)
        cout << showpoint << fixed << setprecision(12) << layers[1][0].weights[j] << " ";
    cout << endl;
}

/**
 * Constructor for the ANN object.  Will both construct the object and call
 * the main instruction set.
 *
 * @param train_input A character pointer to the name of the 'train_inputs' file.
 * @param train_out A character pointer to the name of the 'train_output' file.
 * @param test_input A character pointer to the name of the 'test_input' file.
 * @param test_out A character pointer to the name of the 'test_outputs' file.
 * @param structure A character pointer to the name of the 'structure' file.
 * @param weights A character pointer to the name of the 'weights' file OR
 *        'RANDOM' to generate random starting weights
 * @param encoding A character pointer to the name of the 'encodings' file.
 * @param a A long double for the alpha.
 * @param numIters the number of iterations to run the backpropogation algo.
 *
 * @return An ANN object.
 **/
ANN::ANN(char* train_input, char* train_out, char* test_input, char* test_out, char* structure, char* weights, char* encoding, long double a, int numIters): alpha(a), k(numIters) {
    constructLayers(structure);

    if(weights != "RANDOM")
        getWeights(weights);
    else
        randWeights();
    getDigitEncodings(encoding);
    getOuts(train_out, trainOuts);
    getOuts(test_out, testOuts);
    getIns(train_input, trainIns);
    getIns(test_input, testIns);

    main();
}

/**
 * Set the activation values for the input layer and then calculate the values
 * for all of the following layers.
 *
 * @param ins A vector of long doubles containing the activation values for the
 *        input layer.
 **/
void ANN::calcActivations(vector<long double> ins) {
    // Set input activation values to input vector values (1)
    for(int n=0; n<layers[1].size(); n++) {
        layers[1][n].a = ins[n];
    }

    // Get "in" values and activation functions (2 and 3)
    int curNeuron = 0;
    for(unsigned int l=2; l<layers.size(); l++) {
        for(unsigned int n=0; n<layers[l].size(); n++) {
            // Get "in" value for this neuron
            long double in = layers[0][0].weights[curNeuron];
            for(unsigned int j=0; j<layers[l-1].size(); j++)
                in += layers[l-1][j].weights[n] * layers[l-1][j].a;

            // Get activation function for this neuron
            layers[l][n].a = 1 / (1 + exp(-1 * in));
            ++curNeuron;
        }
    }
}

/**
 * Run the backpropogation algorithm to update the weights to train the ANN for
 * one iteration over the entire training set.
 **/
void ANN::backPropogate() {
    // Each input vector
    for(unsigned int xi=0; xi<trainIns.size(); xi++) {
        // Get activation function values for all nodes after input layer
        calcActivations(trainIns[xi]);

        // Get errors for output layer (4)
        int outputL = layers.size()-1;
        for(unsigned int n=0; n<layers[outputL].size(); n++) {
            long double an = layers[outputL][n].a;
            layers[outputL][n].delta = an * (1 - an) * (encodings[trainOuts[xi]][n] - an);
        }

        // Get errors for layers (output, 1] (5 and 6)
        for(int l=outputL-1; l>0; l--) {
            for(unsigned int n=0; n<layers[l].size(); n++) {
                long double an = layers[l][n].a;

                // Get sum of products of errors and weights leaving this
                // neuron
                long double productSum = 0;
                for(unsigned int j=0; j<layers[l+1].size(); j++) {
                    productSum += layers[l+1][j].delta * layers[l][n].weights[j];
                }

                layers[l][n].delta = an * (1 - an) * productSum;
            }
        }

        // Update weights (7)
        int curNeuron = 0;
        for(unsigned int l=2; l<layers.size(); l++) {
            for(unsigned int j=0; j<layers[l].size(); j++) {
                layers[0][0].weights[curNeuron] += alpha * layers[l][j].delta;
                ++curNeuron;
            }
        }

        for(unsigned int l=1; l<layers.size()-1; l++) {
            for(unsigned int n=0; n<layers[l].size(); n++) {
                for(unsigned int j=0; j<layers[l+1].size(); j++)
                    layers[l][n].weights[j] += alpha * layers[l][n].a * layers[l+1][j].delta;
            }
        }
    }
}

/**
 * Classify all testing inputs and compare to the proper classifications.
 * Prints each classification followed by the accurracy of the ANN.
 **/
void ANN::classify() {
    long double numCorrect = 0;
    for(unsigned int xi=0; xi<testIns.size(); xi++) {
        calcActivations(testIns[xi]);

        // Find Euclidean distance between output layer and all characters
        long double minDist = LONG_MAX;
        long double curDist;
        int minDigit;
        int outputL = layers.size()-1;
        for(unsigned int c=0; c<62; c++) {
            curDist = 0;
            for(unsigned int n=0; n<10; n++)
                curDist += pow(layers[outputL][n].a - encodings[c][n], 2);
            curDist = sqrt(curDist);

            // Current best guess
            if(curDist < minDist) {
                minDist = curDist;
                minDigit = c;
            }
        }

        // Print our digit choice
        cout << minDigit << endl;

        // Keep track of how many we get right
        if(minDigit == testOuts[xi])
            ++numCorrect;
    }

    // Print accurracy
    cout << showpoint << fixed << setprecision(12) << numCorrect / testOuts.size() << endl;
}

/**
 * Learn weights and then test accurracy of ANN.
 **/
void ANN::main() {
    // Iterations
    for(int i=0; i<k; i++)
        backPropogate();

    // Print weights from first input neuron to next layer neurons
    printWeights();

    // Classify the testing data
    classify();
}
