const express = require('express');
const http = require('http');
const WebSocket = require('ws');
const path = require('path');
const EventHubReader = require('./scripts/event-hub-reader.js');
const mysql = require('mysql');

const iotHubConnectionString = process.env.IotHubConnectionString;
if (!iotHubConnectionString) {
  console.error(`Environment variable IotHubConnectionString must be specified.`);
  return;
}
console.log(`Using IoT Hub connection string [${iotHubConnectionString}]`);

const eventHubConsumerGroup = process.env.EventHubConsumerGroup;
console.log(eventHubConsumerGroup);
if (!eventHubConsumerGroup) {
  console.error(`Environment variable EventHubConsumerGroup must be specified.`);
  return;
}
console.log(`Using event hub consumer group [${eventHubConsumerGroup}]`);

// Redirect requests to the public subdirectory to the root
const app = express();
app.use(express.static(path.join(__dirname, 'public')));
app.use((req, res /* , next */) => {
  res.redirect('/');
});

const server = http.createServer(app);
const wss = new WebSocket.Server({ server });

wss.broadcast = (data) => {
  wss.clients.forEach((client) => {
    if (client.readyState === WebSocket.OPEN) {
      try {
        console.log(`Broadcasting data ${data}`);
        client.send(data);
      } catch (e) {
        console.error(e);
      }
    }
  });
};

// Setup database
const db = mysql.createPool({
  host: '127.0.0.1',
  user: 'newuser',
  password: 'P@ssword!',
  database: 'db',
  port: '3306'
})

wss.on('connection', function connection(ws) {
  ws.on('message', function message(data) {

    const dataObject = JSON.parse(data);
    console.log(dataObject);

    connection = db.getConnection( (err, connection)=> {
      if (dataObject.email && dataObject.password) {
        var q = 'SELECT * FROM user WHERE email = \'example@gmail.com\' AND password = \'password\'';
        connection.query(q, async(err, result) => {
          if (err) throw err;
          else ws.send("email: true");
        });
      }

      if (dataObject.radius) {
        var q = `UPDATE user SET radius = ${dataObject.radius}`;
        connection.query(q, async(err, result) => {
          if (err) throw err;
        });
      }

      if (dataObject.latitude && dataObject.longitude) {
        var q = `UPDATE user SET latitude = ${dataObject.latitude}, longitude = ${dataObject.longitude}`;
        connection.query(q, async(err, result) => {
          if (err) throw err;
        });
      }

      var v = `SELECT * FROM user`;
      connection.query(v, async(err, result) => {
        if (err) throw err;
        else {
          const t = {"latitude": `${result[0].latitude}`, "longitude": `${result[0].longitude}`, "radius": `${result[0].radius}`};
          const json = JSON.stringify(t);
          ws.send(json);
        }
      });
    });
  }); 
});

server.listen(process.env.PORT || '3000', () => {
  console.log('Listening on %d.', server.address().port);
});

const eventHubReader = new EventHubReader(iotHubConnectionString, eventHubConsumerGroup);

(async () => {
  await eventHubReader.startReadMessage((message, date, deviceId) => {
    try {
      const payload = {
        IotData: message,
        MessageDate: date || Date.now().toISOString(),
        DeviceId: deviceId,
      };

      wss.broadcast(JSON.stringify(payload));
    } catch (err) {
      console.error('Error broadcasting: [%s] from [%s].', err, message);
    }
  });
})().catch();