#include "webrtcjs.h"

rtc::scoped_ptr<cricket::DeviceManagerInterface> device_manager_;

rtc::scoped_ptr<rtc::Thread> signaling_thread_;
rtc::scoped_ptr<rtc::Thread> worker_thread_;

// rtc::scoped_ptr<cricket::WebRtcVideoDecoderFactory> decoder_factory_;
rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> pc_factory_;

void WebRtcJs::Init() {
  RTC_CHECK(rtc::InitializeSSL()) << "Failed to InitializeSSL()";

  device_manager_.reset(cricket::DeviceManagerFactory::Create());

  worker_thread_.reset(new rtc::Thread());
  worker_thread_->Start();

  signaling_thread_.reset(new rtc::Thread());
  signaling_thread_->Start();

  // pc_factory_ = webrtc::CreatePeerConnectionFactory(signaling_thread_.get(),
  //   worker_thread_.get(), nullptr, nullptr, nullptr);
}

cricket::DeviceManagerInterface* WebRtcJs::GetDeviceManager() {
  return device_manager_.get();
}
