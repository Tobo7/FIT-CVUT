package com.example.vmm;

import com.example.vmm.models.Genome;
import com.example.vmm.models.GenomeDTWSettings;
import com.example.vmm.models.SearchResult;
import com.example.vmm.models.SearchResults;
import com.example.vmm.repositories.GenomeRepository;
import com.example.vmm.services.GenomeSearchUtil;
import org.junit.jupiter.api.Test;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.data.domain.Page;
import org.springframework.data.domain.PageRequest;
import org.springframework.data.domain.Pageable;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.*;
import java.util.stream.Collectors;

@SpringBootTest
class VmmApplicationTests {

    @Autowired
    private GenomeRepository genomeRepository;

    @Autowired
    private GenomeSearchUtil genomeSearchUtil;

    @Test
    void testTime() throws IOException {
        List<Genome> all = genomeRepository.findAll();
        for (Genome g:all) {
            g.setExcluded(false);
        }
        genomeRepository.saveAllAndFlush(all);
        List<Genome> genomes = genomeRepository.findAllById(Arrays.asList(63202L, 63204L));

        Integer maxLen = 100;

        File csvFile = new File("output/timeTest"+ maxLen+".csv");


        try (BufferedWriter writer = new BufferedWriter(new FileWriter(csvFile))) {

            writer.write("Sakoe Chiba value"+ "," + "Duration ms" + "\n");

            for (Integer i = 1; i < maxLen; i+=10) {
                Long avgTime = getAvgTime(maxLen, i, genomes);

                System.out.print("[");
                System.out.print(i);
                System.out.print(",");
                System.out.print(avgTime);
                System.out.println("]");

                writer.write(i+ "," + avgTime + "\n");

                if(i > 100) {
                    i+=10;
                }

                if(i > 500) {
                    i+=15;
                }

                if(i > 1000) {
                    i+=20;
                }

                if(i > 1500) {
                    i+=200;
                }

            }
        }



    }

    @Test
    void testMAE() throws IOException {

        List<Genome> all = genomeRepository.findAll();
        for (Genome g:all) {
            g.setExcluded(true);
        }
        genomeRepository.saveAllAndFlush(all);

        Page<Genome> genomes = genomeRepository.findAll(PageRequest.of(0, 30));
        for (Genome g:genomes) {
            g.setExcluded(false);
        }
        genomeRepository.saveAllAndFlush(genomes);

        Integer maxLen = 500;


        String csvTitle = maxLen == null ? "": maxLen.toString();
        File csvFile = new File("output/maeTest"+ csvTitle+".csv");

        try (BufferedWriter writer = new BufferedWriter(new FileWriter(csvFile))) {

            writer.write("Sakoe Chiba value"+ "," + "Mean average error" + "\n");

            Double lastval = 0.0;
            for (Integer i = 1; i < (maxLen != null ? maxLen : 10000); i++) {
                Double mae = getMAE(maxLen, i, genomes.toList());
                mae = roundAvoid(mae, 3);

                System.out.print("[");
                System.out.print(i);
                System.out.print(",");
                System.out.print(mae);
                System.out.println("]");

                writer.write(i+ "," + mae + "\n");

                if (lastval == 0 && mae == 0) {
                    break;
                }
                lastval = mae;
                writer.flush();

                if(i > 20) {
                    i++;
                }

                if(i > 100) {
                    i++;
                }

                if(i > 200) {
                    i++;
                }

                if(i > 500) {
                    i++;
                }

            }
        }



    }


