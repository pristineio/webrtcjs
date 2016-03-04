'use strict';

var localVideo = document.getElementById('local-video');
var remoteVideo = document.getElementById('remote-video');
var callButton = document.getElementById('callButton');
var hangupButton = document.getElementById('hangupButton');
callButton.disabled = true;
hangupButton.disabled = true;
callButton.onclick = call;
hangupButton.onclick = hangup;

var ws;
var request = null;
var localStream;
var remoteStream;
var localName;
var server;
var myId = -1;
var otherPeers = {};
var peer;
var pc;
var pcConfig = {iceServers: [{url: 'stun:stun.l.google.com:19302'}]};
var pcOptions = {
  mandatory: {
    OfferToReceiveAudio: false,
    OfferToReceiveVideo: true
  }
};

var regexs = {
  alice: /alice/,
  bob: /bob/,
  charlie: /user@ubuntu/
};

function onError(msg) {
  console.error(msg);
}

function trace(text) {
  if(text[text.length - 1] === '\n') {
    text = text.substring(0, text.length - 1);
  }
  if(window.performance) {
    var now = (window.performance.now() / 1000).toFixed(3);
    console.log(now + ': ' + text);
  } else {
    console.log(text);
  }
}

function start() {
  navigator.mediaDevices.getUserMedia({
    audio: false,
    video: {
      mandatory: {
        maxWidth: 640,
        maxHeight: 480
      },
      optional: []
    }
  }).then(function(stream) {
    localVideo.srcObject = stream;
    localStream = stream;
    callButton.disabled = false;
  });
}

function call() {
  callButton.disabled = true;
  hangupButton.disabled = false;

  pc = new RTCPeerConnection(pcConfig, pcOptions);

  pc.onaddstream = function(event) {
    remoteVideo.srcObject = event.stream;
    remoteVideo.play();
  };

  pc.onicecandidate = function(event) {
    if(!event.candidate) return;
    ws.send(JSON.stringify({to: peer, type: 'ICE',
      payload: JSON.stringify(event.candidate)}));
  };

  pc.addStream(localStream);
  pc.createOffer(function(desc) {
    pc.setLocalDescription(desc);
    ws.send(JSON.stringify({to: peer, type: 'OFFER', payload: desc}));
  }, onError);
}

function hangup() {
  pc.close();
  pc = null;
  hangupButton.disabled = true;
  callButton.disabled = false;
  ws.send(JSON.stringify({to: peer, type: 'HANGUP'}));
}

function handleMessage(event) {
  var message = JSON.parse(event.data);
  switch(message.type) {
    case 'ANSWER':
      pc.setRemoteDescription(new RTCSessionDescription(message.payload),
        function(){}, onError);
      break;
    case 'ICE':
      pc.addIceCandidate(new RTCIceCandidate(message.payload));
      break;
  }
}

function connect(name) {
  if(!name) {
    localName = document.getElementById('local').value.toLowerCase();
  } else {
    localName = name;
    document.getElementById('local').value = name;
  }
  peer = 'alice' === name ? 'bob' : 'alice';
  server = document.getElementById('server').value.toLowerCase();
  if(localName.length === 0) {
    alert('I need a name please.');
    document.getElementById('local').focus();
  } else {
    document.getElementById('connect').disabled = true;
    document.getElementById('disconnect').disabled = false;
    ws = new WebSocket(server + '?token=' + localName);
    ws.onmessage = handleMessage;
  }
}

function disconnect() {
  ws.close();
  otherPeers = {};
  document.getElementById('connect').disabled = false;
  document.getElementById('disconnect').disabled = true;
}

window.onbeforeunload = disconnect;

