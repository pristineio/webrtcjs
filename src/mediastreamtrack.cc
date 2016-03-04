#include "mediastreamtrack.h"

Nan::Persistent<v8::Function> MediaStreamTrack::constructor;

NAN_MODULE_INIT(MediaStreamTrack::Init) {
  v8::Local<v8::FunctionTemplate> tpl =
    Nan::New<v8::FunctionTemplate>(MediaStreamTrack::New);
  tpl->SetClassName(Nan::New("MediaStreamTrack").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(tpl, "addSink", MediaStreamTrack::AddSink);
  Nan::SetPrototypeMethod(tpl, "removeSink", MediaStreamTrack::RemoveSink);

  Nan::SetAccessor(tpl->InstanceTemplate(),
    Nan::New("enabled").ToLocalChecked(),
    MediaStreamTrack::GetEnabled,
    MediaStreamTrack::SetEnabled);

  Nan::SetAccessor(tpl->InstanceTemplate(),
    Nan::New("id").ToLocalChecked(),
    MediaStreamTrack::GetId);

  Nan::SetAccessor(tpl->InstanceTemplate(),
    Nan::New("kind").ToLocalChecked(),
    MediaStreamTrack::GetKind);

  Nan::SetAccessor(tpl->InstanceTemplate(),
    Nan::New("readyState").ToLocalChecked(),
    MediaStreamTrack::GetReadyState);

  constructor.Reset<v8::Function>(tpl->GetFunction());
  Nan::Set(target, Nan::New("MediaStreamTrack").ToLocalChecked(),
    Nan::GetFunction(tpl).ToLocalChecked());
}

v8::Local<v8::Value> MediaStreamTrack::New(
    rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> media_stream_track) {
  Nan::EscapableHandleScope scope;
  v8::Local<v8::Value> argv[1];
  v8::Local<v8::Function> instance = Nan::New(MediaStreamTrack::constructor);
  if(instance.IsEmpty() || !media_stream_track.get()) {
    return scope.Escape(Nan::Null());
  }

  v8::Local<v8::Object> ret = instance->NewInstance(0, argv);
  MediaStreamTrack* self = Nan::ObjectWrap::Unwrap<MediaStreamTrack>(ret);

  self->track_ = media_stream_track;
  self->track_->RegisterObserver(self->observer_.get());
  self->Emit(kMediaStreamTrackChanged);

  return scope.Escape(ret);
}

MediaStreamTrack::MediaStreamTrack() {
  observer_ = new rtc::RefCountedObject<MediaStreamTrackObserver>(this);
}

MediaStreamTrack::~MediaStreamTrack() {
  if(track_.get()) {
    track_->UnregisterObserver(observer_.get());
    observer_->RemoveListener(this);
  }
}

NAN_METHOD(MediaStreamTrack::New) {
  if(!info.IsConstructCall()) {
    return Nan::ThrowError("Use new operator");
  }
  MediaStreamTrack* self = new MediaStreamTrack();
  self->Wrap(info.This());
  return info.GetReturnValue().Set(info.This());
}

NAN_METHOD(MediaStreamTrack::AddSink) {
  MediaStreamTrack* self =
    Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info.Holder());

  if(self->track_->kind().compare("audio") == 0) {
    rtc::scoped_refptr<webrtc::AudioTrackInterface>
      audio(static_cast<webrtc::AudioTrackInterface*>(self->track_.get()));
  } else {
    rtc::scoped_refptr<webrtc::VideoTrackInterface>
      video(static_cast<webrtc::VideoTrackInterface*>(self->track_.get()));

    VideoSink* video_sink =
      Nan::ObjectWrap::Unwrap<VideoSink>(info[0]->ToObject());

    video->AddOrUpdateSink(video_sink, rtc::VideoSinkWants());
  }

  info.GetReturnValue().SetUndefined();
}

NAN_METHOD(MediaStreamTrack::RemoveSink) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
}



NAN_GETTER(MediaStreamTrack::GetId) {
  MediaStreamTrack* self =
    Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info.Holder());
  std::string id = self->track_->id();
  info.GetReturnValue().Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(),
    id.c_str()));
}

