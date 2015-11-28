#ifndef __JJCRCHEADER__
#define PPPINITFCS16 0xffff /* Initial FCS value */
#define PPPGOODFCS16 0xf0b8 /* Good final FCS value */
typedef unsigned short u16;
typedef unsigned long u32;
#ifndef __JJCRCFUNC__
int tryfcs16(char* cp, int len);
u16 pppfcs16(u16 fcs, char* cp, int len);
#define __JJCRCFUNC__
#endif
#define __JJCRCHEADER__
#endif
