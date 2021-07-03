#include <Hardware.h>
#include <limero.h>
#include <stdio.h>

#include "LedBlinker.h"
#include "esp_spi_flash.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include <cbor11.h>
#include <comm.h>
#include <msg.h>
#include <ppp_frame.h>

#include <uxr/client/client.h>
#include <uxr/client/profile/transport/serial/serial_transport.h>
#include <uxr/client/util/ping.h>

Log logger(1024);

// ---------------------------------------------- THREAD
Thread thisThread("main");
Thread ledThread("led");
Thread commThread("comm");
ThreadProperties props = {.name = "worker",
                          .stackSize = 5000,
                          .queueSize = 20,
                          .priority = 24 | portPRIVILEGE_BIT};
Thread workerThread(props);
#define PIN_LED 2

LedBlinker led(ledThread, PIN_LED, 100);

void onReceive(void *ptr) {}

// ---------------------------------------------- system properties
LambdaSource<std::string> systemBuild([]() { return __DATE__ " " __TIME__; });
LambdaSource<std::string> systemHostname([]() { return Sys::hostname(); });
LambdaSource<uint32_t> systemHeap([]() { return Sys::getFreeHeap(); });
LambdaSource<uint64_t> systemUptime([]() { return Sys::millis(); });
ValueSource<bool> systemAlive = true;
TimerSource ticker(workerThread, 3000, true, "ticker");

string myHostPrefix = "src/ESP32/";

template <typename T> class ToSpine : public LambdaFlow<T, bytes> {
  string _name;
  Comm *_comm;

public:
  ToSpine(string name, Comm *comm)
      : LambdaFlow<T, bytes>([&](bytes &b, const T &t) {
          if (!comm->connected())
            return false;
          cbor msg = cbor::array{"P", _name, t};
          bytes bs = cbor::encode(msg);
          _comm->outgoing.on(bs);
          return true;
        }),
        _name(name), _comm(comm) {
    string topic = name;
    if (topic.find("src/") == 0 || topic.find("dst/") == 0) {
      DEBUG(" absolute topic %s ", name.c_str());
      _name = name;
    } else {
      DEBUG(" relative topic adding %s to %s ", myHostPrefix.c_str(),
            name.c_str());
      _name = myHostPrefix + name;
    }
  }
};

Poller poller(commThread);

//==========================================================================

void logWriter(char *s, uint32_t length) {
  cbor::binary data;
  cbor msg = cbor::array{"L", s};
  data = cbor::encode(msg);
  //comm.outgoing.on(data);
}
//==========================================================================

extern "C" void app_main(void) {
#ifdef HOSTNAME
  Sys::hostname(S(HOSTNAME));
#endif
  INFO(__DATE__ __TIME__);

  led.init();
  led.blinkSlow.on(true);

  poller.connected = true;
  uxrSerialTransport transport;
  const char *dev = "tty";
  int fd = 1;

  if (!uxr_init_serial_transport(&transport, fd, 0, 1)) {
    WARN("Error during transport creation");
  }

  ticker >> [&](const TimerMsg &) {
    if (uxr_ping_agent_attempts(&transport.comm, 1000, 10)) {
      INFO("Success! Agent is up on device '%s'", dev);
    } else {
      INFO("Sorry, no agent available at device '%s'", dev);
    }

 //   uxr_close_serial_transport(&transport);
  };

  INFO("");
  ledThread.start();
  commThread.start();
  workerThread.start();
  thisThread.run(); // DON'T EXIT , local variable will be destroyed
}
