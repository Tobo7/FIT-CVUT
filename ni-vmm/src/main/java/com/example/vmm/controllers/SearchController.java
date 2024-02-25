package com.example.vmm.controllers;

import com.example.vmm.models.AppSettings;
import com.example.vmm.models.Genome;
import com.example.vmm.models.SearchRequest;
import com.example.vmm.models.SearchResults;
import com.example.vmm.repositories.GenomeRepository;
import com.example.vmm.repositories.SettingsRepository;
import com.example.vmm.services.GenomeSearchUtil;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.ModelAttribute;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.server.ResponseStatusException;

import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;

@Controller
public class SearchController {

    @Autowired
    GenomeRepository genomeRepository;

    @Autowired
    SettingsRepository settingsRepository;

    @Autowired
    GenomeSearchUtil genomeSearchUtil;

    @GetMapping("/search")
    public String search(Model model) {

        List<Genome> genomeList = genomeRepository.findAllNotExcluded();

        List<AppSettings> settingsList = settingsRepository.findAll();
        Map<String, AppSettings> entityMap = settingsList.stream()
                .collect(Collectors.toMap(AppSettings::getId, entity -> entity));
        model.addAttribute("settings", entityMap);

        model.addAttribute("genomes", genomeList);

        return "search";
    }

    @PostMapping(path = "/search")
    public String searchPost(@ModelAttribute SearchRequest searchRequest, Model model) {

        if (!genomeRepository.existsById(searchRequest.getGenomeId())) {
           throw new ResponseStatusException(HttpStatus.NOT_FOUND);
        }

        SearchResults searchResults = genomeSearchUtil.searchByGenomeDTW(searchRequest.getGenomeId(), searchRequest.getDTWSettings());

        model.addAttribute("searchResultGenomes", searchResults.getResults());
        model.addAttribute("searchId", searchRequest.getGenomeId());
        model.addAttribute("searchTime", Math.round(searchResults.getElapsedTime()/10.)/100.);


        return "searchResults";
    }

}
