#ifndef WEBRTCJS_MEDIASTREAMTRACK_H
#define WEBRTCJS_MEDIASTREAMTRACK_H

#include <nan.h>

#include "webrtc/media/base/videosourceinterface.h"

#include "observers.h"
#include "eventemitter.h"
#include "videosink.h"

class MediaStreamTrack : public Nan::ObjectWrap, public EventEmitter {
 friend class MediaStream;
 public:
  static NAN_MODULE_INIT(Init);

  static v8::Local<v8::Value>
    New(rtc::scoped_refptr<webrtc::MediaStreamTrackInterface>
      media_stream_track);

 private:
  explicit MediaStreamTrack();
  ~MediaStreamTrack();
  static Nan::Persistent<v8::Function> constructor;

  void On(Event* event) final;

  static NAN_METHOD(New);
  static NAN_METHOD(AddSink);
  static NAN_METHOD(RemoveSink);


  static NAN_GETTER(GetId);
  static NAN_GETTER(GetKind);
  static NAN_GETTER(GetReadyState);

  static NAN_GETTER(GetEnabled);
  static NAN_SETTER(SetEnabled);

  Nan::Persistent<v8::Function> onstarted_;
  static NAN_GETTER(GetOnStarted);
  static NAN_SETTER(SetOnStarted);

  Nan::Persistent<v8::Function> onmute_;
  static NAN_GETTER(GetOnMute);
  static NAN_SETTER(SetOnMute);

  Nan::Persistent<v8::Function> onunmute_;
  static NAN_GETTER(GetOnUnMute);
  static NAN_SETTER(SetOnUnMute);

  Nan::Persistent<v8::Function> onoverconstrained_;
  static NAN_GETTER(GetOnOverConstrained);
  static NAN_SETTER(SetOnOverConstrained);

  Nan::Persistent<v8::Function> onended_;
  static NAN_GETTER(GetOnEnded);
  static NAN_SETTER(SetOnEnded);

  rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track_;
  rtc::scoped_refptr<MediaStreamTrackObserver> observer_;
};

#endif