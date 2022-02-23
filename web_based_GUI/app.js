require('localstorage-polyfill')
var createError = require('http-errors');
var express = require('express');
var path = require('path');
var cookieParser = require('cookie-parser');
var logger = require('morgan');
var sassMiddleware = require('node-sass-middleware');
const dgram = require('dgram');
require('dotenv').config();
const controller = require('./controller/readUDP');

localStorage = {}

var indexRouter = require('./routes/router');
// var usersRouter = require('./routes/users');

var app = express();

// view engine setup
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'ejs');

app.use(logger('dev'));
app.use(express.json());
app.use(express.urlencoded({ extended: false }));
app.use(cookieParser());
app.use(sassMiddleware({
  src: path.join(__dirname, 'public'),
  dest: path.join(__dirname, 'public'),
  indentedSyntax: false, // true = .sass and false = .scss
  sourceMap: true
}));
app.use(express.static(path.join(__dirname, 'public')));

app.use('/', indexRouter);


// Might be udp.createSocket('udp4')?
const socket = dgram.createSocket('udp4');

// Socket IP currently not working
socket.bind({
  port: process.env.UDP_PORT || 3000,
  address: process.env.UDP_IP || '192.168.0.20'
});

socket.on('listening', () => {
  let addr = socket.address();
  console.log(`Listening for UDP packets at ${addr.address}:${addr.port}`);
});

socket.on('error', (err) => {
  console.error(`UDP error: ${err.stack}`);
});

socket.on('message', (message, r) => {
  controller.readToLocal(message)
});


// catch 404 and forward to error handler
app.use(function(req, res, next) {
  next(createError(404));
});

// error handler
app.use(function(err, req, res, next) {
  // set locals, only providing error in development
  res.locals.message = err.message;
  res.locals.error = req.app.get('env') === 'development' ? err : {};

  // render the error page
  res.status(err.status || 500);
  res.render('error');
});

// This is just for testing UDP transmission
setInterval(function(){
  var random = Math.random()
  localStorage = controller.readToLocal({
    batteryTemp: random,
    motorTemp: random,
    podTemp: random,
    motorVoltage: random,
    electronicsVoltage: random,
    tankPressure: random,
    vesselPressure: random
  })
}, 2000);

module.exports = app;
