$(document).ready(function(){
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