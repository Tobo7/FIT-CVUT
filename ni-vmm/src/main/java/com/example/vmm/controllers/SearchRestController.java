package com.example.vmm.controllers;

import com.example.vmm.models.Genome;
import com.example.vmm.models.GraphData;
import com.example.vmm.models.SearchRequest;
import com.example.vmm.models.SearchResult;
import com.example.vmm.repositories.GenomeRepository;
import com.example.vmm.services.GenomeSearchUtil;
import com.fasterxml.jackson.core.JsonProcessingException;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.*;
import org.springframework.web.server.ResponseStatusException;

import java.util.List;

@RestController
public class SearchRestController {

    @Autowired
    GenomeRepository genomeRepository;

    @Autowired
    GenomeSearchUtil genomeSearchUtil;



    @PostMapping(path = "/graphData")
    public GraphData searchPost(@ModelAttribute SearchRequest searchRequest, @RequestParam Long idGenome2) {
        Genome genome1 = genomeRepository.findById(searchRequest.getGenomeId()).get();
        Genome genome2 = genomeRepository.findById(idGenome2).get();

        return  genomeSearchUtil.getGenomeGraphData(genome1,genome2,searchRequest.getDTWSettings());
    }



}
