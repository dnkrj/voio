var userCount = 0;
var opCount   = 0;

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
    console.log(ops);
    console.log(gifs);

    var loc;

    if (typeof userpage === 'undefined') {
        loc = ops[opCount++];
    } else {
        loc = userpage;
    }

    var gif = userGifURL();
    var url = "/user/" + loc + "/a/" + gif;
    var id = gif.split(".")[0];

    $("#user_container").append("<div class='gif' id='" + id + "'><a href='/u/" + loc + "/" + id + "'><video width='300' height='300' loop webkit-playsinline><source src='" + url + "' type='video/mp4'></video></a></div>");

    $('#' + id + ' video').hover(function(){
        this.play();
    },function(){
        this.pause()
    });
    
    $('#' + id).css('display', 'none');
    $('#' + id).fadeIn(1000);
}

function userGifAvailable() {
    return userCount<gifs.length;
}

function userGifURL() {
    return gifs[userCount++];
}