package com.example.vmm.controllers;

import com.example.vmm.models.Genome;
import com.example.vmm.repositories.GenomeRepository;
import com.example.vmm.repositories.SettingsRepository;
import com.example.vmm.services.FastaFileParser;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.data.domain.Sort;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.multipart.MultipartFile;

import java.io.IOException;
import java.io.InputStreamReader;
import java.nio.charset.StandardCharsets;
import java.util.*;

@Controller
public class DatabaseController {

    @Autowired
    FastaFileParser fastaFileParser;

    @Autowired
    GenomeRepository genomeRepository;

    @Autowired
    SettingsRepository settingsRepository;

    @GetMapping("/database")
    public String databaseGet(Model model) {

        List<Genome> parsedResult = genomeRepository.findAll(Sort.by(Sort.Direction.ASC, "name"));
        model.addAttribute("genomes", parsedResult);
        model.addAttribute("importExcludeDefault", settingsRepository.findById("importExclude").isPresent());

        return "database";
    }

    @PostMapping("/upload")
    public String uploadFile(@RequestParam("file") MultipartFile file, @RequestParam("importExclude") Optional<Boolean> importExclude) throws IOException {

        List<Genome> parsedResult = fastaFileParser.parseFile(file);

        int deleted = 0;

        for (Genome g : parsedResult) {
            Optional<Genome> eg = genomeRepository.findByName(g.getName());
            if(eg.isPresent()) {
                genomeRepository.deleteByName(g.getName());
                g.setExcluded(eg.get().isExcluded());
                deleted++;
            } else  {
                importExclude.ifPresent(g::setExcluded);
            }
        }

        genomeRepository.saveAll(parsedResult);

        return "redirect:/database?i="+ (parsedResult.size() - deleted) + "&u=" + deleted;
    }

    @PostMapping("/database/removeAll")
    public String removeAll()  {
        genomeRepository.deleteAll();
        return "redirect:/database";
    }

    @PostMapping("/database/excludeAll")
    public String excludeAll()  {
        List<Genome> list = genomeRepository.findAllNotExcluded();

        for (Genome g: list) {
            g.setExcluded(true);
        }
        genomeRepository.saveAll(list);

        return "redirect:/database";
    }

    @PostMapping("/database/includeAll")
    public String includeAll()  {
        List<Genome> list = genomeRepository.findAllExcluded();

        for (Genome g: list) {
            g.setExcluded(false);
        }
        genomeRepository.saveAll(list);

        return "redirect:/database";
    }




}
