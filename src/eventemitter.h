#ifndef WEBRTCJS_EVENTEMITTER_H
#define WEBRTCJS_EVENTEMITTER_H

#include <vector>
#include <queue>
#include <uv.h>

#include "webrtc/base/refcount.h"
#include "webrtc/base/scoped_ref_ptr.h"

#include "talk/app/webrtc/peerconnectioninterface.h"

using std::vector;
using std::queue;

enum PeerConnectionEvent {
  kPeerConnectionCreateClosed = 1,
  kPeerConnectionCreateOffer,
  kPeerConnectionCreateOfferError,
  kPeerConnectionCreateAnswer,
  kPeerConnectionCreateAnswerError,
  kPeerConnectionSetLocalDescription,
  kPeerConnectionSetLocalDescriptionError,
  kPeerConnectionSetRemoteDescription,
  kPeerConnectionSetRemoteDescriptionError,
  kPeerConnectionIceCandidate,
  kPeerConnectionSignalChange,
  kPeerConnectionIceChange,
  kPeerConnectionIceGathering,
  kPeerConnectionDataChannel,
  kPeerConnectionAddStream,
  kPeerConnectionRemoveStream,
  kPeerConnectionRenegotiation,
  kPeerConnectionStats
};

template<class T> class EventWrapper;

class Event : public rtc::RefCountInterface {
  template<class T> friend class EventWrapper;
  friend class rtc::RefCountedObject<Event>;
  friend class EventEmitter;

 public:
  inline bool HasWrap() const {
    return wrap_;
  }

  template <class T> inline T As() const {
    return static_cast<T>(event_);
  }

  template<class T> const T &Unwrap() const {
    static T nowrap;
    if(wrap_) {
      const EventWrapper<T> *ptr = static_cast<const EventWrapper<T>*>(this);
      return ptr->content_;
    }
    nowrap = T();
    return nowrap;
  }

 private:
  explicit Event(int event = 0) : event_(event), wrap_(false) { }
  virtual ~Event() { }

 protected:
  int event_;
  bool wrap_;
};

template<class T> class EventWrapper : public Event {
  friend class rtc::RefCountedObject<EventWrapper<T> >;
  friend class Event;
  friend class EventEmitter;

 private:
  explicit EventWrapper(int event, const T &content) : Event(event),
                                                       content_(content) {
    wrap_ = true;
  }

  virtual ~EventWrapper() { }

 protected:
  T content_;
};

class EventEmitter {
 public:
  explicit EventEmitter(bool notify=false);
  explicit EventEmitter(EventEmitter* listener);
  virtual ~EventEmitter();
  virtual void On(Event *event) = 0;
  void AddListener(EventEmitter* listener=nullptr);
  void RemoveListener(EventEmitter* listener=nullptr);
  void RemoveAllListeners();
  void SetReference(bool alive=true);
  void Emit(int event=0);
  void Emit(rtc::scoped_refptr<Event> event);
  template <class T> inline void Emit(int event, const T &content) {
    EventEmitter::Emit(new rtc::RefCountedObject<EventWrapper<T>>(event,
      content));
  }

 private:
  static void onAsync(uv_async_t *handle, int status);
  static void onEnded(uv_handle_t *handle);
  void Dispose();
  void DispatchEvents();
  void AddParent(EventEmitter* listener=nullptr);
  void RemoveParent(EventEmitter* listener=nullptr);

 protected:
  bool notify_;
  uv_mutex_t lock_;
  uv_mutex_t list_;
  uv_async_t* async_;
  std::queue<rtc::scoped_refptr<Event>> events_;
  std::vector<EventEmitter*> listeners_;
  std::vector<EventEmitter*> parents_;
};

#endif