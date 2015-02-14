var count=0;

$(document).ready(function(){
	while(($("#container").height() < $(window).height()) && gifAva()){
		var url = gifURL();
    $("#container").append("<a href='#'><div class='gif' id=" + title + ">" + "<img src='/gifs/" + url + "'>" + "</div></a>");
	}
});

$(window).scroll(function() {
   if((($(window).scrollTop() + $(window).height()) > ($(document).height() - 600)) && gifAva()) {
   		var url = gifURL();
      $("#container").append("<a href='#'><div class='gif' id=" + title + ">" + "<img src='/gifs/" + url + "'>" + "</div></a>");
   }
});

function gifAva() {
  return count<gifs.length;
}

function gifURL() {
  return gifs[count++];
}