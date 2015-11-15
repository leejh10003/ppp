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
unsigned char buf[255];

int main(void){
    int fileDescriptor; //File Descriptor variable
    int response;
    int iterator;
    struct termios oldtio, newtio;

    memset((void*)&buf, 0, (size_t)sizeof(buf)); //Initialize buffer with value 0
    fileDescriptor = open(SERIALDEVICE, O_RDWR | O_NOCTTY, O_NONBLOCK);
    //Open commucation Descriptor with O_NOCTTY option. This function can write and read data from it.
    //
    if(fileDescriptor<0){
        perror(SERIALDEVICE); //Print error prefix
        exit(-1); //Exit this program if error has happened
    }//If fileDescriptor has not opened, handle that error
    tcgetattr(fileDescriptor, &oldtio); //Get fileDescriptor's current terminal attribute and save it to oldtio

    memset((void*)&newtio, 0, sizeof(newtio));//Initialize newtio witah value 0
    newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD; //Give control mode of fileDescriptor's new terminal attribute
    newtio.c_iflag = IGNPAR | ICRNL; //Give input mode of fileDescriptor's new terminal attribute

    tcflush(fileDescriptor, TCIFLUSH); //Kill fileDescriptor's current data
    tcsetattr(fileDescriptor, TCSANOW, &newtio); //Give newtio attribute to fileDescriptor
    fflush(stdin); //Flush current standard input buffer
    fflush(stdout); //Flush current standard output buffer

    for(iterator=0; iterator<255; iterator++){
        buf[iterator] = 0x00;
    } //Fill all byte of buffer with value 0
    tcflush(fileDescriptor, TCIFLUSH); //Kill fileDescriptor's current data
    tcflush(fileDescriptor, TCIFLUSH); //Kill fileDescriptor's current data
    tcflush(fileDescriptor, TCIFLUSH); //Kill fileDescriptor's current data

    while(1)
    {
      fflush(stdin);
      fflush(stdout);
      response = read(fileDescriptor, buf, 255); //Read recieved data from fileDescriptor and save it to buffer
      for (iterator=0 ; iterator<response; iterator++){
          printf("%X\t", buf[iterator]);
      }
    } //Waiting for user's input
    tcsetattr(fileDescriptor,TCSANOW, &oldtio); //restore fileDescriptor's Previous terminal attribute
    return 0;
}
