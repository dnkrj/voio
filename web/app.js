var express  = require('express');
var app      = express();
var path	 = require('path');
var port     = 80;
var mongoose = require('mongoose');
var passport = require('passport');
var flash    = require('connect-flash');

var logger       = require('morgan');
var cookieParser = require('cookie-parser');
var bodyParser   = require('body-parser');
var session      = require('express-session');
var exphbs      = require('express3-handlebars');

var engines = require('consolidate');

var configDB = require('./config/database.js');

mongoose.connect(configDB.url); // connect to our database


//app.use(express.static(__dirname)); // Current directory is root
app.use(express.static(__dirname + '/public')); //  "public" off of current is root

require('./config/passport.js')(passport); // pass passport for configuration

// set up our express application
app.use(logger('dev')); // log every request to the console
app.use(cookieParser()); // read cookies (needed for auth)
app.use(bodyParser.urlencoded({ extended: true })); // get information from html forms

app.engine('html', engines.mustache);
app.set('view engine', 'html');
app.set('views', __dirname + '/public');

// required for passport
app.use(session({ 
					secret: 'voioioioioioioio',
					resave: false,
					saveUninitialized: false
				 }));
app.use(passport.initialize());
app.use(passport.session()); // persistent login sessions
app.use(flash()); // use connect-flash for flash messages stored in session

require('./app/routes.js')(app, passport); // load our routes and pass in our app and fully configured passport

app.listen(port);
console.log('Listening on port 80');