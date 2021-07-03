#include <string>
#include <vector>
using namespace std;

typedef uint8_t byte;
typedef vector<byte> bytes;

#define FNV_PRIME 16777619
#define FNV_OFFSET 2166136261

constexpr uint32_t fnv1(uint32_t h, const char *s) {
  return (*s == 0) ? h
                   : fnv1((h * FNV_PRIME) ^ static_cast<uint32_t>(*s), s + 1);
}

constexpr uint16_t H(const char *s) { return fnv1(FNV_OFFSET, s); }

//=================================================================================================

class Reflector {
public:
  template <typename T>
  Reflector &property(const char *name, T &t, const char *desc = "") {
    return *this;
  };
  template <typename T> Reflector &operator&(T &t) { return *this; };
};

class Reflectable {
public:
  void reflect(Reflector &);
};

template <typename Reflector> void reflect(Reflector &r) {}
//=================================================================================================

class Serializer {
public:
  template <typename X> Serializer &operator<<(X &x) { return *this; };
  bool success();
};

class Deserializer {
public:
  template <typename X> Deserializer &operator>>(X &x) { return *this; };
};
//=================================================================================================

typedef enum { MT_LOG = 0, MT_CONFIG, MT_PUBLISH, MT_SUBSCRIBE } MsgType;

typedef enum {
  // DDS QOS
  QosNone = 0
} QoS;
//=================================================================================================

typedef enum {
  LOG_TRACE = 0,
  LOG_DEBUG,
  LOG_INFO,
  LOG_WARN,
  LOG_ERROR,
  LOG_FATAL,
  LOG_NONE
} LogLevel;

struct LogMsg {
  uint32_t type=H("LogMsg");
  uint64_t time;
  LogLevel level;
  string device;
  string component;
  string file;
  uint32_t line;
  string message;
  Reflector &reflect(Reflector &r) {
    return r & type & time & level & device & component & file & line & message;
  }
};

#include <cbor11.h>
//=================================================================================================
class ToCbor : public Reflector {
  cbor::array _cb;

public:
  template <typename T> Reflector &operator&(T &t) {
    _cb.push_back(t);
    return (*this);
  };
  bytes data() { return cbor::encode(_cb); }
};
//=================================================================================================

class FromCbor : public Reflector {
  cbor _cb;
  uint32_t idx;

public:
  template <typename T> Reflector &operator&(T &t) {
    t = _cb.m_array[idx++];
    return (*this);
  };
  bool data(bytes &bs) {
    _cb = cbor::decode(bs);
    return _cb.is_array();
  }
};
//=================================================================================================
/*
void test___() {
  ToCbor toCbor;
  LogMsg lm;
  lm.reflect(toCbor);
  bytes dump = toCbor.data();
  FromCbor fromCbor;
  fromCbor.data(dump);
  LogMsg lm2;
  lm2.reflect(fromCbor);
}
*/
//=================================================================================================
template <typename T> struct ConfigMsg {
  uint32_t type=H("ConfigMsg");
  string command;
  string property;
  T t;
};
//=================================================================================================

template <typename T> struct PublishMsg {
  uint32_t type=H("PublishMsg");
  string topic;
  T value;
  Reflector &reflect(Reflector &r) { return r & type & topic & value; };
  static void make() { PublishMsg<int> pm = {.topic="", 10}; }
};
//=================================================================================================

struct SubscribeMsg {
  uint32_t type=H("SubscribeMsg");
  string topic;
  Reflector &reflect(Reflector &r) { return r & type & topic; };
};
//=================================================================================================

struct SystemMsg {
  uint32_t type=H("SystemMsg");
  string deviceName;
  uint64_t upTime;
  uint64_t utc;
  uint64_t heap;
  string cpu;
  
  Reflector &reflect(Reflector &r) { return r & type & deviceName; };
};
//=================================================================================================
class Component : public Reflectable {};

class System : public Component {
  void reflect(Reflector &r) {
    r.property("uptime", "the time in milliseconds since boot of this device")
        .property("realtime", "the synchronized time in UTC millisec");
  }
};

class Device : public Reflectable {
  vector<Component> components;
  void reflect(Reflector &r) {
    r.property("name", "device name or host name ");
    for (auto comp : components) {
      comp.reflect(r);
    }
  }
};
