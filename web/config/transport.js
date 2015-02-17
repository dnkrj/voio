var nodemailer   = require('nodemailer');

var transport = nodemailer.createTransport({
    host: 'smtp.mandrillapp.com',
    port: 587,
    auth: {
        user: 'natemail@mail.com',
        pass: '04X5kWuFBO6pD6OQ7xBKlA'
    }
});

module.exports = transport;