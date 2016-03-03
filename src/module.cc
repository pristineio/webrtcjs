#include <nan.h>

#include "webrtcjs.h"
#include "peerconnection.h"

#include "videosink.h"

NAN_MODULE_INIT(InitAll) {
  WebRtcJs::Init();
  PeerConnection::Init(target);
  MediaStream::Init(target);
  MediaStreamTrack::Init(target);

  VideoSink::Init(target);
}

NODE_MODULE(addon, InitAll)
