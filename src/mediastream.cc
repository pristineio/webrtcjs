#include "mediastream.h"

Nan::Persistent<v8::Function> MediaStream::constructor;

NAN_MODULE_INIT(MediaStream::Init) {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("MediaStream").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(tpl, "addTrack", MediaStream::AddTrack);
  Nan::SetPrototypeMethod(tpl, "removeTrack", MediaStream::RemoveTrack);
  Nan::SetPrototypeMethod(tpl, "clone", MediaStream::Clone);
  Nan::SetPrototypeMethod(tpl, "getAudioTracks", MediaStream::GetAudioTracks);
  Nan::SetPrototypeMethod(tpl, "getTrackById", MediaStream::GetTrackById);
  Nan::SetPrototypeMethod(tpl, "getVideoTracks", MediaStream::GetVideoTracks);

  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("active").ToLocalChecked(),
    MediaStream::GetActive);

  Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("id").ToLocalChecked(),
    MediaStream::GetId);

  Nan::SetAccessor(tpl->InstanceTemplate(),
    Nan::New("onaddtrack").ToLocalChecked(),
    MediaStream::GetOnAddTrack,
    MediaStream::SetOnAddTrack);

  Nan::SetAccessor(tpl->InstanceTemplate(),
    Nan::New("onremovetrack").ToLocalChecked(),
    MediaStream::GetOnRemoveTrack,
    MediaStream::SetOnRemoveTrack);

  constructor.Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(target, Nan::New("MediaStream").ToLocalChecked(),
    Nan::GetFunction(tpl).ToLocalChecked());
}

MediaStream::MediaStream() : active_(false) {
  observer_ = new rtc::RefCountedObject<MediaStreamObserver>(this);
}

MediaStream::~MediaStream() {
  if(stream_.get()) {
    stream_->UnregisterObserver(observer_.get());
    observer_->RemoveListener(this);
  }
}

v8::Local<v8::Value> MediaStream::New(
    rtc::scoped_refptr<webrtc::MediaStreamInterface> media_stream) {
  Nan::EscapableHandleScope scope;
  v8::Local<v8::Value> empty;
  v8::Local<v8::Function> instance = Nan::New(MediaStream::constructor);
  if(instance.IsEmpty() || !media_stream.get()) {
    return scope.Escape(Nan::Null());
  }

  v8::Local<v8::Object> ret = instance->NewInstance();
  MediaStream* self = Nan::ObjectWrap::Unwrap<MediaStream>(ret); //info.Holder());
  if(self) {
    self->stream_ = media_stream;
    self->stream_->RegisterObserver(self->observer_.get());
    self->Emit(kMediaStreamChanged);
    return scope.Escape(ret);
  }

  return scope.Escape(Nan::Null());
}

NAN_METHOD(MediaStream::New) {
  if(!info.IsConstructCall()) {
    return Nan::ThrowError("Use new operator");
  }
  MediaStream* self = new MediaStream();
  self->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

NAN_METHOD(MediaStream::AddTrack) {
  bool retval = false;

  MediaStream* self = Nan::ObjectWrap::Unwrap<MediaStream>(info.Holder());
  if(!self) {
    return Nan::ThrowError("Bad pointer to self");
  }

  rtc::scoped_refptr<webrtc::MediaStreamInterface> stream = self->stream_;
  if(!stream) {
    return Nan::ThrowError("Bad pointer to webrtc::MediaStreamInterface");
  }
  if(info.Length() == 0 || !info[0]->IsObject()) {
    return Nan::ThrowError("Bad argument pointer");
  }

  MediaStreamTrack* media_stream_track =
    Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info[0]->ToObject());
  rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track =
    media_stream_track->track_;
  if(!track.get()) {
    return Nan::ThrowError("Bad MediaStreamTrackInterface pointer");
  }

  std::string kind = track->kind();
  if(kind.compare("audio") == 0) {
    rtc::scoped_refptr<webrtc::AudioTrackInterface>
      audio(static_cast<webrtc::AudioTrackInterface*>(track.get()));
    retval = stream->AddTrack(audio);
  } else {
    rtc::scoped_refptr<webrtc::VideoTrackInterface>
      video(static_cast<webrtc::VideoTrackInterface*>(track.get()));
    retval = stream->AddTrack(video);
  }

  return info.GetReturnValue().Set(Nan::New(retval));
}

