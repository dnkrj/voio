var express = require('express');
var fs = require('fs');

module.exports = function(passport) {
	var router = express.Router();
	
	/* GET home page. */
	router.get('/', function(req, res, next) {
		var gifs = getGifs();
	    res.render('index', {
	      	title : 'Voio',
	  	    gifs  : gifs
	    });
	});

	/* GET upload page. */
	router.get('/upload', function(req, res, next) {
	    res.render('upload', { title : 'Upload &middot; Voio' });
	});

	/* GET signup page */
	router.get('/signup', function (req, res, next) {
		res.render('signup', { title : 'Signup &middot; Voio' })
	});

	/* POST signup page */
	router.post('/signup', passport.authenticate('local-signup', {
		successRedirect : '/profile', // redirect to the secure profile section
		failureRedirect : '/signup',  // redirect back to the signup page if there is an error
									  // TODO - return error
	}));

	/* GET login page */
    router.get('/login', function(req, res) {
        res.render('login', { title : 'Login &middot; Voio' }); 
    });

    /* POST login page */
    router.post('/login', passport.authenticate('local-login', {
        successRedirect : '/profile', // redirect to the secure profile section
        failureRedirect : '/login',   // redirect back to the signup page if there is an error
    								  // TODO - return error
    }));

	/* GET profile page */
	// we want this protected to you have to be logged in to access this
	// verification is done by the "isLoggedIn" function
	router.get('/profile', isLoggedIn, function(req, res) {
		res.render('profile', {
			title : 'Profule &middot; Voio',
			user  : req.user
		})
	});




	return router;
};

function getGifs() {
	var dirList = fs.readdirSync(__dirname + '/../public/gifs');
	var gifs = [];
	dirList.forEach(function(gifDir) {
    	gifs.push('"'+gifDir+'"');
	});
	return gifs;
};

function isLoggedIn(req, res, next) {

    // if user is authenticated in the session, carry on 
    if (req.isAuthenticated())
        return next();

    // if they aren't redirect them to the home page
    res.redirect('/login');
}