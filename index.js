'use strict';
var webrtcjs = require('./build/Debug/webrtcjs.node');

console.dir(webrtcjs, {depth: null});

webrtcjs.MediaDevices.enumerateDevices(function(devices) {
  console.log(devices);
});

// var alice = new webrtcjs.PeerConnection();
// var bob = new webrtcjs.PeerConnection();

// alice.onicecandidate = function(event) {
//   var candidate = event.candidate || event;
//   bob.addIceCandidate(candidate);
// };

// bob.onicecandidate = function(event) {
//   var candidate = event.candidate || event;
//   alice.addIceCandidate(candidate);
// };

// // setInterval(function() {
// //   alice.getStats(function(stat) {
// //     console.log('Alice', stat);
// //   });
// // }, 1000);

// alice.createOffer(function(aliceSdp) {
//   alice.setLocalDescription(aliceSdp, function() {
//     bob.setRemoteDescription(aliceSdp, function() {
//       bob.createAnswer(function(bobSdp) {
//         bob.setLocalDescription(bobSdp, function() {
//           alice.setRemoteDescription(bobSdp, function() {
//           });
//         });
//       });
//     });
//   });
// });
