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
    var url = "/user/" + username + "/a/" + gif;
    var id = gif.split(".")[0];

    $("#container").append("<div class='gif' id='" + id + "'>" + "<img data-gifffer='" + url + "'  data-gifffer-width='300' data-gifffer-height='300' data-gifffer-link='/u/" + username + "/" + id + "'/>" + "</div>");

    $('#' + id).css('display', 'none');
    $('#' + id).fadeIn(1000);

    Gifffer();
}

function gifAva() {
    return count<gifs.length;
}

function gifURL() {
    return gifs[count++];
}