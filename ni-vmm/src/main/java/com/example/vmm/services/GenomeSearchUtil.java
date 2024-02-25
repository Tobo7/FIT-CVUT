package com.example.vmm.services;

import com.example.vmm.models.*;
import com.example.vmm.repositories.GenomeRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.List;

@Service
public class GenomeSearchUtil {


    @Autowired
    private GenomeRepository genomeRepository;

    @Autowired
    private DTWSearchUtil dtwSearchUtil;

    public SearchResults searchByGenomeDTW(Long genomeId, GenomeDTWSettings dtwSettings)  {
        return searchByGenomeDTW(genomeRepository.findById(genomeId).get(), dtwSettings);
    }

    public SearchResults searchByGenomeDTW(Genome reqGenome, GenomeDTWSettings dtwSettings)  {


        List<SearchResult> result = new ArrayList<>();
        List<Genome> allGenomes = genomeRepository.findAllNotExcluded();

        long start = System.currentTimeMillis();

        int[] tsReqG = getTimeSequenceFromGenome(reqGenome, dtwSettings);

        for (Genome genome:allGenomes) {
            int[] tsG = getTimeSequenceFromGenome(genome, dtwSettings);
            int distance = dtwSearchUtil.getDTWDistance(tsReqG, tsG, dtwSettings.getSakoeChibaValue());

            result.add(new SearchResult(distance, genome));
        }
        long finish = System.currentTimeMillis();
        long timeElapsed = finish - start;

        result.sort(Comparator.comparingInt(SearchResult::getSimilarity));

        return new SearchResults(timeElapsed, result);
    }

    public Integer getDTWDistance(Long g1, Long g2, GenomeDTWSettings dtwSettings) {

        int[] ts1 = getTimeSequenceFromGenome(genomeRepository.findById(g1).get(), dtwSettings);
        int[] ts2 = getTimeSequenceFromGenome(genomeRepository.findById(g2).get(), dtwSettings);

        return dtwSearchUtil.getDTWDistance(ts1, ts2, dtwSettings.getSakoeChibaValue());
    }

    public Integer getDTWDistance(Genome g1, Genome g2, GenomeDTWSettings dtwSettings) {
        int[] ts1 = getTimeSequenceFromGenome(g1, dtwSettings);
        int[] ts2 = getTimeSequenceFromGenome(g2, dtwSettings);

        return dtwSearchUtil.getDTWDistance(ts1, ts2, dtwSettings.getSakoeChibaValue());
    }

    protected int[] sumPreviousValues(int[] ts) {
        int[] result = new int[ts.length];

        for (int i = 0; i < ts.length; i++) {
            int previousValue = (i > 0) ? result[i - 1] : 0;

            result[i] = ts[i] + previousValue;
        }
        return result;
    }

    protected Integer[] sumPreviousValues(Integer[] ts) {
        Integer[] result = new Integer[ts.length];

        int lastValue = 0;
        for (int i = 0; i < ts.length; i++) {
            if (ts[i] != null) {
                result[i] = ts[i] + lastValue;
                lastValue = result[i];
            }
        }
        return result;
    }


    public GraphData getGenomeGraphData(Genome genome1, Genome genome2, GenomeDTWSettings dtwSettings)  {
        int[] ts1 = getTimeSequenceFromGenome(genome1, dtwSettings);
        int[] ts2 = getTimeSequenceFromGenome(genome2, dtwSettings);

        Integer[][] alignedSequences = dtwSearchUtil.getAlignedSequence(ts1, ts2, dtwSettings.getSakoeChibaValue());

        return new GraphData(sumPreviousValues(ts1), sumPreviousValues(ts2),
                sumPreviousValues(alignedSequences[0]),
                sumPreviousValues(alignedSequences[1])
        );
    }

    protected int[] getTimeSequenceFromGenome(Genome genome, GenomeDTWSettings dtwSettings) {

        String s = genome.getSequence();
        int maxLen;

        if(dtwSettings.getMaxSequenceLength() != null) {
            maxLen = Math.min(s.length(), dtwSettings.getMaxSequenceLength());
        } else {
            maxLen = s.length();
        }

        int[] result = new int[maxLen];

        for (int i = 0; i < maxLen; i++) {

            switch (s.charAt(i)) {
                case 'A':
                    result[i] = dtwSettings.getAWeight();
                    break;
                case 'C':
                    result[i] = dtwSettings.getCWeight();
                    break;
                case 'G':
                    result[i] = dtwSettings.getGWeight();
                    break;
                case 'T':
                    result[i] = dtwSettings.getTWeight();
                    break;
            }
        }

        return result;
    }


    protected String getGenomeSequenceFromTimeSequence(int[] ts1, GenomeDTWSettings settings) {

        StringBuilder result = new StringBuilder();

        for (int i = 0; i < ts1.length; i++) {

            if (ts1[i] == settings.getAWeight()) {
                result.append('A');
            } else if(ts1[i] == settings.getCWeight()) {
                result.append('C');
            } else if(ts1[i] == settings.getGWeight()) {
                result.append('G');
            } else if(ts1[i] == settings.getTWeight()) {
                result.append('T');
            }
        }

        return result.toString();
    }

}
