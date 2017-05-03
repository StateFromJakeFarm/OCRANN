#include <iostream>
#include <iomanip>
#include <string>
#include <stdlib.h>
#include "ann.h"

using namespace std;

int main(int argc, char *argv[]) {
    long double a = atof(argv[8]);
    int k = atoi(argv[9]);

    ANN myANN(argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7], a, k);

    return 0;
}
