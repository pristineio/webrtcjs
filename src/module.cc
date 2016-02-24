#include <nan.h>

#include "webrtcjs.h"
#include "mediadevices.h"
#include "peerconnection.h"

NAN_MODULE_INIT(InitAll) {
  WebRtcJs::Init();
  MediaDevices::Init(target);
  PeerConnection::Init(target);
}

NODE_MODULE(addon, InitAll)
