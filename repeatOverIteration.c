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
#include "crc.h"
#include "lcp.h"
#define MAX_PPP_PACKET_LENGTH 1500
static void pppRead(unsigned char packet[], int length, int flag) {
  int test;
  int iterator;
  for (iterator = 0; iterator < length; iterator++) {
    printf("%X\t", packet[iterator]);
  }
  test = tryfcs16(packet, length);
  printf("\n");
  switch (test) {
    case 1: printf("CRC test result is good\t"); break;
    default: printf("CRC test result is bad\t");
  }
  switch (packet[0]) {
    case 0XFF: printf("PPP protocol\t"); break;
    default: printf("else protocol\t"); break;
  }
  switch (packet[1]) {
    case 0X03: printf("good control\t"); break;
    default: printf("bad control\t"); break;
  }
  switch (*((unsigned short*)(packet+2))) {
    case 0X2180: printf("IPCP protocol\t"); break;
    case 0X21C0: printf("LCP protocol\t"); printf("\n\n\n"); lcpRcvd(packet+4, length-6, packet + length - 3); printf("\n\n\n"); break;
    default: printf("else protocol\t");break;
  }
}

void readPacket(int fileDescriptor, unsigned char buffer[])
{
  int responseLength;
  int iterator;
  static unsigned char packet[MAX_PPP_PACKET_LENGTH];
  static int packetLength = 0;
  static int before = 0x00;
  static int escape_on = 0;
  fflush(stdin);
  fflush(stdout);
  responseLength = read(fileDescriptor, buffer, 255); //Read recieved data from fileDescriptor and save it to buffer
  for (iterator=0 ; iterator<responseLength; iterator++){
    printf("%X\t", buffer[iterator]);
    if(buffer[iterator] == 0X7E){
      if(packetLength > 0){
        printf("\n\n\n");
        pppRead(packet+1, packetLength -1, 0);
        packetLength = 0;
        continue;
      }
      packetLength += 1;
    }
    else if (buffer[iterator] == 0X7D){
      escape_on = 1;
    }
    else{
      if(escape_on == 1){
        packet[packetLength] = (unsigned char)((char)buffer[iterator] - 0X20);
        escape_on = 0;
      }
      else{
        packet[packetLength] = buffer[iterator];
      }
      packetLength += 1;
    }
  }
}
void writePacket(int fileDescriptor) {

}
