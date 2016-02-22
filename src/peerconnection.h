#ifndef WEBRTCJS_PEERCONNECTION_H
#define WEBRTCJS_PEERCONNECTION_H

// #include "Common.h"
// #include "Observers.h"
#include "eventemitter.h"
#include "observers.h"

// #include "MediaConstraints.h"
// #include "Wrap.h"

#include <nan.h>

#include "webrtc/base/scoped_ptr.h"
#include "talk/app/webrtc/peerconnectionfactory.h"
#include "talk/app/webrtc/test/fakeconstraints.h"

class PeerConnection : public Nan::ObjectWrap, public EventEmitter {
 public:
  static NAN_MODULE_INIT(Init);

  void GetUserMedia();
  webrtc::PeerConnectionInterface* GetPeerConnection();

  webrtc::FakeConstraints constraints_;
  webrtc::PeerConnectionInterface::RTCConfiguration config_;

  Nan::Persistent<v8::Function> offer_cb_;
  Nan::Persistent<v8::Function> offer_err_cb_;

  Nan::Persistent<v8::Function> answer_cb_;
  Nan::Persistent<v8::Function> answer_err_cb_;

  Nan::Persistent<v8::Function> local_sdp_cb_;
  Nan::Persistent<v8::Function> local_sdp_err_cb_;

  Nan::Persistent<v8::Function> remote_sdp_cb_;
  Nan::Persistent<v8::Function> remote_sdp_err_cb_;

  Nan::Persistent<v8::Function> onicecandidate_;
  Nan::Persistent<v8::Function> onnegotiationneeded_;


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
  rtc::scoped_ptr<rtc::Thread> signaling_thread_;
  rtc::scoped_ptr<rtc::Thread> worker_thread_;


  // explicit PeerConnection(const v8::Local<v8::Object> &configuration,
  //                         const v8::Local<v8::Object> &constraints);
  explicit PeerConnection();
  ~PeerConnection();
  static Nan::Persistent<v8::Function> constructor;

  static NAN_METHOD(New);
  static NAN_METHOD(CreateOffer);
  static NAN_METHOD(CreateAnswer);
  static NAN_METHOD(SetLocalDescription);
  static NAN_METHOD(SetRemoteDescription);
  static NAN_METHOD(AddIceCandidate);

  static NAN_GETTER(GetOnNegotiationNeeded);
  static NAN_SETTER(SetOnNegotiationNeeded);

  static NAN_GETTER(GetOnIceCandidate);
  static NAN_SETTER(SetOnIceCandidate);



  // static void SetLocalDescription(const Nan::FunctionCallbackInfo<v8::Value> &info);
  // static void SetRemoteDescription(const Nan::FunctionCallbackInfo<v8::Value> &info);
  // static void AddIceCandidate(const Nan::FunctionCallbackInfo<v8::Value> &info);
  // static void CreateDataChannel(const Nan::FunctionCallbackInfo<v8::Value> &info);
  // static void AddStream(const Nan::FunctionCallbackInfo<v8::Value> &info);
  // static void RemoveStream(const Nan::FunctionCallbackInfo<v8::Value> &info);
  // static void GetLocalStreams(const Nan::FunctionCallbackInfo<v8::Value> &info);
  // static void GetRemoteStreams(const Nan::FunctionCallbackInfo<v8::Value> &info);
  // static void GetStreamById(const Nan::FunctionCallbackInfo<v8::Value> &info);
  // static void GetStats(const Nan::FunctionCallbackInfo<v8::Value> &info);
  // static void Close(const Nan::FunctionCallbackInfo<v8::Value> &info);

  // static void GetSignalingState(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
  // static void GetIceConnectionState(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
  // static void GetIceGatheringState(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
  // static void GetOnSignalingStateChange(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
  // static void GetOnIceConnectionStateChange(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
  // static void GetOnIceCandidate(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
  // static void GetOnDataChannel(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
  // static void GetOnNegotiationNeeded(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
  // static void GetOnAddStream(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
  // static void GetOnRemoveStream(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
  // static void GetLocalDescription(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);
  // static void GetRemoteDescription(v8::Local<v8::String> property, const Nan::PropertyCallbackInfo<v8::Value> &info);

  // static void ReadOnly(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);
  // static void SetOnSignalingStateChange(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);
  // static void SetOnIceConnectionStateChange(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);
  // static void SetOnIceCandidate(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);
  // static void SetOnDataChannel(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);
  // static void SetOnNegotiationNeeded(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);
  // static void SetOnAddStream(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);
  // static void SetOnRemoveStream(v8::Local<v8::String> property, v8::Local<v8::Value> value, const Nan::PropertyCallbackInfo<void> &info);

  void On(Event *event) final;

  // bool IsStable();

 // protected:
 //  Nan::Persistent<v8::Function> _onsignalingstatechange;
 //  Nan::Persistent<v8::Function> _oniceconnectionstatechange;
 //  Nan::Persistent<v8::Function> _onicecandidate;
 //  Nan::Persistent<v8::Function> _ondatachannel;
 //  Nan::Persistent<v8::Function> _onnegotiationneeded;
 //  Nan::Persistent<v8::Function> _onaddstream;
 //  Nan::Persistent<v8::Function> _onremovestream;

 protected:
  rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> factory_;
  rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection_;

  // rtc::scoped_refptr<MediaConstraints> constraints_;
};

#endif
