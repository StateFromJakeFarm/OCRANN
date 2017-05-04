#include <iostream> //cout
#include <fstream>  //ifstream, ofstream
#include <vector>   //vector
#include <string>   //string
#include <unistd.h> //getopt
#include "ann.h"

using std::cout;
using std::endl;
using std::fstream;
using std::ifstream;
using std::ofstream;
using std::vector;
using std::string;

#define NUM_LABELS 62
#define LABEL_SIZE 100
#define BLOCK_SIZE 10
#define ERROR 0.15
#define NUM_IT 200

vector<vector<long double>> makeLabels();
int mapLabel(char l);

vector<vector<long double>> readBMP(char* file);
vector<long double> readSingleBMP(char* file);
vector<int> readLabels(char* file);

int main(int argc, char* argv[]) {
  bool train = false;
  bool test = false;
  bool read = false;
  bool save = false;

  char c;
  int num_options = 0;

  //Get where args appear in command string
  int rIndex = argc; int tIndex = argc; int sIndex = argc; int dIndex = argc;
  for(int i=1; i<argc; i++) {
    if(argv[i][0] == '-') {
      char c = argv[i][1];
      if(c == 'r') {
        rIndex = i; train = true;
      } else if(c == 't') {
        tIndex = i; test = true;
      } else if(c == 's') {
        sIndex = i; read = true;
      } else if(c == 'd') {
        dIndex = i; save = true;
      } else {
        cout << "ERROR: Unrecognized argument '" << argv[i] << "'" << endl;
        return -1;
      }
    }
  }

  if(train) cout << "Training" << endl;
  if(test) cout << "Testing" << endl;
  if(read) cout << "Reading" << endl;
  if(save) cout << "Saving" << endl;


  char* weight_file;
  vector<int> structure;
  vector<vector<long double>> labels;
  vector<vector<long double>> training_input, testing_input;

  //Input layer
  structure.push_back(BLOCK_SIZE * BLOCK_SIZE);
  //Hidden layers
  structure.push_back((BLOCK_SIZE*BLOCK_SIZE)*1.5);
  //Output layer
  structure.push_back(LABEL_SIZE);

  if(read) weight_file = "weights.txt";
  else weight_file = NULL;
  labels = makeLabels();
  ANN picasso(structure, labels, weight_file);

  int num_image = 0;
  //Read in all training data images
  //Training label is the first letter of the file name
  for(int i=rIndex+1; argv[i][0]!='-' && i<argc; i++) {
    vector<long double> temp = readSingleBMP(argv[i]);
    //Finds first leter of the file name
    char l = -1;
    for(int j = 0;;j++) {
      char c = argv[i][j];
      if(c == '\0') break;
      if(c == '/') l = argv[i][j+1];
    }

    if(l == -1) {cout << "UHOH" << endl; return -2;}
    int label = mapLabel(l);
    temp.push_back(label);
    training_input.push_back(temp);

    cout << argv[i] << " " << l << "," << label << endl;
    num_image++;
  }
  cout << "Processing " << num_image << " images." << endl;
  if(train) picasso.train(ERROR, NUM_IT, training_input);

  // TESTING
  for(int i=tIndex; argv[i][0]!='-' && i<argc; i++) {
    vector<long double> temp = readSingleBMP(argv[i]);
    //Finds first leter of the file name
    char l = -1;
    for(int j = 0;;j++) {
      char c = argv[i][j];
      if(c == '\0') break;
      if(c == '/') l = argv[i][j+1];
    }

    if(l == -1) {cout << "UHOH" << endl; return -2;}
    int label = mapLabel(l);
    temp.push_back(label);
    testing_input.push_back(temp);
  }
  if(test) picasso.test(testing_input);

  // SAVING
  if(dIndex != argc) {
    //picasso.printNetwork();
    if(save) picasso.saveWeights(argv[dIndex+1]);
  }

  //picasso.printNetwork();
  return 0;
}

vector<vector<long double>> makeLabels() {
  vector<vector<long double>> ret(NUM_LABELS);

  for(int i = 0;i < NUM_LABELS;i++) {
    for(int j = 0;j < LABEL_SIZE;j++) {
      int mod = NUM_LABELS;
      if((i+j) % mod == 0) ret[i].push_back(0.9);
      else ret[i].push_back(0.1);
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

  //cout << width << "x" << height << endl;
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

  //cout << width << "x" << height << endl;
  if(width != BLOCK_SIZE || height != BLOCK_SIZE)
    cout << "WARNING! Image resolution different than accepted in file '" << file << "'." << endl;
  //cout << k << " bits per pixel." << endl;
  //cout << k/8 <<  " bytes per pixel." << endl;
width = BLOCK_SIZE;
height = BLOCK_SIZE;
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
