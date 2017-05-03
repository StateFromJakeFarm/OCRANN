#include <iostream> //cout
#include <fstream>  //ifstream, ofstream
#include <vector>   //vector
#include <stdlib.h>
#include <time.h>
#include "ann.h"

//4 8 8 12 14 16 8

using std::cout;
using std::endl;
using std::fstream;
using std::ifstream;
using std::ofstream;
using std::vector;

#define NUM_LABELS 10
#define LABEL_SIZE 50
#define BLOCK_SIZE 10
#define ERROR 0.15
#define NUM_IT 600

vector<vector<long double>> makeLabels();
vector<vector<long double>> readBMP(char* file);
vector<int> readLabels(char* file);

int main(int argc, char* argv[]) {
  if(argc < 5) {
    cout << "Error: Not enough arguments." << endl;
    cout << "<train.bmp> <train.lbl> <test.bmp> <test.lbl>" << endl;
    return -1;
  }
  srand(time(NULL));
  vector<int> structure;
  vector<vector<long double>> labels;
  vector<vector<long double>> training_input, testing_input;
  vector<int> training_label, testing_label;

  //Input layer
  structure.push_back(BLOCK_SIZE * BLOCK_SIZE);
  //Hidden layers
  structure.push_back(150);
  structure.push_back(75);
  //structure.push_back(150);
  //Output layer
  structure.push_back(LABEL_SIZE);

  labels = makeLabels();
  training_input = readBMP(argv[1]);
  training_label = readLabels(argv[2]);

  if((int)training_input.size() == 0 || (int)training_label.size() == 0) return -1;

  //Append label to end of training_input
  for(int i = 0;i < (int)training_input.size();i++) {
    training_input[i].push_back(training_label[i]);
  }

  testing_input = readBMP(argv[3]);
  testing_label = readLabels(argv[4]);
  for(int i = 0;i < (int)testing_input.size();i++) {
    testing_input[i].push_back(testing_label[i]);
  }

  ANN picasso(structure, labels);
  picasso.train(ERROR, NUM_IT, training_input);

  picasso.test(testing_input);

/*
  for(int i = 0;i < (int)training_input.size();i++) {
    for(int j = 0;j < (int)training_input[i].size()-1;j++) {
      if(j > 0 && j % BLOCK_SIZE == 0) cout << endl;
      cout << training_input[i][j] << " ";
    }
    cout << endl;
    cout << (int)training_input[i][training_input[i].size()-1] << endl;
    cout << endl;
  }
  */

  return 0;
}

vector<vector<long double>> makeLabels() {
  vector<vector<long double>> ret(NUM_LABELS);

  for(int i = 0;i < NUM_LABELS;i++) {
    for(int j = 0;j < LABEL_SIZE;j++) {
      int mod = NUM_LABELS;
      if((i+j) % mod == 0) ret[i].push_back(0.9);
      else ret[i].push_back(0.1);
      /*
      if(j == i) ret[i].push_back(0.9);
      else ret[i].push_back(0.1);
      */
    }
  }

  return ret;
}


vector<vector<long double>> readBMP(char* file) {
  vector<vector<long double>> ret;
  fstream iBMP(file, fstream::in | fstream::binary);

  if(!iBMP.is_open()) {
    cout << "Error: Could not open file '" << file << "'." << endl;
    return ret;
  }

  //Read in 54 byte header
  char info[54];
  iBMP.read(info, 54);

  //Find width and height
  //Represented as bytes 18-21 and 22-25
  long int width = ((int)(unsigned char)info[18] << 0) | ((int)(unsigned char)info[19] << 8)
                    | ((int)(unsigned char)info[20] << 16) | ((int)(unsigned char)info[21] << 24);
  long int height = ((int)(unsigned char)info[22] << 0) | ((int)(unsigned char)info[23] << 8)
                    | ((int)(unsigned char)info[24] << 16) | ((int)(unsigned char)info[25] << 24);
  int num_blocks = (width / BLOCK_SIZE) * (height / BLOCK_SIZE);

  //Bits per pixel
  //int k = info[28];

  cout << width << "x" << height << endl;
  //cout << k << " bits per pixel." << endl;
  //cout << k/8 <<  " bytes per pixel." << endl;

  //Initialize ret
  for(int i = 0;i < num_blocks;i++) {
    vector<long double> temp;
    ret.push_back(temp);
  }

  //Start at bottom-left corner
  for(int i = height-1;i >= 0;i--) {
    for(int j = 0;j < width;j++) {
      int block = (j / BLOCK_SIZE) + (i / BLOCK_SIZE)*(width / BLOCK_SIZE);
      unsigned char R, G, B;

      B = iBMP.get();
      G = iBMP.get();
      R = iBMP.get();

      if((R + B + G) > 0) ret[block].push_back(0.1);
      else ret[block].push_back(0.9);
    }
  }

  iBMP.close();

  return ret;
}

vector<int> readLabels(char* file) {
  vector<int> ret;
  ifstream label(file);

  if(!label.is_open()) {
    cout << "Error: Could not open file '" << file << "'." << endl;
    return ret;
  }

  int x;
  while(label >> x) ret.push_back(x);

  return ret;
}
