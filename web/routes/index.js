var express = require('express');
var fs = require('fs');

module.exports = function(passport) {
	var router = express.Router();
	
	/* GET home page. */
	router.get('/', function(req, res, next) {
		res.render('index', {
	      	title : 'Voio',
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
			title : 'Profile &middot; Voio',
			user  : req.user
		})
	});

    /* GET logout page */
    // logs the user out and then redirects to the home page
    router.get('/logout', function(req, res) {
        req.logout();
        res.redirect('/');
    });


	return router;
};

function isLoggedIn(req, res, next) {

    // if user is authenticated in the session, carry on 
    if (req.isAuthenticated())
        return next();

    // if they aren't redirect them to the home page
    res.redirect('/login');
}