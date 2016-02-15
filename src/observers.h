#ifndef WEBRTCJS_OFFEROBSERVER_H
#define WEBRTCJS_OFFEROBSERVER_H

#include "../eventemitter.h"
#include "talk/app/webrtc/peerconnectioninterface.h"
#include "webrtc/base/json.h"

class OfferObserver
  : public webrtc::CreateSessionDescriptionObserver,
    public EventEmitter {
 public:
  OfferObserver(EventEmitter* listener=nullptr);
  void On(Event *event) final;
  void OnSuccess(webrtc::SessionDescriptionInterface* sdp) final;
  void OnFailure(const std::string &error) final;
};

class AnswerObserver
  : public webrtc::CreateSessionDescriptionObserver,
    public EventEmitter {
 public:
  AnswerObserver(EventEmitter* listener=nullptr);
  void On(Event *event) final;
  void OnSuccess(webrtc::SessionDescriptionInterface* sdp) final;
  void OnFailure(const std::string &error) final;
};

class PeerConnectionObserver
  : public webrtc::PeerConnectionObserver,
    public rtc::RefCountInterface,
    public EventEmitter {
 public:
  PeerConnectionObserver(EventEmitter* listener=nullptr);
  void On(Event *event) final;

  void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState state) final;
  void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState state) final;
  void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState state) final;
  void OnStateChange(webrtc::PeerConnectionObserver::StateType state) final;
  void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) final;
  void OnDataChannel(webrtc::DataChannelInterface* channel) final;
  void OnRenegotiationNeeded() final;
  void OnAddStream(webrtc::MediaStreamInterface* stream) final;
  void OnRemoveStream(webrtc::MediaStreamInterface* stream) final;
};

#endif