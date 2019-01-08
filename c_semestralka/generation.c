/*
    Seminar work of 'Programming in C'
    Theme: finding function extreme by optimalazing function with
      genetic algorithm
    Author: Pavel Trestik

*/
#include "generation.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CMP_ACC 1000000
#define TOP_FIVE 5
#define OUT_GEN "gen.txt"
#define OUT_VAL "val.txt"
#define TEMP_VAL "temp_val.txt"
#define TEMP_GEN "temp_gen.txt"

/* Generation */
generation gen;
/* Intervals */
interval *inters;
/* Coeficient count = interval cont*/
int coef_cnt;

/* Compare function for qsort */
int compare (const void* p1, const void* p2) {
  int v1 = (int) (((specimen*)p1)->ext * CMP_ACC);
  int v2 = (int) (((specimen*)p2)->ext * CMP_ACC);
  if (v1 < v2)
    return 1;
  else if (v1 > v2)
   return -1;
  else
    return 0;
}

/*
* Function creating first generation
*/
int create_first_generation(interval is[], int inter_cnt){
  int i = 0, zr, j = 0;
  double rr;

  /* Assigns specimens their id and allocs memory for coefs */
  for(i = 0; i < GENERATION_SIZE; i++){
    gen.species[i].id = i+1;
    gen.species[i].coefs = (coef*) malloc(sizeof(coef) * inter_cnt);
  }

  /* Generate coeficients for specimens */
  srand((unsigned)time(NULL));
  for(i = 0; i < inter_cnt; i++){
    if(is[i].type == REAL_NUM){
      for(j = 0; j < GENERATION_SIZE; j++){
        rr = is[i].bot + ((double)rand()/RAND_MAX * (is[i].top - is[i].bot));
        gen.species[j].coefs[i].type = REAL_NUM;
        gen.species[j].coefs[i].num = rr;
      }
    }

    if(is[i].type == WHOLE_NUM){
      for(j = 0; j < GENERATION_SIZE; j++){
        zr = (int)is[i].bot + (rand() % ((int)is[i].top - (int)is[i].bot + 1));
        gen.species[j].coefs[i].type = WHOLE_NUM;
        gen.species[j].coefs[i].num = zr;
      }
    }
  }

  if (calc_generation_extremes() != 0) return 1;


  qsort((void*) gen.species, GENERATION_SIZE, sizeof(specimen), compare);

  gen.gen_num = 1;
  coef_cnt = inter_cnt;
  inters = is;

  return 0;
}

/* Prototype of mutate generation */
void mutate_generation();

/*
* Next generation generator
*/
int next_generation() {
  int parents1[GENERATION_SIZE / 4], parents2[GENERATION_SIZE / 4], i;
  int help = -1, gen_pos = GENERATION_SIZE / 2;
  specimen *new_spec1 = (specimen*) malloc(sizeof(specimen));
  specimen *new_spec2 = (specimen*) malloc(sizeof(specimen));
  srand((unsigned)time(NULL));

  /* Generation of paring indexes */
  for (i = 0; i < (GENERATION_SIZE / 4); i++) {
    parents1[i] = rand() % (GENERATION_SIZE / 2);
    help = rand() % (GENERATION_SIZE / 2);
    if (help != parents1[i]) parents2[i] = help;
    else {
      while (help == parents1[i]) {
        help = rand() % (GENERATION_SIZE / 2);
      }
      parents2[i] = help;
    }
  }

  /* Free memory of old specimen coefs */
  for (i = GENERATION_SIZE / 2; i < GENERATION_SIZE; i++) {
    free(gen.species[i].coefs);
  }

  /* Crossbreeding specimens */
  for (i = 0; i < (GENERATION_SIZE / 4); i++) {
    gen.species[gen_pos] = *new_spec1;
    gen.species[gen_pos + 1] = *new_spec2;
    gen.species[gen_pos].id = gen.gen_num * GENERATION_SIZE + gen_pos;
    gen.species[gen_pos + 1].id = gen.gen_num * GENERATION_SIZE + gen_pos + 1;
    gen.species[gen_pos].coefs = (coef*) malloc(sizeof(coef) * coef_cnt);
    gen.species[gen_pos + 1].coefs = (coef*) malloc(sizeof(coef) * coef_cnt);

    specimen *spec1 = &gen.species[gen_pos];
    specimen *spec2 = &gen.species[gen_pos + 1];
    specimen *par1 = &gen.species[parents1[i]];
    specimen *par2 = &gen.species[parents2[i]];
    crossbreed(par1, par2, &spec1, &spec2, coef_cnt, inters);

    gen_pos += 2;
  }

  mutate_generation();

  if (calc_generation_extremes() != 0){
    free(gen.species[gen_pos].coefs);
    free(gen.species[gen_pos + 1].coefs);
    free(new_spec1);
    free(new_spec2);
    return 1;
  }

  qsort((void*) gen.species, GENERATION_SIZE, sizeof(specimen), compare);
  gen.gen_num++;

  free(new_spec1);
  free(new_spec2);

  return 0;
}

