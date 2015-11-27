#ifndef __STATUSHEADER__
#ifndef __STATUSENUM__
enum event {
  up_event,
  down_event,
  open_event,
  close_event,
  rcrPos_event,
  rcrNeg_event,
  rca_event,
  rtr_event,
  rta_event,
  rxjPos_event,
  rxjNeg_event
};
enum ppp_state {
  initial,
  starting,
  closed,
  stopped,
  closing,
  stopping,
  reqSent,
  ackRcvd,
  ackSent,
  opened
};
#define __STATUSENUM__
#endif
#ifndef __STATUSFUNC__
void lcpStateMachine(enum event arg);
void ipcpStateMachine(enum event arg);
#define __STATUSFUNC__
#endif
#define __STATUSHEADER__
#endif
