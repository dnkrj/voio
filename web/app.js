var express        = require('express');
var path           = require('path');
var favicon        = require('serve-favicon');
var logger         = require('morgan');
var cookieParser   = require('cookie-parser');
var bodyParser     = require('body-parser');
var multer         = require('multer');
var methodoverride = require('method-override');
var flash          = require('connect-flash');

var app = express();

// setup user accounts section (database, authentication)
var mongoose = require('mongoose');
var passport = require('passport');
var session  = require('express-session');

var configDB     = require('./config/database.js');
var configPass   = require('./config/passport.js')(passport); // pass passport for configuration

mongoose.connect(configDB.url); // connect to our database

// required for passport
app.use(cookieParser()); // read cookies (needed for auth)
app.use(session({ 
                    secret: 'voioioioioioioio',
                    resave: false,
                    saveUninitialized: false
                 }));
app.use(passport.initialize());
app.use(passport.session()); // persistent login sessions

// setupt emailing

// setup routing
var routes = require('./routes/index')(passport); //pass passport object for use

// Use connect-flash middleware for displaying messages to user
app.use(flash());

// view engine setup
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'ejs');



app.use(favicon(__dirname + '/public/favicon.ico'));
app.use(logger('dev')); // log every request to the server
app.use(bodyParser.urlencoded({ extended: true })); // allows wider range of inputs to forms

app.use(multer({ dest: './uploads/' }));
app.use(methodoverride());

app.use(express.static(__dirname + '/public')); //  "public" off of current directory is root



app.use('/', routes);

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