/*
* Function mutating generation
*/
void mutate_generation() {
  int victims_cnt, i, *victims, *spared, spared_cnt, j, is_spared = 0;
  specimen *spec;

  if (mutation_rate < 5 || mutation_rate > 45) {
    printf("Unexpected mutation error - exiting.\n");
    return;
  }

  /* If mutation rate of 0 % was allowed
  if (mutation_rate == 0) {
    return;
  }*/

  victims_cnt = (GENERATION_SIZE / 100) * mutation_rate;

  /* If there is less mutated specimens then not mutated specimens
     generates position of specimens to be mutated
  */
  if (victims_cnt < GENERATION_SIZE / 4){
    victims = (int*) malloc(sizeof(int) * victims_cnt);

    for (i = 0; i < victims_cnt; i++) {
      /* Prevents top 5 specimens of being mutated */
      victims[i] = (TOP_FIVE + 1) + rand() % ((GENERATION_SIZE / 2) - TOP_FIVE);
    }

    /* Mutating specimens on generated position */
    for (i = 0; i < victims_cnt; i++) {
      spec = &gen.species[victims[i]];
      mutate(&spec, inters, coef_cnt);
    }
    free(victims);
  } else { /* if there is more more victimes than survivors, generates
    position of specimens to be spared
    */
    spared_cnt = (GENERATION_SIZE / 2) - victims_cnt;
    spared = (int*) malloc(sizeof(int) * spared_cnt);

    for (i = 0; i < spared_cnt; i++) {
      spared[i] = (TOP_FIVE + 1) + rand() % ((GENERATION_SIZE / 2) - TOP_FIVE);
    }

    /* Mutating specimens on every other position but the generated one */
    for (i = 6; i < (GENERATION_SIZE / 2); i++) {
      for (j = 0; j < spared_cnt; j++) {
        if (i == spared[j]) {
          is_spared = 1;
        }
      }
      if (is_spared) {
        spec = &gen.species[i];
        mutate(&spec, inters, coef_cnt);
        is_spared = 0;
      }
    }
    free(spared);
  }
}

/*
* Function calculates extremes of the whole generation
*/
int calc_generation_extremes(){
  int i = 0;
  specimen *spec;

  for (i = 0; i < GENERATION_SIZE; i++) {
    spec = &gen.species[i];
    if (get_extreme(&spec) != 0) return 1;
  }

  return 0;
}

/*
* Frees memory of all specimens coefs
*/
void free_gen() {
  int i;
  for (i = 0; i < GENERATION_SIZE; i++) {
    free(gen.species[i].coefs);
  }
}

