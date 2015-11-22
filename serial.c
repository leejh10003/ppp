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

#define BAUDRATE B38400
#define SERIALDEVICE "/dev/ttyS0" //Use commucation device
unsigned char buffer[255];
void repeatOverIteration(int fileDescriptor);
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
  static int before = 0x00;
  fflush(stdin);
  fflush(stdout);
  responseLength = read(fileDescriptor, buffer, 255); //Read recieved data from fileDescriptor and save it to buffer
  for (iterator=0 ; iterator<responseLength; iterator++){
    if (before == 0x7E && buffer[iterator] != 0x7E)
      printf("\n%X\t", buffer[iterator]);
    else if (buffer[iterator] == 0x7D){
      iterator += 1;
      printf("%X\t", (unsigned char)((char)(buffer[iterator])-0X20));
    }
    else
      printf("%X\t", buffer[iterator]);
      before = buffer[iterator];
    }
}
