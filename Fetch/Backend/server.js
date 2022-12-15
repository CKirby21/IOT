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
  const dataObject = JSON.parse(data);
  const q = `INSERT INTO location (latitude, longitude) VALUES (${dataObject.IotData.latitude}, ${dataObject.IotData.longitude})`;
  connection = db.getConnection( (err, connection)=> {
    if (err) throw (err);
    console.log ("DB connected successful: " + connection.threadId)
    connection.query(q, async(err, result) => {
      if (err) throw err;
      console.log("Location inserted");
    });
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
    // Check if message has email and password
    if (dataObject.email && dataObject.password) {

      // Search for email
      connection = db.getConnection( (err, connection)=> {
        if (err) throw (err);
        console.log ("DB connected successful: " + connection.threadId)
        connection.query('SELECT * FROM users WHERE email = \'example@gmail.com\' AND password = \'password\'', async(err, result) => {
          if (err) throw err;
          console.log(result);
        });
      });

      // TODO move this inside connection. Send if email was found
      ws.send("email: true");
    }
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