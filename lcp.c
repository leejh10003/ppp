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
#include "status.h"
unsigned char mru_value[2]={0X15,0X00};
unsigned char accm_value[4]={0x00,0x00,0x00,0x00};
unsigned char magic_num[4]={0x00,0x00,0x00,0x00};
unsigned char protocol_comp[2]={0x00,0x00};
unsigned char adr_cont_comp[2]={0x00,0x00};
unsigned char callback_value[1]={0x00};
enum lcpPacketForm{
  lcpNone,
  confReq,
  confAck,
  confNak,
  confRej,
  termReq,
  termAck,
  codeRej,
  prtclRej,
  echoReq,
  echoRply,
  dscd
};
static int mruDecode(unsigned char packetCutted[]) {
  int i;
  printf("MRU\t");
  if(packetCutted[1] == 4){
    printf("Good length\t Data: \t");
    for(i = 0; i< 2; ++i){
      printf("%X\t", packetCutted[2+i]);
    }
    if (*((int*)(packetCutted+2)) == 0X0000) {
      printf("\nMRU unchanged. MRU is 0x");
    }
    else{
      memcpy((void*)mru_value, (void*)(packetCutted+2), 2);
      printf("\nMRU changed. MRU is 0x");
    }
    for(i=0;i<2;++i)
      printf("%X\n", mru_value[i]);
    printf("\n");
    return 4;
  }
  else{
    printf("Bad length\n");
    return -1;
  }
}
static int accmDecode(unsigned char packetCutted[]) {
  int i;
  printf("ACCM\t");
  if(packetCutted[1] == 6){
    printf("Good length\t Data: \t");
    for(i = 0; i< 4; ++i){
      printf("%X\t", packetCutted[2+i]);
    }
    memcpy((void*)accm_value, (void*)(packetCutted+2), 4);
    printf("\n");
    return 6;
  }
  else{
    printf("Bad length\n");
    return -1;
  }
}
static int authDecode(unsigned char packetCutted[]){
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
static int magicNumDecode(unsigned char packetCutted[]){
  int i;
  printf("Magic Number\t");
  if(packetCutted[1] == 6){
    printf("Good length\t Data: \t");
    for(i = 0; i< 4; ++i){
      printf("%X\t", packetCutted[2+i]);
    }
    memcpy((void*)magic_num, (void*)(packetCutted+2), 4);
    printf("\n");
    return 6;
  }
  else{
    printf("Bad length\n");
    return -1;
  }
}
static int prtclCompDecode(unsigned char packetCutted[]){
  int i;
  printf("Protocol Compress\t");
  if(packetCutted[1] == 2){
    printf("Good length\t Data: \t");
    for(i = 0; i< 2; ++i){
      printf("%X\t", packetCutted[2+i]);
    }
    memcpy((void*)protocol_comp, (void*)(packetCutted+2), 2);
    printf("\n");
    return 4;
  }
  else{
    printf("Bad length\n");
    return -1;
  }
}
static int adrCtrlCompDecode(unsigned char packetCutted[]){
  int i;
  printf("Address/Control Compress\t");
  if(packetCutted[1] == 2){
    printf("Good length\t Data: \t");
    for(i = 0; i< 2; ++i){
      printf("%X\t", packetCutted[2+i]);
    }
    memcpy((void*)adr_cont_comp, (void*)(packetCutted+2), 2);
    printf("\n");
    return 4;
  }
  else{
    printf("Bad length\n");
    return -1;
  }
}
static int callbackDecode(unsigned char packetCutted[]){
  int i;
  printf("Callback\t");
  if(packetCutted[1] == 3){
    printf("Good length\t Data: \t%X\t\n", packetCutted[2+i]);
    memcpy((void*)callback_value, (void*)(packetCutted+2), 1);
    return 3;
  }
  else{
    printf("Bad length\n");
    return -1;
  }
}
static void lcpOptionDecode(unsigned char packetCutted[], int length) {
  int move = 0;
  int returned;
  while (move < length){
    switch (packetCutted[0 + move]) {
      case 0X01: returned = mruDecode(packetCutted + move); break;
      case 0X02: returned = accmDecode(packetCutted + move); break;
      case 0X03: returned = authDecode(packetCutted + move); break;
      case 0X05: returned = magicNumDecode(packetCutted + move); break;
      case 0X07: returned = prtclCompDecode(packetCutted + move); break;
      case 0X08: returned = adrCtrlCompDecode(packetCutted + move); break;
      case 0X0D: returned = callbackDecode(packetCutted + move); break;
      default: printf("Unknown\t"); break;
    };
    if(returned < 0)
      break;
    else
      move += returned;
  };
  printf("\n");
}
void lcpSend(enum flag flags) {
}
void lcpRcvd(unsigned char packetCutted[], int length) {
  int iterator;
  int id;
  short lengthInPacket;
  for (iterator = 0; iterator < length; iterator++) {
    printf("%X\t", packetCutted[iterator]);
  }
  printf("\n");
  switch (packetCutted[0]) {
    case confReq: printf("Configure Request\t"); break;
    case confAck: printf("Configure Acknowlogement\t"); lcpStateMachine(rca_event); break;
    case confNak: printf("Configure None-Acknowlogement\t"); break;
    case confRej: printf("Configure Reject\t"); break;
    case termReq: printf("Terminate Request\t"); break;
    case termAck: printf("Terminate Acknowlogement\t"); break;
    case codeRej: printf("Code Reject\t"); break;
    case prtclRej: printf("Protocol Reject\t"); break;
    case echoReq: printf("Echo Request\t"); break;
    case echoRply: printf("Echo Reply\t"); break;
    case dscd: printf("Discard Request\t"); break;
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
  lcpOptionDecode(packetCutted + 4, length - 4);
  lcpStateMachine(up_event);
}
