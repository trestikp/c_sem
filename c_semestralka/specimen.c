/*
    Seminar work of 'Programming in C'
    Theme: finding function extreme by optimalazing function with
      genetic algorithm
    Author: Pavel Trestik

*/
#include "specimen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define MAX_LINE_LEN 80
#define TEMP_FILE "temp.txt"

/*
* Function for creating specimen. Since pointer to the specimen
* and his coeficients (and their count) are already given to
* the function only allocs memory
*/
void create_specimen(specimen **spec, coef coefs[], int coef_cnt){
  *spec = (specimen*) malloc(sizeof(specimen));
  (*spec)->coefs = (coef*) malloc(sizeof(coef) * coef_cnt);
  (*spec)->coefs = coefs;
}

/*
* Function calculates specimens extreme (= fitness) by modifying
* meta file and calling external program
*/
int get_extreme(specimen **spec){
  FILE *f, *temp_f, *func_out;
  char line[MAX_LINE_LEN], var_type;
  int rewrite_next = 0, it, num_cnt = 0;
  double res;

  f = fopen(meta_name, "r");

  if (f == NULL) {
    printf("Failed to load file.\n");
    return 1;
  }

  temp_f = fopen(TEMP_FILE, "w");

  if (temp_f == NULL) {
    printf("Failed to create temp file.\n");
    fclose(f);
    return 1;
  }

  while (!feof(f)) {
    strcpy(line, "\0");
    fgets(line, MAX_LINE_LEN, f);

    if (rewrite_next == 1) {
      if (var_type == REAL_NUM)
        fprintf(temp_f, "%c = %f\n", line[0] , (*spec)->coefs[num_cnt].num);
      if (var_type == WHOLE_NUM)
        fprintf(temp_f, "%c = %d\n", line[0], (int)(*spec)->coefs[num_cnt].num);

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
      /*
      * !!!!!!!! cr - crlf -> -1, nebo -2;
      */
      if (line[it - 1] == '\r') {
        var_type = line[it - 2];
      } else {
        var_type = line[it - 1];
      }
    }

    fprintf(temp_f, "%s", line);
  }

  fclose(f);
  fclose(temp_f);

  /* Serves for the source files being in work dir
  *
  remove(meta_source);
  rename(TEMP_FILE, meta_source);
  */

  /*
  * Used for modifying meta file in its source directory

  f = fopen(meta_source, "w");
  temp_f = fopen(TEMP_FILE, "r");

  char c;
  while ((c = fgetc(temp_f)) != EOF) {
    fputc(c, f);
  }

  fclose(f);
  fclose(temp_f);
  */

  remove(meta_name);
  rename(TEMP_FILE, meta_name);

  func_out = popen(calc_func, "r");

  if (func_out == NULL) {
    printf("Failed to execute extreme calculation\n");
    return 1;
  }

  while (fgets(line, MAX_LINE_LEN, func_out)) {
    sscanf(line, "%lf", &res);
  }

  fclose(func_out);

  (*spec)->ext = res;

  return 0;
}

