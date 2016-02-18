#include "peerconnection.h"

NAN_MODULE_INIT(PeerConnection::Init) {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("PeerConnection").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(tpl, "createOffer", PeerConnection::CreateOffer);

  constructor.Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(target, Nan::New("PeerConnection").ToLocalChecked(),
    Nan::GetFunction(tpl).ToLocalChecked());
}

NAN_METHOD(PeerConnection::New) {
  if(!info.IsConstructCall()) {
    return Nan::ThrowError("Use new operator");
  }
  PeerConnection *obj = new PeerConnection();
  obj->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

void PeerConnection::GetUserMedia() {
  LOG(LS_INFO) << __FUNCTION__;

  std::string stream_id = "stream_1";
  rtc::scoped_refptr<webrtc::MediaStreamInterface> stream =
    factory_->CreateLocalMediaStream(stream_id);

  // Audio
  rtc::scoped_refptr<webrtc::AudioTrackInterface> audio_track(
    factory_->CreateAudioTrack("audio_1",
      factory_->CreateAudioSource(&constraints_)));

  stream->AddTrack(audio_track);

  // // Video
  // FakeConstraints constraints = video_constraints;
  // constraints.SetMandatoryMaxFrameRate(10);
  // rtc::scoped_refptr<webrtc::VideoSourceInterface> source =
  //     peer_connection_factory_->CreateVideoSource(
  //         new webrtc::FakePeriodicVideoCapturer(), &constraints);
  // std::string videotrack_label = label + kVideoTrackLabelBase;
  // rtc::scoped_refptr<webrtc::VideoTrackInterface> video_track(
  //     peer_connection_factory_->CreateVideoTrack(videotrack_label, source));
  // stream->AddTrack(video_track);

  peer_connection_->AddStream(stream);
}

NAN_METHOD(PeerConnection::CreateOffer) {
  LOG(LS_INFO) << __FUNCTION__;

  PeerConnection* self = Nan::ObjectWrap::Unwrap<PeerConnection>(info.Holder());
  self->offer_cb_.Reset();
  self->offer_err_cb_.Reset();

  webrtc::PeerConnectionInterface *peer_connection = self->GetPeerConnection();
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

  // peer_connection->CreateOffer(self->offer_observer_, &self->constraints_);

  info.GetReturnValue().SetUndefined();
}

// static NAN_METHOD(CreateAnswer) {
//   LOG(LS_INFO) << __PRETTY_FUNCTION__;

//   PeerConnection* self = Nan::ObjectWrap::Unwrap<PeerConnection>(info.Holder());
//   self->answer_cb_.Reset();
//   self->answer_err_cb_.Reset();
//   webrtc::PeerConnectionInterface *peer_connection = self->GetPeerConnection();
//   if(!peer_connection) {
//     Nan::ThrowError("Internal Error");
//   }

//   if(!info[0].IsEmpty() && info[0]->IsFunction()) {
//     self->answer_cb_.Reset<v8::Function>(
//       v8::Local<v8::Function>::Cast(info[0]));
//   }
//   if(!info[1].IsEmpty() && info[1]->IsFunction()) {
//     self->answer_err_cb_.Reset<v8::Function>(
//       v8::Local<v8::Function>::Cast(info[1]));
//   }

//   peer_connection->CreateAnswer(self->answer_observer_.get(),
//     &self->constraints_);

//   info.GetReturnValue().SetUndefined();
// }

Nan::Persistent<v8::Function> PeerConnection::constructor;

class BlockingThread : public rtc::Thread {
 public:
  virtual void Run() {
    LOG(LS_INFO) << __FUNCTION__;
      rtc::Thread::SetAllowBlockingCalls(true);
      rtc::Thread::Run();
    }
};

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

  BlockingThread* t = new BlockingThread();
  t->Start();
  rtc::ThreadManager::Instance()->SetCurrentThread(t);

  factory_ = webrtc::CreatePeerConnectionFactory(rtc::Thread::Current(),
    rtc::Thread::Current(), nullptr, nullptr, nullptr);
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

webrtc::PeerConnectionInterface* PeerConnection::GetPeerConnection() {
  if(!factory_.get()) {
    Nan::ThrowError("Internal Factory Error");
  }
  if(!peer_connection_.get()) {
    EventEmitter::SetReference(true);
    peer_connection_ = factory_->CreatePeerConnection(config_, &constraints_,
      nullptr, nullptr, peer_connection_observer_);
  }
  return peer_connection_.get();
}

void PeerConnection::On(Event *event) {
  PeerConnectionEvent type = event->As<PeerConnectionEvent>();

  Nan::HandleScope scope;

  int argc = 0;
  v8::Local<v8::Value> argv[1];

  v8::Local<v8::Function> fn;
  bool isError = false;
  std::string data;

  Json::Reader reader;
  Json::Value msg;

  switch(type) {
    case kPeerConnectionAddStream:
    case kPeerConnectionCreateAnswer:
    case kPeerConnectionCreateAnswerError:
    case kPeerConnectionCreateClosed:
      break;

    case kPeerConnectionCreateOffer:
      LOG(LS_INFO) << __FUNCTION__ << ": kPeerConnectionCreateOffer";
      fn = Nan::New<v8::Function>(offer_cb_);
      offer_cb_.Reset();
      offer_err_cb_.Reset();
      data = event->Unwrap<std::string>();
      // if(!reader.parse(Nan::New(data.c_str()).ToLocalChecked(), msg)) {
      // if(!reader.parse(data, msg)) {
      //   LOG(WARNING) << "Unknown message: " << data;
      // }
      // argv[0] = Json::Parse();
      argv[0] = Nan::New(data.c_str()).ToLocalChecked(); // data is offer SDP
      argc = 1;
      break;

    case kPeerConnectionCreateOfferError:
      LOG(LS_INFO) << __FUNCTION__ << ": kPeerConnectionCreateOfferError";
      fn = Nan::New<v8::Function>(offer_err_cb_);
      offer_cb_.Reset();
      offer_err_cb_.Reset();
      isError = true;
      data = event->Unwrap<std::string>();
      argv[0] = Nan::Error(data.c_str());
      argc = 1;
      break;

    case kPeerConnectionDataChannel:
    case kPeerConnectionIceCandidate:
    case kPeerConnectionIceChange:
    case kPeerConnectionIceGathering:
    case kPeerConnectionRemoveStream:
      break;

    case kPeerConnectionRenegotiation:
      LOG(LS_INFO) << __FUNCTION__ << ": kPeerConnectionRenegotiation";
      peer_connection_->CreateOffer(offer_observer_.get(), &constraints_);
      break;

    case kPeerConnectionSetLocalDescription:
    case kPeerConnectionSetLocalDescriptionError:
    case kPeerConnectionSetRemoteDescription:
    case kPeerConnectionSetRemoteDescriptionError:
    case kPeerConnectionSignalChange:
    case kPeerConnectionStats:
      break;
  }

  if(!fn.IsEmpty() && fn->IsFunction()) {
    Nan::Callback cb(fn);
    cb.Call(argc, argv);
  } else if(isError) {
    Nan::ThrowError(argv[0]);
  }
}

