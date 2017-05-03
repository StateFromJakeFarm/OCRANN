#include <stdio.h>

#define NUM_LABELS 8
#define LABEL_SIZE 8

int main(int argc,char* argv[]) {
  if(argc < 2) {
    printf("Error: Need output file.\n");
    return -1;
  }
  FILE* oFile = fopen(argv[1], "w");

  for(int i = 0;i < NUM_LABELS;i++) {
    for(int j = 0;j < LABEL_SIZE;j++) {
      if(j == i) fprintf(oFile, "0.1");
      else fprintf(oFile, "0.9");
      fprintf(oFile, " ");
    }
    fprintf(oFile, "\n");
  }

  fclose(oFile);
}
