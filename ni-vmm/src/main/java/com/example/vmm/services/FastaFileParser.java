package com.example.vmm.services;

import com.example.vmm.models.Genome;
import org.springframework.core.io.InputStreamSource;
import org.springframework.stereotype.Service;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.List;

@Service
public class FastaFileParser {

    public List<Genome> parseFile(InputStreamSource inputStream) throws IOException {
        List<Genome> objects = new ArrayList<>();


        try (BufferedReader reader = new BufferedReader(new InputStreamReader(inputStream.getInputStream()))) {
            String line;
            String name = null;
            Genome currentObject = null;

            while ((line = reader.readLine()) != null) {
                if (line.startsWith(">")) {
                    if (currentObject != null) {
                        objects.add(currentObject);
                    }
                    name = line.substring(1).trim();
                    currentObject = new Genome(name);
                } else if (currentObject != null) {
                    if(currentObject.getSequence() != null) {
                        currentObject.setSequence(currentObject.getSequence()  + line);
                    } else  {
                        currentObject.setSequence(line);
                    }
                }
            }

            if (currentObject != null) {
                objects.add(currentObject);
            }
        }

        return objects;
    }
}
