package com.example.vmm.controllers;

import com.example.vmm.models.AppSettings;
import com.example.vmm.repositories.SettingsRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestParam;

import java.util.List;
import java.util.Map;
import java.util.Optional;
import java.util.stream.Collectors;

@Controller
public class SettingsController {

    @Autowired
    private SettingsRepository settingsRepository;

    @GetMapping("/settings")
    public String get(Model model) {

        List<AppSettings> settingsList = settingsRepository.findAll();
        Map<String, AppSettings> entityMap = settingsList.stream()
                .collect(Collectors.toMap(AppSettings::getId, entity -> entity));
        model.addAttribute("settings", entityMap);

        return "settings";
    }

    @PostMapping("/settings")
    public String save(@RequestParam Map<String, String> settings) {

        if(!settings.containsKey("importExclude")) {
            settings.put("importExclude", "");
        }

        settings.forEach((key, value) -> {
            Optional<AppSettings> s = settingsRepository.findById(key);


            if(s.isPresent()) {
                s.get().setValue(value);
                if (value.isBlank()) {
                    settingsRepository.delete(s.get());
                } else {
                    settingsRepository.save(s.get());
                }
            } else {
                if (!value.isBlank()) {
                    AppSettings ns = new AppSettings();
                    ns.setId(key);
                    ns.setValue(value);
                    settingsRepository.save(ns);
                }

            }
        });

        return "redirect:/settings";
    }




}
