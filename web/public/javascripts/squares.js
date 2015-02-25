var userCount=0;

$(document).ready(function(){
	while((($("#user_container").height() - 300) < $(window).height()) && userGifAvailable()){
		addUserGif();
    }
});

$(window).scroll(function() {
    if((($(window).scrollTop() + $(window).height()) > ($(document).height() - 600)) && userGifAvailable()) {
        addUserGif();
    }
});

function addUserGif() {
    var gif = userGifURL();
    var url = "/user/" + userpage + "/a/" + gif;
    var id = gif.split(".")[0];

    $("#user_container").append("<div class='gif' id='" + id + "'>" + "<img data-gifffer='" + url + "'  data-gifffer-width='300' data-gifffer-height='300' data-gifffer-link='/u/" + userpage + "/" + id + "'/>" + "</div>");

    $('#' + id).css('display', 'none');
    $('#' + id).fadeIn(1000);

    Gifffer();
}

function userGifAvailable() {
    return userCount<gifs.length;
}

function userGifURL() {
    return gifs[userCount++];
}