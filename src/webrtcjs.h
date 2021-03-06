#ifndef WEBRTCJS_H
#define WEBRTCJS_H

#include <nan.h>

#include "webrtc/base/checks.h"
#include "webrtc/base/ssladapter.h"
#include "webrtc/media/devices/devicemanager.h"
#include "webrtc/api/peerconnectionfactory.h"

class WebRtcJs {
 public:
  static void Init();
  static webrtc::PeerConnectionFactoryInterface* GetPeerConnectionFactory();
};

#endif