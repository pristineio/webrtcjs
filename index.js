var webrtcjs = require('./build/Debug/webrtcjs.node');

var pc = new webrtcjs.PeerConnection();

console.log('');

pc.createOffer(function(data) {
  console.dir({
    javascript: data
  });
}, function(err) {
  console.dir(err);
});

