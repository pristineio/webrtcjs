'use strict';
process.env.NODE_TLS_REJECT_UNAUTHORIZED = '0';
var WebSocket = require('wsd');
var webrtcjs = require('./build/Debug/webrtcjs.node');
var ws = new WebSocket('wss://localhost:3232/ws?token=bob');

var pcConfig = {iceServers: [{url: 'stun:stun.l.google.com:19302'}]};
var pcConstraints = {mandatory: {
  OfferToReceiveAudio: false,
  OfferToReceiveVideo: true
}};

// var aliceVideoTrack;
// var aliceVideoSink = new webrtcjs.VideoSink();
// aliceVideoSink.onframe = function(data) {
//   console.log('onframe', JSON.stringify(data));
// };

var bob = new webrtcjs.RTCPeerConnection(); //pcConfig, pcConstraints);

bob.onicecandidate = function(e) {
  ws.send(JSON.stringify({to: 'alice', type: 'ICE', payload: e.candidate}));
};

bob.onaddstream = function(aliceStream) {
  // aliceVideoTrack = aliceStream.getVideoTracks()[0];
  // aliceVideoTrack.addSink(aliceVideoSink);
  bob.addStream(aliceStream);
  bob.createAnswer(function(bobSdp) {
    console.log('    Bob: createAnswer ' + JSON.stringify(bobSdp));
    ws.send(JSON.stringify({to: 'alice', type: 'ANSWER', payload: bobSdp}));
    bob.setLocalDescription(bobSdp, function() {
      console.log('    Bob: setLocalDescription');
    });
  });
};


ws.on('message', function(data) {
  var message = JSON.parse(data);
  console.log(message);

  switch(message.type) {
    case 'OFFER':
      bob.setRemoteDescription(message.payload, function() {
        console.log('    Bob: setRemoteDescription');
      });
      break;

    case 'ICE':
      bob.addIceCandidate(JSON.parse(message.payload));
      break;

    case 'HANGUP':
      bob.close();
      break;
  }
});




// // setInterval(function() {
// //   alice.getStats(function(stat) {
// //     console.log('Alice', stat);
// //   });
// // }, 1000);

// alice.createOffer(function(aliceSdp) {
//   console.log('    Alice: createOffer ' + JSON.stringify(aliceSdp));
//   alice.setLocalDescription(aliceSdp, function() {
//     console.log('    Alice: setLocalDescription');
//     bob.setRemoteDescription(aliceSdp, function() {
//       console.log('    Bob: setRemoteDescription');
//       bob.createAnswer(function(bobSdp) {
//         console.log('    Bob: createAnswer ' + JSON.stringify(bobSdp));
//         bob.setLocalDescription(bobSdp, function() {
//           console.log('    Bob: setLocalDescription');
//           alice.setRemoteDescription(bobSdp, function() {
//             console.log('    Alice: setRemoteDescription');
//           });
//         });
//       });
//     });
//   });
// });
