#include <stdio.h>
#include <stdlib.h>
#include "specimen.h"
#include "generation.h"

#define MAX_LINE_LEN 80

interval *is;
int is_count;

int init_intervals(char *meta_file){
  FILE *f = fopen(meta_file, "r");
  char line[MAX_LINE_LEN], type;
  float bot, top;
  int i = 0;

  is_count = 0;

  if (f == NULL) {
    return EXIT_FAILURE;
  }

  while (fgets(line, MAX_LINE_LEN, f)) {
    if(line[0] == '#' /*&& line[1] == '_'*/ && line[2] == '(') {
      is_count++;
    }
  }

  is = (interval*) malloc(sizeof(interval) * is_count);
  fseek(f, 0L, SEEK_SET);

  while (fgets(line, MAX_LINE_LEN, f)) {
    if(line[0] == '#' && line[1] == '_' && line[2] == '(') {
      sscanf(line, "#_(%f,%f);%c", &bot, &top, &type);
      is[i].bot = bot;
      is[i].top = top;
      is[i].type = type;
      i++;
    }
  }

  fclose(f);
  return 0;
}


int main(){
    /*FILE *f;
    int i, count = 0;
    f = fopen("/res/func01_meta.txt", "r");

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *string = malloc(sizeof(fsize + 1));
    fread(string, fsize, 1, f);

    fclose(f);

    string[fsize] = 0;

    printf("%s\n", string);

    for(i = 0; i < sizeof(string); i++){
        if((string + i) == "#"){
            count++;
        }
    }
    */


/*
    interval i1;
    i1.type = 'R';
    i1.bot = 10;
    i1.top = 18;

    interval i2;
    i2.type = 'Z';
    i2.bot = 0;
    i2.top = 500;

    interval is[2];
    is[0] = i1;
    is[1] = i2;

    generation* gen = create_first_generation(is, 2);

    print_generation();
    */
    int i = 0;

    init_intervals("func01_meta.txt");

    for(i = 0; i < is_count; i++) {
      printf("IS: %d\nBot: %f\nTop: %f\nType: %c\n------\n", (i+1), is[i].bot, is[i].top, is[i].type);
    }

    generation* gen = create_first_generation(is, is_count);

    print_generation();

    for(i = 0; i < 1000; i++) {
      next_generation();
    }

    print_generation();

    //printf("SELECTED SPEC ID: %d\n", gen->species[98].id);

/*
    int i = 0, j = 0;
    for(i = 0; i < GENERATION_SIZE; i++){
      printf("Specimen %d\n", gen->species[i].id);
      for(j = 0; j < 2; j++){
        if(gen->species[i].coefs->type == 'R')
          printf("%f\n", gen->species[i].coefs->f);
        if(gen->species[i].coefs->type == 'Z')
          printf("%d\n", gen->species[i].coefs->d);
      }
      printf("----------\n");
    }
*/
/*
    specimen *spec[100];
    //spec = (specimen*) malloc(sizeof(specimen) * 100);
    int i = 0, j = 0;

    make_inters(2, is);

    for(i = 0; i < 100; i++){
      printf("Creating specimen %d: \n", i);
      create_specimen(&spec[i]);
      printf("Finished creatings specimen: \n");
      for(j = 0; j < 2; j++){
        if((*spec)->coefs[j].type == 'R'){
          printf("Type: %c\n", (*spec)->coefs[j].type);
          printf("Value: %f\n", (*spec)->coefs[j].f);
          printf("Address: %p\n", (void*)&(*spec)->coefs);
        }
        if((*spec)->coefs[j].type == 'Z'){
          printf("Type: %c\n", (*spec)->coefs[j].type);
          printf("Val: %d\n", (*spec)->coefs[j].d);
          printf("Add: %p\n", (*spec)->coefs);
        }

      }

      printf("Specimen address: %p\n-------------\n", (void*)&spec[i]);
    }
*/
    //create_specimen(&spec);

    /*
    for(i = 0; i < 100; i++){
      for(j = 0; j < 2; j++){
        if((*spec)->coefs[j].type == 'R')
          printf("%f\n", (*spec)->coefs[j].f);
        if((*spec)->coefs[j].type == 'Z')
          printf("%d\n", (*spec)->coefs[j].d);
      }
      printf("-------\n");
    }
    */
    //get_specimen(spec);

    return 0;
}
