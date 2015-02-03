$(document).ready(function(){
	var i = 0;
	while(i < 30){
		var url = getURL();
       	$("#container").append("<a href='#'><div class='gif' id=" + url + "></div></a>");
       	$("#" + url).css("background-color","#" + url);
       	i++;
	}
});

$(window).scroll(function() {
   if($(window).scrollTop() + $(window).height() > $(document).height() - 600) {
   		var url = getURL();
       	$("#container").append("<a href='#'><div class='gif' id=" + url + "></div></a>");
       	$("#" + url).css("background-color","#" + url);
   }
});

function getURL() {
	return Math.floor(Math.random()*999);
}