var nodemailer   = require('nodemailer');

var transport = nodemailer.createTransport({
    service: "Mandrill",
    auth: {
        user: 'natemail@mail.com',
        pass: '04X5kWuFBO6pD6OQ7xBKlA'
    }
});

module.exports = transport;