#include "mediaconstraints.h"

MediaConstraints::MediaConstraints() : audio_(false), video_(false) { }

MediaConstraints::~MediaConstraints() { }

rtc::scoped_refptr<MediaConstraints> MediaConstraints::New() {
  return new rtc::RefCountedObject<MediaConstraints>();
}

rtc::scoped_refptr<MediaConstraints> MediaConstraints::New(
    const v8::Local<v8::Object>& constraints) {
  Nan::HandleScope scope;
  rtc::scoped_refptr<MediaConstraints> self = MediaConstraints::New();
  if(constraints.IsEmpty()) {
    return self;
  }
  v8::Local<v8::Value> optional_value =
    constraints->Get(Nan::New("optional").ToLocalChecked());
  if(!optional_value.IsEmpty() && optional_value->IsArray()) {
    v8::Local<v8::Array> options = v8::Local<v8::Array>::Cast(optional_value);
    for(unsigned int index = 0; index < options->Length(); index++) {
      v8::Local<v8::Value> option_value = options->Get(index);
      if(!option_value.IsEmpty() && option_value->IsObject()) {
        v8::Local<v8::Object> option =
          v8::Local<v8::Object>::Cast(option_value);
        v8::Local<v8::Value> DtlsSrtpKeyAgreement =
          option->Get(Nan::New("DtlsSrtpKeyAgreement").ToLocalChecked());
        v8::Local<v8::Value> RtpDataChannels =
          option->Get(Nan::New("RtpDataChannels").ToLocalChecked());
        v8::Local<v8::Value> googDscp =
          option->Get(Nan::New("googDscp").ToLocalChecked());
        v8::Local<v8::Value> googIPv6 =
          option->Get(Nan::New("googIPv6").ToLocalChecked());
        v8::Local<v8::Value> googSuspendBelowMinBitrate =
          option->Get(Nan::New("googSuspendBelowMinBitrate").ToLocalChecked());
        v8::Local<v8::Value> googCombinedAudioVideoBwe =
          option->Get(Nan::New("googCombinedAudioVideoBwe").ToLocalChecked());
        v8::Local<v8::Value> googScreencastMinBitrate =
          option->Get(Nan::New("googScreencastMinBitrate").ToLocalChecked());
        v8::Local<v8::Value> googPayloadPadding =
          option->Get(Nan::New("googPayloadPadding").ToLocalChecked());

        self->SetOptional(webrtc::MediaConstraintsInterface::kEnableDtlsSrtp,
          DtlsSrtpKeyAgreement);
        self->SetOptional(
          webrtc::MediaConstraintsInterface::kEnableRtpDataChannels,
            RtpDataChannels);
        self->SetOptional(webrtc::MediaConstraintsInterface::kEnableDscp,
          googDscp);
        self->SetOptional(webrtc::MediaConstraintsInterface::kEnableIPv6,
          googIPv6);
        self->SetOptional(
          webrtc::MediaConstraintsInterface::kEnableVideoSuspendBelowMinBitrate,
            googSuspendBelowMinBitrate);
        self->SetOptional(
          webrtc::MediaConstraintsInterface::kCombinedAudioVideoBwe,
            googCombinedAudioVideoBwe);
        self->SetOptional(
          webrtc::MediaConstraintsInterface::kScreencastMinBitrate,
            googScreencastMinBitrate);
        self->SetOptional(webrtc::MediaConstraintsInterface::kPayloadPadding,
          googPayloadPadding);
      }
    }
  }

  v8::Local<v8::Value> mandatory_value =
    constraints->Get(Nan::New("mandatory").ToLocalChecked());

  if(!mandatory_value.IsEmpty() && mandatory_value->IsObject()) {
    v8::Local<v8::Object> mandatory =
      v8::Local<v8::Object>::Cast(mandatory_value);
    v8::Local<v8::Value> OfferToReceiveAudio =
      mandatory->Get(Nan::New("OfferToReceiveAudio").ToLocalChecked());
    v8::Local<v8::Value> OfferToReceiveVideo =
      mandatory->Get(Nan::New("OfferToReceiveVideo").ToLocalChecked());
    v8::Local<v8::Value> VoiceActivityDetection =
      mandatory->Get(Nan::New("VoiceActivityDetection").ToLocalChecked());
    v8::Local<v8::Value> IceRestart =
      mandatory->Get(Nan::New("IceRestart").ToLocalChecked());
    v8::Local<v8::Value> googUseRtpMUX =
      mandatory->Get(Nan::New("googUseRtpMUX").ToLocalChecked());
    self->SetMandatory(webrtc::MediaConstraintsInterface::kOfferToReceiveAudio,
      OfferToReceiveAudio);
    self->SetMandatory(webrtc::MediaConstraintsInterface::kOfferToReceiveVideo,
      OfferToReceiveVideo);
    self->SetMandatory(
      webrtc::MediaConstraintsInterface::kVoiceActivityDetection,
        VoiceActivityDetection);
    self->SetMandatory(webrtc::MediaConstraintsInterface::kIceRestart,
      IceRestart);
    self->SetMandatory(webrtc::MediaConstraintsInterface::kUseRtpMux,
      googUseRtpMUX);
  }

  v8::Local<v8::Value> audio_value =
    constraints->Get(Nan::New("audio").ToLocalChecked());

  if(!audio_value.IsEmpty()) {
    if(audio_value->IsTrue() || audio_value->IsFalse()) {
      self->audio_ = true;
    } else if(audio_value->IsObject()) {
      v8::Local<v8::Object> audio = v8::Local<v8::Object>::Cast(audio_value);
      optional_value = audio->Get(Nan::New("optional").ToLocalChecked());
      if(!optional_value.IsEmpty() && optional_value->IsArray()) {
        v8::Local<v8::Array> options =
          v8::Local<v8::Array>::Cast(optional_value);
        for(unsigned int index = 0; index < options->Length(); index++) {
          v8::Local<v8::Value> option_value = options->Get(index);
          if(!option_value.IsEmpty() && option_value->IsObject()) {
            v8::Local<v8::Object> option =
              v8::Local<v8::Object>::Cast(option_value);
            v8::Local<v8::Value> googEchoCancellation =
              option->Get(Nan::New("googEchoCancellation").ToLocalChecked());
            v8::Local<v8::Value> googEchoCancellation2 =
              option->Get(Nan::New("googEchoCancellation2").ToLocalChecked());
            v8::Local<v8::Value> googDAEchoCancellation =
              option->Get(Nan::New("googDAEchoCancellation").ToLocalChecked());
            v8::Local<v8::Value> googAutoGainControl =
              option->Get(Nan::New("googAutoGainControl").ToLocalChecked());
            v8::Local<v8::Value> googAutoGainControl2 =
              option->Get(Nan::New("googAutoGainControl2").ToLocalChecked());
            v8::Local<v8::Value> googNoiseSuppression =
              option->Get(Nan::New("googNoiseSuppression").ToLocalChecked());
            v8::Local<v8::Value> googNoiseSuppression2 =
              option->Get(Nan::New("googNoiseSuppression2").ToLocalChecked());
            v8::Local<v8::Value> googHighpassFilter =
              option->Get(Nan::New("googHighpassFilter").ToLocalChecked());
            v8::Local<v8::Value> googTypingNoiseDetection =
              option->Get(Nan::New("googTypingNoiseDetection")
                .ToLocalChecked());
            v8::Local<v8::Value> googAudioMirroring =
              option->Get(Nan::New("googAudioMirroring").ToLocalChecked());
            v8::Local<v8::Value> sourceId =
              option->Get(Nan::New("sourceId").ToLocalChecked());

            self->SetOptional(
              webrtc::MediaConstraintsInterface::kEchoCancellation,
                googEchoCancellation);
            self->SetOptional(
              webrtc::MediaConstraintsInterface::
                kExtendedFilterEchoCancellation,
                  googEchoCancellation2);
            self->SetOptional(
              webrtc::MediaConstraintsInterface::kDAEchoCancellation,
                googDAEchoCancellation);
            self->SetOptional(
              webrtc::MediaConstraintsInterface::kAutoGainControl,
                googAutoGainControl);
            self->SetOptional(
              webrtc::MediaConstraintsInterface::kExperimentalAutoGainControl,
                googAutoGainControl2);
            self->SetOptional(
              webrtc::MediaConstraintsInterface::kNoiseSuppression,
                googNoiseSuppression);
            self->SetOptional(
              webrtc::MediaConstraintsInterface::kExperimentalNoiseSuppression,
                googNoiseSuppression2);
            self->SetOptional(
              webrtc::MediaConstraintsInterface::kHighpassFilter,
                googHighpassFilter);
            self->SetOptional(
              webrtc::MediaConstraintsInterface::kTypingNoiseDetection,
                googTypingNoiseDetection);
            self->SetOptional(
              webrtc::MediaConstraintsInterface::kAudioMirroring,
              googAudioMirroring);

            if(!sourceId.IsEmpty() && sourceId->IsString()) {
              v8::String::Utf8Value sourceId_str(sourceId->ToString());
              self->audio_id_ = *sourceId_str;
            }
          }
        }
      }

      self->audio_ = true;
    }
  }

  v8::Local<v8::Value> video_value = constraints->Get(Nan::New("video")
    .ToLocalChecked());

  if(!video_value.IsEmpty()) {
    if(video_value->IsTrue() || video_value->IsFalse()) {
      self->video_ = true;
    } else if(video_value->IsObject()) {
      v8::Local<v8::Object> video = v8::Local<v8::Object>::Cast(audio_value);
      optional_value = video->Get(Nan::New("optional").ToLocalChecked());

      if(!optional_value.IsEmpty() && optional_value->IsArray()) {
        v8::Local<v8::Array> options =
          v8::Local<v8::Array>::Cast(optional_value);

        for (unsigned int index = 0; index < options->Length(); index++) {
          v8::Local<v8::Value> option_value = options->Get(index);
          if(!option_value.IsEmpty() && option_value->IsObject()) {
            v8::Local<v8::Object> option =
              v8::Local<v8::Object>::Cast(option_value);
            v8::Local<v8::Value> minAspectRatio =
              option->Get(Nan::New("minAspectRatio").ToLocalChecked());
            v8::Local<v8::Value> maxAspectRatio =
              option->Get(Nan::New("maxAspectRatio").ToLocalChecked());
            v8::Local<v8::Value> maxWidth =
              option->Get(Nan::New("maxWidth").ToLocalChecked());
            v8::Local<v8::Value> minWidth =
              option->Get(Nan::New("minWidth").ToLocalChecked());
            v8::Local<v8::Value> maxHeight =
              option->Get(Nan::New("maxHeight").ToLocalChecked());
            v8::Local<v8::Value> minHeight =
              option->Get(Nan::New("minHeight").ToLocalChecked());
            v8::Local<v8::Value> maxFrameRate =
              option->Get(Nan::New("maxFrameRate").ToLocalChecked());
            v8::Local<v8::Value> minFrameRate =
              option->Get(Nan::New("minFrameRate").ToLocalChecked());
            v8::Local<v8::Value> sourceId =
              option->Get(Nan::New("sourceId").ToLocalChecked());

            self->SetOptional(
              webrtc::MediaConstraintsInterface::kMinAspectRatio,
                minAspectRatio);
            self->SetOptional(
              webrtc::MediaConstraintsInterface::kMaxAspectRatio,
                maxAspectRatio);
            self->SetOptional(webrtc::MediaConstraintsInterface::kMaxWidth,
              maxWidth);
            self->SetOptional(webrtc::MediaConstraintsInterface::kMinWidth,
              minWidth);
            self->SetOptional(webrtc::MediaConstraintsInterface::kMaxHeight,
              maxHeight);
            self->SetOptional(webrtc::MediaConstraintsInterface::kMinHeight,
              minHeight);
            self->SetOptional(webrtc::MediaConstraintsInterface::kMaxFrameRate,
              maxFrameRate);
            self->SetOptional(webrtc::MediaConstraintsInterface::kMinFrameRate,
              minFrameRate);

            if(!sourceId.IsEmpty() && sourceId->IsString()) {
              v8::String::Utf8Value sourceId_str(sourceId->ToString());
              self->video_id_ = *sourceId_str;
            }
          }
        }
      }
      self->video_ = true;
    }
  }
  return self;
}