NAN_METHOD(MediaStream::RemoveTrack) {
  bool retval = false;
  MediaStream* self = Nan::ObjectWrap::Unwrap<MediaStream>(info.Holder());
  if(!self) {
    return Nan::ThrowError("Bad pointer to self");
  }

  rtc::scoped_refptr<webrtc::MediaStreamInterface> stream = self->stream_;
  if(!stream) {
    return Nan::ThrowError("Bad pointer to webrtc::MediaStreamInterface");
  }
  if(info.Length() == 0 || !info[0]->IsObject()) {
    return Nan::ThrowError("Bad argument pointer");
  }

  MediaStreamTrack* media_stream_track =
    Nan::ObjectWrap::Unwrap<MediaStreamTrack>(info[0]->ToObject());
  rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track =
    media_stream_track->track_;
  if(!track.get()) {
    return Nan::ThrowError("Bad MediaStreamTrackInterface pointer");
  }

  std::string kind = track->kind();
  if(kind.compare("audio") == 0) {
    rtc::scoped_refptr<webrtc::AudioTrackInterface>
      audio(static_cast<webrtc::AudioTrackInterface*>(track.get()));
    retval = stream->RemoveTrack(audio);
  } else {
    rtc::scoped_refptr<webrtc::VideoTrackInterface>
      video(static_cast<webrtc::VideoTrackInterface*>(track.get()));
    retval = stream->RemoveTrack(video);
  }

  return info.GetReturnValue().Set(Nan::New(retval));
}

NAN_METHOD(MediaStream::Clone) {
  rtc::scoped_refptr<webrtc::MediaStreamInterface> self =
    MediaStream::Unwrap(info.This());
  if(!self.get()) {
    Nan::ThrowError("Bad pointer to webrtc::MediaStreamInterface");
  }

  rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> factory =
    WebRtcJs::GetPeerConnectionFactory();
  if(!factory.get()) {
    Nan::ThrowError("Bad pointer to webrtc::PeerConnectionFactoryInterface");
  }

  rtc::scoped_refptr<webrtc::MediaStreamInterface> stream =
    factory->CreateLocalMediaStream("stream");
  if(!stream.get()) {
    Nan::ThrowError("Could not create new webrtc::MediaStreamInterface");
  }

  webrtc::AudioTrackVector audio_list = self->GetAudioTracks();
  std::vector<rtc::scoped_refptr<webrtc::AudioTrackInterface> >::iterator
    audio_it;
  for(audio_it = audio_list.begin(); audio_it != audio_list.end(); audio_it++) {
    rtc::scoped_refptr<webrtc::AudioTrackInterface> track(*audio_it);
    if(track.get()) {
      stream->AddTrack(track.get());
    }
  }

  webrtc::VideoTrackVector video_list = self->GetVideoTracks();
  std::vector<rtc::scoped_refptr<webrtc::VideoTrackInterface> >::iterator
    video_it;
  for(video_it = video_list.begin(); video_it != video_list.end(); video_it++) {
    rtc::scoped_refptr<webrtc::VideoTrackInterface> track(*video_it);
    if(track.get()) {
      stream->AddTrack(track.get());
    }
  }

  info.GetReturnValue().Set(MediaStream::New(stream));
}

