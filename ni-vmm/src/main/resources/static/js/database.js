import './index.js'
import $ from "jquery";

window.deleteGenome = (idGenome) => {
    $.ajax({
        type: "POST",
        url: '/deleteGenome',
        data: "idGenome="+idGenome,
        success: function()
        {
            $("[data-genome-id="+idGenome+"]").remove();
        }
    });
};

window.excludeGenome = (idGenome) => {
    $.ajax({
        type: "POST",
        url: '/excludeGenome',
        data: "idGenome="+idGenome,
        success: function(isExcluded)
        {
            const $genome = $("[data-genome-id="+idGenome+"]");

            if(isExcluded) {
                $genome.addClass('table-danger');
            } else {
                $genome.removeClass('table-danger');
            }
        }
    });
};


