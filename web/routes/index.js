var express = require('express');
var fs = require('fs');

module.exports = function(passport) {
	var router = express.Router();
	
	/* GET home page. */
	router.get('/', function(req, res, next) {
		res.render('index', {
			user: req.user,
	      	title: 'Voio'
	    });
	});

	/* GET user page. */
	router.get('/u/:id', function(req, res, next) {
		var username = req.params.id;
		fs.readdir(__dirname + '/../public/users/' + username + '/gifs', function(err, files){
			var gifs = [];
			if (files === undefined) {
				username = "no gifs here."
			} else {
				files.forEach(function(gifDir) {
    				gifs.push('"'+gifDir+'"');
				});
			}	
			res.render('user', {
	      		title : username + '&middot; Voio',
	      		user: username,
	  	    	gifs  : gifs
	    	});
		});
	});

	//Pages for logged out users

	/* GET signup page */
	router.get('/signup', isLoggedIn(false), function (req, res, next) {
		res.render('signup', { 
			title : 'Signup &middot; Voio',
			message: req.flash('signupMessage')
			})
	});

	/* POST signup page */
	router.post('/signup', passport.authenticate('local-signup', {
		successRedirect : '/upload', // redirect to the secure profile section
		failureRedirect : '/signup',  // redirect back to the signup page if there is an error
	}));

	/* GET login page */
    router.get('/login', isLoggedIn(false), function (req, res) {
        res.render('login', { 
        	title : 'Login &middot; Voio',
        	message: req.flash('loginMessage')
        }); 
    });

    /* POST login page */
    router.post('/login', passport.authenticate('local-login', {
        successRedirect : '/profile', // redirect to the user page
        failureRedirect : '/login',   // redirect back to the signup page if there is an error
    }));

	
	// Pages for logged in users

	/* Redirects profile to user page */
	router.get('/profile', isLoggedIn(true), function(req, res) {
		res.redirect('/u/'+req.user.local.email);
	});

	/* GET upload page */
	router.get('/upload', isLoggedIn(true), function(req, res) {
	    res.render('upload', {
	    	title : 'Upload &middot; Voio',
	    	user: req.user
	    });
	});

	/* GET pending page */
	router.get('/pending', isLoggedIn(true), function(req, res) {
	    res.render('pending', {
	    	title : 'Pending &middot; Voio',
	    	user: req.user
	    });
	});

    /* GET logout page */
    // logs the user out and then redirects to the home page
    router.get('/logout', function(req, res) {
        req.logout();
        res.redirect('/');
    });


	return router;
};

function isLoggedIn (loggedin) {
    return function (req, res, next) {
    	// if user is authentication matches in the session, carry on 
    	if (req.isAuthenticated() === loggedin)
    	    return next();

    	// if they aren't redirect them to the home page
    	res.redirect('/');
    }
}