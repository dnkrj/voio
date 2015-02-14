var express = require('express');
var router = express.Router();
var uploadManager = require('./uploadManager')(router);

/* GET home page. */
router.get('/', function(req, res) {
  res.render('index', { title: 'Upload &middot; Voio' });
});

module.exports = router;