rtc::scoped_refptr<MediaConstraints> MediaConstraints::New(
    const v8::Local<v8::Value> &constraints) {
  Nan::HandleScope scope;
  if(!constraints.IsEmpty() && constraints->IsObject()) {
    v8::Local<v8::Object> obj = v8::Local<v8::Object>::Cast(constraints);
    return MediaConstraints::New(obj);
  }
  return MediaConstraints::New();
}


void MediaConstraints::SetOptional(std::string key,
    v8::Local<v8::Value> value) {
  if(!value.IsEmpty() && !value->IsNull() && !value->IsUndefined()) {
    if(value->IsTrue() || value->IsFalse()) {
      MediaConstraints::SetOptional(key, value->IsTrue() ?
        std::string(webrtc::MediaConstraintsInterface::kValueTrue) :
        std::string(webrtc::MediaConstraintsInterface::kValueFalse));
    } else if(value->IsNumber()) {
      MediaConstraints::SetOptional(key, value->NumberValue());
    } else if(value->IsInt32()) {
      MediaConstraints::SetOptional(key, value->Int32Value());
    } else if(value->IsUint32()) {
      MediaConstraints::SetOptional(key, value->Uint32Value());
    } else {
      Nan::ThrowError("Unknown MediaConstraints Type");
    }
  }
}

