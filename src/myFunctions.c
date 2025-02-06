#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "myFunctions.h"

#ifdef _WIN32
#include <windows.h> 
#else
#include <unistd.h>  
#endif


long int getLong(char* display){
  //learned this from 
  //https://sekrit.de/webdocs/c/beginners-guide-away-from-scanf.html
  //This code requires <stdlib.h> and <errno.h>

  char buff[128];
  long int output;
  int success;

  do{
    printf("%s", display);
    if(fgets(buff, 128, stdin) == NULL){
      //reading input failed
      printf("Reading input failed\n");
      return -1;
    }

    char *endptr;
    errno = 0; //reset error number

    output = strtol(buff, &endptr, 10);
    if(errno == ERANGE){
      printf("This number is either too small or too large\n");
      success = 0;
    }else if(endptr == buff){
      printf("No characters read, please enter an integer\n");
      success = 0;
    }else if(*endptr && *endptr != '\n'){
      //*endptr is neither end of string or \n 
      //so the *whole* input wasnt read 
      printf("Enter *only* an integer value\n");
      success = 0;
    }else{
      success = 1;
    }
  }while(!success);

  return output;
}

double getDouble(char* display){

  char buff[128];
  double output;
  int success;

  do{
    printf("%s", display);
    if(fgets(buff, 128, stdin) == NULL){
      //reading input failed
      printf("Reading input failed\n");
      return -1;
    }

    char *endptr;
    errno = 0; //reset error number

    output = strtol(buff, &endptr, 10);
    if(errno == ERANGE){ //THIS DOESNT WORK FIX LATER
      printf("This number is either too small or too large\n");
      success = 0;
    }else if(endptr == buff){
      printf("No characters read, please enter a floating point number\n");
      success = 0;
    }else if(*endptr && *endptr != '\n'){
      //*endptr is neither end of string or \n 
      //so the *whole* input wasnt read 
      printf("Enter *only* a floating point number\n");
      success = 0;
    }else{
      success = 1;
    }
  }while(!success);

  return output;
}
void matrixPrint(double* matrix, int size_x, int size_y){
  for(int i = 0; i < size_x; ++i){
    for(int j = 0; j < size_y; ++j){
      printf("[ %.2f ]", matrix[i * size_x + j]);
    }
    printf("\n");
  }
  printf("\n");
}

double* matrixAddSym(double* m1, double* m2, int size){
  double* dest = (double*)malloc(sizeof(double) * size * size);
  if(sizeof(m1) >= sizeof(dest) && sizeof(m2) >= sizeof(dest)){
    for(int i = 0; i < size * size; ++i){
      dest[i] = m1[i] + m2[i];
    }
  }else{
    printf("One or both of the matrices to add are not big enough\n");
  }
  return dest;
}

double* matrixMultSym(double* m1, double* m2, int size){
  double* dest = (double*)malloc(sizeof(double) * size * size);
  //nvm i dont actually wanna do this
  
  return dest;
}

double* matrixAddAsym(double* m1, double* m2, int size_x, int size_y){
  //todo
}

double* matrixMultAsym(double* m1, double* m2, int size_x, int size_y){
  //todo
}

//sleep functions are platform dependent
void sleep_s(double seconds){ 
  if(seconds < 0)
    return;

  #ifdef _WIN32
  Sleep(seconds * 1000); //windows sleep function uses ms
  #else
  sleep((unsigned int)seconds);
  #endif
}

void clearKBBuff(){
  char c = '\0';
  while(c = getchar() != '\n');
}
