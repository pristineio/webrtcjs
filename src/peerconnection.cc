#include "peerconnection.h"

NAN_MODULE_INIT(PeerConnection::Init) {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("PeerConnection").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  constructor.Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(target, Nan::New("PeerConnection").ToLocalChecked(),
    Nan::GetFunction(tpl).ToLocalChecked());
}

Nan::Persistent<v8::Function> PeerConnection::constructor;
PeerConnection::PeerConnection() {
  factory_ = webrtc::CreatePeerConnectionFactory(rtc::Thread::Current(),
    rtc::Thread::Current(), nullptr, nullptr, nullptr);

  webrtc::PeerConnectionInterface::RTCConfiguration config;
  webrtc::FakeConstraints constraints;

  offer_observer_ = new rtc::RefCountedObject<OfferObserver>(this);
  answer_observer_ = new rtc::RefCountedObject<AnswerObserver>(this);
  peer_connection_observer_ =
    new rtc::RefCountedObject<PeerConnectionObserver>(this);

  peer_connection_ = factory_->CreatePeerConnection(config, &constraints,
    nullptr, nullptr, peer_connection_observer_);
}

PeerConnection::~PeerConnection() { }

NAN_METHOD(PeerConnection::New) {
  if(!info.IsConstructCall()) {
    return Nan::ThrowError("Use new operator");
  }
  PeerConnection *obj = new PeerConnection();
  obj->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

void PeerConnection::On(Event *event) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  PeerConnectionEvent type = event->As<PeerConnectionEvent>();
  switch(type) {
    case kPeerConnectionAddStream:
    case kPeerConnectionCreateAnswer:
    case kPeerConnectionCreateAnswerError:
    case kPeerConnectionCreateClosed:
    case kPeerConnectionCreateOffer:
    case kPeerConnectionCreateOfferError:
    case kPeerConnectionDataChannel:
    case kPeerConnectionIceCandidate:
    case kPeerConnectionIceChange:
    case kPeerConnectionIceGathering:
    case kPeerConnectionRemoveStream:
    case kPeerConnectionRenegotiation:
    case kPeerConnectionSetLocalDescription:
    case kPeerConnectionSetLocalDescriptionError:
    case kPeerConnectionSetRemoteDescription:
    case kPeerConnectionSetRemoteDescriptionError:
    case kPeerConnectionSignalChange:
    case kPeerConnectionStats:
      break;
  }
}