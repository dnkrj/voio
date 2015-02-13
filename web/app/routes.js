// app/routes.js
module.exports = function(app, passport) {

    // HOME PAGE (with login links)
    app.get('/', function(req, res) {
        res.render('index.html'); // load the index.html file
    });


    // LOGIN 
    app.get('/login', function(req, res) {

        // render the page and pass in any flash data if it exists
        res.render('login.html', { message: req.flash('loginMessage') }); 
    });


    // process the login form
    app.post('/login', passport.authenticate('local-login', {
        successRedirect : '/profile', // redirect to the secure profile section
        failureRedirect : '/login', // redirect back to the signup page if there is an error
        failureFlash : true // allow flash messages
    }));


    // SIGNUP
    // show the signup form
    app.get('/signup', function(req, res) {

        // render the page and pass in any flash data if it exists
        res.render('signup.html', { message: req.flash('signupMessage') });
    });

    // process the signup form
    app.post('/signup', passport.authenticate('local-signup', {
        successRedirect : '/profile', // redirect to the secure profile section
        failureRedirect : '/signup', // redirect back to the signup page if there is an error
        failureFlash : true // allow flash messages
    }));


    // PROFILE
    // we will want this protected so you have to be logged in to visit
    // the isLoggedIn function verifies this
    app.get('/profile', isLoggedIn, function(req, res) {
        res.render('profile.html', {
            user : req.user, // get the user out of session and pass to template
            message: req.flash('loginMessage')
        });
    });

    // LOGOUT
    app.get('/logout', function(req, res) {
        req.logout();
        res.redirect('/');
    });

    // EMAIL VERIFICATION
    var rand; // SHOULD BE IN DB
    var host;
    var link;
    app.get('/send', function(req, res) {
        rand = Math.floor((Math.random() *100) + 54);
        host = req.get('host');
        link="http://"+req.get('host')+"/verify?id="+rand;
        mailOptions = {
            to      : req.query.to,
            subject : "Please confirm your Email Account",
            html    : "Hello voio user,<br> Please Click on the link to verify your email.<br><a href="+link+">Click here to verify</a>" 
        }
        // TODO -  work out how exporting to module works
        transport.sendMail(mailOptions, function(error, response) {
            if (error) {
                console.log(error);
                res.end("error");
            } else {
                res.end("sent");
            }
        });
    });

    app.get('/verify', function(req, res) {
        if (req.query.id == rand) { // CHECK VALUE IN DB
            res.end("<h1>Email " + mailOptions.to + " is been Successfully verified");
        }
    })
};

// route middleware to make sure a user is logged in
function isLoggedIn(req, res, next) {

    // if user is authenticated in the session, carry on 
    if (req.isAuthenticated())
        return next();

    // if they aren't redirect them to the home page
    res.redirect('/login');
}