#include <stdio.h>
#ifndef SPECIMEN_H
#define SPECIMEN_H

typedef struct{
  char type;
  float num;
} coef;

typedef struct{
  int id;
  float ext;
  coef *coefs;
}specimen;


typedef struct{
  char type;
  float bot;
  float top;
} interval;

//void make_inters(int num, interval is[]);
void create_specimen(specimen **spec, coef coefs[], int coef_cnt);
void create_test(specimen **spec, interval is[], int is_size);
float get_extreme(specimen *spec, char orig_source[]);
void crossbreed(specimen *par1, specimen *par2, specimen **spec1, specimen **spec2, int coef_cnt);
void print_spec(specimen *spec);

#endif
