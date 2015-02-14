// setup db

// user accounts (database, authentication)
var mongoose = require('mongoose');
var passport = require('passport');

var configDB 	 = require('./config/database.js');
var configPass   = require('./config/passport.js')(passport); // pass passport for configuration

mongoose.connect(configDB.url); // connect to our database

app.use(cookieParser()); // read cookies (needed for auth)