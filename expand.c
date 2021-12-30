//
// File for the entire program expand, also intitalizes and makes use of our stack.
//
//
// Adithya Jose
// U. of Illinois, Chicago
// CS 211: Fall 2021
//

#include<stdio.h>

#include<stdlib.h>

#include<string.h>

#include<unistd.h>

// Stack Data Structure specifically implemented for this program.
struct Stack {
  char ** string;
  char * invalid;
  int numofFiles;
  int empty;
  int length;
  int capacity;
};

typedef struct Stack Stack;

// Implements the actual stack.
Stack * initialize() {
  Stack * stack = malloc(sizeof(Stack));
  stack -> string = malloc(4 * sizeof(char * ));
  stack -> invalid = malloc(256 * sizeof(char));
  stack -> numofFiles = -1;
  stack -> empty = 0;
  stack -> length = -1;
  stack -> capacity = 4;

  for (int i = 0; i < stack -> capacity; ++i) {
    stack -> string[i] = NULL;
  }
  return stack;
}

// Checks is there is a copy of a string in the stack. Returns 1 if true else returns 0.
int copy(Stack * stack, char * other) {
  for (int i = 0; i <= stack -> numofFiles; ++i) {
    if (!(strcmp(stack -> string[i], other))) {
      return 1;
    }
  }
  return 0;
}

// Pushes a file name to the stack. If there is a copy it returns 0. Also increases the size of the stack accordingly.
int push(Stack * stack, char * string) {
  if (copy(stack, string)) {
    return 0;
  };
  stack -> numofFiles++;
  stack -> length++;

  if ((stack -> numofFiles) >= (stack -> capacity)) {
    stack -> string = realloc(stack -> string, stack -> capacity * 2 * sizeof(char * ));
    stack -> capacity *= 2;
    for (int i = stack -> numofFiles + 1; i < stack -> capacity; ++i) {
      stack -> string[i] = NULL;
    }
  }

  stack -> string[stack -> numofFiles] = string;
  return 1;
}

// edits number of of elements in the stack as well as adjusts the pointer as well.
void pop(Stack * stack) {
  if (stack -> numofFiles < 0) {
    return;
  }
  stack -> numofFiles -= 1;
}

// deletes allocated memory for the stack and frees memory
void deleteStack(Stack * stack) {
  for (int i = 0; i <= stack -> length; ++i) {
    if ((i >= stack -> capacity) || (stack -> string[i] == NULL)) {
      continue;
    }
  }

  free(stack -> string);
  free(stack -> invalid);
  free(stack);
}

// deletes white space from specified space. Returns 0 when certain conditions meet.
int whiteSpaceDeleter(char * string, int counter) {
  if (strlen(string) <= 7) {
    return 0;
  }
  char * newS = string;
  int count = counter;
  int beggining = 0;

  int right = 0;
  int left = 0;
  int quotes = 0;

  for (int i = 0; i < strlen(string); ++i) {
    if (string[i] != ' ') {
      if (string[i] == '"') {
        left = 1;
        break;
      }
    };
    count++;
  }
  if ((string[strlen(string) - 2] == '"') || (string[strlen(string) - 1] == '"')) {
    right = 1;
  }

  for (int i = count; i < strlen(string) - counter; ++i) {
    if ((i != count) && (string[i] == '"')) {
      right = 1;
      break;
    };
    newS[beggining] = string[i];
    beggining++;
  }

  for (int i = beggining; i < strlen(string); ++i) {
    newS[i] = ' ';
  }
  if (right && left)
    quotes = 1;
  memmove(newS, & string[0], beggining - counter);
  newS[beggining] = '\0';
  string = newS;
  return quotes;
}

int trueFile(char * string) {
  char * s = string;

  if (access(s, F_OK) == 0) {

    return 1;
  } else {
    return 0;
  }

}

// closes the file and frees the stack's memory
void empty(FILE * file, Stack * stack) {
  pop(stack);
  fclose(file);
}

// get error string
char * getInvalid(Stack * stack) {
  if (!strcmp(stack -> invalid, " ")) {
    return " ";
  }
  return stack -> invalid;
}

void setInvalid(Stack * stack, char * string) {
  strcpy(stack -> invalid, string);
}

int confirmInvalid(Stack * stack) {
  return stack -> empty;
}

// reads a file recursively. If there is an infinite loop it returns 0.
int readFile(FILE * file, Stack * stack) {
  char b[257];

  while (fgets(b, 256, file) != NULL) {
    if ((b[0] == '#') && (b[7] == 'e')) {

      char ss[257];
      memcpy(ss, & b[8], 256);
      ss[256] = '\0';
      int quotes = whiteSpaceDeleter(ss, 1);
      if ((!trueFile(ss) || !quotes)) {
        strcpy(stack -> invalid, ss);
        stack -> empty = 1;
        if (!quotes) {
          empty(file, stack);
          return -1;
        }
        if (!trueFile(ss)) {
          empty(file, stack);
          return -2;
        }
      }

      int complete = push(stack, ss);

      FILE * file = fopen(ss, "r");
      if (!(complete == 1)) {
        fclose(file);
        empty(file, stack);
        return complete;
      };

      int completeAdvanced = readFile(file, stack);
      if (!(completeAdvanced == 1)) {
        empty(file, stack);
        return completeAdvanced;
      }

    } else
      printf("%s", b);
  }

  pop(stack);

  fclose(file);
  return 1;
}

int main(int c, char * v[]) {
  char * inputFile = "f1.txt";

  for (int i = 0; i < c; ++i) {
    inputFile = v[i];
  }

  char b[257];
  FILE * file = fopen(inputFile, "r");
  Stack * stack = initialize();
  int complete = -5;

  while (fgets(b, 256, file) != NULL) {
    if ((b[0] == '#') && (b[7] == 'e')) {
      char ss[257];
      memcpy(ss, & b[8], 256);
      ss[256] = '\0';
      int quotes = whiteSpaceDeleter(ss, 1);
      if ((!trueFile(ss) || !quotes)) {
        setInvalid(stack, ss);
        if (!quotes) {
          complete = -1;
          break;
        }
        if (!trueFile(ss)) {
          complete = -2;
          break;
        }
      }
      if (!strcmp(inputFile, ss)) {
        complete = 0;
        pop(stack);
        break;
      }

      FILE * file = fopen(ss, "r");
      push(stack, ss);

      complete = readFile(file, stack);

      if (!(complete == 1)) {
        if (!confirmInvalid(stack)) {
          setInvalid(stack, ss);
        }
        break;
      }

    } else
      printf("%s", b);
  }

  if (complete == 0) {
    fprintf(stderr, "**error: Infinite recursion detected!\nExpansion terminated.");
  } else if (complete == -1) {
    fprintf(stderr, "**error: No file found after '#include'!\nExpansion terminated.");
  } else if (complete == -2) {
    fprintf(stderr, "**error: File '%s' doesn't exist!\nExpansion terminated.", getInvalid(stack));
  }

  deleteStack(stack);
  fclose(file);
}