void MediaConstraints::SetMandatory(std::string key,
    v8::Local<v8::Value> value) {
  if(!value.IsEmpty() && !value->IsNull() && !value->IsUndefined()) {
    if(value->IsTrue() || value->IsFalse()) {
      MediaConstraints::SetMandatory(key, value->IsTrue() ?
        std::string(webrtc::MediaConstraintsInterface::kValueTrue) :
        std::string(webrtc::MediaConstraintsInterface::kValueFalse));
    } else if(value->IsNumber()) {
      MediaConstraints::SetMandatory(key, value->NumberValue());
    } else if(value->IsInt32()) {
      MediaConstraints::SetMandatory(key, value->Int32Value());
    } else if(value->IsUint32()) {
      MediaConstraints::SetMandatory(key, value->Uint32Value());
    } else {
      Nan::ThrowError("Unknown MediaConstraints Type");
    }
  }
}

bool MediaConstraints::IsMandatory(const std::string& key) {
  std::string value;
  if(mandatory_.FindFirst(key, &value)) {
    return true;
  }
  return false;
}

bool MediaConstraints::GetMandatory(const std::string& key) {
  std::string value;
  if(mandatory_.FindFirst(key, &value)) {
    if(!value.compare("true")) {
      return true;
    }
  }
  return false;
}

