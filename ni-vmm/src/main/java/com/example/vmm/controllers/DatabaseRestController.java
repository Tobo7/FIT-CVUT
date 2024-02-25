package com.example.vmm.controllers;

import com.example.vmm.models.Genome;
import com.example.vmm.repositories.GenomeRepository;
import com.example.vmm.services.FastaFileParser;
import javassist.NotFoundException;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.web.multipart.MultipartFile;

import java.io.IOException;
import java.util.List;
import java.util.Optional;

@RestController
public class DatabaseRestController {


    @Autowired
    GenomeRepository genomeRepository;

    @PostMapping("/excludeGenome")
    public Boolean excludeGenome(@RequestParam("idGenome") Long idGenome) throws IOException, NotFoundException {

        Optional<Genome> genome = genomeRepository.findById(idGenome);

        if(genome.isPresent()) {

            Genome g = genome.get();
            g.setExcluded(!g.isExcluded());

            genomeRepository.save(g);

            return g.isExcluded();
        }

        throw new NotFoundException("Genome not found");

    }


    @PostMapping("/deleteGenome")
    public void uploadFile(@RequestParam("idGenome") Long idGenome) throws IOException, NotFoundException {

        Optional<Genome> genome = genomeRepository.findById(idGenome);

        if(genome.isPresent()) {
            genomeRepository.deleteById(idGenome);
            return;
        }

        throw new NotFoundException("Genome not found");

    }



}
