#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <linux/ip.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/netlink.h>
#include <sys/stat.h>
#include <time.h>
#include <assert.h>
#include <fcntl.h>
static int mru(unsigned char packetCutted[]) {
  int i;
  printf("MRU\t");
  if(packetCutted[1] == 4){
    printf("Good length\t Data: \t");
    for(i = 0; i< 2; ++i){
      printf("%X\t", packetCutted[2+i]);
    }
    printf("\n");
    return 4;
  }
  else{
    printf("Bad length\n");
    return -1;
  }
}
static int accm(unsigned char packetCutted[]) {
  int i;
  printf("ACCM\t");
  if(packetCutted[1] == 6){
    printf("Good length\t Data: \t");
    for(i = 0; i< 4; ++i){
      printf("%X\t", packetCutted[2+i]);
    }
    printf("\n");
    return 6;
  }
  else{
    printf("Bad length\n");
    return -1;
  }
}
static int auth(unsigned char packetCutted[]){
  int i;
  printf("Authentication\t");
  if(packetCutted[1] == 5 || packetCutted[1] == 6){
    printf("Good length\t Data: \t");
    for(i = 0; i< packetCutted[1]-2; ++i){
      printf("%X\t", packetCutted[2+i]);
    }
    printf("\n");
    return packetCutted[1];
  }
  else{
    printf("Bad length\n");
    return -1;
  }
}
static int magicNum(unsigned char packetCutted[]){
  int i;
  printf("Magic Number\t");
  if(packetCutted[1] == 6){
    printf("Good length\t Data: \t");
    for(i = 0; i< 4; ++i){
      printf("%X\t", packetCutted[2+i]);
    }
    printf("\n");
    return 6;
  }
  else{
    printf("Bad length\n");
    return -1;
  }
}
static int prtclComp(unsigned char packetCutted[]){
  int i;
  printf("Protocol Compress\t");
  if(packetCutted[1] == 2){
    printf("Good length\t Data: \t");
    for(i = 0; i< 2; ++i){
      printf("%X\t", packetCutted[2+i]);
    }
    printf("\n");
    return 4;
  }
  else{
    printf("Bad length\n");
    return -1;
  }
}
static int adrCtrlComp(unsigned char packetCutted[]){
  int i;
  printf("Address/Control Compress\t");
  if(packetCutted[1] == 2){
    printf("Good length\t Data: \t");
    for(i = 0; i< 2; ++i){
      printf("%X\t", packetCutted[2+i]);
    }
    printf("\n");
    return 4;
  }
  else{
    printf("Bad length\n");
    return -1;
  }
}
static int callback(unsigned char packetCutted[]){
  int i;
  printf("Callback\t");
  if(packetCutted[1] == 3){
    printf("Good length\t Data: \t%X\t\n", packetCutted[2+i]);
    return 3;
  }
  else{
    printf("Bad length\n");
    return -1;
  }
}
static void lcpDecode(unsigned char packetCutted[], int length) {
  int move = 0;
  int returned;
  while (move < length){
    switch (packetCutted[0 + move]) {
      case 0X01: returned = mru(packetCutted + move); break;
      case 0X02: returned = accm(packetCutted + move); break;
      case 0X03: returned = auth(packetCutted + move); break;
      case 0X05: returned = magicNum(packetCutted + move); break;
      case 0X07: returned = prtclComp(packetCutted + move); break;
      case 0X08: returned = adrCtrlComp(packetCutted + move); break;
      case 0X0D: returned = callback(packetCutted + move); break;
      default: printf("Unknown\t"); break;
    };
    if(returned < 0)
      break;
    else
      move += returned;
  };
  printf("\n");
}
void lcp(unsigned char packetCutted[], int length) {
  int iterator;
  int id;
  short lengthInPacket;
  for (iterator = 0; iterator < length; iterator++) {
    printf("%X\t", packetCutted[iterator]);
  }
  printf("\n");
  switch (packetCutted[0]) {
    case 0X01: printf("Configure Request\t"); break;
    case 0x02: printf("Configure Acknowlogement\t"); break;
    case 0X03: printf("Configure None-Acknowlogement\t"); break;
    case 0X04: printf("Configure Reject\t"); break;
    case 0X05: printf("Terminate Request\t"); break;
    case 0X06: printf("Terminate Acknowlogement\t"); break;
    case 0X07: printf("Code Reject\t"); break;
    case 0X08: printf("Protocol Reject\t"); break;
    case 0X09: printf("Echo Eequest\t"); break;
    case 0X0A: printf("Echo Reply\t"); break;
    case 0X0B: printf("Discard Request\t"); break;
    default: printf("Strange LCP code\t");
  }
  id = packetCutted[1];
  ((char*)&lengthInPacket)[0] = packetCutted[3];
  ((char*)&lengthInPacket)[1] = packetCutted[2];
  printf("%Xis length given to function. \t", length);
  printf("%X is ", lengthInPacket);
  if(length == lengthInPacket){
    printf("Good length\t");
  }
  else{
    printf("bad length\t");
  }
  printf("\n\n\n");
  lcpDecode(packetCutted + 4, length - 4);
}
