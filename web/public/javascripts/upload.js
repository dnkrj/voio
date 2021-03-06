$(function() {
  $('#upFile').change( function() {
    if (isOwner) {
      var formData = new FormData();
      var file = document.getElementById('upFile').files[0];

      formData.append('upFile', file);
      
      var xhr = new XMLHttpRequest();
      
      xhr.open('post', '/upload', true);

      $('#upButton').fadeOut(1000);
      $('#upFile').hide();
      $('div.message').text("Uploading " + file.name);
      $('#upMessage').css('opacity', '1');
      
      xhr.upload.onprogress = function(e) {
        if (e.lengthComputable) {
          var percentage = (e.loaded / e.total) * 100;
          $('div.progress div.bar').css('width', percentage + '%');
        }
      };
      
      xhr.onerror = function(e) {
        $('div.progress').fadeOut();
        $('div.message').text('An error occurred when uploading your file.');
      };
      
      xhr.onload = function() {
        $('div.progress div.bar').css('width', '0%');
        $('#upButton').fadeIn(100);
        $('#upFile').show();  
//        $('#upMessage').css('opacity', '0');
        $('div.message').text("Upload Complete.\nCheck back soon to see your moments!");
          
      };
      
      xhr.send(formData);
    }
  });
});