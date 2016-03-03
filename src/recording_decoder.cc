
#include "webrtc/base/logging.h"
#include "recording_decoder.h"
#include "ivf_utils.h"

using webrtc::kVideoCodecVP8;

//
// RecordingDecoderFactory
//

RecordingDecoderFactory::RecordingDecoderFactory(EventEmitter* listener)
  : EventEmitter(listener) { }

webrtc::VideoDecoder* RecordingDecoderFactory::CreateVideoDecoder(
    webrtc::VideoCodecType type) {
  webrtc::VideoDecoder* decoder = new RecordingDecoder(type);
  Emit(kVideoDecoderCreated, decoder);
  return decoder;
}

void RecordingDecoderFactory::DestroyVideoDecoder(
    webrtc::VideoDecoder* decoder) {
  Emit(kVideoDecoderDestroyed, decoder);
  delete decoder;
}

void RecordingDecoderFactory::On(Event* event) {
  LOG(LS_INFO) << __FUNCTION__;
}

//
// RecordingDecoder
//
int32_t RecordingDecoder::decoder_counter = 0;

RecordingDecoder::RecordingDecoder(webrtc::VideoCodecType codec_type)
  : linked_encoder_(NULL),
    request_intra_(false) {
  LOG(LS_INFO) << __PRETTY_FUNCTION__;
  frame_counter_ = 0;
  if(codec_type != webrtc::kVideoCodecVP8) {
    LOG(LS_ERROR) << "Recording decoder only suports VP8 and not " <<
      codec_type;
    return;
  }
  decoder_ = webrtc::VideoDecoder::Create(webrtc::VideoDecoder::kVp8);
}

int32_t RecordingDecoder::InitDecode(const webrtc::VideoCodec* codec_settings,
    int32_t number_of_cores) {
  LOG(LS_INFO) << "\n\n" << __PRETTY_FUNCTION__ << "\n\n";

  char file_name[256];
  frame_counter_ = 0;
  sprintf(file_name, "test_record%04d.ivf", decoder_counter);
  decoder_counter++;
  if(!(file_ = fopen(file_name, "wb"))) {
    LOG(LS_ERROR) << "Failed opening file " << file_name;
  } else {
    write_ivf_file_header(file_, 320, 240, 1000); // these resolution values make no difference
  }
  return decoder_->InitDecode(codec_settings, number_of_cores);
}

int32_t RecordingDecoder::Decode(const webrtc::EncodedImage& input_image,
    bool missing_frames, const webrtc::RTPFragmentationHeader* fragmentation,
    const webrtc::CodecSpecificInfo* codec_specific_info,
    int64_t render_time_ms) {
  LOG(LS_INFO) << "\n\n" << __PRETTY_FUNCTION__ << "\n\n";

  int32_t status = WEBRTC_VIDEO_CODEC_OK;
  frame_counter_++;

  if(file_) {
    write_ivf_frame_header(file_, input_image._timeStamp, input_image._length);
    fwrite(input_image._buffer, sizeof(char), input_image._length, file_);
  }

  // if(linked_encoder_) {
  //   status = linked_encoder_->BypassEncode(input_image, codec_specific_info, fragmentation);
  //   if(request_intra_) {
  //     if(input_image._frameType != webrtc::kVideoFrameKey) {
  //       status = WEBRTC_VIDEO_CODEC_ERROR;
  //     } else {
  //       request_intra_ = false;
  //     }
  //   }
  // }

  // decoder_->Decode(input_image, missing_frames, fragmentation,
  //   codec_specific_info, render_time_ms);

  return status;
}

int32_t RecordingDecoder::RegisterDecodeCompleteCallback(
    webrtc::DecodedImageCallback* callback) {
  return decoder_->RegisterDecodeCompleteCallback(callback);
}

int32_t RecordingDecoder::Release() {
  LOG(LS_INFO) << "Closing decoder, decoded " << frame_counter_ << " frames";
  if(file_) {
    fclose(file_);
  }
  return decoder_->Release();
}

int32_t RecordingDecoder::Reset() {
  return decoder_->Reset();
}

int32_t RecordingDecoder::SetLinkedEncoder(webrtc::VideoEncoder* encoder) {
  linked_encoder_ = encoder;
  request_intra_ = true; // request intra from peer when relinking encoder
  return WEBRTC_VIDEO_CODEC_OK;
}