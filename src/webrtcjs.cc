#include <nan.h>
#include "peerconnection.h"

#include "webrtc/base/checks.h"
#include "webrtc/base/ssladapter.h"

using v8::FunctionTemplate;
using v8::Handle;
using v8::Object;
using v8::String;
using Nan::GetFunction;
using Nan::New;
using Nan::Set;

NAN_MODULE_INIT(InitAll) {
  RTC_CHECK(rtc::InitializeSSL()) << "Failed to InitializeSSL()";
  PeerConnection::Init(target);
}

NODE_MODULE(addon, InitAll)
