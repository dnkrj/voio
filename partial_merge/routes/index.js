var express = require('express');
var router = express.Router();
var fs = require('fs');

function getGifs() {
	var dirList = fs.readdirSync(__dirname + '/../public/gifs');
	var gifs = [];
	dirList.forEach(function(gifDir) {
    	gifs.push('"'+gifDir+'"');
	});
	return gifs;
}

/* GET home page. */
router.get('/', function(req, res, next) {
	var gifs = getGifs();
  res.render('index', {
  	title: 'Voio',
  	gifs: gifs
  });
});

/* GET upload page. */
router.get('/upload', function(req, res, next) {
  res.render('upload', { title: 'Upload &middot; Voio' });
});

module.exports = router;