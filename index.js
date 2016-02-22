var webrtcjs = require('./build/Debug/webrtcjs.node');

var alice = new webrtcjs.PeerConnection();
var bob = new webrtcjs.PeerConnection();

alice.onicecandidate = function(event) {
  var candidate = event.candidate || event;
  bob.addIceCandidate(candidate);
};

bob.onicecandidate = function(event) {
  var candidate = event.candidate || event;
  alice.addIceCandidate(candidate);
};

// var sdp = JSON.parse('{"sdp":"v=0\r\no=- 4857849893176363072 2 IN IP4 127.0.0.1\r\ns=-\r\nt=0 0\r\na=group:BUNDLE audio\r\na=msid-semantic: WMS stream_1\r\nm=audio 9 UDP/TLS/RTP/SAVPF 111 103 104 9 102 0 8 106 105 13 127 126\r\nc=IN IP4 0.0.0.0\r\na=rtcp:9 IN IP4 0.0.0.0\r\na=ice-ufrag:xO+prAQgW9yxYi3o\r\na=ice-pwd:BVB/pjXkrGfY9FzqXOrNUfbe\r\na=fingerprint:sha-256 6B:72:CB:07:12:4B:86:30:7F:AD:EA:69:E4:D8:E8:2F:06:2A:1F:55:DE:83:CD:BA:E1:41:D1:1E:B5:E3:2A:77\r\na=setup:actpass\r\na=mid:audio\r\na=extmap:1 urn:ietf:params:rtp-hdrext:ssrc-audio-level\r\na=extmap:3 http://www.webrtc.org/experiments/rtp-hdrext/abs-send-time\r\na=sendrecv\r\na=rtcp-mux\r\na=rtpmap:111 opus/48000/2\r\na=rtcp-fb:111 transport-cc\r\na=fmtp:111 minptime=10; useinbandfec=1\r\na=rtpmap:103 ISAC/16000\r\na=rtpmap:104 ISAC/32000\r\na=rtpmap:9 G722/8000\r\na=rtpmap:102 ILBC/8000\r\na=rtpmap:0 PCMU/8000\r\na=rtpmap:8 PCMA/8000\r\na=rtpmap:106 CN/32000\r\na=rtpmap:105 CN/16000\r\na=rtpmap:13 CN/8000\r\na=rtpmap:127 red/8000\r\na=rtpmap:126 telephone-event/8000\r\na=maxptime:60\r\na=ssrc:3914160127 cname:IyO0VPpl/oKrCOH5\r\na=ssrc:3914160127 msid:stream_1 audio_1\r\na=ssrc:3914160127 mslabel:stream_1\r\na=ssrc:3914160127 label:audio_1\r\n","type":"offer"}');

// alice.onnegotiationneeded = function() {
//   console.log('\n    Alice: onnegotiationneeded\n');
// };

alice.createOffer(function(sdp) {
  alice.setLocalDescription(sdp, function() {
    bob.setRemoteDescription(sdp, function() {
      bob.createAnswer(function(sdp) {
        bob.setLocalDescription(sdp, function() {
           alice.setRemoteDescription(sdp, function() {

           });
        });
      });
    });
  });
});
