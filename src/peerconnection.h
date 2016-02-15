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


class PeerConnection : public Nan::ObjectWrap, public EventEmitter { /*public RTCWrap, public EventEmitter*/
 public:
  static NAN_MODULE_INIT(Init);

 private:
  // explicit PeerConnection(const v8::Local<v8::Object> &configuration,
  //                         const v8::Local<v8::Object> &constraints);
  explicit PeerConnection();
  ~PeerConnection();

  static NAN_METHOD(New);
  static Nan::Persistent<v8::Function> constructor;

  rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface>
    factory_;

  rtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection_;

  // static void CreateOffer(const Nan::FunctionCallbackInfo<v8::Value> &info);
  // static void CreateAnswer(const Nan::FunctionCallbackInfo<v8::Value> &info);
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

  // webrtc::PeerConnectionInterface *GetSocket();

 // protected:
 //  Nan::Persistent<v8::Function> _onsignalingstatechange;
 //  Nan::Persistent<v8::Function> _oniceconnectionstatechange;
 //  Nan::Persistent<v8::Function> _onicecandidate;
 //  Nan::Persistent<v8::Function> _ondatachannel;
 //  Nan::Persistent<v8::Function> _onnegotiationneeded;
 //  Nan::Persistent<v8::Function> _onaddstream;
 //  Nan::Persistent<v8::Function> _onremovestream;
 //  Nan::Persistent<v8::Function> _offerCallback;
 //  Nan::Persistent<v8::Function> _offerErrorCallback;
 //  Nan::Persistent<v8::Function> _answerCallback;
 //  Nan::Persistent<v8::Function> _answerErrorCallback;
 //  Nan::Persistent<v8::Function> _localCallback;
 //  Nan::Persistent<v8::Function> _localErrorCallback;
 //  Nan::Persistent<v8::Function> _remoteCallback;
 //  Nan::Persistent<v8::Function> _remoteErrorCallback;
 //  Nan::Persistent<v8::Function> _onstats;
 //  Nan::Persistent<v8::Object> _localsdp;
 //  Nan::Persistent<v8::Object> _remotesdp;

 //  static Nan::Persistent<v8::Function> constructor;

 //  rtc::scoped_refptr<StatsObserver> _stats;
  rtc::scoped_refptr<OfferObserver> offer_observer_;
  rtc::scoped_refptr<AnswerObserver> answer_observer_;
 //  rtc::scoped_refptr<LocalDescriptionObserver> _local;
 //  rtc::scoped_refptr<RemoteDescriptionObserver> _remote;
  rtc::scoped_refptr<PeerConnectionObserver> peer_connection_observer_;
 //  rtc::scoped_refptr<webrtc::PeerConnectionInterface> _socket;
 //  rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> _factory;

 //  rtc::scoped_refptr<MediaConstraints> _constraints;
 //  webrtc::PeerConnectionInterface::IceServers _servers;
};

#endif
