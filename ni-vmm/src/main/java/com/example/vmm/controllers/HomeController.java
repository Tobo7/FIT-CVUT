package com.example.vmm.controllers;

import jakarta.servlet.http.HttpServletRequest;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.ControllerAdvice;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.ModelAttribute;

@Controller
public class HomeController {

    @ControllerAdvice
    public class GlobalController {

        @ModelAttribute("servletPath")
        String getRequestServletPath(HttpServletRequest request) {
            return request.getServletPath();
        }
    }

    @GetMapping("/")
    public String index() {

        return "README";
    }




}
