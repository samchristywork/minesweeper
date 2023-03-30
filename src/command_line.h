#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Argument {
  char *longName;
  char *description;
  char *value;
  int set;
  void (*function)();
};

struct Argument g_arguments[26] = {{0}};

void set_arg_function(void (*function)(), char short_name) {
  int idx = short_name - 'a';

  if (idx >= 0 && idx < 26) {
    g_arguments[idx].function = function;
  }
}

void add_arg(char short_name, const char *longName, const char *description) {
  int idx = short_name - 'a';

  if (idx >= 0 && idx < 26) {
    g_arguments[idx].longName = (char *)malloc(strlen(longName) + 1);
    g_arguments[idx].description = (char *)malloc(strlen(description) + 1);

    strcpy(g_arguments[idx].longName, longName);
    strcpy(g_arguments[idx].description, description);
  }
}

int get_is_set(char short_name) {
  int idx = short_name - 'a';

  if (idx >= 0 && idx < 26) {
    return g_arguments[idx].set;
  }

  return 0;
}

char *get_value(char short_name) {
  int idx = short_name - 'a';

  if (idx >= 0 && idx < 26) {
    if (g_arguments[idx].value) {
      return g_arguments[idx].value;
    }
  }

  return (char *)malloc(0);
}

void usage() {
  printf("Usage:\n");
  for (int i = 0; i < 26; i++) {
    if (g_arguments[i].description) {
      printf("  -%c,--%s ", 'a' + i, g_arguments[i].longName);
      for (int j = strlen(g_arguments[i].longName); j < 17; j++) {
        printf(" ");
      }
      printf("%s\n", g_arguments[i].description);
    }
  }

  exit(EXIT_SUCCESS);
}

void parse_opts(int argc, char *argv[]) {
  add_arg('h', "help", "Display Usage statement.");
  set_arg_function(usage, 'h');

  int last_arg = -1;

  for (int i = 1; i < argc; i++) {
    if (strlen(argv[i]) >= 2) {
      if (argv[i][0] == '-') {

        // Short opts
        if (argv[i][1] != '-') {
          for (int j = 1; j < strlen(argv[i]); j++) {
            if (argv[i][j] >= 'a' && argv[i][j] <= 'z') {
              last_arg = argv[i][j] - 'a';
              g_arguments[last_arg].set = 1;
              if (g_arguments[last_arg].function) {
                g_arguments[last_arg].function();
              }
            }
          }

          // Long opts
        } else {
          if (strlen(argv[i]) >= 3) {
            for (int j = 0; j < 26; j++) {
              if (g_arguments[j].longName) {
                if (strcmp(g_arguments[j].longName, argv[i] + 2) == 0) {
                  last_arg = j;
                  g_arguments[last_arg].set = 1;
                  if (g_arguments[last_arg].function) {
                    g_arguments[last_arg].function();
                  }
                  break;
                }
              }
            }

            // Double hyphen
          } else {
            return;
          }
        }

        // Value
      } else {
        if (last_arg != -1) {
          g_arguments[last_arg].value = (char *)malloc(strlen(argv[i]) + 1);
          strcpy(g_arguments[last_arg].value, argv[i]);
        }
      }

      // Single character arg
    } else {
      if (last_arg != -1) {
        g_arguments[last_arg].value = (char *)malloc(strlen(argv[i]) + 1);
        strcpy(g_arguments[last_arg].value, argv[i]);
      }
    }
  }
}

#endif
