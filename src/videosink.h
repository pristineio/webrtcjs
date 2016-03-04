#ifndef WEBRTCJS_VIDEOSINK_H
#define WEBRTCJS_VIDEOSINK_H

#include <nan.h>

#include "webrtc/base/scoped_ptr.h"
#include "webrtc/media/base/videoframe.h"
#include "webrtc/media/base/videosinkinterface.h"

#include "eventemitter.h"

class VideoSink : public Nan::ObjectWrap,
    public rtc::VideoSinkInterface<cricket::VideoFrame>,
    public EventEmitter {
  explicit VideoSink();
  ~VideoSink();
  static Nan::Persistent<v8::Function> constructor;
  static NAN_METHOD(New);

  Nan::Persistent<v8::Function> onframe_;
  static NAN_GETTER(GetOnFrame);
  static NAN_SETTER(SetOnFrame);

  int32_t number_of_rendered_frames_ = 0;

 public:
  static NAN_MODULE_INIT(Init);
  void OnFrame(const cricket::VideoFrame& frame) override;
  void On(Event* event) final;
};

#endif