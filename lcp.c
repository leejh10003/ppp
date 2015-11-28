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
#include "crc.h"
#define __JJLCPFUNC__
#include "lcp.h"
unsigned char mru_value[2]={0X15,0X00};
unsigned char mru_recieved=0x0;
unsigned char accm_value[4]={0x00,0x00,0x00,0x00};
unsigned char accm_recieved=0x0;
unsigned char magic_num[4]={0x00,0x00,0x00,0x00};
unsigned char magic_recieved=0x0;
unsigned char protocol_comp[2]={0x0,0x00};
unsigned char protocol_comp_recieved=0x0;
unsigned char adr_cont_comp[2]={0x00,0x00};
unsigned char adr_cont_comp_recieved=0x0;
unsigned char callback_value[1]={0x00};
unsigned char callback_recieved=0x0;
unsigned char recieved_crc[2];
unsigned char id;
static void escape(unsigned char packet[], unsigned char escaped[], int length){
  int iterator;
  int index = 1;
  for(iterator = 0; iterator < length; ++ iterator){
    if (packet[iterator] <0X20 && packet[iterator] >= 0x00){
      escaped[index] = 0X7D;
      index += 1;
      escaped[index] = packet[iterator] + 0x20;
    }
    else{
      escaped[index] = packet[iterator];
    }
    index += 1;
  }
}
static int getEscapedLength(unsigned char packet[], int length){
  int iterator;
  int ret = 4;
  for(iterator = 0; iterator < length; ++ iterator){
    if(packet[iterator] >= 0x0 && packet[iterator] < 0x20)
      ret += 1;
      ret += 1;
    }
  return ret;
}
static int getPacketLength(){
  int ret = 6;
  if (mru_recieved == 1)
    ret += 4;
  if (accm_recieved == 1)
    ret += 6;
  if (magic_recieved == 1)
    ret += 6;
  if (protocol_comp_recieved == 1)
    ret += 4;
  if (adr_cont_comp_recieved == 1)
    ret += 4;
  if (callback_recieved == 1)
    ret += 3;
  return ret;
}
void lcpOptionsAppend(unsigned char* packet) {
  int index = 8;
  if (mru_recieved == 1){
    packet[index] = 0x01;
    packet[index+1] = 0x04;
    packet[index+2] = mru_value[0];
    packet[index+3] = mru_value[1];
    index += 0x04;
  }
  if (accm_recieved == 1){
    packet[index] = 0x02;
    packet[index+1] = 0x06;
    packet[index+2] = accm_value[0];
    packet[index+3] = accm_value[1];
    packet[index+4] = accm_value[2];
    packet[index+5] = accm_value[3];
    index += 0x06;
  }
  if (magic_recieved == 1){
    packet[index] = 0x05;
    packet[index+1] = 0x06;
    packet[index+2] = magic_num[0];
    packet[index+3] = magic_num[1];
    packet[index+4] = magic_num[2];
    packet[index+5] = magic_num[3];
    index += 0x06;
  }
  if (protocol_comp_recieved == 1){
    packet[index] = 0x07;
    packet[index+1] = 0x02;
    packet[index+2] = protocol_comp[0];
    packet[index+3] = protocol_comp[1];
    index += 0x04;
  }
  if (adr_cont_comp_recieved == 1){
    packet[index] = 0x08;
    packet[index+1] = 0x02;
    packet[index+2] = adr_cont_comp[0];
    packet[index+3] = adr_cont_comp[1];
    index += 0x04;
  }
  if (callback_recieved == 1){
    packet[index] = 0x0D;
    packet[index+1] = 0x03;
    packet[index+2] = callback_value[0];
    index += 0x03;
  }
}
void lcpSend(enum lcpPacketForm packetForm, int fileDescriptor){
  unsigned char* sendPacket;
  unsigned char* escaped;
  int length;
  int escapedLength;
  int iterator;
  short lengthInPacket;
  length = getPacketLength();
  printf("%X\n", length);
  sendPacket = (unsigned char*)malloc(sizeof(unsigned char)*length + 2);
  sendPacket[0] = 0xFF;
  sendPacket[1] = 0x03;
  sendPacket[2] = 0xC0;
  sendPacket[3] = 0x21;
  switch (packetForm) {
    case confReq: sendPacket[4] = 0X01; break;
    case confAck: sendPacket[4] = 0X02; break;
    case confNak: sendPacket[4] = 0X03; break;
    case confRej: sendPacket[4] = 0X04; break;
    case termReq: sendPacket[4] = 0X05; break;
    case termAck: sendPacket[4] = 0X06; break;
    case codeRej: sendPacket[4] = 0X07; break;
    case prtclRej: sendPacket[4] = 0X08; break;
    case echoReq: sendPacket[4] = 0X09; break;
    case echoRply: sendPacket[4] = 0X0A; break;
    case dscd: sendPacket[4] = 0X0B; break;
    default: break;
  }
  sendPacket[5] = id;
  lengthInPacket = length;
  lengthInPacket -= 2;
  printf("%X\n", lengthInPacket);
  printf("%X\t%X\n", recieved_crc[0], recieved_crc[1]);
  sendPacket[6] = ((unsigned char*)(&lengthInPacket))[1];
  sendPacket[7] = ((unsigned char*)(&lengthInPacket))[0];
  lcpOptionsAppend(sendPacket);
  // printf("%X\n", pppfcs16( PPPINITFCS16, sendPacket, length));
  for(iterator = 0; iterator < length+2; ++ iterator)
    printf("%X\t", sendPacket[iterator]);
  printf("\n\n");
  escapedLength = getEscapedLength(sendPacket, length) + 4;
  escaped = (unsigned char*)malloc(sizeof(unsigned char)*escapedLength);
  escape(sendPacket, escaped, length + 2);
  escaped[0] = 0x7E;
  escaped[escapedLength - 3] = recieved_crc[0];
  escaped[escapedLength - 2] = recieved_crc[1];
  escaped[escapedLength - 1] = 0x7E;
  write(fileDescriptor, escaped, escapedLength);
  printf("%X\n", escapedLength);
  for(iterator = 0; iterator < escapedLength; ++ iterator)
    printf("%X\t", escaped[iterator]);
}
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
    mru_recieved=0x1;
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
    accm_recieved=0x1;
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
    magic_recieved=0x1;
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
    protocol_comp_recieved=0x1;
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
    adr_cont_comp_recieved=0x1;
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
    callback_recieved=0x1;
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
void lcpRcvd(unsigned char packetCutted[], int length, unsigned char crc[], int fileDescriptor) {
  int iterator;
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
  memcpy(recieved_crc, crc, 2);
  lcpOptionDecode(packetCutted + 4, length - 4);
  lcpStateMachine(up_event);
  lcpSend(confAck, fileDescriptor);
}
