#ifndef PUBSUB_ABSTRACT_H
#define PUBSUB_ABSTRACT_H
#include <limero.h>

#include <set>
using namespace std;

class Spine;
template <class T> class ToSpine;
template <class T> class FromSpine;
template <class T> class SpineFlow;

struct SpineMessage {
  string topic;
  string message;
};

//____________________________________________________________________________________________________________
//
class Spine : public Actor {
public:
  string dstPrefix;
  string srcPrefix;
  set<string> subscriptions;
  QueueFlow<SpineMessage> incoming;
  Sink<SpineMessage> outgoing;
  ValueSource<bool> connected;
  //  TimerSource keepAliveTimer;
  Spine(Thread &thr)
      : Actor(thr), incoming(20, "incoming"), outgoing(30, "outgoing") {
    dstPrefix = "dst/";
    dstPrefix += Sys::hostname();
    dstPrefix += "/";
    srcPrefix = "src/";
    srcPrefix += Sys::hostname();
    srcPrefix += "/";
  };
  ~Spine(){};
  void init();
  template <class T> Subscriber<T> &toTopic(const char *name) {
    auto flow = new ToSpine<T>(name, this);
    *flow >> outgoing;
    return *flow;
  }
  template <class T> Source<T> &fromTopic(const char *name) {
    auto newSource = new FromSpine<T>(name, this);
    incoming >> *newSource;
    return *newSource;
  }
  template <class T> Flow<T, T> &topic(const char *name) {
    auto flow = new SpineFlow<T>(name, this);
    incoming >> flow->fromSpine;
    flow->toSpine >> outgoing;
    return *flow;
  }
};

//____________________________________________________________________________________________________________
//
template <class T> class ToSpine : public LambdaFlow<T, SpineMessage> {
  string _name;
  Spine *_spine;

public:
  ToSpine(string name, Spine *spine)
      : LambdaFlow<T, SpineMessage>([&](SpineMessage &msg, const T &event) {
          if (!_spine->connected())
            return false;
          DynamicJsonDocument doc(100);
          JsonVariant variant = doc.to<JsonVariant>();
          variant.set(event);
          serializeJson(doc, msg.message);
          msg.topic = _name;
          //          INFO("%s", msg.topic.c_str());
          return true;
        }),
        _name(name) {
    _spine = spine;
    string topic = name;
    if (topic.find("src/") == 0 || topic.find("dst/") == 0) {
      DEBUG(" absolute topic %s ", name.c_str());
      _name = name;
    } else {
      DEBUG(" relative topic adding %s to %s ", spine->srcPrefix.c_str(),
            name.c_str());
      _name = spine->srcPrefix + name;
    }
  }
  void request(){};
};
//_______________________________________________________________________________________________________________
//
template <class T> class FromSpine : public LambdaFlow<SpineMessage, T> {
  string _name;

public:
  FromSpine(string name, Spine *spine)
      : LambdaFlow<SpineMessage, T>([&](T &t,
                                        const SpineMessage &spineMessage) {
          /*   INFO(" from topic '%s':'%s' vs '%s'", spineMessage.topic.c_str(),
                 spineMessage.message.c_str(), _name.c_str());*/
          if (spineMessage.topic != _name) {
            return false;
          }
          DynamicJsonDocument doc(100);
          auto error = deserializeJson(doc, spineMessage.message);
          if (error) {
            WARN(" failed JSON parsing '%s' : '%s' ",
                 spineMessage.message.c_str(), error.c_str());
            return false;
          }
          JsonVariant variant = doc.as<JsonVariant>();
          if (variant.isNull()) {
            WARN(" is not a JSON variant '%s' ", spineMessage.message.c_str());
            return false;
          }
          if (variant.is<T>() == false) {
            WARN(" message '%s' JSON type doesn't match.",
                 spineMessage.message.c_str());
            return false;
          }
          t = variant.as<T>();
          return true;
          // emit doesn't work as such
          // https://stackoverflow.com/questions/9941987/there-are-no-arguments-that-depend-on-a-template-parameter
        }) {

    string topic = name;
    string targetTopic;
    if (topic.find("src/") == 0 || topic.find("dst/") == 0) {
      DEBUG(" absolute topic %s ", name.c_str());
      _name = name;
      spine->subscriptions.emplace(
          _name); // add explicit subscription beside the implicit src/device/#
    } else {
      DEBUG(" relative topic, adding prefix %s to %s ",
            spine->dstPrefix.c_str(), name.c_str());
      _name = spine->dstPrefix + name;
    }
  };
  void request(){};
};

//____________________________________________________________________________________________________________
//
template <class T> class SpineFlow : public Flow<T, T> {
public:
  ToSpine<T> toSpine;
  FromSpine<T> fromSpine;
  SpineFlow(const char *topic, Spine *spine)
      : toSpine(topic, spine),
        fromSpine(topic, spine){

            //       INFO(" created SpineFlow : %s ",topic);
        };
  void request() { fromSpine.request(); };
  void on(const T &t) { toSpine.on(t); }
  void subscribe(Subscriber<T> *tl) { fromSpine.subscribe(tl); };
};
#endif