/*
* Function crossbreeds 2 specimens into 2 new ones.
*/
void crossbreed(specimen *par1, specimen *par2, specimen **spec1,
   specimen **spec2, int coef_cnt, interval is[]) {

  int i, c, size = sizeof(int) * 8, num1_len = 0, num2_len = 0;
  int swap_count = 0, higher_pos = 0, res1 = 0, res2 = 0;
  int int_coef1, int_coef2;
  char bin1[size], bin2[size], temp[size];
  double res;

  /* Variables for ensuring interval bounds */
  char temp_bin1[size], temp_bin2[size];
  int temp1, temp2, j, top, bot;
  int valid_swaps = 0, *solutions;

  srand((unsigned)time(NULL));

  for (c = 0; c < coef_cnt; c++) {
    /* Real number coeficient crossbreed calculation */
    if (par1->coefs[c].type == REAL_NUM) {
      res = (par1->coefs[c].num + par2->coefs[c].num) / 2;
      (*spec1)->coefs[c].num = res;
      (*spec1)->coefs[c].type = REAL_NUM;
      (*spec2)->coefs[c].num = res;
      (*spec2)->coefs[c].type = REAL_NUM;
    }

    /* Whole number coeficient crossbreed calculation */
    if (par1->coefs[c].type == WHOLE_NUM) {

      int_coef1 = (int)par1->coefs[c].num;
      int_coef2 = (int)par2->coefs[c].num;
      top = is[c].top;
      bot = is[c].bot;

      /* Otherwise if coefs are equal -> Floating point exception */
      if (int_coef1 == int_coef2) {
        res1 = int_coef2;
        res2 = int_coef1;

        (*spec1)->coefs[c].num = res1;
        (*spec1)->coefs[c].type = WHOLE_NUM;
        (*spec2)->coefs[c].num = res2;
        (*spec2)->coefs[c].type = WHOLE_NUM;
        continue;
      }

      /* Number coeficients to binary */
      for (i = (size - 1); i >= 0; i--) {
        if ((int_coef1 >> i) & 1) {
          bin1[i] = BIT_ONE;
          num1_len = num1_len < i ? i : num1_len;
        } else bin1[i] = BIT_ZERO;

        if ((int_coef2 >> i) & 1) {
          bin2[i] = BIT_ONE;
          num2_len = num2_len < i ? i : num2_len;
        } else bin2[i] = BIT_ZERO;
      }

      /* Longer binary num */
      higher_pos = num1_len >= num2_len ? num1_len : num2_len;
      memcpy(temp, bin1, size);

      /* Not the best malloc, but still smaller than 32 */
      solutions = (int*) malloc(sizeof(int) * (higher_pos + 1));

      /* Unfortunatly brute force swap determination */
      for (i = higher_pos; i >= 0; i--) {
        if (bin1[i] == bin2[i]) continue;

        /* Init helping vars to default */
        temp1 = 0;
        temp2 = 0;
        for (j = 0; j < size; j++) {
          temp_bin1[j] = bin1[j];
          temp_bin2[j] = bin2[j];
        }

        /* Swapping */
        for (j = higher_pos; j > i; j--) {
          temp_bin1[j] = temp_bin2[j];
          temp_bin2[j] = temp[j];
        }

        /* To int */
        for (j = (size -1); j >= 0; j--) {
          if (temp_bin1[j] == BIT_ONE) temp1 += (int) pow(2, j);
          if (temp_bin2[j] == BIT_ONE) temp2 += (int) pow(2, j);
        }

        /* If block swap is valid, add to possible solutions */
        if (temp1 < top && temp1 > bot && temp2 < top && temp2 > bot) {
          solutions[valid_swaps] = i;
          valid_swaps++;
        }
      }

      /* If no swaps are valid */
      if (valid_swaps == 0) {
        res1 = int_coef2;
        res2 = int_coef1;

        (*spec1)->coefs[c].num = res1;
        (*spec1)->coefs[c].type = WHOLE_NUM;
        (*spec2)->coefs[c].num = res2;
        (*spec2)->coefs[c].type = WHOLE_NUM;
        continue;
      }

      if (valid_swaps == 1) {
        swap_count = 0;
      } else {
        swap_count = rand() % valid_swaps;
      }

      /* Swapping */
      for (i = higher_pos; i > solutions[swap_count]; i--) {
        bin1[i] = bin2[i];
        bin2[i] = temp[i];
      }

      /* Binary to int number */
      for (i = (size -1); i >= 0; i--) {
        if (bin1[i] == BIT_ONE) res1 += (int) pow(2, i);
        if (bin2[i] == BIT_ONE) res2 += (int) pow(2, i);
      }

      (*spec1)->coefs[c].num = res1;
      (*spec1)->coefs[c].type = WHOLE_NUM;
      (*spec2)->coefs[c].num = res2;
      (*spec2)->coefs[c].type = WHOLE_NUM;

      free(solutions);
    }
  }
}

