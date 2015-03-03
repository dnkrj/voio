var pendingCount=0;
var userCount=0;

$(document).ready(function(){
    while((($("#pending_container").height() + $("#user_container").height() - 300) < $(window).height()) && pendingGifAvailable()){
        addPendingGif();
    }
    while((($("#user_container").height() + $("#pending_container").height() - 300) < $(window).height()) && userGifAvailable()){
        addUserGif();
    }
});

$(window).scroll(function() {
    if((($(window).scrollTop() + $(window).height()) > ($(document).height() - 600)) && pendingGifAvailable()) {
        addPendingGif();
    } else if((($(window).scrollTop() + $(window).height()) > ($(document).height() - 600)) && userGifAvailable()) {
        addUserGif();
    }
});

function addPendingGif() {
    var gif = pendingGifURL();
    var url = "/user/" + userpage + "/p/" + gif;
    var id = gif.split(".")[0];
    $("#pending_container").append("<div class='gif pending' id='" + id + "'>" + "<a id='A" + id + "' href='#'><div class='gifButton'>approve</div></a><video width='300' height='300' loop webkit-playsinline><source src='" + url + "' type='video/mp4'></video><a id='D" + id + "' href='#'><div class='gifButton'>delete</div></a></div>");

    $( "#A" + id ).click(function(e) {
        $.get( "/a/" + id, function( data ) {
            $("#user_container h1").after("<div class='gif' id='" + data + "'><a href='/u/" + userpage + "/" + data + "'><video width='300' height='300' loop><source src='/user/" + userpage + "/a/" + data + ".mp4' type='video/mp4'></video></a></div>");
            $('#' + data).css('display', 'none');
            $('#' + data).fadeIn(1000);
            $('#' + data + ' video').hover(function(){
                this.play();
            },function(){
                this.pause()
            });
        });
        $("#" + id).fadeOut(500);
        e.preventDefault();
    });

    $( "#D" + id ).click(function(e) {
        $.get("/d/" + id);
        $("#" + id).fadeOut(500);
        e.preventDefault();
    });

    $('#' + id + ' video').hover(function(){
        this.play();
    },function(){
        this.pause()
    });

    $('#' + id).css('display', 'none');
    $('#' + id).fadeIn(1000);
}

function addUserGif() {
    var gif = userGifURL();
    var url = "/user/" + userpage + "/a/" + gif;
    var id = gif.split(".")[0];

    $("#user_container").append("<div class='gif' id='" + id + "'><a href='/u/" + userpage + "/" + id + "'><video width='300' height='300' loop webkit-playsinline><source src='" + url + "' type='video/mp4'></video></a></div>");

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

function pendingGifAvailable() {
    return pendingCount<pendingGifs.length;
}

function pendingGifURL() {
    return pendingGifs[pendingCount++];
}