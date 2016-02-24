#ifndef WEBRTCJS_H
#define WEBRTCJS_H

#include <nan.h>

#include "webrtc/base/checks.h"
#include "webrtc/base/ssladapter.h"
#include "webrtc/media/devices/devicemanager.h"
#include "talk/app/webrtc/peerconnectionfactory.h"

#include "recording_decoder.h"

class WebRtcJs {
 public:
  static void Init();
  static cricket::DeviceManagerInterface* GetDeviceManager();
};

#endif