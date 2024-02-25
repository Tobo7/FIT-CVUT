import './index.js'
import '../css/search.scss'
import './inputs-common'
import $ from 'jquery'

import Chart from 'chart.js/auto';

function initGenomeSelect() {
    var $select = $('#select-tools').selectize({
        valueField: 'id',
        labelField: 'title',
        searchField: 'title',
        create: false
    });
}

function initFormSubmitHandler() {
    $('form').submit(function(e) {
        e.preventDefault();

        var form = $(this);
        var actionUrl = form.attr('action');

        const $button = $('.search-button');
        $button.find('span').show();
        $button.attr('disabled', true);

        $.ajax({
            type: "POST",
            url: actionUrl,
            data: form.serialize(), // serializes the form's elements.
            success: function(data)
            {
                $button.find('span').hide();
                $button.attr('disabled', false);
                $('#searchResults').html($(data).find('#searchResults'))
            }
        });

    });
}


let graph1 = null;
let graph2 = null;
function drawGraph(data) {

    function getConfig(seq1, seq2, title) {
        const len1 = Math.max(seq1.length, seq2.length);

        const skipped = (ctx, value) => ctx.p0.skip || ctx.p1.skip ? value : undefined;

        const config = {
            type: 'line',
            data: {
                labels: [...Array(len1).keys()],
                datasets: [
                    {
                        label: 'Sequence A',
                        data: [...seq1],
                        tension: -0.05,
                        segment: {
                            borderDash: ctx => skipped(ctx, [3, 5]),
                        },
                        spanGaps: true
                    },
                    {
                        label: 'Sequence B',
                        data: [...seq2],
                        tension: 0.05,
                        segment: {
                            borderDash: ctx => skipped(ctx, [3, 5]),
                        },
                        spanGaps: true
                    }
                ]
            },
            options: {
                animation: {
                    duration: 0
                },
                responsive: true,
                maintainAspectRatio: false,
                plugins: {
                    legend: {
                        title: {
                            display: true,
                            text: title,
                            position: 'start',
                            font: {weight:'bold'}
                        },
                        align: 'start'

                    }
                }

            },
        };

        return config;
    }

    if(graph1){
        graph1.destroy();
    }
    if(graph2){
        graph2.destroy();
    }

    const len1 = Math.max(data.seq1.length, data.seq2.length);
    const len2 = Math.max(data.seq1Aligned.length, data.seq2Aligned.length);

    $('.chart1-wrap').width((len1*20)+'px');
    $('.chart2-wrap').width((len2*20)+'px');

    graph1 = new Chart($('#chart1'), getConfig(data.seq1,data.seq2, 'Original sequences'));
    graph2 = new Chart($('#chart2'), getConfig(data.seq1Aligned,data.seq2Aligned, 'Aligned sequences'));

}


function initRestoreDefaults() {
    let defaultValues = {};

    $('form :input').each(function() {
        var inputName = $(this).attr('name');
        var inputValue = $(this).val();
        defaultValues[inputName] = inputValue;
    });

    $('.restore-defaults-button').on('click', function () {
        Object.keys(defaultValues).forEach(function(key) {
            $("[name="+key+"]").val(defaultValues[key]);
        });
    });


}

$(document).ready(function() {
    initRestoreDefaults();
    initGenomeSelect();
    initFormSubmitHandler();
});


window.openModal = (idGenome1, idGenome2) => {
    $.ajax({
        type: "POST",
        url: '/graphData',
        data: $('form').serialize() + "&idGenome1="+idGenome1+"&idGenome2="+idGenome2,
        success: function(data)
        {
            drawGraph(data)
        }
    });
};

