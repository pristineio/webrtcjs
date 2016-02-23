#include "observers.h"

//
// LocalDescriptionObserver
//
LocalDescriptionObserver::LocalDescriptionObserver(EventEmitter *listener) :
  EventEmitter(listener) { }

void LocalDescriptionObserver::On(Event* event) {
  LOG(LS_INFO) << __FUNCTION__;
}

void LocalDescriptionObserver::OnSuccess() {
  Emit(kPeerConnectionSetLocalDescription);
}

void LocalDescriptionObserver::OnFailure(const std::string &error) {
  Emit(kPeerConnectionSetLocalDescriptionError, error);
}

//
// RemoteDescriptionObserver
//
RemoteDescriptionObserver::RemoteDescriptionObserver(EventEmitter *listener) :
  EventEmitter(listener) { }

void RemoteDescriptionObserver::On(Event* event) {
  LOG(LS_INFO) << __FUNCTION__;
}

void RemoteDescriptionObserver::OnSuccess() {
  Emit(kPeerConnectionSetRemoteDescription);
}

void RemoteDescriptionObserver::OnFailure(const std::string &error) {
  Emit(kPeerConnectionSetRemoteDescriptionError, error);
}

//
// OfferObserver
//
OfferObserver::OfferObserver(EventEmitter* listener) :
  EventEmitter(listener) { }

void OfferObserver::On(Event* event) { }

void OfferObserver::OnSuccess(webrtc::SessionDescriptionInterface* desc) {
  Json::FastWriter writer;
  Json::Value msg;
  std::string sdp;
  if(desc->ToString(&sdp)) {
    msg["type"] = desc->type();
    msg["sdp"] = sdp;
    Emit(kPeerConnectionCreateOffer, writer.write(msg));
  }
}

void OfferObserver::OnFailure(const std::string& error) {
  Emit(kPeerConnectionCreateOfferError, error);
}

//
// AnswerObserver
//
AnswerObserver::AnswerObserver(EventEmitter* listener) :
  EventEmitter(listener) { }

void AnswerObserver::On(Event* event) { }

void AnswerObserver::OnSuccess(webrtc::SessionDescriptionInterface* desc) {
  Json::FastWriter writer;
  Json::Value msg;
  std::string sdp;
  if(desc->ToString(&sdp)) {
    msg["type"] = desc->type();
    msg["sdp"] = sdp;
    Emit(kPeerConnectionCreateAnswer, writer.write(msg));
  }
}

void AnswerObserver::OnFailure(const std::string& error) {
  Emit(kPeerConnectionCreateAnswerError, error);
}

//
// StatsObserver
//
StatsObserver::StatsObserver(EventEmitter* listener) :
  EventEmitter(listener) { }

void StatsObserver::On(Event* event) { }

void StatsObserver::OnComplete(const webrtc::StatsReports& reports) {
  Emit(kPeerConnectionStats, reports);
}

//
// PeerConnectionObserver
//
PeerConnectionObserver::PeerConnectionObserver(EventEmitter *listener) :
    EventEmitter(listener) {
  LOG(LS_INFO) << __FUNCTION__;
}

void PeerConnectionObserver::On(Event* event) {
  LOG(LS_INFO) << __FUNCTION__;
}

void PeerConnectionObserver::OnStateChange(
    webrtc::PeerConnectionObserver::StateType state) {
  LOG(LS_INFO) << __FUNCTION__;
}

void PeerConnectionObserver::OnSignalingChange(
    webrtc::PeerConnectionInterface::SignalingState state) {
  LOG(LS_INFO) << __FUNCTION__;
  Emit(kPeerConnectionSignalChange);
  if(state == webrtc::PeerConnectionInterface::kClosed) {
    Emit(kPeerConnectionCreateClosed);
  }
}

void PeerConnectionObserver::OnIceConnectionChange(
    webrtc::PeerConnectionInterface::IceConnectionState state) {
  LOG(LS_INFO) << __FUNCTION__;
  Emit(kPeerConnectionIceChange);
}

void PeerConnectionObserver::OnIceGatheringChange(
    webrtc::PeerConnectionInterface::IceGatheringState state) {
  LOG(LS_INFO) << __FUNCTION__;
  Emit(kPeerConnectionIceGathering);
}

void PeerConnectionObserver::OnDataChannel(
    webrtc::DataChannelInterface *channel) {
  LOG(LS_INFO) << __FUNCTION__;
  rtc::scoped_refptr<webrtc::DataChannelInterface> dataChannel = channel;
  if(dataChannel.get()) {
    Emit(kPeerConnectionDataChannel, dataChannel);
  }
}

void PeerConnectionObserver::OnAddStream(webrtc::MediaStreamInterface *stream) {
  LOG(LS_INFO) << __FUNCTION__;
  rtc::scoped_refptr<webrtc::MediaStreamInterface> mediaStream = stream;
  if(mediaStream.get()) {
    Emit(kPeerConnectionAddStream, mediaStream);
  }
}

void PeerConnectionObserver::OnRemoveStream(
    webrtc::MediaStreamInterface *stream) {
  LOG(LS_INFO) << __FUNCTION__;
  rtc::scoped_refptr<webrtc::MediaStreamInterface> mediaStream = stream;
  if(mediaStream.get()) {
    Emit(kPeerConnectionRemoveStream, mediaStream);
  }
}

void PeerConnectionObserver::OnRenegotiationNeeded() {
  LOG(LS_INFO) << __FUNCTION__;
  Emit(kPeerConnectionRenegotiation);
}

void PeerConnectionObserver::OnIceCandidate(
    const webrtc::IceCandidateInterface* candidate) {
  LOG(LS_INFO) << __FUNCTION__;
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