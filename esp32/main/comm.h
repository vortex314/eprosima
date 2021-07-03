#ifndef _COMM_H_
#define _COMM_H_
#include <Hardware.h>
#include <cbor11.h>
#include <ppp_frame.h>
#include <limero.h>

#include "session.h"
#include <vector>
typedef uint8_t byte;
typedef std::vector<byte> bytes;

class Comm : public Session {
  UART &_uart;
  bytes _frameData;
  bytes _cleanData;
  bytes _outputFrame;
  const uint32_t MAX_FRAME_SIZE = 1024;
  uint32_t _baudrate;
  TimerSource _heartbeat;
  TimerSource _heartbeatTimeout;

public:
  Comm(Thread &);
  bool init();
  static void onReceive(void *);
  void serialRxd();
  bool handleFrame(bytes &bs);
  int send(const bytes &);
  int baudrate(uint32_t);
};
#endif