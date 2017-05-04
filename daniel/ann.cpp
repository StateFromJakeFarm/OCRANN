#include "ann.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <stdlib.h>

using std::cout;
using std::endl;
using std::showpoint;
using std::fixed;
using std::setprecision;
using std::fstream;
using std::ofstream;
using std::ifstream;

ANN::Node::Node() {
  srand(4);
  in = 0;
  out = 0;
  err = 0;
}

ANN::Node::~Node() {
  for(int i = 0;i < (int)from.size();i++) {
    Edge* e = from[i];
    if(e != NULL) delete e;
    e = NULL;
  }
  //All edges can be reached by from
}
/******************************************************************************/

ANN::Edge::Edge() {
  a = NULL;
  b = NULL;
  w = 0;
}

ANN::Edge::Edge(Node* a, long double w, Node* b) {
  this->a = a;
  this->b = b;
  this->w = w;
}

/******************************************************************************/

//ANN::ANN(vector<int> structure, vector<vector<long double> > weights, vector<vector<long double> > encoding) {
ANN::ANN(vector<int> structure, vector<vector<long double> > encoding, char* wfile) {
  //Create nodes
  num_nodes = 0;
  for(int i = 0;i < (int)structure.size();i++) {
    layers.push_back(structure[i]);
    num_nodes += structure[i];

    vector<Node*> t;
    nodes.push_back(t);
  }

  for(int i = 0;i < (int)nodes.size();i++) {
    for(int j = 0;j < layers[i];j++) {
      nodes[i].push_back(new Node);
    }
  }

  if(wfile == NULL) {
    //set weights for each edge
    int w = 0;
    //For each layer of nodes
    for(int i = 0;i < (int)nodes.size();i++) {
      //For each node in that layer
      for(int j = 0;j < (int)nodes[i].size();j++) {
        Node* a = nodes[i][j];
        //If not input layer
        if(i != 0) {
          //Add dummy weight
          nodes[i][j]->from.push_back(new Edge(NULL,0.01,a));
          //For each node in the previous layer
          for(int k = 0;k < (int)nodes[i-1].size();k++) {
            Node* n = nodes[i-1][k];
            nodes[i][j]->from.push_back(n->to[j]);
            //nodes[i][j]->from.push_back(new Edge(n, weight));
          }
        }
        //If not output layer
        if(i != (int)nodes.size()-1) {
          //For each node in the next layer
          for(int k = 0;k < (int)nodes[i+1].size();k++) {
            Node* n = nodes[i+1][k];
            //long double weight = weights[w][k];
            int temp = rand() % 10;
            long double weight = temp / 100.0;
            nodes[i][j]->to.push_back(new Edge(a, weight, n));
          }
        }
        //Next node
        w++;
      }
    }
  }else readWeights(wfile);

  //Read in labels
  labels = encoding;

  //printNetwork();
}

ANN::~ANN() {
  for(int i = 0;i < (int)nodes.size();i++) {
    for(int j = 0;j < (int)nodes[i].size();j++) {
      Node* n = nodes[i][j];
      if(n != NULL) delete n;
      n = NULL;
    }
  }
}

void ANN::train(long double error, int num_it, vector<vector<long double> > input) {
  for(int it = 0;it < num_it;it++) {
    if(it % (num_it / 20) == 0)
      cout << "Training Iteration #" << it << "/" << num_it << endl;

    for(int i = 0;i < (int)input.size();i++) {
      //Initialize input layer
      for(int j = 0;j < (int)nodes[0].size();j++) {
        Node* n = nodes[0][j];
        n->out = input[i][j];
      }

      //Calculate a for all nodes
      for(int j = 1;j < (int)nodes.size();j++) {
        for(int k = 0;k < (int)nodes[j].size();k++) {
          Node* n = nodes[j][k];
          n->in = 0;
          for(int l = 0;l < (int)n->from.size();l++) {
            Edge* e = n->from[l];

            if(l == 0) n->in += e->w;
            else n->in += e->a->out *e->w;
          }
          n->out = 1.0/(1.0 + exp(-n->in));
        }
      }

      //Grab output values and calculate errors
      vector<long double> output;
      int output_layer = nodes.size()-1;
      int goal = input[i][input[i].size()-1];

      for(int j = 0;j < (int)nodes[output_layer].size();j++) {
        Node* n = nodes[output_layer][j];
        n->err = n->out * (1 - n->out) * (labels[goal][j] - n->out);
        //update dummy weight
        n->from[0]->w = n->from[0]->w + error * n->err;

        output.push_back(n->out);
      }

      //Propagate errors backwards and update weights
      for(int j = output_layer-1;j >= 0;j--) {
        for(int k = 0;k < (int)nodes[j].size();k++) {
          Node* n = nodes[j][k];
          long double sum = 0;
          for(int l = 0;l < (int)n->to.size();l++) {
            Edge* e = n->to[l];
            sum += e->w * e->b->err;

            e->w = e->w + error * n->out * e->b->err;
          }
          n->err = n->out * (1 - n->out) * sum;

          //Update dummy weight
          if(j != 0) n->from[0]->w = n->from[0]->w + error * n->err;
        }
      }
    }//end of i
  }//end of it

  //printNetwork();
}