NAN_GETTER(MediaStreamTrack::GetKind) {
  MediaStreamTrack* self =
    Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info.Holder());
  std::string kind = self->track_->kind();
  info.GetReturnValue().Set(v8::String::NewFromUtf8(v8::Isolate::GetCurrent(),
    kind.c_str()));
}

NAN_GETTER(MediaStreamTrack::GetReadyState) {
  MediaStreamTrack* self =
    Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info.Holder());
  info.GetReturnValue().Set(Nan::New(
    static_cast<int32_t>(self->track_->state())));
}


NAN_GETTER(MediaStreamTrack::GetEnabled) {
  MediaStreamTrack* self =
    Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info.Holder());
  info.GetReturnValue().Set(Nan::New(self->track_->enabled()));
}

NAN_SETTER(MediaStreamTrack::SetEnabled) {
  MediaStreamTrack* self =
    Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info.Holder());
  if(!value.IsEmpty() && value->IsBoolean()) {
    self->track_->set_enabled(value->BooleanValue());
  }
}


NAN_GETTER(MediaStreamTrack::GetOnStarted) {
  MediaStreamTrack* self =
    Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info.Holder());
  return info.GetReturnValue().Set(Nan::New<v8::Function>(self->onstarted_));
}

NAN_SETTER(MediaStreamTrack::SetOnStarted) {
  MediaStreamTrack* self =
    Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info.Holder());
  self->onstarted_.Reset();
  if(!value.IsEmpty() && value->IsFunction()) {
    self->onstarted_.Reset<v8::Function>(v8::Local<v8::Function>::Cast(value));
  }
}


NAN_GETTER(MediaStreamTrack::GetOnMute) {
  MediaStreamTrack* self =
    Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info.Holder());
  return info.GetReturnValue().Set(Nan::New<v8::Function>(self->onmute_));
}

NAN_SETTER(MediaStreamTrack::SetOnMute) {
  MediaStreamTrack* self =
    Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info.Holder());
  self->onmute_.Reset();
  if(!value.IsEmpty() && value->IsFunction()) {
    self->onmute_.Reset<v8::Function>(v8::Local<v8::Function>::Cast(value));
  }
}


NAN_GETTER(MediaStreamTrack::GetOnUnMute) {
  MediaStreamTrack* self =
    Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info.Holder());
  return info.GetReturnValue().Set(Nan::New<v8::Function>(self->onunmute_));
}

NAN_SETTER(MediaStreamTrack::SetOnUnMute) {
  MediaStreamTrack* self =
    Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info.Holder());
  self->onunmute_.Reset();
  if(!value.IsEmpty() && value->IsFunction()) {
    self->onunmute_.Reset<v8::Function>(v8::Local<v8::Function>::Cast(value));
  }
}


NAN_GETTER(MediaStreamTrack::GetOnOverConstrained) {
  MediaStreamTrack* self =
    Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info.Holder());
  return info.GetReturnValue().Set(Nan::New<v8::Function>(
    self->onoverconstrained_));
}

NAN_SETTER(MediaStreamTrack::SetOnOverConstrained) {
  MediaStreamTrack* self =
    Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info.Holder());
  self->onoverconstrained_.Reset();
  if(!value.IsEmpty() && value->IsFunction()) {
    self->onoverconstrained_.Reset<v8::Function>(
      v8::Local<v8::Function>::Cast(value));
  }
}


NAN_GETTER(MediaStreamTrack::GetOnEnded) {
  MediaStreamTrack* self =
    Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info.Holder());
  return info.GetReturnValue().Set(Nan::New<v8::Function>(self->onended_));
}

NAN_SETTER(MediaStreamTrack::SetOnEnded) {
  MediaStreamTrack* self =
    Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info.Holder());
  self->onended_.Reset();
  if(!value.IsEmpty() && value->IsFunction()) {
    self->onended_.Reset<v8::Function>(v8::Local<v8::Function>::Cast(value));
  }
}

void MediaStreamTrack::On(Event* event) {
  Nan::HandleScope scope;
  EventType type = event->As<EventType>();
  if(type != kMediaStreamTrackChanged) {
    return;
  }
}