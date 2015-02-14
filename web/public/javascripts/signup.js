function verifyPassword() {
	if (($('#pass1')[0].value === $('#pass2')[0].value) && ($('#pass1')[0].value !== "")) {
		console.log("THEN");
		$('#subbutton').removeAttr('disabled');
	} else {
		console.log("ELSE");
		$('#subbutton').attr('disabled', 'disabled');	
	}
}