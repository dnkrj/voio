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
