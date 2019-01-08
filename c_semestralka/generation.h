/*
    Seminar work of 'Programming in C'
    Theme: finding function extreme by optimalazing function with
      genetic algorithm
    Author: Pavel Trestik

*/
#include "specimen.h"
#ifndef GENERATION_H
#define GENERATION_H

#define GENERATION_SIZE 100

/* Mutation rate percent */
int mutation_rate;

/* Generation strutct */
typedef struct{
  /* Generationg number (id) */
  int gen_num;
  /* Specimens of the generation*/
  specimen species[GENERATION_SIZE];
} generation;

/* create_first_generation prototype */
int create_first_generation(interval is[], int inter_cnt);
/* next_generation prototype */
int next_generation();
/* calc_generation_extremes prototype */
int calc_generation_extremes();
/* print_generation prototype */
void print_generation();
/* free_gen prototype */
void free_gen();
/* print_gen_to_file prototype */
int print_gen_to_file();
/* print_val_to_file prototype */
int print_val_to_file();
/* print_files prototypy */
int print_files();

#endif
