var webrtcjs = require('./build/Debug/webrtcjs.node');
console.dir(webrtcjs, {depth: null});

var pc = new webrtcjs.PeerConnection();
console.dir(pc);
