var count=0;

$(document).ready(function(){
	while((($("#container").height() - 300) < $(window).height()) && gifAva()){
		addGif();
	}
});

$(window).scroll(function() {
   if((($(window).scrollTop() + $(window).height()) > ($(document).height() - 600)) && gifAva()) {
   		addGif();
   }
});

function addGif() {
  var gif = gifURL();
  var url = "/user/" + username + "/p/" + gif;
  var id = gif.split(".")[0];
  $("#container").append("<a href='/u/" + username + "/" + id + "'><div class='gif' id='" + id + "'>" + "<img src='" + url + "'>" + "</div></a>");
  $('#' + id).css('display', 'none');
  $('#' + id).fadeIn(1000);
}

function gifAva() {
  return count<gifs.length;
}

function gifURL() {
  return gifs[count++];
}