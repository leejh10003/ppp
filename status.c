#include "status.h"
static enum ppp_state lcpState;
static enum ppp_state ipcpState;
void lcpStateMachine(enum event arg) {
  switch (lcpState) {
    case initial:
      switch (arg) {
        case open_event: lcpState = starting; break;
        case up_event: lcpState = closed; ipcpStateMachine(up_event); break;
        default: break;
      }
    case starting:
      switch (arg) {
        case up_event: lcpState = reqSent; break;
        default: break;
      }
    case closed:
      switch (arg) {
        case open_event: lcpSend(flags); lcpState = reqSent; break;
        default: break;
      }
    case stopped:
      switch (arg) {
        case down_event: lcpState = starting; break;
        case close_event: lcpState = closed; break;
        default: break;
      }
    case closing:
      switch (arg) {
        case rta_event: lcpState = closed; break;
        default: break;
      }
    case stopping:
      switch (arg) {
        case down_event: lcpState = starting; break;
        case close_event: lcpState = closing; break;
        case rta_event: lcpState = stopped; break;
        default: break;
      }
    case reqSent:
      switch (arg) {
        case rca_event: lcpState = ackRcvd; break;
        case rcrPos_event: lcpState = ackSent; break;
        default: break;
      }
    case ackRcvd:
      switch (arg) {
        case rcrPos_event: lcpState = opened; break;
        default: break;
      }
    case ackSent:
      switch (arg) {
        case rca_event: lcpState = opened; break;
        default: break;
      }
    case opened:
      switch (arg) {
        case down_event: lcpState = starting; break;
        case close_event: lcpState = closing; break;
        default: break;
      }
    default: break;
  }
}
void ipcpStateMachine(enum event arg) {
  switch (lcpState) {
    case initial:
      switch (arg) {
        case open_event: lcpState = starting; break;
        case up_event: lcpState = closed; lcpStateMachine(open_event); break;
        default: break;
      }
    case starting:
      switch (arg) {
        case up_event: lcpState = reqSent;  break;
        default: break;
      }
    case closed:
      switch (arg) {
        case open_event: lcpState = reqSent; break;
        default: break;
      }
    case stopped:
      switch (arg) {
        case down_event: lcpState = starting; break;
        case close_event: lcpState = closed; break;
        default: break;
      }
    case closing:
      switch (arg) {
        case rta_event: lcpState = closed; break;
        default: break;
      }
    case stopping:
      switch (arg) {
        case down_event: lcpState = starting; break;
        case close_event: lcpState = closing; break;
        case rta_event: lcpState = stopped; break;
        default: break;
      }
    case reqSent:
      switch (arg) {
        case rca_event: lcpState = ackRcvd; break;
        case rcrPos_event: lcpState = ackSent; break;
        default: break;
      }
    case ackRcvd:
      switch (arg) {
        case rcrPos_event: lcpState = opened; break;
        default: break;
      }
    case ackSent:
      switch (arg) {
        case rca_event: lcpState = opened; break;
        default: break;
      }
    case opened:
      switch (arg) {
        case down_event: lcpState = starting; break;
        case close_event: lcpState = closing; break;
        default: break;
      }
    default: break;
  }
}
enum ppp_state getLcpState(){
  return lcpState;
}
enum ppp_state getIpcpState(){
  return ipcpState;
}
