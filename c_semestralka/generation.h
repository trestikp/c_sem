#include "specimen.h"
#ifndef GENERATION_H
#define GENERATION_H

#define GENERATION_SIZE 100

typedef struct{
  int gen_num;
  specimen species[GENERATION_SIZE];
} generation;

/*
typedef struct{
  char type;
  float bot;
  float top;
} interval;
*/

//void make_inters(int num, interval is[]);
generation* create_first_generation(interval is[], int inter_cnt);
void next_generation();
void calc_generation_extremes();
void print_generation();

#endif
