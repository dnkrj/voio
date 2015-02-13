// config/email.js

var nodemailer   = require("nodemailer");

// set up email transport
var transport = nodemailer.createTransport({
    host: 'smtp.mandrillapp.com',
    port: 587,
    auth: {
        user: 'natemail@mail.com',
        pass: '04X5kWuFBO6pD6OQ7xBKlA'
    }
});
module.exports = transport;


// routes

    // EMAIL VERIFICATION
    var rand; // SAVE IN DB
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