/*
* Prints information about best specimen from generation
* at the start of gen.txt
*/
int print_gen_to_file() {
  FILE *in_out, *temp;
  int i;
  char vars[] = {'X', 'Y', 'Z', 'W', 'F'};
  char c;

  temp = fopen(TEMP_GEN, "w+");

  if (temp == NULL) {
    printf("Failed creating temp file.\n");
    return 1;
  }

  fprintf(temp, "--- GENERATION %d ---\n", gen.gen_num);
  fprintf(temp, "%f\n", gen.species[0].ext);
  for (i = 0; i < coef_cnt; i++) {
    if (gen.species[0].coefs[i].type == WHOLE_NUM) {
      fprintf(temp, "%c=%d#(%d,%d);Z\n", vars[i],
        (int) gen.species[0].coefs[i].num,
        (int) inters[i].bot, (int) inters[i].top);
    }
    if (gen.species[0].coefs[i].type == REAL_NUM) {
      fprintf(temp, "%c=%.2f#(%.2f,%.2f);R\n", vars[i],
        gen.species[0].coefs[i].num, inters[i].bot, inters[i].top);
    }
  }
  fprintf(temp, "\n");

  if (gen.gen_num == 1) {
    in_out = fopen(OUT_GEN, "w");

    while ((c = fgetc(temp)) != EOF) {
      fputc('c', in_out);
    }

    rename(TEMP_GEN, OUT_GEN);
    fclose(in_out);
    fclose(temp);
    return 0;
  }

  in_out = fopen(OUT_GEN, "r");

  if (in_out == NULL) {
    fclose(temp);
    remove(TEMP_GEN);
    printf("Failed to load file.\n");
    return 1;
  }

  while ((c = fgetc(in_out)) != EOF) {
    putc(c, temp);
  }

  fclose(in_out);
  fclose(temp);

  remove(OUT_GEN);
  rename(TEMP_GEN, OUT_GEN);

  return 0;
}

/*
* Prints all specimens fitness and their coeficients to val.txt
*/
int print_val_to_file() {
  FILE *in_out, *temp;
  int i, j;
  char vars[] = {'X', 'Y', 'Z', 'W', 'F'};
  char c;

  temp = fopen(TEMP_VAL, "w+");

  if (temp == NULL) {
    printf("Failed creating temp file.\n");
    return 1;
  }

  for (j = 0; j < GENERATION_SIZE; j++) {
    fprintf(temp, "%f\n", gen.species[j].ext);
    for (i = 0; i < coef_cnt; i++) {
      if (gen.species[j].coefs[i].type == WHOLE_NUM) {
        fprintf(temp, "%c=%d#(%d,%d);Z\n", vars[i],
          (int) gen.species[j].coefs[i].num,
          (int) inters[i].bot, (int) inters[i].top);
      }
      if (gen.species[j].coefs[i].type == REAL_NUM) {
        fprintf(temp, "%c=%.2f#(%.2f,%.2f);R\n", vars[i],
          gen.species[j].coefs[i].num, inters[i].bot, inters[i].top);
      }
    }
    fprintf(temp, "\n");
  }

  if (gen.gen_num == 1) {
    in_out = fopen(OUT_VAL, "w");

    while ((c = fgetc(temp)) != EOF) {
      fputc('c', in_out);
    }

    rename(TEMP_VAL, OUT_VAL);
    fclose(in_out);
    fclose(temp);
    return 0;
  }

  in_out = fopen(OUT_VAL, "r");

  if (in_out == NULL) {
    fclose(temp);
    remove(TEMP_VAL);
    printf("Failed to load file.\n");
    return 1;
  }

  while ((c = fgetc(in_out)) != EOF) {
    putc(c, temp);
  }

  fclose(in_out);
  fclose(temp);

  remove(OUT_VAL);
  rename(TEMP_VAL, OUT_VAL);

  return 0;
}

/*
* Functions calls other 2 print functions, so they don't have to be
* called separatedly.
*/
int print_files() {
  print_gen_to_file();
  print_val_to_file();

  return 0;
}

/*
* Function prints info about new generation on terminal
*/
void print_generation(){
  printf("\nGENERATION NUMBER: %d\n", gen.gen_num);
  printf("TOP SPEC\n");
  printf("Specimen %d\n", gen.species[0].id);
  printf("Extreme: %f\n", gen.species[0].ext);
  printf("---------\n");
  printf("WORST SPEC\n");
  printf("Specimen %d\n", gen.species[GENERATION_SIZE - 1].id);
  printf("Extreme: %f\n", gen.species[GENERATION_SIZE - 1].ext);
  printf("************\n");
}
