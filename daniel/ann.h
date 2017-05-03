#ifndef ANN_H
#define ANN_H

#include <vector>

using std::vector;

class ANN {
private:
  class Node;
  class Edge;
private:
  class Node {
  public:
    long double in;
    long double out;
    long double err;
    vector<Edge*> from;
    vector<Edge*> to;
  public:
    Node();
    ~Node();
  };
  class Edge {
  public:
    Node* a;
    Node* b;
    long double w;
  public:
    Edge();
    Edge(Node*, long double, Node* b);
  };
private:
  int num_nodes;
  vector<int> layers;
  vector<vector<Node*> > nodes;
  vector<vector<long double> > labels;
public:
  //structure, weights, labels
  //ANN(vector<int>, vector<vector<long double> >, vector<vector<long double> >);
  ANN(vector<int>, vector<vector<long double> >);
  ~ANN();
public:
  //error, num_it, data
  void train(long double, int, vector<vector<long double> >);
  void test(vector<vector<long double> >);
  int findLabel(vector<long double>);
  void printNetwork();
};

#endif
