'use strict';
var express = require('express');
var WebSocketServer = require('wsd').Server;
var https = require('https');
var http = require('http');
var url = require('url');
var ServerResponse = require('http').ServerResponse;
var fs = require('fs');


var app = express();
app.use(express.static('./'));

var options = {
  key: fs.readFileSync('./ssl/key'),
  cert: fs.readFileSync('./ssl/crt')
};

app.server = http.createServer(app).listen(3232);

var wss = new WebSocketServer({server: app.server, path: '/ws'});

wss.on('headers', function(incomingHeaders) {
  var headers = {};
  incomingHeaders.forEach(function(x) {
    var kv = x.split(': ');
    headers[kv[0]] = kv[1];
  });
  incomingHeaders.push('x-wsd-client-key: ' +
    headers.authorization.split('bearer ')[1]);
});

wss.on('connection', function(ws) {
  ws.user = url.parse(ws.upgradeReq.url, true).query.token;

  console.log(ws.user + ' connected');

  var response = new ServerResponse(ws.upgradeReq);
  response.writeHead = function(statusCode) {
    if(statusCode > 200) {
      ws.close();
    }
  };

  app.handle(ws.upgradeReq, response, function(err) {
    ws.on('close', function(code) {
      console.log(ws.user + ' close');
    });
    ws.on('message', function(data) {
      var message = JSON.parse(data);
      var recipient = wss.clients[message.to];
      if(!recipient) {
        return console.log(message.to + ' not online');
      }
      recipient.send(data);
    });
  });
});

