#include "webrtcjs.h"

rtc::scoped_ptr<rtc::Thread> signaling_thread_;
rtc::scoped_ptr<rtc::Thread> worker_thread_;

// rtc::scoped_ptr<cricket::WebRtcVideoDecoderFactory> decoder_factory_;

rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pc_factory_;

void WebRtcJs::Init() {
  RTC_CHECK(rtc::InitializeSSL()) << "Failed to InitializeSSL()";

  worker_thread_.reset(new rtc::Thread());
  worker_thread_->SetName("WebRTC Worker", NULL);
  worker_thread_->Start();

  signaling_thread_.reset(new rtc::Thread());
  signaling_thread_->SetName("WebRTC Signaling", NULL);
  signaling_thread_->Start();

  pc_factory_ = webrtc::CreatePeerConnectionFactory(signaling_thread_.get(),
    worker_thread_.get(), nullptr, nullptr, nullptr);
}

webrtc::PeerConnectionFactoryInterface* WebRtcJs::GetPeerConnectionFactory() {
  return pc_factory_.get();
}