void MediaConstraints::RemoveMandatory(const std::string& key) {
  std::string value;
  if(mandatory_.FindFirst(key, &value)) {
    for(webrtc::MediaConstraintsInterface::Constraints::iterator
        iter = mandatory_.begin(); iter != mandatory_.end(); ++iter) {
      if(iter->key == key) {
        mandatory_.erase(iter);
        break;
      }
    }
  }
}

void MediaConstraints::AddMandatory(const std::string& key,
    const std::string& value) {
  mandatory_.push_back(webrtc::MediaConstraintsInterface::Constraint(key,
    value));
}

void MediaConstraints::SetMandatory(const std::string &key,
    const std::string &value) {
  MediaConstraints::RemoveMandatory(key);
  MediaConstraints::AddMandatory(key, value);
}

bool MediaConstraints::IsOptional(const std::string& key) {
  std::string value;
  if(mandatory_.FindFirst(key, &value)) {
    return true;
  }
  return false;
}

bool MediaConstraints::GetOptional(const std::string& key) {
  std::string value;
  if(optional_.FindFirst(key, &value)) {
    if(!value.compare("true")) {
      return true;
    }
  }
  return false;
}

void MediaConstraints::RemoveOptional(const std::string& key) {
  std::string value;
  if(optional_.FindFirst(key, &value)) {
    for(webrtc::MediaConstraintsInterface::Constraints::iterator
        iter = optional_.begin(); iter != optional_.end(); ++iter) {
      if(iter->key == key) {
        optional_.erase(iter);
        break;
      }
    }
  }
}

void MediaConstraints::AddOptional(const std::string& key,
    const std::string& value) {
  optional_.push_back(webrtc::MediaConstraintsInterface::Constraint(key,
    value));
}

void MediaConstraints::SetOptional(const std::string& key,
    const std::string& value) {
  MediaConstraints::RemoveOptional(key);
  MediaConstraints::AddOptional(key, value);
}

bool MediaConstraints::UseAudio() const {
  return audio_;
}

bool MediaConstraints::UseVideo() const {
  return video_;
}

std::string MediaConstraints::AudioId() const {
  return audio_id_;
}

std::string MediaConstraints::VideoId() const {
  return video_id_;
}

const webrtc::MediaConstraintsInterface*
    MediaConstraints::ToConstraints() const {
  return this;
}

const webrtc::MediaConstraintsInterface::Constraints&
    MediaConstraints::GetMandatory() const {
  return mandatory_;
}

const webrtc::MediaConstraintsInterface::Constraints&
    MediaConstraints::GetOptional() const {
  return optional_;
}