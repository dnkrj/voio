var userCount = 0;
var noOfGifs  = gifs.length;
var tsIndex   = 0;
var loc;


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
    var url = "/user/" + loc + "/a/" + gif;
    var id = gif.split(".")[0];

    $("#user_container").append("<div class='gif' id='" + id + "'><a href='/u/" + loc + "/" + id + "'><video width='300' height='300' loop><source src='" + url + "' type='video/mp4'></video></a></div>");

    $('#' + id + ' video').hover(function(){
        this.play();
    },function(){
        this.pause()
    });
    
    $('#' + id).css('display', 'none');
    $('#' + id).fadeIn(1000);
}

function userGifAvailable() {
    return userCount<noOfGifs;
}

function userGifURL() {

    max = ts[0];
    tsIndex = 0;
    for (var i = 1; i < ts.length; i++) {
        if (ts[i] > max) {
            tsIndex = i;
            max = ts[i];
        }
    }

    var gif = gifs[tsIndex];
    loc = ops[tsIndex];
    opCount = tsIndex + userCount;
    ts.splice(tsIndex, 1);
    gifs.splice(tsIndex, 1);
    ops.splice(tsIndex, 1);
    userCount++;

    return gif;
}