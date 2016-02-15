#include "offerobserver.h"
#include "answerobserver.h"
#include "peerconnectionobserver.h"

//
// OfferObserver
//
OfferObserver::OfferObserver(EventEmitter *listener) :
  EventEmitter(listener) { }

void OfferObserver::On(Event* event) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
}

void OfferObserver::OnSuccess(webrtc::SessionDescriptionInterface* desc) {
  Json::StyledWriter writer;
  Json::Value msg;
  std::string sdp;
  if(desc->ToString(&sdp)) {
    msg["type"] = desc->type();
    msg["sdp"] = sdp;
    Emit(kPeerConnectionCreateOffer, writer.write(msg));
  }
}

void OfferObserver::OnFailure(const std::string &error) {
  Emit(kPeerConnectionCreateOfferError, error);
}

//
// AnswerObserver
//
AnswerObserver::AnswerObserver(EventEmitter *listener) :
  EventEmitter(listener) { }

void AnswerObserver::On(Event* event) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
}

void AnswerObserver::OnSuccess(webrtc::SessionDescriptionInterface* desc) {
  Json::StyledWriter writer;
  Json::Value msg;
  std::string sdp;
  if(desc->ToString(&sdp)) {
    msg["type"] = desc->type();
    msg["sdp"] = sdp;
    Emit(kPeerConnectionCreateOffer, writer.write(msg));
  }
}

void AnswerObserver::OnFailure(const std::string &error) {
  Emit(kPeerConnectionCreateOfferError, error);
}

//
// PeerConnectionObserver
//
PeerConnectionObserver::PeerConnectionObserver(EventEmitter *listener) :
  EventEmitter(listener) { }

void PeerConnectionObserver::On(Event* event) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
}

void PeerConnectionObserver::OnStateChange(
    webrtc::PeerConnectionObserver::StateType state) { }

void PeerConnectionObserver::OnSignalingChange(
    webrtc::PeerConnectionInterface::SignalingState state) {
  Emit(kPeerConnectionSignalChange);
  if(state == webrtc::PeerConnectionInterface::kClosed) {
    Emit(kPeerConnectionCreateClosed);
  }
}

void PeerConnectionObserver::OnIceConnectionChange(
    webrtc::PeerConnectionInterface::IceConnectionState state) {
  Emit(kPeerConnectionIceChange);
}

void PeerConnectionObserver::OnIceGatheringChange(
    webrtc::PeerConnectionInterface::IceGatheringState state) {
  Emit(kPeerConnectionIceGathering);
}

void PeerConnectionObserver::OnDataChannel(
    webrtc::DataChannelInterface *channel) {
  rtc::scoped_refptr<webrtc::DataChannelInterface> dataChannel = channel;
  if(dataChannel.get()) {
    Emit(kPeerConnectionDataChannel, dataChannel);
  }
}

void PeerConnectionObserver::OnAddStream(webrtc::MediaStreamInterface *stream) {
  rtc::scoped_refptr<webrtc::MediaStreamInterface> mediaStream = stream;
  if(mediaStream.get()) {
    Emit(kPeerConnectionAddStream, mediaStream);
  }
}

void PeerConnectionObserver::OnRemoveStream(
    webrtc::MediaStreamInterface *stream) {
  rtc::scoped_refptr<webrtc::MediaStreamInterface> mediaStream = stream;
  if(mediaStream.get()) {
    Emit(kPeerConnectionRemoveStream, mediaStream);
  }
}

void PeerConnectionObserver::OnRenegotiationNeeded() {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  Emit(kPeerConnectionRenegotiation);
}

void PeerConnectionObserver::OnIceCandidate(
    const webrtc::IceCandidateInterface* candidate) {
  Json::StyledWriter writer;
  Json::Value msg;
  std::string sdp;
  if(candidate->ToString(&sdp)) {
    msg["sdpMid"] = candidate->sdp_mid();
    msg["sdpMLineIndex"] = candidate->sdp_mline_index();
    msg["candidate"] = sdp;
    Emit(kPeerConnectionIceCandidate, writer.write(msg));
  }
}