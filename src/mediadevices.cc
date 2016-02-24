#include "mediadevices.h"

Nan::Persistent<v8::Function> MediaDevices::constructor;

NAN_MODULE_INIT(MediaDevices::Init) {
  v8::Local<v8::FunctionTemplate> tpl =
    Nan::New<v8::FunctionTemplate>(MediaDevices::New);
  tpl->SetClassName(Nan::New("MediaDevices").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetMethod(tpl, "getUserMedia", MediaDevices::GetUserMedia);
  Nan::SetMethod(tpl, "enumerateDevices", MediaDevices::EnumerateDevices);

  constructor.Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(target, Nan::New<v8::String>("MediaDevices").ToLocalChecked(),
    Nan::GetFunction(tpl).ToLocalChecked());
}

NAN_METHOD(MediaDevices::New) {
  if(!info.IsConstructCall()) {
    return Nan::ThrowError("Use new operator");
  }
  MediaDevices* self = new MediaDevices();
  self->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

NAN_METHOD(MediaDevices::GetUserMedia) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
}

NAN_METHOD(MediaDevices::EnumerateDevices) {
  v8::Local<v8::Array> list = Nan::New<v8::Array>();
  uint32_t index = 0;

  cricket::DeviceManagerInterface* device_manager =
    WebRtcJs::GetDeviceManager();

  std::vector<cricket::Device> audio_devs;
  std::vector<cricket::Device> video_devs;

  if(!device_manager) {
    return Nan::ThrowError("Bad pointer to DeviceManager");
  }
  if(!device_manager->GetAudioInputDevices(&audio_devs)) {
    return Nan::ThrowError("Cannot get audio devices");
  }
  if(!device_manager->GetVideoCaptureDevices(&video_devs)) {
    return Nan::ThrowError("Cannot get video devices");
  }

  std::vector<cricket::Device>::iterator audio_it;
  for(audio_it = audio_devs.begin(); audio_it != audio_devs.end(); audio_it++) {
    const cricket::Device &dev = *audio_it;
    v8::Local<v8::Object> dev_obj = Nan::New<v8::Object>();
    dev_obj->Set(Nan::New("kind").ToLocalChecked(),
      Nan::New("audio").ToLocalChecked());
    dev_obj->Set(Nan::New("label").ToLocalChecked(),
      Nan::New(dev.name.c_str()).ToLocalChecked());
    dev_obj->Set(Nan::New("id").ToLocalChecked(),
      Nan::New(dev.id.c_str()).ToLocalChecked());
    list->Set(index, dev_obj);
    index++;
  }

  std::vector<cricket::Device>::iterator video_it = video_devs.begin();
  for(video_it = video_devs.begin(); video_it != video_devs.end(); video_it++) {
    const cricket::Device &dev = *video_it;
    v8::Local<v8::Object> dev_obj = Nan::New<v8::Object>();
    dev_obj->Set(Nan::New("kind").ToLocalChecked(),
      Nan::New("video").ToLocalChecked());
    dev_obj->Set(Nan::New("label").ToLocalChecked(),
      Nan::New(dev.name.c_str()).ToLocalChecked());
    dev_obj->Set(Nan::New("id").ToLocalChecked(),
      Nan::New(dev.id.c_str()).ToLocalChecked());
    list->Set(index, dev_obj);
    index++;
  }

  if(info.Length() == 1 && info[0]->IsFunction()) {
    v8::Local<v8::Function> callback = v8::Local<v8::Function>::Cast(info[0]);
    v8::Local<v8::Value> argv[1] = { list };
    callback->Call(info.This(), 1, argv);
  }

  info.GetReturnValue().SetUndefined();
}

