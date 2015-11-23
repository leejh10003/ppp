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

#define MAX_PPP_PACKET_LENGTH 1500
#define BAUDRATE B38400
#define SERIALDEVICE "/dev/ttyS0" //Use commucation device
unsigned char buffer[255];
void repeatOverIteration(int fileDescriptor);
void handlePacket(unsigned char packet[], int length, int flag);
void lcp(unsigned char packetCutted[], int length);
int main(void){
    int fileDescriptor; //File Descriptor variable
    struct termios firstTermConf, secondTermConf;

    memset((void*)&buffer, 0, (size_t)sizeof(buffer)); //Initialize buffer with value 0
    fileDescriptor = open(SERIALDEVICE, O_RDWR | O_NOCTTY, O_NONBLOCK);
    //Open commucation Descriptor with O_NOCTTY option. This function return file descriptor which can write and read data from it.

    if(fileDescriptor<0){
        perror(SERIALDEVICE); //Print error prefix
        exit(-1); //Exit this program if error has happened
    }//If fileDescriptor has not opened, handle that error
    tcgetattr(fileDescriptor, &firstTermConf); //Get fileDescriptor's current terminal attribute and save it to firstTermConf

    memset((void*)&secondTermConf, 0, sizeof(secondTermConf));//Initialize secondTermConf witah value 0
    secondTermConf.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD; //Give control mode of fileDescriptor's new terminal attribute
    secondTermConf.c_iflag = IGNPAR | ICRNL; //Give input mode of fileDescriptor's new terminal attribute

    tcflush(fileDescriptor, TCIFLUSH); //Kill fileDescriptor's current data
    tcsetattr(fileDescriptor, TCSANOW, &secondTermConf); //Give secondTermConf attribute to fileDescriptor
    fflush(stdin); //Flush current standard input buffer
    fflush(stdout); //Flush current standard output buffer
    memset((void*)buffer, 0, sizeof(buffer));

    tcflush(fileDescriptor, TCIFLUSH); //Kill fileDescriptor's current data

    while(1)
    {
      repeatOverIteration(fileDescriptor);
    } //Waiting for user's input
    tcsetattr(fileDescriptor,TCSANOW, &firstTermConf); //restore fileDescriptor's Previous terminal attribute
    return 0;
}
void repeatOverIteration(int fileDescriptor)
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
    if(buffer[iterator] == 0X7E){
      if(packetLength > 0){
        handlePacket(packet+1, packetLength -1, 0);
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
void handlePacket(unsigned char packet[], int length, int flag) {
  int test;
  int iterator;
  test = tryfcs16(packet, length);
  for (iterator = 0; iterator < length; iterator++) {
    printf("%X\t", packet[iterator]);
  }
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
    case 0X21C0: printf("LCP protocol\t"); printf("\n"); lcp(packet+4, length-6); printf("\n\n\n"); break;
    default: printf("else protocol\t");break;
  }
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
  switch (packetCutted[1]) {
    case 0X01: printf("MRU\t"); break;
    case 0X02: printf("ACCM\t"); break;
    case 0X03: printf("Authentication\t"); break;
    case 0X05: printf("Magic Number\t"); break;
    case 0X07: printf("Protocol compress\t"); break;
    case 0X08: printf("Address/Control field compress\t"); break;
    case 0X0D: printf("Callback\t"); break;
    default: printf("Unknown\n"); break;
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
}
