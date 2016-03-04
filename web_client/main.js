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
var startTime;
var request = null;
var localStream;
var remoteStream;
var localName;
var server;
var myId = -1;
var otherPeers = {};
var peer;
var pc;
var pcConfig = {'iceServers': [{'url': 'stun:stun.l.google.com:19302'}]};
var pcOptions = {
  // optional: [{DtlsSrtpKeyAgreement: true}],
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
  trace('Requesting local stream');
  navigator.mediaDevices.getUserMedia({
    audio: false,
    video: {
      mandatory: {
        maxWidth: 1280,
        maxHeight: 720,
        minAspectRatio: 1,
        minFrameRate: 3,
        maxFrameRate: 30
      },
      optional: []
    }
  })
  .then(function(stream) {
    trace('Received local stream');
    localVideo.srcObject = stream;
    localStream = stream;
    callButton.disabled = false;
  });
}

function call() {
  callButton.disabled = true;
  hangupButton.disabled = false;
  trace('Starting call');
  startTime = window.performance.now();
  var videoTracks = localStream.getVideoTracks();
  var audioTracks = localStream.getAudioTracks();
  if(videoTracks.length > 0) {
    trace('Using video device: ' + videoTracks[0].label);
  }
  if(audioTracks.length > 0) {
    trace('Using audio device: ' + audioTracks[0].label);
  }
  pc = new RTCPeerConnection(); // pcConfig, pcOptions);
  trace('Created local peer connection object pc');
  pc.onaddstream = function() {
    console.log('Remote stream added:', URL.createObjectURL(event.stream));
    remoteVideo.src = URL.createObjectURL(event.stream);
    remoteVideo.play();
  };
  pc.onicecandidate = function(event) {
    if(!event.candidate) {
      return;
    }
    ws.send(JSON.stringify({
      to: peer,
      type: 'ICE',
      payload: JSON.stringify(event.candidate)
    }));
    trace(event.candidate.candidate);
  };
  pc.addStream(localStream);
  trace('Added local stream to pc');
  console.dir(videoTracks[0]);
  trace('pc createOffer start');
  pc.createOffer(
    function onCreateOfferSuccess(desc) {
      console.log('Offer\n' + desc.sdp);
      pc.setLocalDescription(desc);
      ws.send(JSON.stringify({
        to: peer,
        type: 'OFFER',
        payload: desc
      }));
    },
    onError,
    {mandatory: {
      OfferToReceiveAudio: false,
      OfferToReceiveVideo: true
    }}
  );
}

function hangup() {
  trace('Ending call');
  pc.close();
  pc = null;
  hangupButton.disabled = true;
  callButton.disabled = false;
  ws.send(JSON.stringify({
    to: peer,
    type: 'HANGUP'
  }));
}




// function handlePeerMessage(peer_id, data) {

//   if(data === 'BYE') {
//     hangup();
//     return;
//   }

//   var dataJson = JSON.parse(data);

//   console.log('Message from:', peer_id, ':', dataJson);

//   if('type' in dataJson) {
//     if (dataJson.type === 'offer') {
//       pc = new RTCPeerConnection(pcConfig, pcOptions);
//       pc.addStream(localStream);
//       pc.onaddstream = function(e) {
//         remoteVideo.srcObject = e.stream;
//       };
//       pc.onicecandidate = function(e) {
//         onIceCandidate(pc, e);
//       };
//       pc.oniceconnectionstatechange = function(e) {
//         onIceStateChange(pc, e);
//       };
//       pc.setRemoteDescription(new RTCSessionDescription(dataJson));
//       pc.createAnswer(
//         function(sessionDescription) {
//           console.log('Create answer:', sessionDescription.sdp);
//           pc.setLocalDescription(sessionDescription);
//           ws.send(JSON.stringify({
//             to: peer,
//             type: 'ANSWER',
//             payload: JSON.stringify(sessionDescription)
//           }));
//         },
//         function(error) {
//           console.log('Create answer error:', error);
//         },
//         {mandatory: {
//           OfferToReceiveAudio: false,
//           OfferToReceiveVideo: true
//         }}
//       );
//     } else if(dataJson.type === '') {
//       if(pc.remoteDescription.sdp !== dataJson.sdp) {
//         pc.createOffer(
//           function(desc) {
//             console.log('Renegotiation Offer\n' + desc.sdp);
//             pc.setLocalDescription(desc, function() {
//               trace('setLocalDescription complete');
//             }, onSetSessionDescriptionError);
//           },
//           null,
//           {mandatory: {
//             OfferToReceiveAudio: false,
//             OfferToReceiveVideo: true
//           }}
//         );
//       }
//       pc.setRemoteDescription(new RTCSessionDescription(dataJson));
//     }
//   } else if('candidate' in dataJson) {
//     pc.addIceCandidate(new RTCIceCandidate(dataJson));
//   }
// }

function signIn() {
  ws = new WebSocket(server + '?token=' + localName);
  ws.onopen = function() {
    console.log(localName + ' connected');
  };
  ws.onmessage = function(event) {
    var message = JSON.parse(event.data);
    switch(message.type) {
      case 'ANSWER':
        pc.setRemoteDescription(new RTCSessionDescription(message.payload));
        break;
    }
  };
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
    signIn();
  }
}

function disconnect() {
  ws.close();
  otherPeers = {};
  document.getElementById('connect').disabled = false;
  document.getElementById('disconnect').disabled = true;
}

window.onbeforeunload = disconnect;