    @Test
    void testPercentsMax() throws IOException {

        List<Genome> all = genomeRepository.findAll();
        for (Genome g:all) {
            g.setExcluded(true);
        }
        genomeRepository.saveAllAndFlush(all);

        Page<Genome> genomes = genomeRepository.findAll(PageRequest.of(0, 40));
        for (Genome g:genomes) {
            g.setExcluded(false);
        }
        genomeRepository.saveAllAndFlush(genomes);

        Integer maxLen = 100;


        String csvTitle = maxLen == null ? "": maxLen.toString();
        File csvFile = new File("output/percTestMax"+ csvTitle+".csv");

        try (BufferedWriter writer = new BufferedWriter(new FileWriter(csvFile))) {

            writer.write("Sakoe Chiba value"+ "," + "Mean average error" + "\n");

            Double lastval = 0.;
            for (Integer i = 1; i < (maxLen != null ? maxLen : 10000); i++) {
                Double mae = getPercentErrMax(maxLen, i, genomes.toList());
                mae = roundAvoid(mae, 3);

                System.out.print("[");
                System.out.print(i);
                System.out.print(",");
                System.out.print(mae);
                System.out.println("]");

                writer.write(i+ "," + mae + "\n");


                if (lastval == 0 && mae == 0) {
                    break;
                }
                lastval = mae;

                if(i > 20) {
                    i++;
                }

                if(i > 100) {
                    i++;
                }

                if(i > 200) {
                    i++;
                }

                if(i > 500) {
                    i++;
                }

            }
        }



    }


    @Test
    void testPercentsAvg() throws IOException {

        List<Genome> all = genomeRepository.findAll();
        for (Genome g:all) {
            g.setExcluded(true);
        }
        genomeRepository.saveAllAndFlush(all);

        Page<Genome> genomes = genomeRepository.findAll(PageRequest.of(0, 40));
        for (Genome g:genomes) {
            g.setExcluded(false);
        }
        genomeRepository.saveAllAndFlush(genomes);

        Integer maxLen = 100;


        String csvTitle = maxLen == null ? "": maxLen.toString();
        File csvFile = new File("output/percTestAvg"+ csvTitle+".csv");

        try (BufferedWriter writer = new BufferedWriter(new FileWriter(csvFile))) {

            writer.write("Sakoe Chiba value"+ "," + "Mean average error" + "\n");

            Double lastval = 0.;
            for (Integer i = 1; i < (maxLen != null ? maxLen : 10000); i++) {
                Double mae = getPercentErrAvg(maxLen, i, genomes.toList());
                mae = roundAvoid(mae, 3);

                System.out.print("[");
                System.out.print(i);
                System.out.print(",");
                System.out.print(mae);
                System.out.println("]");

                writer.write(i+ "," + mae + "\n");


                if (lastval == 0 && mae == 0) {
                    break;
                }
                lastval = mae;

                if(i > 20) {
                    i++;
                }

                if(i > 100) {
                    i++;
                }

                if(i > 200) {
                    i++;
                }

                if(i > 500) {
                    i++;
                }


            }
        }



    }


    private Long getAvgTime(Integer maxLen, Integer chiba, List<Genome> genomes) {

        GenomeDTWSettings referenceSettings =
                new GenomeDTWSettings(-2,-1,1,2,null, maxLen);

        GenomeDTWSettings testSettings = referenceSettings.clone();
        testSettings.setSakoeChibaValue(chiba);

        Long avgTime = 0L;

        for (Genome genome: genomes) {
            SearchResults testResults = genomeSearchUtil.searchByGenomeDTW(genome, testSettings);
            avgTime += testResults.getElapsedTime();
        }


        avgTime /= genomes.size();

        return  avgTime;

    }


    private Double getMAE(Integer maxLen, Integer chiba, List<Genome> genomes ) {

        GenomeDTWSettings referenceSettings =
                new GenomeDTWSettings(-2,-1,1,2,null, maxLen);

        GenomeDTWSettings testSettings = referenceSettings.clone();
        testSettings.setSakoeChibaValue(chiba);

        Double avgError = 0.;

        for (Genome genome: genomes) {
            Double sum = 0.;

            List<SearchResult> refResults = genomeSearchUtil.searchByGenomeDTW(genome, referenceSettings).getResults();
            List<SearchResult> testResults = genomeSearchUtil.searchByGenomeDTW(genome, testSettings).getResults();

            Collections.sort(refResults, Comparator.comparingLong(SearchResult::getGenomeId));
            Collections.sort(testResults, Comparator.comparingLong(SearchResult::getGenomeId));

            Integer div = refResults.size();

            for (int i = 0; i < refResults.size(); i++) {

                double optimal = refResults.get(i).getSimilarity();
                double approx = testResults.get(i).getSimilarity();

                if(optimal == 0.) {
                    div--;
                    continue;
                }

                sum += approx - optimal;
            }

            sum /= div;

            avgError += sum;

        }

        avgError /= genomes.size();

        return  avgError;

    }


