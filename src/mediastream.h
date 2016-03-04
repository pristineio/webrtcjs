#ifndef WEBRTCJS_MEDIASTREAM_H
#define WEBRTCJS_MEDIASTREAM_H

#include <nan.h>

#include "webrtc/base/scoped_ptr.h"

#include "webrtcjs.h"
#include "eventemitter.h"
#include "observers.h"
#include "mediastreamtrack.h"

class MediaStream : public Nan::ObjectWrap, public EventEmitter {
  explicit MediaStream();
  ~MediaStream();
  static Nan::Persistent<v8::Function> constructor;

  void On(Event* event) final;

  static NAN_METHOD(New);
  static NAN_METHOD(AddTrack);
  static NAN_METHOD(RemoveTrack);
  static NAN_METHOD(Clone);
  static NAN_METHOD(GetTrackById);
  static NAN_METHOD(GetAudioTracks);
  static NAN_METHOD(GetVideoTracks);

  Nan::Persistent<v8::Function> onaddtrack_;
  static NAN_GETTER(GetOnAddTrack);
  static NAN_SETTER(SetOnAddTrack);

  Nan::Persistent<v8::Function> onremovetrack_;
  static NAN_GETTER(GetOnRemoveTrack);
  static NAN_SETTER(SetOnRemoveTrack);

  static NAN_GETTER(GetActive);
  static NAN_GETTER(GetId);

  rtc::scoped_refptr<MediaStreamObserver> observer_;
  rtc::scoped_refptr<webrtc::MediaStreamInterface> stream_;

  webrtc::AudioTrackVector audio_tracks_;
  webrtc::VideoTrackVector video_tracks_;

  bool active_;

 public:
  static NAN_MODULE_INIT(Init);

  static v8::Local<v8::Value>
    New(rtc::scoped_refptr<webrtc::MediaStreamInterface> media_stream);

  static rtc::scoped_refptr<webrtc::MediaStreamInterface>
    Unwrap(v8::Local<v8::Object> value);

  static rtc::scoped_refptr<webrtc::MediaStreamInterface>
    Unwrap(v8::Local<v8::Value> value);
};

#endif