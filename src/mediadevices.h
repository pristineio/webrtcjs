#ifndef WEBRTCJS_MEDIADEVICES_H
#define WEBRTCJS_MEDIADEVICES_H

#include <nan.h>

#include "webrtc/base/logging.h"
#include "webrtc/base/scoped_ptr.h"

#include "webrtcjs.h"


class MediaDevices : public Nan::ObjectWrap {
 public:
  static NAN_MODULE_INIT(Init);

 private:
  static Nan::Persistent<v8::Function> constructor;
  explicit MediaDevices();
  ~MediaDevices();

  static NAN_METHOD(New);
  static NAN_METHOD(GetUserMedia);
  static NAN_METHOD(EnumerateDevices);
};

#endif