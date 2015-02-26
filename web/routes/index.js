var express   = require('express');
var fs        = require('fs');

var transport = require('../config/transport'); // email configuration
var User      = require('../config/user');      // user model (db)
var Gif       = require('../config/gif');       // gif model (db)

var util      = require('util'); //Calling bash script
var exec      = require('child_process').exec;
var child;

module.exports = function(passport) {
	var router = express.Router();
	
	/* GET home page. */
	router.get('/', function(req, res, next) {
		if (req.user) {
			res.render('index', {
				user: req.user.local,
		      	title: 'Voio'
		    });
		} else {
			res.render('index', {
		      	title: 'Voio'
		    });			
		}
	});

	/* GET user page. */
	router.get('/u/:id', function(req, res, next) {
		var userpage = req.params.id;
        var message = [];
        var pendingGifs = [];
        var gifs = [];


		var DBgifs = [];
		User.findOne( { 'local.username' : userpage}, 
			"posted _id op",
			{ sort:{ posted: -1 } }, //Sort by Date posted DESC
			function(err, user) {
				console.log('query complete');
				if (err) {
					console.log(err);
				}
	        	var stream = Gif.find({ op: user._id }).stream();
	        	stream.on('data', function(gif) {	        		
	        		DBgifs.push('"' + gif._id + '.gif"');
	        	}).on('close', function() {
		        	var isOwner = req.user && req.user.local.username == userpage;
	            	var isVerified = isOwner && req.user.local.verified;
	            	console.log(DBgifs);
		            if(isOwner) {
		                //console.log("Trying to find the gifs now! "+ userpage);
		                fs.readdir(__dirname + '/../public/user/' + userpage + '/p', function(err, pfiles){ 
		                     if(pfiles) {
		                        pendingGifs = pfiles.map( function(dir) { return '"' + dir + '"'}).reverse();
		                     }
		                    //console.log(pendingGifs);
		                    res.render('user', {
		                        title         : userpage + '&middot; Voio',
		                        userpage      : userpage,
		                        gifs          : DBgifs,
		                        user          : req.user.local,
		                        isOwner       : true, 
		                        isVerified	  : isVerified,
		                        hostname      : req.hostname,
		                        message       : req.flash('profileMessage'),
		                        pendingGifs   : pendingGifs
		                    });
		                });
		            } else {
		                res.render('user', {
		                    title         : userpage + '&middot; Voio',
		                    userpage      : userpage,
		                    isOwner       : false,
		                    isVerified	  : isVerified,
		                    gifs          : DBgifs,
		                    message       : message,
		                    pendingGifs   : pendingGifs
		                });
		            }
	        		
	        	})


			}
		);


		// fs.readdir(__dirname + '/../public/user/' + userpage + '/a', function(err, files){
		// 	if (! files) {
		// 		message = "no gifs here."
		// 	} else {
		// 		gifs = files.map( function(dir) { return '"' + dir + '"'}).reverse();
		// 	}
            
  //           var isOwner = req.user && req.user.local.username == userpage;
  //           var isVerified = isOwner && req.user.local.verified;
            
  //           if(isOwner) {
  //               //console.log("Trying to find the gifs now! "+ userpage);
  //               fs.readdir(__dirname + '/../public/user/' + userpage + '/p', function(err, pfiles){ 
  //                    if(pfiles) {
  //                       pendingGifs = pfiles.map( function(dir) { return '"' + dir + '"'}).reverse();
  //                    }
  //                   //console.log(pendingGifs);
  //                   res.render('user', {
  //                       title         : userpage + '&middot; Voio',
  //                       userpage      : userpage,
  //                       gifs          : DBgifs,
  //                       user          : req.user.local,
  //                       isOwner       : true, 
  //                       isVerified	  : isVerified,
  //                       hostname      : req.hostname,
  //                       message       : req.flash('profileMessage'),
  //                       pendingGifs   : pendingGifs
  //                   });
  //               });
  //           } else {
  //               res.render('user', {
  //                   title         : userpage + '&middot; Voio',
  //                   userpage      : userpage,
  //                   isOwner       : false,
  //                   isVerified	  : isVerified,
  //                   gifs          : dbgifs,
  //                   message       : message,
  //                   pendingGifs   : pendingGifs
  //               });
  //           }
  //       });
    });

	router.get('/u/:id/:gif', function(req, res, next) {
		var userpage = req.params.id;
		var gifview = req.params.gif;
		var userlocal;
		if (req.user) {
			userlocal = req.user.local;
		}
		res.render('gif', {
	      	title      : userpage + '&middot; Voio',
	      	userpage   : userpage,
	  	    gifview    : gifview,
	  	    user       : userlocal
	    });
	});

	//Pages for logged out users

	/* GET signup page */
	router.get('/signup', isLoggedIn(false), function (req, res, next) {
		res.render('signup', { 
			title	: 'Signup &middot; Voio',
			message : req.flash('signupMessage')
		});
	});

	/* POST signup page */
	router.post('/signup', passport.authenticate('local-signup', {
		successRedirect : '/profile', // redirect to the secure profile section
		failureRedirect : '/signup',  // redirect back to the signup page if there is an error
	}));

	/* GET login page */
    router.get('/login', isLoggedIn(false), function (req, res) {
        res.render('login', { 
        	title	: 'Login &middot; Voio',
        	message : req.flash('loginMessage')
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
		res.redirect('/u/'+req.user.local.username);
	});

	/* GET upload page */
	router.get('/upload', isLoggedIn(true), function(req, res) {
	    res.render('upload', {
	    	title : 'Upload &middot; Voio',
	    	user  : req.user.local
	    });
	});


	/* POST upload a video */
	router.post('/upload', function(req, res) {
		console.log("/// File uploaded at: " + req.files.upFile.path + ", by: " + req.user.local.username);
		res.end();
        var bashCall ='signalAnalysis ' + req.files.upFile.path + " " +
                    __dirname+ '/../public/user/'+req.user.local.username+'/p/';
        var path = __dirname + '/../bin' //Adds our bin to our path
        child = exec(bashCall,
                     {env :{PATH: path}},// adding environment
                    function (err, stdout, stderr) {      // one easy function to capture data/errors
                        console.log('stdout: ' + stdout);
                        console.log('stderr: ' + stderr);
                        if (err) {
                            console.log('exec error: ' + err);
                        }
                    });
        
	});

	/* GET Approve Gif */
	router.get('/a/:gif', isLoggedIn(true), function(req, res) {
		var username   = req.user.local.username;
		var gifname    = req.params.gif;

		var newGif = new Gif();
		var newgifname = newGif._id + ".gif";

		newGif.caption = "";
		newGif.tags    = "";
		newGif.likes   = 0;
		newGif.posted  = new Date();
		newGif.url     = username + '/a/' + newgifname;
		newGif.op      = req.user._id;


		fs.rename(__dirname + '/../public/user/' + username + '/p/' + gifname + '.gif',
				  __dirname + '/../public/user/' + username + '/a/' + newgifname,
				  function (err) {
				  	if (err) {
				  		console.log("/// FAILed to approve: " + gifname + ", for user: " + username);
				  		console.log(err);
				  	} else {
				  		newGif.save(function(err) {
				  			if (err) {
				  				console.log("/// Failed to add " + gifname + " to gif db. Returning to initial state.");
				  				console.log(err);
				  				// Putting back in pending folder
				  				fs.rename(
				  					__dirname + '/../public/user/' + username + '/a/' + newgifname,
				  					__dirname + '/../public/user/' + username + '/p/' + gifname + '.gif',
				  					function(err) {
				  						if (err) {
				  						  	console.log('/// Failed to return to pending directory.');
				  						  	console.log(err);
				  						}
				  					}
				  				);			  						  
				  			}
				  			User.findByIdAndUpdate(
				  				req.user._id,
				  				{ push: { "own_gifs" : newGif._id } },
				  				{},
				  				function(err) {
				  					if (err) {
				  						console.log("/// Failed to add reference to gif to user document. Returning to initial state.")
				  						console.log(err);
				  					}
				  				});

				  		});
				  	}
				  	res.end();
				  });
	});

	/* GET Delete Gif */
	router.get('/d/:gif', isLoggedIn(true), function(req, res) {
		var username = req.user.local.username;
		var gifname = req.params.gif;
		fs.rename(__dirname + '/../public/user/' + username + '/p/' + gifname + '.gif',
				  __dirname + '/../public/user/' + username + '/d/' + Date.now() + '.gif',
				  function (err) {
				  	if (err) {
				  		console.log("/// FAILed to delete: " + gifname + ", for user: " + username);
				  		console.log(err);
				  	}
				  	res.end();
				  });
	});

    /* GET logout page */
    // logs the user out and then redirects to the home page
    router.get('/logout', function(req, res) {
        req.logout();
        res.redirect('/');
    });

    // Requests for emailing
    router.get('/send', function(req, res) {
    	User.findOne({ 'local.email' : req.query.to }, function(err, user) {
    		if (err) {
    			console.log(err);
    			req.flash('profileMessage', "There was a problem on our side, we'll try and fix it soon!");
    			res.redirect('/profile');
    		}
    		if (typeof user !== 'undefined') {
		        link="http://" + req.hostname + "/verify?email=" + req.query.to + 
		        										"&code=" + req.query.code +
		        										"&_id="  + user._id;
		        mailOptions = {
		            to      : req.query.to,
		            from    : "no-reply@voio.io",
		            subject : "Please confirm your Email Account",
		            html    : "Hello " + user.local.username + ",<br> Please Click on the link to verify your email.<br><a href="+link+">Click here to verify</a>" 
		        }
		        transport.sendMail(mailOptions, function(error, response) {
		            if (err) {
		                console.log(err);
		                req.flash('profileMessage', "There was a problem, sending your email.\n We'll do our best to fix it soon! Maybe try again?");
		                res.redirect('/profile');
		            } else {
		                req.flash('profileMessage', "Email sent! Please check your inbox.");
		                res.redirect('/profile');
		            }
		        });    			
    		}
    	});
    });

    router.get('/verify', function(req, res) {
    	var id    = req.query._id;
        var email = req.query.email;
        var code  = req.query.code;
        console.log("verifying");
        User.findOneAndUpdate( 
        	{ 
        		'_id'            : id,
        		'local.email'    : email, 
        		'local.vericode' : code }, 
        	{ 'local.verified' : true },
        	{},
        	function(err) {
        		if (err) {
        			console.log('/// Failed to verify email address: ' + email);
        			console.log(err);
        		}
        		res.redirect("/login");
        	}
        );
    });

    router.get('/ready', function(req, res) {
    	console.log(req.query.user);
    	User.findOne({ 'local.username' : req.query.user }, function(err, user) {
    		console.log(user);
    		if (err) {
    			console.log(err);
    			res.send('FAILED');
    		} else {
	    		if (typeof user !== 'undefined') {

			        link="http://" + req.hostname + "/login";
			        mailOptions = {
			            to      : user.local.email,
			            from    : "no-reply@voio.io",
			            subject : "Your Gifs are ready!",
			            html    : "Hello " + user.local.username + ",<br><br> Login to voio.io to see your gifs!<br><br><a href="+link+">Click here to go to the website.</a>" 
			        }
			        transport.sendMail(mailOptions, function(error, response) {
			            if (err) {
			                console.log(err);
			                res.send('FAILED');
			            } else {
			                res.send('SUCCESS');
			            }
			        });    			
	    		}  	
    		}
    	});
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