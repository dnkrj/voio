$(document).ready(function(){
    var from,to,subject,text;
    $("#verify").click(function(){      
        to=userobject.email;
        $.get("http://localhost:3000/send",{to:to});
	});
});