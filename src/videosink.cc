#include "videosink.h"

Nan::Persistent<v8::Function> VideoSink::constructor;

NAN_MODULE_INIT(VideoSink::Init) {
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("VideoSink").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetAccessor(tpl->InstanceTemplate(),
    Nan::New("onframe").ToLocalChecked(),
    VideoSink::GetOnFrame,
    VideoSink::SetOnFrame);

  constructor.Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(target, Nan::New("VideoSink").ToLocalChecked(),
    Nan::GetFunction(tpl).ToLocalChecked());
}

VideoSink::VideoSink() { }

VideoSink::~VideoSink() { }

void VideoSink::OnFrame(const cricket::VideoFrame& frame) {
  ++number_of_rendered_frames_;
  Emit(kVideoSinkOnFrame, number_of_rendered_frames_);
}

void VideoSink::On(Event* event) {
  EventType type = event->As<EventType>();
  if(type != kVideoSinkOnFrame) {
    return;
  }
  if(onframe_.IsEmpty()) {
    return;
  }
  int32_t n = event->Unwrap<int32_t>();
  Nan::HandleScope scope;
  v8::Local<v8::Value> argv[1];
  v8::Local<v8::Object> container = Nan::New<v8::Object>();
  container->Set(Nan::New("framesRendered").ToLocalChecked(),
    Nan::New<v8::Int32>(n));
  v8::Local<v8::Function> fn = Nan::New<v8::Function>(onframe_);
  argv[0] = container;
  Nan::Callback cb(fn);
  cb.Call(1, argv);
}

NAN_METHOD(VideoSink::New) {
  if(!info.IsConstructCall()) {
    return Nan::ThrowError("Use new operator");
  }
  VideoSink* self = new VideoSink();
  self->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

NAN_GETTER(VideoSink::GetOnFrame) {
  VideoSink* self = Nan::ObjectWrap::Unwrap<VideoSink>(info.Holder());
  return info.GetReturnValue().Set(Nan::New<v8::Function>(self->onframe_));
}

NAN_SETTER(VideoSink::SetOnFrame) {
  VideoSink* self = Nan::ObjectWrap::Unwrap<VideoSink>(info.Holder());
  self->onframe_.Reset();
  if(value.IsEmpty() || !value->IsFunction()) {
    return Nan::ThrowError("Callback is not a function");
  }
  self->onframe_.Reset<v8::Function>(v8::Local<v8::Function>::Cast(value));
}
