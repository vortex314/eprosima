#ifndef _SESSION_H_
#define _SESSION_H_

#include <limero.h>
typedef uint8_t byte;
typedef std::vector<byte> bytes;

class Session : public Actor {
public:
  ValueSource<bool> connected;
  QueueFlow<bytes> incoming;
  Sink<bytes> outgoing;
  Session(Thread &thr, int inSize = 20, int outSize = 30)
      : Actor(thr), incoming(inSize, "incoming"),
        outgoing(outSize, "outgoing") {
    INFO("ctor");
  };
};
#endif