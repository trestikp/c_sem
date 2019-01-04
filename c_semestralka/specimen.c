#include "specimen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define MAX_LINE_LEN 80

void create_specimen(specimen **spec, coef coefs[], int coef_cnt){
  *spec = (specimen*) malloc(sizeof(specimen));
  (*spec)->coefs = (coef*) malloc(sizeof(coef) * coef_cnt);
  (*spec)->coefs = coefs;
}

/*
void create_test(specimen **spec, interval is[], int is_size){
  int i = 0, dr;
  float rr;
  //*spec = (specimen*) malloc(sizeof(specimen));
  (*spec)->coefs = (coef*) malloc(sizeof(coef) * is_size);

  srand((unsigned)time(NULL));
  for(i = 0; i < is_size; i++){
    if (is[i].type == 'R') {
      rr = is[i].bot + ((float)rand()/RAND_MAX * (is[i].top - is[i].bot));
      (*spec)->coefs[i].num = rr;
      (*spec)->coefs[i].type = 'R';
    }
    if (is[i].type == 'Z') {
      dr = (int)is[i].bot + (rand() % ((int)is[i].top - (int)is[i].bot + 1));
      (*spec)->coefs[i].num = dr;
      (*spec)->coefs[i].type = 'Z';
    }
  }
}
*/

float get_extreme(specimen *spec, char orig_source[]){
  FILE *f, *temp_f, *func_out;
  char line[MAX_LINE_LEN], var_type;
  int rewrite_next = 0, it, num_cnt = 0;
  float res;

  f = fopen(orig_source, "r");

  if (f == NULL) {
    printf("Failed to load file.\n");
    return EXIT_FAILURE;
  }

  temp_f = fopen("temp.txt", "w");

  if (temp_f == NULL) {
    printf("Failed to create temp file.\n");
    fclose(f);
    return EXIT_FAILURE;
  }

  while (!feof(f)) {
    strcpy(line, "\0");
    fgets(line, MAX_LINE_LEN, f);

    if (rewrite_next == 1) {
      if (var_type == 'R')
        fprintf(temp_f, "%c = %f\n", line[0] , spec->coefs[num_cnt].num);
      if (var_type == 'Z')
        fprintf(temp_f, "%c = %d\n", line[0], (int)spec->coefs[num_cnt].num);

      rewrite_next = 0;
      num_cnt++;
      continue;
    }

    if (line[0] == '#' && line[2] == '(') {
      rewrite_next = 1;
      it = 3;
      while (line[it] != '\n') {
        it++;
      }
      var_type = line[it - 2];
    }

    fprintf(temp_f, "%s", line);
  }

  fclose(f);
  fclose(temp_f);

  remove(orig_source);
  rename("temp.txt", orig_source);

  func_out = popen("./func", "r");

  if (func_out == NULL) {
    printf("Failed to execute extreme calculation\n");
    return EXIT_FAILURE;
  }

  while (fgets(line, MAX_LINE_LEN, func_out)) {
    sscanf(line, "%f", &res);
  }

  fclose(func_out);

  return res;
}

void crossbreed(specimen *par1, specimen *par2, specimen **spec1, specimen **spec2, int coef_cnt) {
  int i, c, size = sizeof(int) * 8, num1_len = 0, num2_len = 0;
  int swap_count = 0, higher_pos = 0, res1 = 0, res2 = 0;
  int int_coef1, int_coef2, min_bit_swap = 0;
  char bin1[size], bin2[size], temp[size];
  float res;
  srand((unsigned)time(NULL));

  for (c = 0; c < coef_cnt; c++) {
    /* Real number coeficient crossbreed calculation */
    if (par1->coefs[c].type == 'R') {
      res = (par1->coefs[c].num + par2->coefs[c].num) / 2;
      (*spec1)->coefs[c].num = res;
      (*spec1)->coefs[c].type = 'R';
      (*spec2)->coefs[c].num = res;
      (*spec2)->coefs[c].type = 'R';
    }

    /* Whole number coeficient crossbreed calculation */
    if (par1->coefs[c].type == 'Z') {

      int_coef1 = (int)par1->coefs[c].num;
      int_coef2 = (int)par2->coefs[c].num;

      /* Otherwise if coefs are equal -> Floating point exception */
      if (int_coef1 == int_coef2) {
        res1 = int_coef2;
        res2 = int_coef1;

        (*spec1)->coefs[c].num = res1;
        (*spec1)->coefs[c].type = 'Z';
        (*spec2)->coefs[c].num = res2;
        (*spec2)->coefs[c].type = 'Z';
        continue;
      }

      /* Number coeficients to binary */
      for (i = (size - 1); i >= 0; i--) {
        if ((int_coef1 >> i) & 1) {
          bin1[i] = '1';
          num1_len = num1_len < i ? i : num1_len;
        } else bin1[i] = '0';

        if ((int_coef2 >> i) & 1) {
          bin2[i] = '1';
          num2_len = num2_len < i ? i : num2_len;
        } else bin2[i] = '0';
      }

      /* Longer binary num */
      higher_pos = num1_len >= num2_len ? num1_len : num2_len;
      memcpy(temp, bin1, size);

      /* Calculation of minimum bits needed to swap to change coef value */
      for (i = higher_pos; i >= 0; i--) {
        if (bin1[i] == bin2[i]) min_bit_swap++;
        else break;
      }

      /* Generator of random bit count, which will be swapped */
      swap_count = (min_bit_swap + 1) + rand() %
        ((higher_pos + 1) - min_bit_swap);


      /* Swapping */
      for (i = higher_pos; i > (higher_pos - swap_count); i--) {
        bin1[i] = bin2[i];
        bin2[i] = temp[i];
      }

      /* Binary to int number */
      for (i = (size -1); i >= 0; i--) {
        if (bin1[i] == '1') res1 += (int) pow(2, i);
        if (bin2[i] == '1') res2 += (int) pow(2, i);
      }

      (*spec1)->coefs[c].num = res1;
      (*spec1)->coefs[c].type = 'Z';
      (*spec2)->coefs[c].num = res2;
      (*spec2)->coefs[c].type = 'Z';
    }
  }
}

void print_spec(specimen *spec) {
  printf("----- SPEC ID: %d ----\n", spec->id);
  int i;
  for(i = 0; i < 2; i++) {
    printf("COEF %d: %f\n", (i + 1), spec->coefs[i].num);
  }
  printf("EXT: %f\n-----\n", spec->ext);
}
