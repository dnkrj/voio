// config/user.js
// load the things we need
var mongoose = require('mongoose');
var bcrypt   = require('bcrypt-nodejs');

// define the schema for our user model
var userSchema = new mongoose.Schema({
    local           : {
        username    : String,
        email       : String,
        verified    : Boolean,
        vericode    : Number,
        password    : String,
        subscribe   : [mongoose.Schema.ObjectId],
        reblog_gifs_ops : [{ gif : mongoose.Schema.ObjectId, op : String, reblogged : Date }],
        own_gifs	: [mongoose.Schema.ObjectId],
        own_gifs_posted : [{ id : mongoose.Schema.ObjectId, posted: Date }]
    }
});

// methods
// generating a hash
userSchema.methods.generateHash = function(password) {
    return bcrypt.hashSync(password, bcrypt.genSaltSync(8), null);
};

// checking if password is valid
userSchema.methods.validPassword = function(password) {
    return bcrypt.compareSync(password, this.local.password);
};

// create the model for users and expose it to our app
module.exports = mongoose.model('User', userSchema);