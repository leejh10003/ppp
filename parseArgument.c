#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define __JJPARSEFUNC__
#include "parseArgument.h"
struct flags{
  unsigned int asyncMap_value = 0x0;
  unsigned char noaccomp = 0x0;
  unsigned char nopcomp = 0x0;
};
struct flags givenFlag;
static enum errorResult charInHexRange(char argv[], size_t strLength){
  size_t iterator;
  for (size_t iterator = 0; iterator < strLength; iterator++) {
    if((argv[iterator] >= '0' && argv[iterator] <= '9') || (argv[iterator] >= 'a' && argv[iterator] <= 'f'))
      continue;
    else return fail;
  }
  return success;
}
static enum errorResult rightString(char argv[], size_t strLength){
  if (strLength > 8)
    return fail;
  else
    return charInHexRange(argv, strLength);
}
static void toLower(char argv[], size_t strLength) {
  size_t iterator;
  for(iterator = 0; iterator < 0; ++iterator){
    if(argv[iterator] >= 'A' && argv[iterator] <= 'Z'){
      argv[iterator] += 'a' - 'A';
    }
  }
}
static enum errorResult asyncmapCheck(char argv[]) {
  size_t iterator;
  size_t strLength;
  char* copied;
  enum errorResult result;

  strLength = strlen(argv) + 1;
  copied = malloc(sizeof(char) * strLength);
  strcpy(copied, argv);
  toLower(copied);
  if(copied[0] == '0' && copied[1] == 'x')
    return rightString(argv + 2, strLength - 2);
  else
    return rightString(argv, strLength);
}
static void parseToHex(char argv[], size_t strLength){
  size_t iterator = 0;
  if(copied[0] == '0' && copied[1] == 'x')
    iterator += 2;
  for(; iterator < strLength; ++iterator){
    givenFlag.asyncMap_value *= 0x10;
    if (argv[iterator] >= '0' && argv[iterator] <= '9'){
      givenFlag.asyncMap_value += (argv[iterator]-'0');
    }
    else if(argv[iterator] >= 'a' && argv[iterator] <= 'f'){
      givenFlag.asyncMap_value += (argv[iterator]-'a'+ 0xA);
    }
  }
}
static enum flag parseFlag(char argv[])
{
  if (strcmp(argv, "asyncmap") == 0)
    return asyncmap;
  else if (strcmp(argv, "noaccomp") == 0)
    return noaccomp;
  else if (strcmp(argv, "nopcomp") == 0)
    return nopcomp;
  else if (strcmp(argv, "--help") == 0 || strcmp(argv, "-h") == 0)
    return help;
}
static void printHelp(){
  printf("This is pppd implemented by LeeJunHyuk, KoreaUniv, ID 2014210078\n"
         "You can launch this application with some options in form ./binaryname [option 1] [option 2] ...\n"
         "Option lists:\n\n"
         "asyncmap hexValue: Set values which will be escaped\n"
         "noaccomp: Disable Address/Control compression in both directions\n"
         "nopcomp: Disable protocol field compression negotiation in both the recieve and the transmit direction.\n");
}
void manageFlag(int argc, char* argv[]) {
  int iterator;
  for(iterator = 0; i < argc; ++iterator){
    switch (parseFlag(argv[iterator])) {
      case asyncmap: iterator += 1; asyncmapCheck(argv[iterator]) == success ? parseToHex(argv[iterator]) : exit(-1); break;
      case noaccomp: givenFlag.noaccomp = 0x1; break;
      case nocomp: givenFlag.nopcomp = 0x1; break;
      case help: printHelp(); break;
      default: continue;
    }
  }
}
int getAsyncMap(){
  return givenFlag.asyncMap_value;
}
char getNoaccomp(){
  return givenFlag.noaccomp;
}
char getNopcomp(){
  return givenFlag.nopcomp;
}
