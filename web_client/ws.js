'use strict';

var WebSocket = require('wsd');
process.env.NODE_TLS_REJECT_UNAUTHORIZED = '0';

var ws = new WebSocket('wss://localhost:3232/ws?token=bob');

ws.on('open', function() {
  ws.send(JSON.stringify({
    to: 'alice',
    type: 'OFFER',
    payload: '123'
  }));
});

