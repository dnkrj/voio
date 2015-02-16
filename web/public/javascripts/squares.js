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
  var url = gifURL();
  var id = url.split(".")[0];
  $("#container").append("<a href='#'><div class='gif' id='" + id + "'>" + "<img class='freezeframe' src='/gifs/" + url + "'>" + "</div></a>");
  console.log(id);  
  $('#' + id).css('display', 'none');
  $('#' + id).fadeIn(1000);
}

function gifAva() {
  return count<gifs.length;
}

function gifURL() {
  return gifs[count++];
}