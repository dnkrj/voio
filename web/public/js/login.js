$(document).ready(function(){
       	$("#login_form").append('<form action="/login" method="post">Email:<br><input type="text" name="email">'
       								+ '<br><br>Password:<br>'
									+ '<input type="password" name="password">'
									+ '<br><br><input type="submit" value="Submit"></form>');
});