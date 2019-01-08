/*
    Seminar work of 'Programming in C'
    Theme: finding function extreme by optimalazing function with
      genetic algorithm
    Author: Pavel Trestik

*/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include "specimen.h"
#include "generation.h"

#define MAX_LINE_LEN 80
#define DEFAULT_MUTATION 5
#define TEMP_META "temp_meta.txt"

/* Intervals */
interval *is;
/* Number of intervals */
int is_count = 0;
/* Number of generations to run */
int generation_to_run = 1;

/*
* Function prepares meta file in working dir
* also fills @calc_func and @meta_name
*/
int prepare_func_file(char *meta_file) {
  FILE *meta, *func;
  char line[MAX_LINE_LEN];
  int calc_name_len = 0, i = 0, j, has_slash = 0, size = 0;

  while (meta_file[i] != '\0') {
    if (meta_file[i] == '/' || meta_file[i] == '\\') has_slash = i;
    i++;
  }

  if (has_slash == 0) {
    meta_name = meta_file;
  } else {
    size = i - has_slash;
    /* mallocs + 1 for terminating '\0' */
    meta_name = (char*)malloc(sizeof(char) * size + 1);
    meta_name[size] = '\0';
    for (j = (size - 1);  j >= 0; j--) {
      meta_name[j] = meta_file[i];
      i--;
    }
  }

  meta = fopen(meta_file, "r");

  if (meta == NULL) {
    printf("Failed to open meta file!\n");
    return 1;
  }

  /* TEMP_META because if the meta file is in root dir and has same name -> error */
  func = fopen(TEMP_META, "w");

  if (func == NULL) {
    fclose(meta);
    printf("Failed to create func file\n");
    return 1;
  }

  while (fgets(line, MAX_LINE_LEN, meta)) {
    if (line[0] == '#' && line[2] != '(') {
      calc_name_len = 2;
      while (line[calc_name_len] != '\n') {
        calc_name_len++;
      }
      /* crlf condition */
      if (line[calc_name_len - 1] == '\r') {
        calc_func = (char*) malloc(sizeof(char) * (calc_name_len - 3 + 1));
        for (i = 0; i < (calc_name_len - 3); i++) {
          calc_func[i] = line[i + 2];
        }
        calc_func[i] = '\0';
      } else {
        calc_func = (char*) malloc(sizeof(char) * (calc_name_len - 2 + 1));
        for (i = 0; i < (calc_name_len - 2); i++) {
          calc_func[i] = line[i + 2];
        }
        calc_func[i] = '\0';
      }
    }
    fputs(line, func);
  }

  fclose(meta);
  fclose(func);

  rename(TEMP_META, meta_name);

  return 0;
}

/*
* Function initializes intervals read from meta_file
*/
int init_intervals(char *meta_file){
  FILE *f = fopen(meta_file, "r");
  char line[MAX_LINE_LEN], type;
  float bot, top;
  int i = 0;

  is_count = 0;

  if (f == NULL) {
    printf("Failed to load file!\n");
    return 1;
  }

  while (fgets(line, MAX_LINE_LEN, f)) {
    if(line[0] == '#' /*&& line[1] == '_'*/ && line[2] == '(') {
      is_count++;
    }
  }

  is = (interval*) malloc(sizeof(interval) * is_count);
  fseek(f, 0L, SEEK_SET);

  while (fgets(line, MAX_LINE_LEN, f)) {
    if(line[0] == '#' /*&& line[1] == '_'*/ && line[2] == '(') {
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

/*
* Function prints help to console
*/
void help() {
  printf("Usage: \n");
  printf("   gms [meta_file] [generation_count] -optional\n");
  printf("      meta_file - file with function data\n");
  printf("      generation_count - number of generations to be done\n");
  printf("   Optional parameter: -m [number]\n");
  printf("      number <5, 45> - percent of generation to be mutated\n");
  printf("If option -m isn't used, default mutation percent is 5\n");
}

/*
* Function operates arguments
*/
int init(int argc, char *argv[]) {
  int mut;

  if (argc == 2 && argv[1][0] == '-' && argv[1][1] == 'h') {
    help();
    return 0;
  }

  mutation_rate = DEFAULT_MUTATION;
  if (argc < 3 || argc == 4 || argc > 5) {
    printf("Invalid paremeter count!\n");
    help();
    return 1;
  }

  if (prepare_func_file(argv[1]) != 0) return 1;
  if (init_intervals(argv[1]) != 0) return 1;
  /*meta_source = argv[1];*/
  generation_to_run = strtol(argv[2], NULL, 10);

  if (argc > 3) {
    if (argv[3][0] == '-' && argv[3][1] == 'm') {
      mut = strtol(argv[4], NULL, 10);
      if (mut < 5 || mut > 45) {
        printf("Invalid mutation percent!\n");
        help();
        return 1;
      } else {
        mutation_rate = mut;
      }
    } else {
      printf("Unknown option!\n");
      help();
      return 1;
    }
  }
  return 0;
}

/*
* Function runs number of entered generations and calls print functions
*/
int run() {
  int i;

  if (create_first_generation(is, is_count) != 0) return 1;
  print_generation();
  print_files();

  for(i = 1; i < generation_to_run; i++) {
    if (next_generation() != 0) return 1;
    print_generation();
    print_files();
  }

  return 0;
}

/*
* Function frees memory after run is finished
*/
void shutdown() {
  remove(meta_name);
  free_gen();
  free(is);
  free(calc_func);
  /* Weird free - doesn't work if intial meta file was in the working dir
     works otherwise !!!*/
  free(meta_name);

  /* Unused with current config
  free(meta_source);
  */
}

void intHandler(int sig) {
  if (sig == SIGTERM) {
    /* "normal free on SIGTERM" */
    shutdown();
    printf("You killed it!\n");
    exit(1);
  }
}

int main(int argc, char *argv[]){
  signal(SIGTERM, intHandler);
  if (init(argc, argv) != 0) {
    shutdown();
    printf("Initialization failed!\n");
    return 1;
  }
  if (run() != 0) {
    shutdown();
    printf("Run error!\n");
    return 1;
  }
  shutdown();

  return 0;
}
