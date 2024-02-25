
import $ from "jquery";
$(document).ready(function() {
    $('.input-disable-button').on('click', function (e) {
        $(this).siblings('input').val('');
    });

});



