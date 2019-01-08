/*
    Seminar work of 'Programming in C'
    Theme: finding function extreme by optimalazing function with
      genetic algorithm
    Author: Pavel Trestik

*/
#include <stdio.h>
#ifndef SPECIMEN_H
#define SPECIMEN_H

#define REAL_NUM 'R'
#define WHOLE_NUM 'Z'
#define BIT_ONE '1'
#define BIT_ZERO '0'

/*#define FUNC_CALC_FILE "func01_meta.txt"*/

/* Name of external program, that calculates extreme */
char *calc_func;
/* Original name of meta file with path
char *meta_source;
*/
/* Name of meta file without path */
char *meta_name;
/* If the program is terminated with SIGINT */

/* Coef structure */
typedef struct{
  /* Coef type */
  char type;
  /* Coef value - used float - double for higher accuracy */
  double num;
} coef;

/* Specimen structure */
typedef struct{
  /* Specimen id */
  int id;
  /* Specimen extreme = fitness value */
  double ext;
  /* Specimens coefs */
  coef *coefs;
} specimen;

/* Interval structure */
typedef struct{
  /* Interval type */
  char type;
  /* Interval bot limit */
  float bot;
  /* Interval top limit */
  float top;
} interval;

/* Create specimen prototype */
void create_specimen(specimen **spec, coef coefs[], int coef_cnt);
/* Specimen fitness prototype */
int get_extreme(specimen **spec);
/* Crossbreed two specimen prototype */
void crossbreed(specimen *par1, specimen *par2, specimen **spec1,
   specimen **spec2, int coef_cnt, interval is[]);
/* Mutate specimen prototype */
void mutate(specimen **spec, interval is[], int coef_cnt);

#endif