    private Double getPercentErrMax(Integer maxLen, Integer chiba, List<Genome> genomes ) {

        GenomeDTWSettings referenceSettings =
                new GenomeDTWSettings(-2,-1,1,2,null, maxLen);

        GenomeDTWSettings testSettings = referenceSettings.clone();
        testSettings.setSakoeChibaValue(chiba);

        Double avgError = 0.;
        Double max = 0.;

        for (Genome genome: genomes) {
            Double sum = 0.;

            List<SearchResult> refResults = genomeSearchUtil.searchByGenomeDTW(genome, referenceSettings).getResults();
            List<SearchResult> testResults = genomeSearchUtil.searchByGenomeDTW(genome, testSettings).getResults();

            Collections.sort(refResults, Comparator.comparingLong(SearchResult::getGenomeId));
            Collections.sort(testResults, Comparator.comparingLong(SearchResult::getGenomeId));

            Integer div = refResults.size();

            for (int i = 0; i < refResults.size(); i++) {
                double optimal = refResults.get(i).getSimilarity();
                double approx = testResults.get(i).getSimilarity();

                if(optimal == 0) {
                    div--;
                    continue;
                }

                Double tmp = (Math.abs(approx - optimal) / optimal) * 100.;

                sum += tmp;
                max = Math.max(tmp, max);
            }

            sum /= div;

            avgError += sum;

        }

        avgError /= genomes.size();

        return  max;

    }

    private Double getPercentErrAvg(Integer maxLen, Integer chiba, List<Genome> genomes ) {

        GenomeDTWSettings referenceSettings =
                new GenomeDTWSettings(-2,-1,1,2,null, maxLen);

        GenomeDTWSettings testSettings = referenceSettings.clone();
        testSettings.setSakoeChibaValue(chiba);

        Double avgError = 0.;
        Double max = 0.;

        for (Genome genome: genomes) {
            Double sum = 0.;

            List<SearchResult> refResults = genomeSearchUtil.searchByGenomeDTW(genome, referenceSettings).getResults();
            List<SearchResult> testResults = genomeSearchUtil.searchByGenomeDTW(genome, testSettings).getResults();

            Collections.sort(refResults, Comparator.comparingLong(SearchResult::getGenomeId));
            Collections.sort(testResults, Comparator.comparingLong(SearchResult::getGenomeId));

            Integer div = refResults.size();

            for (int i = 0; i < refResults.size(); i++) {
                double optimal = refResults.get(i).getSimilarity();
                double approx = testResults.get(i).getSimilarity();

                if(optimal == 0) {
                    div--;
                    continue;
                }

                Double tmp = (Math.abs(approx - optimal) / optimal) * 100.;

                sum += tmp;
                max = Math.max(tmp, max);
            }

            sum /= div;

            avgError += sum;

        }

        avgError /= genomes.size();

        return  avgError;

    }

    @Test
    void test1() {
        GenomeDTWSettings referenceSettings =
                new GenomeDTWSettings(-2,-1,1,2,200, 500);

        Integer distance = genomeSearchUtil.getDTWDistance(60754L,60757L, referenceSettings);

        System.out.println(distance);
    }

    public double roundAvoid(double value, int places) {
        double scale = Math.pow(10, places);
        return Math.round(value * scale) / scale;
    }


}
