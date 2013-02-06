var app = require('http').createServer(handler), 
    io = require('/usr/local/lib/node_modules/socket.io').listen(app), 
    fs = require('fs'),
    serialport = require("/usr/local/lib/node_modules/serialport"),
    SerialPort = serialport.SerialPort,
    serial = new SerialPort("/dev/ttyUSB0", {
        baudrate: 115200,
        parser: serialport.parsers.readline("\n") 
    });

app.listen(8080);
console.log("Listening on http://raspberrypi:8080...");

serial.write("hellz!")

// directs page requests to html files

function handler (req, res) {
  fs.readFile(__dirname + '/index.html',
  function (err, data) {
    if (err) {
      res.writeHead(500);
      return res.end('Error loading index.html');
    }
    res.writeHead(200);
    res.end(data);
  });
}

// this handles socket.io comm from html files

io.sockets.on('connection', function(socket) {
    socket.send('connected...');

    socket.on('message', function(data) {
        console.log("from client: " + data);
        serial.write(data + "\n");

        if (data == 'turn on') {
            console.log('+');
            
            socket.broadcast.send("let there be light!");
        }
        if (data == 'turn off') {
            console.log('-');
            
            socket.broadcast.send("who turned out the light?");
        }
        return;
    });

    socket.on('disconnect', function() {
        socket.send('disconnected...');
    });
});

serial.on("data", function (data) {
    console.log("from arduino: "+data);
});