/*
* Functions mutates a specimen.
*/
void mutate(specimen **spec, interval is[], int coef_cnt) {
  int mod_inter = -1, size = sizeof(int) * 8, i, orig_num;
  int num_in = 0, changes_cnt = 0, j = 0, res = 0, power = 0;
  int  *swaps, rand_num, curr_len = 0, bot, top, diff;
  char bin[size];
  double new_val;
  srand((unsigned)time(NULL));

  /* Random gene, which will be mutated */
  mod_inter = rand() % coef_cnt;

  /* If mutated gene is real number */
  if ((*spec)->coefs[mod_inter].type == REAL_NUM) {
    new_val = is[mod_inter].bot + ((double)rand()/RAND_MAX *
      (is[mod_inter].top - is[mod_inter].bot));
    (*spec)->coefs[mod_inter].num = new_val;
  }
  /* If muated gene is whole number */
  if ((*spec)->coefs[mod_inter].type == WHOLE_NUM) {
    orig_num = (int)(*spec)->coefs[mod_inter].num;
    bot = is[mod_inter].bot;
    top = is[mod_inter].top;

    /* Coef to binary */
    for (i = (size - 1); i >= 0; i--) {
      if ((orig_num >> i) & 1) {
        bin[i] = BIT_ONE;
        num_in = num_in < i ? i : num_in;
      } else bin[i] = BIT_ZERO;
    }

    /* Max change half of number bits */
    changes_cnt = 1 + rand() % ((num_in + 1) / 2);
    swaps = (int*) malloc(sizeof(int) * (changes_cnt + 1));

    /* Generating which positions will be negated */
    for (i = 0; i < changes_cnt; i++) {
      /* To ensure that the program doesn't just negate 1 bit x times
      *  program later compares random position to all already generated
      *  positions, if the position is alredy there, jumps here and
      *  generates new position
      */
      sem:
      rand_num = rand() % (num_in + 1);
      if (i == 0) {
        if (curr_len >= 1) continue;
        swaps[curr_len] = rand_num;
        curr_len++;
      } else {
        /* Testing if generated postion isn't there yet
        *  if it is, jumps to @sem with goto
        */
        for (j = 0; j < curr_len; j++) {
          if (swaps[j] == rand_num) {
            goto sem;
          }
        }
        swaps[curr_len] = rand_num;
        curr_len++;
      }
    }

    /* Apply changes (negations) */
    for (i = 0; i < changes_cnt; i++) {
      if (bin[swaps[i]] == BIT_ONE) bin[swaps[i]] = BIT_ZERO;
      else bin[swaps[i]] = BIT_ONE;
    }

    free(swaps);

    /* To int */
    for (i = (size -1); i >= 0; i--) {
      if (bin[i] == BIT_ONE) res += (int) pow(2, i);
    }

    i = 0;
    /* Ensuring top limit of interval */
    if (res > top) {
      diff = res - top;
      while (power < diff) {
        power = pow(2, i);
        i++;
      }
      while (bin[i] != BIT_ONE) {
        i++;
      }
      bin[i] = BIT_ZERO;
    }


    i = 0;
    diff = 0;
    power = 0;
    /* Ensuring bot limit of interval
    * Note - Maybe change closes highest power similar to top limit
    * instead of swapping all 0 to 1 from the end?
    */
    if (res < bot) {
      while (res < bot) {
        if (bin[i] == BIT_ONE) {
          i++;
          continue;
        } else {
          bin[i] = BIT_ONE;
          res += pow(2, i);
          i++;
        }
      }
    }

    /* Recounting result due to mutation fixes (may not change) */
    res = 0;
    for (i = (size -1); i >= 0; i--) {
      if (bin[i] == BIT_ONE) res += (int) pow(2, i);
    }
  }
}
