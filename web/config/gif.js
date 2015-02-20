var mongoose = require('mongoose');

var gifSchema = new mongoose.Schema({
	caption		: String,
	tags		: [String],
	likes		: Number,
    posted      : Date,
    url         : String,
	op			: mongoose.Schema.ObjectId
});

module.exports = mongoose.model('Gif' , gifSchema);


// testing - use these to manually add elements to db
// var User = mongoose.model('User', userSchema);
// var Gif  = mongoose.model('Gif', gifSchema);

// var m = new Gif;
// User.findOne({ 'local.username' :  '~' }, function(err, user) {
//     m.caption = 'test_005';
//     m.tags.push("test_gif_5");
//     m.tags.push("005");
//     m.tags.push("testing is fun");
//     m.likes = 0;
//     m.posted = new Date;
//     m.url = '/~/a';
//     m.op = user._id;
//     m.save(function(err) {
//         if (err) console.log(err);
//     });
// });