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
    
    $("#container").append("<div class='gif pending' id='" + id + "'>" + "<a id='A" + id + "' href='#'><img src='" + url + "'></a><a id='D" + id + "' href='#'><div class='delete'>delete</div></a>" + "</div>");

    $( "#A" + id ).click(function() {
        $.get("/a/" + id);
        $("#" + id).fadeOut(500);
        $
    });
    $( "#D" + id ).click(function() {
        $.get("/d/" + id);
        $("#" + id).fadeOut(500);
        $
    });

    $('#' + id).css('display', 'none');
    $('#' + id).fadeIn(1000);
}

function gifAva() {
    return count<gifs.length;
}

function gifURL() {
    return gifs[count++];
}