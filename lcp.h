#ifndef __JJLCPHEADER__
#ifndef __JJLCPENUM__
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
#define __JJLCPENUM__
#endif
#ifndef __JJLCPFUNC__
void lcpRcvd(unsigned char packetCutted[], int length, unsigned char crc[]);
void lcpSend(enum lcpPacketForm packetForm);
#define __JJLCPFUNC__
#endif
#define __JJLCPHEADER__
#endif
