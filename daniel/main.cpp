#include <iostream> //cout
#include <fstream>  //ifstream, ofstream
#include <vector>   //vector
#include <stdlib.h>
#include <time.h>
#include "ann.h"

using std::cout;
using std::endl;
using std::fstream;
using std::ifstream;
using std::ofstream;
using std::vector;

#define NUM_LABELS 62
#define LABEL_SIZE 100
#define BLOCK_SIZE 10
#define ERROR 0.25
#define NUM_IT 1000

vector<vector<long double>> makeLabels();
int mapLabel(char l);

vector<vector<long double>> readBMP(char* file);
vector<long double> readSingleBMP(char* file);
vector<int> readLabels(char* file);

int main(int argc, char* argv[]) {
  for(char c = '0';c <= '9';c++) cout << c << " " << mapLabel(c) << endl;
  for(char c = 'A';c <= 'Z';c++) cout << c << " " << mapLabel(c) << endl;
  for(char c = 'a';c <= 'z';c++) cout << c << " " << mapLabel(c) << endl;

  if(argc < 5) {
    cout << "Error: Not enough arguments." << endl;
    cout << "<train.bmp> <train.lbl> <test.bmp> <test.lbl>" << endl;
    return -1;
  }

  vector<int> structure;
  vector<vector<long double>> labels;
  vector<vector<long double>> training_input, testing_input;
  vector<int> training_label, testing_label;

  //Input layer
  structure.push_back(BLOCK_SIZE * BLOCK_SIZE);
  //Hidden layers
  structure.push_back(150);
  //Output layer
  structure.push_back(LABEL_SIZE);

  labels = makeLabels();
  ANN picasso(structure, labels);

  training_input = readBMP(argv[1]);
  training_label = readLabels(argv[2]);

  /****TRAINING****************************************************************/
  if((int)training_input.size() == 0 || (int)training_label.size() == 0) return -1;

  //Append label to end of training_input
  for(int i = 0;i < (int)training_input.size();i++) {
    training_input[i].push_back(training_label[i]);
  }

  picasso.train(ERROR, NUM_IT, training_input);

  /****TESTING*****************************************************************/
  testing_input = readBMP(argv[3]);
  testing_label = readLabels(argv[4]);
  for(int i = 0;i < (int)testing_input.size();i++) {
    testing_input[i].push_back(testing_label[i]);
  }



  picasso.test(testing_input);

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

int mapLabel(char l) {
  int in_label;
  if(l >= '0' && l <= '9') in_label = l - 48;
  else if(l >= 'A' && l <= 'Z') in_label = l - 55;
  else if(l >= 'a' && l <= 'z') in_label = l - 61;

  return in_label;
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

vector<long double> readSingleBMP(char* file) {
  vector<long double> ret;
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

  //Bits per pixel
  //int k = info[28];

  cout << width << "x" << height << endl;
  if(width != BLOCK_SIZE || height != BLOCK_SIZE) cout << "WARNING! Image resolution different than accepted." << endl;
  //cout << k << " bits per pixel." << endl;
  //cout << k/8 <<  " bytes per pixel." << endl;

  //Start at bottom-left corner
  for(int i = height-1;i >= 0;i--) {
    for(int j = 0;j < width;j++) {
      unsigned char R, G, B;

      B = iBMP.get();
      G = iBMP.get();
      R = iBMP.get();

      if((R + B + G) > 0) ret.push_back(0.1);
      else ret.push_back(0.9);
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
