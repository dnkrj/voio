// config/passport.js
var fs = require('fs');

// load all the things we need
var LocalStrategy   = require('passport-local').Strategy;
var User            = require('./user'); // user model (db)

// expose this function to our app using module.exports
module.exports = function(passport) {

    // passport session setup
    // required for persistent login sessions
    // passport needs ability to serialize and unserialize users out of session

    // used to serialize the user for the session
    passport.serializeUser(function(user, done) {
        done(null, user.id);
    });

    // used to deserialize the user
    passport.deserializeUser(function(id, done) {
        User.findById(id, function(err, user) {
            done(err, user);
        });
    });


    // LOCAL SIGNUP
    // we are using named strategies since we have one for login and one for signup
    passport.use('local-signup', new LocalStrategy({
            // by default, local strategy uses username and password
            usernameField : 'username',
            passwordField : 'password',
            passReqToCallback : true // allows us to pass back the entire request to the callback
        },
        function(req, username, password, done) {
            if (password !== req.body.verify_password) {
                return done(null, false, req.flash('signupMessage', 'Passwords do not match'))
            }

            // asynchronous
            // User.findOne wont fire unless data is sent back
            process.nextTick(function() {

                // find a user whose username is the same as the forms username
                // we are checking to see if the user trying to login already exists
                User.findOne({ 'local.username' :  username }, function(err, user) {
                    if (err) return done(err);

                    // check to see if there's already a user with that username
                    if (user) {
                        return done(null, false, req.flash('signupMessage', 'Username already taken'));
                    };

                    return User.findOne({ 'local.email' : req.body.email }, function(err, email) {
                        if (err) return done(err);
                        console.log("INSIDE EMAIL QUERY ........................................");
                        console.log(email);

                        if (email) {
                            console.log("EMAIL OBJECT FOUND.......................................");
                            return done(null, false, req.flash('signupMessage', 'Email already used'));
                        };
                        // if there are any errors, return the error


                        // if there is no user with that username
                        // create the user
                        var newUser = new User();

                        // set the user's local credentials
                        newUser.local.username = username;
                        newUser.local.email    = req.body.email;
                        newUser.local.password = newUser.generateHash(password);
                        newUser.local.verified = false;
                        newUser.local.vericode = random();

                        // save the user
                        newUser.save(function(err) {
                            if (err) { throw err };
                            fs.mkdir(__dirname + "/../public/user/" + username, 7777, function(err) {
                                if (err) { throw err };
                                fs.mkdir(__dirname + "/../public/user/" + username + "/gifs/", 7777, function(err) {
                                    if (err) { throw err };  
                                    fs.mkdir(__dirname + "/../public/user/" + username + "/gifs/p", 7777, function(err) { 
                                        if (err) { throw err};
                                        fs.mkdir(__dirname + "/../public/user/" + username + "/gifs/a", 7777, function(err) { 
                                            if (err) { throw err};
                                            fs.mkdir(__dirname + "/../public/user/" + username + "/gifs/d", 7777, function(err) { 
                                                if (err) { throw err};
                                                return done(null, newUser);
                                            });
                                        });
                                    });

                                });
                            });
                        });
                    });
                });    
            });
        })
    );


    // LOCAL LOGIN
    // we are using named strategies since we have one for login and one for signup
    passport.use('local-login', new LocalStrategy({
            // by default, local strategy uses username and password
            usernameField : 'username',
            passwordField : 'password',
            passReqToCallback : true // allows us to pass back the entire request to the callback
        },
        function(req, username, password, done) { // callback with username and password from our form
            // find a user whose username is the same as the form's username
            // we are checking to see if the user trying to login already exists
            User.findOne({ 'local.username' :  username }, function(err, user) {
                // if there are any errors, return the error before anything else
                if (err) return done(err);

                if (!user) {
                    return User.findOne( { 'local.email' : email}, function(err, email) {
                        // if no user is found, return the message
                        if (!email) return done(null, false, req.flash('loginMessage', 'Incorrect username/email'));
                        // if the user is found but the password is wrong
                        if (!email.validPassword(password)) return done(null, false, req.flash('loginMessage', 'Incorrect password')); 

                        // all is well, return successful user
                        return done(null, user);               
                    });
                }

                // if the user is found but the password is wrong
                if (!user.validPassword(password)) return done(null, false, req.flash('loginMessage', 'Incorrect password')); 

                // all is well, return successful user
                return done(null, user);
            });
        })
    );

};


function random() {
    return Math.floor(Math.random() *1000000);
};