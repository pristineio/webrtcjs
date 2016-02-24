#include "peerconnection.h"

Nan::Persistent<v8::Function> PeerConnection::constructor;

NAN_MODULE_INIT(PeerConnection::Init) {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("PeerConnection").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(tpl, "createOffer", PeerConnection::CreateOffer);
  Nan::SetPrototypeMethod(tpl, "createAnswer", PeerConnection::CreateAnswer);
  Nan::SetPrototypeMethod(tpl, "getStats", PeerConnection::GetStats);
  Nan::SetPrototypeMethod(tpl, "setLocalDescription",
    PeerConnection::SetLocalDescription);
  Nan::SetPrototypeMethod(tpl, "setRemoteDescription",
    PeerConnection::SetRemoteDescription);
  Nan::SetPrototypeMethod(tpl, "addIceCandidate",
    PeerConnection::AddIceCandidate);

  Nan::SetAccessor(tpl->InstanceTemplate(),
    Nan::New("onnegotiationneeded").ToLocalChecked(),
    PeerConnection::GetOnNegotiationNeeded,
    PeerConnection::SetOnNegotiationNeeded);

  Nan::SetAccessor(tpl->InstanceTemplate(),
    Nan::New("onicecandidate").ToLocalChecked(),
    PeerConnection::GetOnIceCandidate,
    PeerConnection::SetOnIceCandidate);

  constructor.Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(target, Nan::New("PeerConnection").ToLocalChecked(),
    Nan::GetFunction(tpl).ToLocalChecked());
}

