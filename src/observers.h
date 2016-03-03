#ifndef WEBRTCJS_OBSERVERS_H
#define WEBRTCJS_OBSERVERS_H

#include "webrtc/api/peerconnectioninterface.h"
#include "webrtc/base/json.h"

#include "eventemitter.h"


class LocalDescriptionObserver
  : public webrtc::SetSessionDescriptionObserver,
    public EventEmitter {
 public:
  LocalDescriptionObserver(EventEmitter* listener=nullptr);
  void On(Event* event) final;
  void OnSuccess() final;
  void OnFailure(const std::string &error) final;
};

class RemoteDescriptionObserver
  : public webrtc::SetSessionDescriptionObserver,
    public EventEmitter {
 public:
  RemoteDescriptionObserver(EventEmitter* listener=nullptr);
  void On(Event* event) final;
  void OnSuccess() final;
  void OnFailure(const std::string &error) final;
};

class OfferObserver
  : public webrtc::CreateSessionDescriptionObserver,
    public EventEmitter {
 public:
  OfferObserver(EventEmitter* listener=nullptr);
  virtual void On(Event* event) final;
  virtual void OnSuccess(webrtc::SessionDescriptionInterface* sdp);
  virtual void OnFailure(const std::string &error);
};

class AnswerObserver
  : public webrtc::CreateSessionDescriptionObserver,
    public EventEmitter {
 public:
  AnswerObserver(EventEmitter* listener=nullptr);
  void On(Event* event) final;
  void OnSuccess(webrtc::SessionDescriptionInterface* sdp) final;
  void OnFailure(const std::string &error) final;
};

class StatsObserver : public webrtc::StatsObserver, public EventEmitter {
 public:
  StatsObserver(EventEmitter* listener=nullptr);
  void On(Event* event) final;
  void OnComplete(const webrtc::StatsReports& reports) final;
};

class PeerConnectionObserver
  : public webrtc::PeerConnectionObserver,
    public rtc::RefCountInterface,
    public EventEmitter {
 public:
  PeerConnectionObserver(EventEmitter* listener=nullptr);
  void On(Event* event) final;
  void OnSignalingChange(
    webrtc::PeerConnectionInterface::SignalingState state) final;
  void OnIceConnectionChange(
    webrtc::PeerConnectionInterface::IceConnectionState state) final;
  void OnIceGatheringChange(
    webrtc::PeerConnectionInterface::IceGatheringState state) final;
  void OnStateChange(webrtc::PeerConnectionObserver::StateType state);
  void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) final;
  void OnDataChannel(webrtc::DataChannelInterface* channel) final;
  void OnRenegotiationNeeded() final;
  void OnAddStream(webrtc::MediaStreamInterface* stream) final;
  void OnRemoveStream(webrtc::MediaStreamInterface* stream) final;
};

class MediaStreamTrackObserver
  : public webrtc::ObserverInterface,
    public rtc::RefCountInterface,
    public EventEmitter {
 public:
  MediaStreamTrackObserver(EventEmitter* listener=nullptr);
  void On(Event* event) final;
  void OnChanged() final;
};

class MediaStreamObserver
  : public webrtc::ObserverInterface,
    public rtc::RefCountInterface,
    public EventEmitter {
 public:
  MediaStreamObserver(EventEmitter* listener=nullptr);
  void On(Event* event) final;
  void OnChanged() final;
};

#endif