NAN_METHOD(MediaStream::GetTrackById) {
  rtc::scoped_refptr<webrtc::MediaStreamInterface> self =
    MediaStream::Unwrap(info.This());
  info.GetReturnValue().SetUndefined();
  if(!self.get()) {
    return Nan::ThrowError("Internal Error");
  }
  if(info.Length() == 0 || !info[0]->IsString()) {
    return info.GetReturnValue().Set(Nan::Null());
  }
  v8::String::Utf8Value id_value(info[0]->ToString());
  std::string id(*id_value);
  rtc::scoped_refptr<webrtc::AudioTrackInterface> audio =
    self->FindAudioTrack(id);
  if(audio.get()) {
    return info.GetReturnValue().Set(MediaStreamTrack::New(audio.get()));
  }
  rtc::scoped_refptr<webrtc::VideoTrackInterface> video =
    self->FindVideoTrack(id);
  if(video.get()) {
    return info.GetReturnValue().Set(MediaStreamTrack::New(video.get()));
  }
}

NAN_METHOD(MediaStream::GetAudioTracks) {
  rtc::scoped_refptr<webrtc::MediaStreamInterface> self =
    MediaStream::Unwrap(info.This());
  info.GetReturnValue().SetUndefined();
  if(!self.get()) {
    return Nan::ThrowError("Internal Error");
  }
  webrtc::AudioTrackVector audio_list = self->GetAudioTracks();
  std::vector<rtc::scoped_refptr<webrtc::AudioTrackInterface>>::iterator
    audio_it;
  v8::Local<v8::Array> list = Nan::New<v8::Array>();
  uint32_t index = 0;
  for(audio_it = audio_list.begin(); audio_it != audio_list.end(); audio_it++) {
    rtc::scoped_refptr<webrtc::AudioTrackInterface> track(*audio_it);
    if(track.get()) {
      list->Set(index, MediaStreamTrack::New(track.get()));
      index++;
    }
  }
  info.GetReturnValue().Set(list);
}

NAN_METHOD(MediaStream::GetVideoTracks) {
  rtc::scoped_refptr<webrtc::MediaStreamInterface> self =
    MediaStream::Unwrap(info.This());
  info.GetReturnValue().SetUndefined();
  if(!self.get()) {
    return Nan::ThrowError("Internal Error");
  }
  webrtc::VideoTrackVector video_list = self->GetVideoTracks();
  std::vector<rtc::scoped_refptr<webrtc::VideoTrackInterface>>::iterator
    video_it;
  v8::Local<v8::Array> list = Nan::New<v8::Array>();
  uint32_t index = 0;
  for(video_it = video_list.begin(); video_it != video_list.end(); video_it++) {
    rtc::scoped_refptr<webrtc::VideoTrackInterface> track(*video_it);
    if(track.get()) {
      list->Set(index, MediaStreamTrack::New(track.get()));
      index++;
    }
  }
  info.GetReturnValue().Set(list);
}

rtc::scoped_refptr<webrtc::MediaStreamInterface> MediaStream::Unwrap(
    v8::Local<v8::Object> value) {
  if(value.IsEmpty()) {
    return nullptr;
  }
  MediaStream* self = Nan::ObjectWrap::Unwrap<MediaStream>(value);
  return self->stream_;
}

rtc::scoped_refptr<webrtc::MediaStreamInterface> MediaStream::Unwrap(
    v8::Local<v8::Value> value) {
  if(value.IsEmpty() || !value->IsObject()) {
    return nullptr;
  }
  v8::Local<v8::Object> stream = v8::Local<v8::Object>::Cast(value);
  return MediaStream::Unwrap(stream);
}

NAN_GETTER(MediaStream::GetActive) {
  MediaStream* self = Nan::ObjectWrap::Unwrap<MediaStream>(info.Holder());
  return info.GetReturnValue().Set(Nan::New(self->active_));
}

NAN_GETTER(MediaStream::GetId) {
  rtc::scoped_refptr<webrtc::MediaStreamInterface> stream =
    MediaStream::Unwrap(info.This());
  if(!stream.get()) {
    info.GetReturnValue().SetUndefined();
    return Nan::ThrowError("Internal Error");
  }
  return info.GetReturnValue().Set(Nan::New(stream->label().c_str())
    .ToLocalChecked());
}

NAN_GETTER(MediaStream::GetOnAddTrack) {
  MediaStream* self = Nan::ObjectWrap::Unwrap<MediaStream>(info.Holder());
  return info.GetReturnValue().Set(Nan::New<v8::Function>(self->onaddtrack_));
}

