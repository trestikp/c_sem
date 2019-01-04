#include "generation.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

generation gen;
int coef_cnt;

int compare (const void* p1, const void* p2) {
  int v1 = (int) (((specimen*)p1)->ext * 1000000);
  int v2 = (int) (((specimen*)p2)->ext * 1000000);
  if (v1 < v2)
    return 1;
  else if (v1 > v2)
   return -1;
  else
    return 0;
}

generation* create_first_generation(interval is[], int inter_cnt){
  int i = 0, zr, j = 0;
  float rr;


  for(i = 0; i < GENERATION_SIZE; i++){
    gen.species[i].id = i+1;
    gen.species[i].coefs = (coef*) malloc(sizeof(coef) * inter_cnt);
  }

  srand((unsigned)time(NULL));
  for(i = 0; i < inter_cnt; i++){
    if(is[i].type == 'R'){
      for(j = 0; j < GENERATION_SIZE; j++){
        rr = is[i].bot + ((float)rand()/RAND_MAX * (is[i].top - is[i].bot));
        gen.species[j].coefs[i].type = 'R';
        gen.species[j].coefs[i].num = rr;
      }
    }

    if(is[i].type == 'Z'){
      for(j = 0; j < GENERATION_SIZE; j++){
        zr = (int)is[i].bot + (rand() % ((int)is[i].top - (int)is[i].bot + 1));
        gen.species[j].coefs[i].type = 'Z';
        gen.species[j].coefs[i].num = zr;
      }
    }
  }

  calc_generation_extremes();

  qsort((void*) gen.species, GENERATION_SIZE, sizeof(specimen), compare);

  gen.gen_num = 1;
  coef_cnt = inter_cnt;

  return &gen;

  /*
  for (i = 0; i < GENERATION_SIZE; i++) {
    specimen *spec = &gen.species[i];
    create_test(&spec, is, inter_cnt);
    gen.species[i].id = (i + 1) + (gen.gen_num * GENERATION_SIZE);
  }

  return &gen;
  */
}

void next_generation() {
  int parents1[GENERATION_SIZE / 4], parents2[GENERATION_SIZE / 4], i;
  int help = -1, gen_pos = GENERATION_SIZE / 2;
  specimen *new_spec1 = (specimen*) malloc(sizeof(specimen));
  specimen *new_spec2 = (specimen*) malloc(sizeof(specimen));
  srand((unsigned)time(NULL));

  /* Generation of parents indexes */
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
    crossbreed(par1, par2, &spec1, &spec2, coef_cnt);

    gen_pos += 2;
  }

  calc_generation_extremes();
  qsort((void*) gen.species, GENERATION_SIZE, sizeof(specimen), compare);
  gen.gen_num++;
}

void calc_generation_extremes(){
  int i = 0;
  float ext;

  for (i = 0; i < GENERATION_SIZE; i++) {
    ext = get_extreme(&gen.species[i], "func01_meta.txt");
    gen.species[i].ext = ext;
  }
}


void print_generation(){
  printf("\n\nGENERATION NUMBER: %d\n", gen.gen_num);
  int i = 0, j = 0;
  /*
  for(i = 0; i < GENERATION_SIZE; i++){
    printf("Specimen %d\n", gen.species[i].id);
    for(j = 0; j < coef_cnt; j++){
      if(gen.species[i].coefs[j].type == 'Z'){
        printf("Coef add: %p\n", &gen.species[i].coefs[j]);
        printf("Int coef: %d\n", (int)gen.species[i].coefs[j].num);
      }
      if(gen.species[i].coefs[j].type == 'R'){
        printf("Coef add: %p\n", &gen.species[i].coefs[j]);
        printf("Fl coef: %f\n", gen.species[i].coefs[j].num);
      }
    }
    printf("Spec. ext: %f\n", gen.species[i].ext);
    printf("**********\n");
  }
  */
  printf("TOP 3: \n");
  for(i = 0; i < 3; i++){
    printf("Specimen %d\n", gen.species[i].id);
    for(j = 0; j < coef_cnt; j++){
      if(gen.species[i].coefs[j].type == 'Z'){
        printf("Int coef: %d\n", (int)gen.species[i].coefs[j].num);
      }
      if(gen.species[i].coefs[j].type == 'R'){
        printf("Fl coef: %f\n", gen.species[i].coefs[j].num);
      }
    }
    printf("Spec. ext: %f\n", gen.species[i].ext);
    printf("**********\n");
  }
  printf("WORST 3: \n");
  for(i = (GENERATION_SIZE - 1); i >= (GENERATION_SIZE - 3); i--){
    printf("Specimen %d\n", gen.species[i].id);
    for(j = 0; j < coef_cnt; j++){
      if(gen.species[i].coefs[j].type == 'Z'){
        printf("Int coef: %d\n", (int)gen.species[i].coefs[j].num);
      }
      if(gen.species[i].coefs[j].type == 'R'){
        printf("Fl coef: %f\n", gen.species[i].coefs[j].num);
      }
    }
    printf("Spec. ext: %f\n", gen.species[i].ext);
    printf("***********\n");
  }
}
