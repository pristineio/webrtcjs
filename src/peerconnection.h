#ifndef WEBRTCJS_PEERCONNECTION_H
#define WEBRTCJS_PEERCONNECTION_H

#include <nan.h>

#include "webrtc/base/scoped_ptr.h"
#include "webrtc/api/videosourceinterface.h"
#include "webrtc/api/peerconnectionfactory.h"
#include "webrtc/media/engine/webrtcvideodecoderfactory.h"

#include "webrtcjs.h"
#include "eventemitter.h"
#include "observers.h"
#include "mediastream.h"
#include "mediaconstraints.h"

class PeerConnection : public Nan::ObjectWrap, public EventEmitter {
 public:
  static NAN_MODULE_INIT(Init);

  webrtc::PeerConnectionInterface* GetPeerConnection();
  MediaConstraints* GetConstraints();

  webrtc::PeerConnectionInterface::RTCConfiguration config_;

  Nan::Persistent<v8::Function> offer_cb_;
  Nan::Persistent<v8::Function> offer_err_cb_;

  Nan::Persistent<v8::Function> answer_cb_;
  Nan::Persistent<v8::Function> answer_err_cb_;

  Nan::Persistent<v8::Function> local_sdp_cb_;
  Nan::Persistent<v8::Function> local_sdp_err_cb_;

  Nan::Persistent<v8::Function> remote_sdp_cb_;
  Nan::Persistent<v8::Function> remote_sdp_err_cb_;

  Nan::Persistent<v8::Function> onstats_;
  Nan::Persistent<v8::Function> ondatachannel_;

 //  Nan::Persistent<v8::Function> _onstats;

  Nan::Persistent<v8::Object> local_sdp_;
  Nan::Persistent<v8::Object> remote_sdp_;

  rtc::scoped_refptr<StatsObserver> stats_observer_;
  rtc::scoped_refptr<OfferObserver> offer_observer_;
  rtc::scoped_refptr<AnswerObserver> answer_observer_;
  rtc::scoped_refptr<LocalDescriptionObserver> local_description_observer_;
  rtc::scoped_refptr<RemoteDescriptionObserver> remote_description_observer_;
  rtc::scoped_refptr<PeerConnectionObserver> peer_connection_observer_;

 private:
  explicit PeerConnection();
  ~PeerConnection();
  static Nan::Persistent<v8::Function> constructor;

  static NAN_METHOD(New);
  static NAN_METHOD(CreateOffer);
  static NAN_METHOD(CreateAnswer);
  static NAN_METHOD(SetLocalDescription);
  static NAN_METHOD(SetRemoteDescription);
  static NAN_METHOD(AddIceCandidate);
  static NAN_METHOD(GetStats);
  static NAN_METHOD(AddStream);
  static NAN_METHOD(RemoveStream);
  static NAN_METHOD(IsStable);
  static NAN_METHOD(Close);

  Nan::Persistent<v8::Function> onnegotiationneeded_;
  static NAN_GETTER(GetOnNegotiationNeeded);
  static NAN_SETTER(SetOnNegotiationNeeded);

  Nan::Persistent<v8::Function> onicecandidate_;
  static NAN_GETTER(GetOnIceCandidate);
  static NAN_SETTER(SetOnIceCandidate);

  Nan::Persistent<v8::Function> oniceconnectionstatechange_;
  static NAN_GETTER(GetOnIceConnectionStateChange);
  static NAN_SETTER(SetOnIceConnectionStateChange);

  Nan::Persistent<v8::Function> onsignalingstatechange_;
  static NAN_GETTER(GetOnSignalingStateChange);
  static NAN_SETTER(SetOnSignalingStateChange);

  Nan::Persistent<v8::Function> onaddstream_;
  static NAN_GETTER(GetOnAddStream);
  static NAN_SETTER(SetOnAddStream);

  Nan::Persistent<v8::Function> onremovestream_;
  static NAN_GETTER(GetOnRemoveStream);

  static NAN_GETTER(GetSignalingState);

  void On(Event* event) final;

  rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection_;
  rtc::scoped_refptr<MediaConstraints> constraints_;
  rtc::scoped_ptr<rtc::Thread> signaling_thread_;
  rtc::scoped_ptr<rtc::Thread> worker_thread_;


  // static void CreateDataChannel(const Nan::FunctionCallbackInfo<v8::Value> &info);
  // static void GetLocalStreams(const Nan::FunctionCallbackInfo<v8::Value> &info);
  // static void GetRemoteStreams(const Nan::FunctionCallbackInfo<v8::Value> &info);
  // static void GetStreamById(const Nan::FunctionCallbackInfo<v8::Value> &info);
  // static void GetStats(const Nan::FunctionCallbackInfo<v8::Value> &info);

  // static void GetIceConnectionState(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
  // static void GetIceGatheringState(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
  // static void GetOnDataChannel(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
  // static void GetOnRemoveStream(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
  // static void GetLocalDescription(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
  // static void GetRemoteDescription(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);

  // static void ReadOnly(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);
  // static void SetOnDataChannel(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);
  // static void SetOnRemoveStream(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);

  // rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> factory_;
  // rtc::scoped_ptr<cricket::WebRtcVideoDecoderFactory> decoder_factory_;
  // rtc::scoped_refptr<webrtc::PeerConnectionInterface> encoder_factory_;
};

#endif
