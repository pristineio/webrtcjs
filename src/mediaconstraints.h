#ifndef WEBRTCJS_MEDIACONSTRAINTS_H
#define WEBRTCJS_MEDIACONSTRAINTS_H

#include <nan.h>

#include "webrtc/base/scoped_ptr.h"
#include "webrtc/base/scoped_ref_ptr.h"
#include "webrtc/base/refcount.h"
#include "webrtc/base/stringencode.h"
#include "webrtc/api/mediaconstraintsinterface.h"


class MediaConstraints
  : public webrtc::MediaConstraintsInterface,
    public rtc::RefCountInterface {
 public:
  friend class rtc::RefCountedObject<MediaConstraints>;

  static rtc::scoped_refptr<MediaConstraints> New();
  static rtc::scoped_refptr<MediaConstraints> New(
    const v8::Local<v8::Object>& constraints);
  static rtc::scoped_refptr<MediaConstraints> New(
    const v8::Local<v8::Value>& constraints);

  bool UseAudio() const;
  bool UseVideo() const;

  std::string AudioId() const;
  std::string VideoId() const;

  bool IsMandatory(const std::string& key);
  bool GetMandatory(const std::string& key);
  void RemoveMandatory(const std::string& key);
  void AddMandatory(const std::string& key, const std::string& value);
  void SetMandatory(const std::string& key, const std::string& value);

  template <class T> void SetMandatory(const std::string& key, const T& value) {
    SetMandatory(key, rtc::ToString<T>(value));
  }

  bool IsOptional(const std::string& key);
  bool GetOptional(const std::string& key);
  void RemoveOptional(const std::string& key);
  void AddOptional(const std::string& key, const std::string& value);
  void SetOptional(const std::string& key, const std::string& value);

  template <class T> void SetOptional(const std::string& key, const T& value) {
    SetOptional(key, rtc::ToString<T>(value));
  }

  const webrtc::MediaConstraintsInterface* ToConstraints() const;
  const webrtc::MediaConstraintsInterface::Constraints& GetMandatory()
    const final;
  const webrtc::MediaConstraintsInterface::Constraints& GetOptional()
    const final;

 private:
  explicit MediaConstraints();
  ~MediaConstraints() override;

  void SetOptional(std::string key, v8::Local<v8::Value> value);
  void SetMandatory(std::string key, v8::Local<v8::Value> value);

 protected:
  bool audio_;
  bool video_;

  std::string audio_id_;
  std::string video_id_;

  webrtc::MediaConstraintsInterface::Constraints mandatory_;
  webrtc::MediaConstraintsInterface::Constraints optional_;
};

#endif