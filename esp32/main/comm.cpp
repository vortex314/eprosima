#include <Hardware.h>
#include <cbor11.h>
#include <msg.h>
#include <ppp_frame.h>

#include <comm.h>


Comm::Comm(Thread &thr)
    : Session(thr, 10, 10), _uart(UART::create(0, 1, 3)),
      _heartbeat(thr, 1000, true, "heartbeat"),
      _heartbeatTimeout(thr, 2000, true, "heatbeatTimeout") {
  outgoing.async(thread(), [&](const bytes &m) { send(m); });
  _heartbeat >> [&](const TimerMsg &) {
    PublishMsg<uint64_t> pm;
    ToCbor toCbor;
    pm.reflect(toCbor);
    send(toCbor.data());
  };
  _heartbeatTimeout >> [&](const TimerMsg &) {

  };

  connected = false;
  INFO("ctor");
  _baudrate = 115200;
}

bool Comm::init() {
  _uart.setClock(_baudrate);
  _uart.onRxd(onReceive, this);
  _uart.mode("8N1");
  _uart.init();
  connected = true;
  return true;
}

bool Comm::handleFrame(bytes &bs) {
  if (bs.size() == 0)
    return true;
  _cleanData.clear();
  bool isGoodFrame = ppp_deframe(_cleanData, bs);
  if (isGoodFrame) {
    incoming.on(_cleanData);
  } else {
    if (bs.size()) {
      bs.push_back('\0');
      INFO("ESP32 garbage input %s", bs.data());
    }
  }
  return isGoodFrame;
}

void Comm::serialRxd() {
  while (_uart.hasData()) {
    byte b = _uart.read();
    if (b == PPP_FLAG_CHAR) {
      if (_frameData.size()) {
        handleFrame(_frameData);
        _frameData.clear();
      }
    } else {
      _frameData.push_back(b);
      if (_frameData.size() > MAX_FRAME_SIZE) // avoid Out of Memory
        _frameData.clear();
    }
  }
}

void Comm::onReceive(void *pv) {
  Comm *me = (Comm *)pv;
  me->serialRxd();
}

int Comm::send(const bytes &out) {
  ppp_frame(_outputFrame, out);
  return _uart.write(_outputFrame.data(), _outputFrame.size());
}

int Comm::baudrate(uint32_t br) {
  _baudrate = br;
  return 0;
}