NAN_SETTER(MediaStream::SetOnAddTrack) {
  MediaStream* self = Nan::ObjectWrap::Unwrap<MediaStream>(info.Holder());
  self->onaddtrack_.Reset();
  if(!value.IsEmpty() && value->IsFunction()) {
    self->onaddtrack_.Reset<v8::Function>(v8::Local<v8::Function>::Cast(value));
  }
}

NAN_GETTER(MediaStream::GetOnRemoveTrack) {
  MediaStream* self = Nan::ObjectWrap::Unwrap<MediaStream>(info.Holder());
  return info.GetReturnValue().Set(Nan::New<v8::Function>(
    self->onremovetrack_));
}

NAN_SETTER(MediaStream::SetOnRemoveTrack) {
  MediaStream* self = Nan::ObjectWrap::Unwrap<MediaStream>(info.Holder());
  self->onremovetrack_.Reset();
  if(!value.IsEmpty() && value->IsFunction()) {
    self->onremovetrack_.Reset<v8::Function>(
      v8::Local<v8::Function>::Cast(value));
  }
}

void MediaStream::On(Event* event) {
  Nan::HandleScope scope;
  EventType type = event->As<EventType>();

  if(type != kMediaStreamChanged) {
    return;
  }

  if(!stream_.get()) {
    return Nan::ThrowError("Bad pointer to stream");
  }

  webrtc::AudioTrackVector audio_list = stream_->GetAudioTracks();
  webrtc::VideoTrackVector video_list = stream_->GetVideoTracks();

  LOG(LS_INFO) << __FUNCTION__ << ": There are " << video_list.size() <<
    " video tracks and " << audio_list.size() << " audio tracks";

  if(!audio_list.empty() || !video_list.empty()) {
    LOG(LS_INFO) << __FUNCTION__ << ": Setting stream active";
    active_ = true;
  } else {
    LOG(LS_INFO) << __PRETTY_FUNCTION__ << ": Setting stream inactive";
    active_ = false;
  }

  if(audio_list.size() != audio_tracks_.size()) {
    // std::vector<rtc::scoped_refptr<webrtc::AudioTrackInterface>>::iterator
    //   last_audio, cur_audio;

    // if(audio_list.size() > audio_tracks_.size()) {
    //   for(cur_audio = audio_list.begin(); cur_audio != audio_list.end();
    //       cur_audio++) {
    //     rtc::scoped_refptr<webrtc::AudioTrackInterface> cur_track(*cur_audio);
    //     std::string cur_id;
    //     bool found = false;
    //     if(cur_track.get()) {
    //       cur_id = cur_track->id();
    //     }
    //     if(audio_tracks_.size()) {
    //       for(last_audio = audio_tracks_.begin();
    //           last_audio != audio_tracks_.end(); last_audio++) {
    //         rtc::scoped_refptr<webrtc::AudioTrackInterface>
    //           last_track(*last_audio);
    //         std::string last_id;
    //         if(last_track.get()) {
    //           last_id = last_track->id();
    //         }
    //         if(cur_id.compare(last_id) == 0) {
    //           found = true;
    //           break;
    //         }
    //       }
    //     }
    //     if(!found) {
    //       v8::Local<v8::Function> fn = Nan::New<v8::Function>(onaddtrack_);
    //       v8::Local<v8::Value> argv[] = {
    //         MediaStreamTrack::New(cur_track.get())
    //       };
    //       if(!fn.IsEmpty() && fn->IsFunction()) {
    //         Nan::Callback cb(fn);
    //         cb.Call(1, argv);
    //       }
    //     }
    //   }
    // } else {
    //   for(cur_audio = audio_tracks_.begin(); cur_audio != audio_tracks_.end();
    //       cur_audio++) {
    //     rtc::scoped_refptr<webrtc::AudioTrackInterface> cur_track(*cur_audio);
    //     std::string cur_id;
    //     bool found = false;
    //     if(cur_track.get()) {
    //       cur_id = cur_track->id();
    //     }
    //     if(audio_list.size()) {
    //       for(last_audio = audio_list.begin(); last_audio != audio_list.end();
    //           last_audio++) {
    //         rtc::scoped_refptr<webrtc::AudioTrackInterface>
    //           last_track(*last_audio);
    //         std::string last_id;
    //         if(last_track.get()) {
    //           last_id = last_track->id();
    //         }
    //         if(cur_id.compare(last_id) == 0) {
    //           found = true;
    //           break;
    //         }
    //       }
    //     }
    //     if(!found) {
    //       v8::Local<v8::Function> fn = Nan::New<v8::Function>(onremovetrack_);
    //       v8::Local<v8::Value> argv[] = {
    //         MediaStreamTrack::New(cur_track.get())
    //       };
    //       if(!fn.IsEmpty() && fn->IsFunction()) {
    //         Nan::Callback cb(fn);
    //         cb.Call(1, argv);
    //       }
    //     }
    //   }
    // }
    audio_tracks_ = audio_list;
  }

  if(video_list.size() != video_tracks_.size()) {
    // std::vector<rtc::scoped_refptr<webrtc::VideoTrackInterface> >::iterator
    //   last_video, cur_video;

    // if(video_list.size() > video_tracks_.size()) {
    //   for(cur_video = video_list.begin(); cur_video != video_list.end();
    //       cur_video++) {
    //     rtc::scoped_refptr<webrtc::VideoTrackInterface> cur_track(*cur_video);
    //     std::string cur_id;
    //     bool found = false;
    //     if(cur_track.get()) {
    //       cur_id = cur_track->id();
    //     }
    //     if(video_tracks_.size()) {
    //       for(last_video = video_tracks_.begin();
    //           last_video != video_tracks_.end(); last_video++) {
    //         rtc::scoped_refptr<webrtc::VideoTrackInterface>
    //           last_track(*last_video);
    //         std::string last_id;
    //         if(last_track.get()) {
    //           last_id = last_track->id();
    //         }
    //         if(cur_id.compare(last_id) == 0) {
    //           found = true;
    //           break;
    //         }
    //       }
    //     }
    //     if(!found) {
    //       v8::Local<v8::Function> fn = Nan::New<v8::Function>(onaddtrack_);
    //       v8::Local<v8::Value> argv[] = {
    //         MediaStreamTrack::New(cur_track.get())
    //       };
    //       if(!fn.IsEmpty() && fn->IsFunction()) {
    //         Nan::Callback cb(fn);
    //         cb.Call(1, argv);
    //       }
    //     }
    //   }
    // } else {
    //   for(cur_video = video_tracks_.begin(); cur_video != video_tracks_.end();
    //       cur_video++) {
    //     rtc::scoped_refptr<webrtc::VideoTrackInterface> cur_track(*cur_video);
    //     std::string cur_id;
    //     bool found = false;
    //     if(cur_track.get()) {
    //       cur_id = cur_track->id();
    //     }
    //     if(video_list.size()) {
    //       for(last_video = video_list.begin(); last_video != video_list.end();
    //           last_video++) {
    //         rtc::scoped_refptr<webrtc::VideoTrackInterface>
    //           last_track(*last_video);
    //         std::string last_id;
    //         if(last_track.get()) {
    //           last_id = last_track->id();
    //         }
    //         if(cur_id.compare(last_id) == 0) {
    //           found = true;
    //           break;
    //         }
    //       }
    //     }
    //     if(!found) {
    //       v8::Local<v8::Function> fn = Nan::New<v8::Function>(onremovetrack_);
    //       v8::Local<v8::Value> argv[] = {
    //         MediaStreamTrack::New(cur_track.get())
    //       };
    //       if(!fn.IsEmpty() && fn->IsFunction()) {
    //         Nan::Callback cb(fn);
    //         cb.Call(1, argv);
    //       }
    //     }
    //   }
    // }
    video_tracks_ = video_list;
  }
}