NAN_METHOD(PeerConnection::New) {
  if(!info.IsConstructCall()) {
    return Nan::ThrowError("Use new operator");
  }
  PeerConnection* self = new PeerConnection();
  self->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

NAN_METHOD(PeerConnection::CreateOffer) {
  LOG(LS_INFO) << __FUNCTION__;

  PeerConnection* self = Nan::ObjectWrap::Unwrap<PeerConnection>(info.Holder());
  self->offer_cb_.Reset();
  self->offer_err_cb_.Reset();

  webrtc::PeerConnectionInterface* peer_connection = self->GetPeerConnection();
  if(!peer_connection) {
    Nan::ThrowError("Internal Error");
  }

  if(!info[0].IsEmpty() && info[0]->IsFunction()) {
    self->offer_cb_.Reset<v8::Function>(v8::Local<v8::Function>::Cast(info[0]));
  }

  if(!info[1].IsEmpty() && info[1]->IsFunction()) {
    self->offer_err_cb_.Reset<v8::Function>(
      v8::Local<v8::Function>::Cast(info[1]));
  }

  self->GetUserMedia();

  info.GetReturnValue().SetUndefined();
}

NAN_METHOD(PeerConnection::CreateAnswer) {
  LOG(LS_INFO) << __FUNCTION__;

  PeerConnection* self = Nan::ObjectWrap::Unwrap<PeerConnection>(info.Holder());
  self->answer_cb_.Reset();
  self->answer_err_cb_.Reset();

  webrtc::PeerConnectionInterface* peer_connection = self->GetPeerConnection();
  if(!peer_connection) {
    Nan::ThrowError("Internal Error");
  }

  if(!info[0].IsEmpty() && info[0]->IsFunction()) {
    self->answer_cb_.Reset<v8::Function>(
      v8::Local<v8::Function>::Cast(info[0]));
  }
  if(!info[1].IsEmpty() && info[1]->IsFunction()) {
    self->answer_err_cb_.Reset<v8::Function>(
      v8::Local<v8::Function>::Cast(info[1]));
  }

  peer_connection->CreateAnswer(self->answer_observer_.get(), nullptr);

  info.GetReturnValue().SetUndefined();
}

NAN_METHOD(PeerConnection::SetLocalDescription) {
  LOG(LS_INFO) << __FUNCTION__;

  PeerConnection* self = Nan::ObjectWrap::Unwrap<PeerConnection>(info.Holder());
  webrtc::PeerConnectionInterface* peer_connection = self->GetPeerConnection();
  self->local_sdp_cb_.Reset();
  self->local_sdp_err_cb_.Reset();

  if(!peer_connection) {
    Nan::ThrowError("Internal error");
  }

  if(info[0].IsEmpty() || !info[0]->IsObject()) {
    Nan::ThrowError("Invalid SessionDescription");
  }

  v8::Local<v8::Object> desc_obj = v8::Local<v8::Object>::Cast(info[0]);
  v8::Local<v8::Value> type_value = desc_obj->Get(Nan::New("type")
    .ToLocalChecked());
  v8::Local<v8::Value> sdp_value = desc_obj->Get(Nan::New("sdp")
    .ToLocalChecked());

  if(type_value.IsEmpty() || !type_value->IsString()) {
    Nan::ThrowError("Invalid SessionDescription type");
  }

  if(sdp_value.IsEmpty() || !sdp_value->IsString()) {
    Nan::ThrowError("Invalid SessionDescription");
  }

  if(!info[1].IsEmpty() && info[1]->IsFunction()) {
    self->local_sdp_cb_.Reset<v8::Function>(
      v8::Local<v8::Function>::Cast(info[1]));
  }

  if(!info[2].IsEmpty() && info[2]->IsFunction()) {
    self->local_sdp_err_cb_.Reset<v8::Function>(
      v8::Local<v8::Function>::Cast(info[2]));
  }

  v8::String::Utf8Value type(type_value->ToString());
  v8::String::Utf8Value sdp(sdp_value->ToString());
  webrtc::SessionDescriptionInterface* desc(
    webrtc::CreateSessionDescription(*type, *sdp, 0));

  if(!desc) {
    Nan::ThrowError("webrtc::CreateSessionDescription failure");
  }

  self->local_sdp_.Reset<v8::Object>(desc_obj);
  peer_connection->SetLocalDescription(self->local_description_observer_.get(),
    desc);

  info.GetReturnValue().SetUndefined();
}

NAN_METHOD(PeerConnection::SetRemoteDescription) {
  LOG(LS_INFO) << __FUNCTION__;

  PeerConnection* self = Nan::ObjectWrap::Unwrap<PeerConnection>(info.Holder());
  webrtc::PeerConnectionInterface* peer_connection = self->GetPeerConnection();
  self->remote_sdp_cb_.Reset();
  self->remote_sdp_err_cb_.Reset();

  if(!peer_connection) {
    Nan::ThrowError("Internal error");
  }

  if(info[0].IsEmpty() || !info[0]->IsObject()) {
    Nan::ThrowError("Invalid SessionDescription");
  }

  v8::Local<v8::Object> desc_obj = v8::Local<v8::Object>::Cast(info[0]);
  v8::Local<v8::Value> type_value = desc_obj->Get(Nan::New("type")
    .ToLocalChecked());
  v8::Local<v8::Value> sdp_value = desc_obj->Get(Nan::New("sdp")
    .ToLocalChecked());

  if(type_value.IsEmpty() || !type_value->IsString()) {
    Nan::ThrowError("Invalid SessionDescription type");
  }

  if(sdp_value.IsEmpty() || !sdp_value->IsString()) {
    Nan::ThrowError("Invalid SessionDescription");
  }

  if(!info[1].IsEmpty() && info[1]->IsFunction()) {
    self->remote_sdp_cb_.Reset<v8::Function>(
      v8::Local<v8::Function>::Cast(info[1]));
  }

  if(!info[2].IsEmpty() && info[2]->IsFunction()) {
    self->remote_sdp_err_cb_.Reset<v8::Function>(
      v8::Local<v8::Function>::Cast(info[2]));
  }

  v8::String::Utf8Value type(type_value->ToString());
  v8::String::Utf8Value sdp(sdp_value->ToString());
  webrtc::SessionDescriptionInterface* desc(
    webrtc::CreateSessionDescription(*type, *sdp, 0));

  if(!desc) {
    Nan::ThrowError("webrtc::CreateSessionDescription failure");
  }

  self->remote_sdp_.Reset<v8::Object>(desc_obj);
  peer_connection->SetRemoteDescription(
    self->remote_description_observer_.get(), desc);

  info.GetReturnValue().SetUndefined();
}

NAN_METHOD(PeerConnection::AddIceCandidate) {
  PeerConnection* self = Nan::ObjectWrap::Unwrap<PeerConnection>(info.Holder());
  webrtc::PeerConnectionInterface* peer_connection = self->GetPeerConnection();

  v8::Local<v8::Value> argv[1];

  if(!peer_connection) {
    Nan::ThrowError("Internal error");
  }

  if(info[0].IsEmpty() || !info[0]->IsObject()) {
    Nan::ThrowError("Invalid SDP");
  }

  v8::Local<v8::Object> desc = v8::Local<v8::Object>::Cast(info[0]);

  v8::Local<v8::Value> sdpMid_value = desc->Get(Nan::New("sdpMid")
    .ToLocalChecked());

  if(sdpMid_value.IsEmpty() || !sdpMid_value->IsString()) {
    Nan::ThrowError("Invalid sdpMid");
  }

  v8::Local<v8::Value> sdpMLineIndex_value = desc->Get(
    Nan::New("sdpMLineIndex").ToLocalChecked());

  if(sdpMLineIndex_value.IsEmpty() || !sdpMLineIndex_value->IsInt32()) {
    Nan::ThrowError("Invalid sdpMLineIndex");
  }

  v8::Local<v8::Value> sdp_value = desc->Get(Nan::New("candidate")
    .ToLocalChecked());

  if(sdp_value.IsEmpty() || !sdp_value->IsString()) {
    Nan::ThrowError("Invalid SDP");
  }

  v8::Local<v8::Int32> sdpMLineIndex(sdpMLineIndex_value->ToInt32());
  v8::String::Utf8Value sdpMid(sdpMid_value->ToString());
  v8::String::Utf8Value sdp(sdp_value->ToString());
  rtc::scoped_ptr<webrtc::IceCandidateInterface> candidate(
    webrtc::CreateIceCandidate(*sdpMid, sdpMLineIndex->Value(), *sdp, 0));

  if(!candidate.get()) {
    Nan::ThrowError("Invalid ICE candidate");
  }

  if(!peer_connection->AddIceCandidate(candidate.get())) {
    Nan::ThrowError("Failed to add ICE candidate");
  }

  if(!info[1].IsEmpty() && info[1]->IsFunction()) {
    v8::Local<v8::Function> success = v8::Local<v8::Function>::Cast(info[1]);
    success->Call(info.This(), 0, argv);
  }

  info.GetReturnValue().SetUndefined();
}

NAN_METHOD(PeerConnection::GetStats) {
  PeerConnection* self = Nan::ObjectWrap::Unwrap<PeerConnection>(info.Holder());
  webrtc::PeerConnectionInterface* peer_connection = self->GetPeerConnection();

  if(!peer_connection) {
    Nan::ThrowError("Internal error");
  }

  if(!info[0].IsEmpty() && info[0]->IsFunction()) {
    self->onstats_.Reset<v8::Function>(v8::Local<v8::Function>::Cast(info[0]));
  }

  if(!peer_connection->GetStats(self->stats_observer_.get(), 0,
      webrtc::PeerConnectionInterface::kStatsOutputLevelStandard)) {
    v8::Local<v8::Function> callback = Nan::New<v8::Function>(self->onstats_);
    v8::Local<v8::Value> argv[1] = { Nan::Null() };
    callback->Call(info.This(), 1, argv);
    self->onstats_.Reset();
  }

  info.GetReturnValue().SetUndefined();
}

NAN_GETTER(PeerConnection::GetOnNegotiationNeeded) {
  PeerConnection* self = Nan::ObjectWrap::Unwrap<PeerConnection>(info.Holder());
  return info.GetReturnValue().Set(Nan::New<v8::Function>(
    self->onnegotiationneeded_));
}

NAN_SETTER(PeerConnection::SetOnNegotiationNeeded) {
  PeerConnection* self = Nan::ObjectWrap::Unwrap<PeerConnection>(info.Holder());
  self->onnegotiationneeded_.Reset();
  if(!value.IsEmpty() && value->IsFunction()) {
    self->onnegotiationneeded_.Reset<v8::Function>(
      v8::Local<v8::Function>::Cast(value));
  }
}


NAN_GETTER(PeerConnection::GetOnIceCandidate) {
  PeerConnection* self = Nan::ObjectWrap::Unwrap<PeerConnection>(info.Holder());
  return info.GetReturnValue().Set(Nan::New<v8::Function>(
    self->onicecandidate_));
}

NAN_SETTER(PeerConnection::SetOnIceCandidate) {
  PeerConnection* self = Nan::ObjectWrap::Unwrap<PeerConnection>(info.Holder());
  self->onicecandidate_.Reset();
  if(!value.IsEmpty() && value->IsFunction()) {
    self->onicecandidate_.Reset<v8::Function>(
      v8::Local<v8::Function>::Cast(value));
  }
}

PeerConnection::PeerConnection() {
  stats_observer_ = new rtc::RefCountedObject<StatsObserver>(this);
  offer_observer_ = new rtc::RefCountedObject<OfferObserver>(this);
  answer_observer_ = new rtc::RefCountedObject<AnswerObserver>(this);
  local_description_observer_ =
    new rtc::RefCountedObject<LocalDescriptionObserver>(this);
  remote_description_observer_ =
    new rtc::RefCountedObject<RemoteDescriptionObserver>(this);
  peer_connection_observer_ =
    new rtc::RefCountedObject<PeerConnectionObserver>(this);

  // decoder_factory_.reset(new RecordingDecoderFactory(this));

  // worker_thread_.reset(new rtc::Thread());
  // worker_thread_->Start();

  // signaling_thread_.reset(new rtc::Thread());
  // signaling_thread_->Start();

  // factory_ = webrtc::CreatePeerConnectionFactory(signaling_thread_.get(),
  //   worker_thread_.get(), nullptr, nullptr, decoder_factory_.get());
}

PeerConnection::~PeerConnection() {
  LOG(LS_INFO) << __FUNCTION__;
  stats_observer_->RemoveListener(this);
  offer_observer_->RemoveListener(this);
  answer_observer_->RemoveListener(this);
  local_description_observer_->RemoveListener(this);
  remote_description_observer_->RemoveListener(this);
  peer_connection_observer_->RemoveListener(this);
}

void PeerConnection::GetUserMedia() {
  LOG(LS_INFO) << __FUNCTION__;

  // // Constraints
  // constraints_.AddOptional(
  //   webrtc::MediaConstraintsInterface::kEnableDtlsSrtp, "true");
  // constraints_.AddMandatory(
  //   webrtc::MediaConstraintsInterface::kOfferToReceiveVideo, "true");

  // // MediaStream
  // rtc::scoped_refptr<webrtc::MediaStreamInterface> stream =
  //   factory_->CreateLocalMediaStream("stream_1");

  // // Audio
  // rtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track(
  //   factory_->CreateAudioTrack("audio_1",
  //     factory_->CreateAudioSource(&constraints_)));

  // stream->AddTrack(audio_track);

  // // Video
  // rtc::scoped_refptr<webrtc::VideoSourceInterface> video_source =
  //   factory_->CreateVideoSource(new webrtc::FakePeriodicVideoCapturer(),
  //     &constraints_);

  // rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track(
  //   factory_->CreateVideoTrack("video_1", video_source));

  // stream->AddTrack(video_track);

  // peer_connection_->AddStream(stream);
}

webrtc::PeerConnectionInterface* PeerConnection::GetPeerConnection() {
  // if(!factory_.get()) {
  //   Nan::ThrowError("Internal Factory Error");
  // }
  // if(!peer_connection_.get()) {
  //   EventEmitter::SetReference(true);
  //   peer_connection_ = factory_->CreatePeerConnection(config_, &constraints_,
  //     nullptr, nullptr, peer_connection_observer_);
  // }
  return peer_connection_.get();
}

void PeerConnection::On(Event *event) {
  PeerConnectionEvent type = event->As<PeerConnectionEvent>();

  Nan::HandleScope scope;

  int argc = 0;
  v8::Local<v8::Value> argv[1];

  v8::Local<v8::String> s;
  v8::Local<v8::Function> fn;
  v8::Local<v8::Object> container;
  bool isError = false;
  std::string data;

  switch(type) {
    case kVideoDecoderCreated:
      LOG(LS_INFO) << "kVideoDecoderCreated";
      break;

    case kVideoDecoderDestroyed:
      LOG(LS_INFO) << "kVideoDecoderDestroyed";
      break;

    case kPeerConnectionAddStream:
    case kPeerConnectionCreateClosed:
    case kPeerConnectionDataChannel:
    case kPeerConnectionIceChange:
    case kPeerConnectionIceGathering:
    case kPeerConnectionRemoveStream:
    case kPeerConnectionSignalChange:
      break;

    case kPeerConnectionStats:
      fn = Nan::New<v8::Function>(onstats_);

      // v8::Local<v8::Array> stats_reports = Nan::New<v8::Array>(reports.size());

      // std::vector<const webrtc::StatsReport*> raw =
      //   event->Unwrap<webrtc::StatsReports>();

      argv[0] = { Nan::Null() };
      argc = 1;
      break;

    case kPeerConnectionCreateOffer:
      fn = Nan::New<v8::Function>(offer_cb_);
      offer_cb_.Reset();
      offer_err_cb_.Reset();
      data = event->Unwrap<std::string>();
      s = v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), data.c_str());
      argv[0] = v8::JSON::Parse(s);
      argc = 1;
      break;

    case kPeerConnectionCreateOfferError:
      fn = Nan::New<v8::Function>(offer_err_cb_);
      offer_cb_.Reset();
      offer_err_cb_.Reset();
      isError = true;
      data = event->Unwrap<std::string>();
      argv[0] = Nan::Error(data.c_str());
      argc = 1;
      break;

    case kPeerConnectionCreateAnswer:
      fn = Nan::New<v8::Function>(answer_cb_);
      answer_cb_.Reset();
      answer_err_cb_.Reset();
      data = event->Unwrap<std::string>();
      s = v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), data.c_str());
      argv[0] = v8::JSON::Parse(s);
      argc = 1;
      break;

    case kPeerConnectionCreateAnswerError:
      fn = Nan::New<v8::Function>(answer_err_cb_);
      answer_cb_.Reset();
      answer_err_cb_.Reset();
      isError = true;
      data = event->Unwrap<std::string>();
      argv[0] = Nan::Error(data.c_str());
      argc = 1;
      break;

    case kPeerConnectionIceCandidate:
      fn = Nan::New<v8::Function>(onicecandidate_);
      container = Nan::New<v8::Object>();
      data = event->Unwrap<std::string>();
      container->Set(Nan::New("candidate").ToLocalChecked(), Nan::Null());
      if(!data.empty()) {
        s = v8::String::NewFromUtf8(v8::Isolate::GetCurrent(), data.c_str());
        container->Set(Nan::New("candidate").ToLocalChecked(),
          v8::JSON::Parse(s));
      }
      argv[0] = container;
      argc = 1;
      break;

    case kPeerConnectionRenegotiation:
      fn = Nan::New<v8::Function>(onnegotiationneeded_);
      onnegotiationneeded_.Reset();
      peer_connection_->CreateOffer(offer_observer_.get(), nullptr);
      break;

    case kPeerConnectionSetLocalDescription:
      fn = Nan::New<v8::Function>(local_sdp_cb_);
      local_sdp_cb_.Reset();
      local_sdp_err_cb_.Reset();
      break;

    case kPeerConnectionSetLocalDescriptionError:
      fn = Nan::New<v8::Function>(local_sdp_err_cb_);
      local_sdp_cb_.Reset();
      local_sdp_err_cb_.Reset();
      isError = true;
      data = event->Unwrap<std::string>();
      argv[0] = Nan::Error(data.c_str());
      argc = 1;
      break;

    case kPeerConnectionSetRemoteDescription:
      fn = Nan::New<v8::Function>(remote_sdp_cb_);
      remote_sdp_cb_.Reset();
      remote_sdp_err_cb_.Reset();
      break;

    case kPeerConnectionSetRemoteDescriptionError:
      fn = Nan::New<v8::Function>(remote_sdp_err_cb_);
      remote_sdp_cb_.Reset();
      remote_sdp_err_cb_.Reset();
      isError = true;
      data = event->Unwrap<std::string>();
      argv[0] = Nan::Error(data.c_str());
      argc = 1;
      break;
  }

  if(!fn.IsEmpty() && fn->IsFunction()) {
    Nan::Callback cb(fn);
    cb.Call(argc, argv);
  } else if(isError) {
    Nan::ThrowError(argv[0]);
  }
}

