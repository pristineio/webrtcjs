#ifndef WEBRTCJS_RECORDING_DECODER_H
#define WEBRTCJS_RECORDING_DECODER_H

#include "webrtc/video_encoder.h"
#include "webrtc/video_decoder.h"
#include "webrtc/modules/video_coding/include/video_error_codes.h"
#include "webrtc/media/engine/webrtcvideodecoderfactory.h"

#include "eventemitter.h"

class RecordingDecoderFactory
  : public cricket::WebRtcVideoDecoderFactory,
    public EventEmitter {
 public:
  RecordingDecoderFactory(EventEmitter* listener=nullptr);
  virtual ~RecordingDecoderFactory() { }
  webrtc::VideoDecoder* CreateVideoDecoder(webrtc::VideoCodecType type)
    override;
  void DestroyVideoDecoder(webrtc::VideoDecoder* decoder) override;
  virtual void On(Event* event) final;
};

class RecordingDecoder : public webrtc::VideoDecoder {
 public:
  RecordingDecoder(webrtc::VideoCodecType type);
  int32_t InitDecode(const webrtc::VideoCodec* codecSettings, int32_t numberOfCores)
    override;

  int32_t Decode(const webrtc::EncodedImage& inputImage, bool missingFrames,
    const webrtc::RTPFragmentationHeader* fragmentation,
    const webrtc::CodecSpecificInfo* codecSpecificInfo = NULL,
    int64_t renderTimeMs = -1) override;

  int32_t RegisterDecodeCompleteCallback(webrtc::DecodedImageCallback* callback)
    override;

  int32_t Release() override;
  int32_t Reset();
  int32_t SetLinkedEncoder(webrtc::VideoEncoder* encoder);
  static int32_t decoder_counter;

 private:
  webrtc::VideoDecoder* decoder_;
  webrtc::VideoEncoder* linked_encoder_;
  bool request_intra_;
  int32_t frame_counter_;
  FILE* file_;
};

#endif