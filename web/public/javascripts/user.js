function verifyemail(email, vericode) {      
	console.log("SENDING EMAIL FUNCTION");
    $.get("http://localhost:80/send",{ 
    	to   : email,
    	code : vericode
    });
};