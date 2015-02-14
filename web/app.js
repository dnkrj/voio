var express      = require('express');
var path         = require('path');
var favicon      = require('serve-favicon');
var logger       = require('morgan');
var cookieParser = require('cookie-parser');
var bodyParser   = require('body-parser');

var app = express();

// setup user accounts (database, authentication)
var mongoose = require('mongoose');
var passport = require('passport');
var session  = require('express-session');

var configDB     = require('./config/config/database.js');
var configPass   = require('./config/config/passport.js')(passport); // pass passport for configuration

mongoose.connect(configDB.url); // connect to our database

app.use(cookieParser()); // read cookies (needed for auth)
// required for passport
app.use(session({ 
                    secret: 'voioioioioioioio',
                    resave: false,
                    saveUninitialized: false
                 }));
app.use(passport.initialize());
app.use(passport.session()); // persistent login sessions


var routes = require('./routes/index')(passport);
var users = require('./routes/users');


// view engine setup
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'ejs');

app.use(favicon(__dirname + '/public/favicon.ico'));
app.use(logger('dev')); // log every request to the server
//app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true })); // allows wider range of inputs to forms
//app.use(cookieParser());
app.use(express.static(__dirname + '/public')); //  "public" off of current directory is root


// ASK DAN -why are these separate?
// What advatages does it bring?
// How does it work?
// http://expressjs.com/api.html#app.use
app.use('/', routes);
app.use('/users', users);

// catch 404 and forward to error handler
app.use(function(req, res, next) {
    var err = new Error('Not Found');
    err.status = 404;
    next(err);
});

// error handlers

// development error handler
// will print stacktrace
if (app.get('env') === 'development') {
    app.use(function(err, req, res, next) {
        res.status(err.status || 500);
        res.render('error', {
            title: 'Oh dear!',
            message: err.message,
            error: err
        });
    });
}

// production error handler
// no stacktraces leaked to user
app.use(function(err, req, res, next) {
    res.status(err.status || 500);
    res.render('error', {
        title: 'Oh dear!',
        message: err.message,
        error: {}
    });
});

module.exports = app;