/*
void ANN::test(vector<vector<long double> > input) {
  int num_tested = 0;
  int num_correct = 0;
  //For every record in input
  for(int i = 0;i < (int)input.size();i++) {
    //Set the input layer
    for(int k = 0;k < (int)nodes[0].size();k++) {
      nodes[0][k]->out = input[i][k];
    }

    //For every layer up to output
    for(int j = 1;j < (int)nodes.size();j++) {
      //For every node in that layer
      for(int k = 0;k < (int)nodes[j].size();k++) {
        Node* n = nodes[j][k];

        //Read input from previous layer
        n->in = 0;
        vector<Edge*> from = n->from;
        for(int l = 0;l < (int)from.size();l++) {
          if(l == 0) n->in += from[0]->w;
          else n->in += from[l]->w * from[l]->a->out;
        }
        n->out = 1.0/(1.0 + exp(-1 * n->in));
      }
    }//end of layers, j

    //Collect output
    vector<long double> output;
    int output_layer = nodes.size()-1;
    int goal = input[i][input[i].size()-1];

    //For the output layer
    for(int j = 0;j < (int)nodes[output_layer].size();j++) {
      Node* n = nodes[output_layer][j];
      output.push_back(n->out);
    }

    int label = findLabel(output);
    cout << label << " - " << goal << endl;

    if(label == goal) num_correct++;
    num_tested++;
  }//end of input, i

  double accuracy = 1.0 * num_correct / num_tested;
  cout << showpoint << fixed << setprecision(12) << accuracy << endl;
}
*/

vector<int> ANN::test(vector<vector<long double> > input) {
  vector<int> ret;

  int num_tested = 0;
  int num_correct = 0;
  //For every record in input
  for(int i = 0;i < (int)input.size();i++) {
    //Set the input layer
    for(int k = 0;k < (int)nodes[0].size();k++) {
      nodes[0][k]->out = input[i][k];
    }

    //For every layer up to output
    for(int j = 1;j < (int)nodes.size();j++) {
      //For every node in that layer
      for(int k = 0;k < (int)nodes[j].size();k++) {
        Node* n = nodes[j][k];

        //Read input from previous layer
        n->in = 0;
        vector<Edge*> from = n->from;
        for(int l = 0;l < (int)from.size();l++) {
          if(l == 0) n->in += from[0]->w;
          else n->in += from[l]->w * from[l]->a->out;
        }
        n->out = 1.0/(1.0 + exp(-1 * n->in));
      }
    }//end of layers, j

    //Collect output
    vector<long double> output;
    int output_layer = nodes.size()-1;
    int goal = input[i][input[i].size()-1];

    //For the output layer
    for(int j = 0;j < (int)nodes[output_layer].size();j++) {
      Node* n = nodes[output_layer][j];
      output.push_back(n->out);
    }

    int label = findLabel(output);
    cout << label << " - " << goal << endl;
    ret.push_back(label);

    if(label == goal) num_correct++;
    num_tested++;
  }//end of input, i

  double accuracy = 1.0 * num_correct / num_tested;
  cout << showpoint << fixed << setprecision(12) << accuracy << endl;

  return ret;
}

//Finds closest label by Euclidean distance
int ANN::findLabel(vector<long double> out) {
  vector<long double> differences;

  for(int i = 0;i < (int)labels.size();i++) {
    long double diff = 0;

    for(int j = 0;j < (int)labels[i].size();j++) {
      diff += pow((out[j] - labels[i][j]), 2);
    }
    diff = sqrt(diff);
    differences.push_back(diff);
  }

  int in = -4;
  long double smallest = 9999999;
  for(int i = 0;i < (int)differences.size();i++) {
    if(differences[i] < smallest) {
      in = i;
      smallest = differences[i];
    }
  }

  return in;
}

void ANN::printNetwork() {
  cout << "To:" << endl;
  int na = 1;
  for(int i = 0;i < (int)nodes.size();i++) {
    cout << "Layer " << i << endl;
    for(int j = 0;j < (int)nodes[i].size();j++) {
      cout << na << ": ";
      Node* n = nodes[i][j];
      for(int k = 0;k < (int)n->to.size();k++) {
        cout << n->to[k]->w << " ";
      }
      cout << endl;
      na++;
    }
    cout << endl;
  }

  cout << "From:" << endl;
  na = 1;
  for(int i = 0;i < (int)nodes.size();i++) {
    cout << "Layer " << i << endl;
    for(int j = 0;j < (int)nodes[i].size();j++) {
      cout << na << ": ";
      Node* n = nodes[i][j];
      for(int k = 0;k < (int)n->from.size();k++) {
        cout << n->from[k]->w << " ";
      }
      cout << endl;
      na++;
    }
    cout << endl;
  }
}

//Save weights each node recieves
bool ANN::saveWeights(char* file) {
  ofstream oFile;
  oFile.open(file);

  if(!oFile.is_open()) {
    cout << "Error: Could not open file '" << file << "' for writing." << endl;
    return false;
  }
  //For every layer in nodes except input layer
  for(int i = 1;i < (int)nodes.size();i++) {
    //For every node in that layer
    for(int j = 0;j < (int)nodes[i].size();j++) {
      Node* n = nodes[i][j];
      for(int k = 0;k < (int)n->from.size();k++) {
        oFile << n->from[k]->w << " ";
      }
      oFile << endl;
    }
  }

  oFile.close();
  return true;
}

bool ANN::readWeights(char* file) {
  ifstream iFile;
  iFile.open(file);

  if(!iFile.is_open()) {
    cout << "Error: Could not open file '" << file << "' for reading." << endl;
    return false;
  }

  //For every layer of nodes except the input layer
  for(int i = 1;i < (int)nodes.size();i++) {
    //For every node in that layer
    for(int j = 0;j < (int)nodes[i].size();j++) {
      Node* n = nodes[i][j];
      long double weight;

      iFile >> weight;
      n->from.push_back(new Edge(NULL, weight, n));
      for(int k = 0;k < (int)nodes[i-1].size();k++) {
        Node* f = nodes[i-1][k];
        iFile >> weight;

        Edge* e = new Edge(f, weight, n);
        f->to.push_back(e);
        n->from.push_back(e);
      }
    }
  }

  iFile.close();
  return true;
}
