package com.example.vmm.models;

public class SearchResult {

    int distance;
    Genome genome;

    public SearchResult(int distance, Genome genome) {
        this.distance = distance;
        this.genome = genome;
    }

    public int getSimilarity() {
        return distance;
    }

    public void setSimilarity(int similarity) {
        this.distance = similarity;
    }

    public Genome getGenome() {
        return genome;
    }

    public void setGenome(Genome genome) {
        this.genome = genome;
    }


    public Long getGenomeId() {
        return genome.getId();
    }
}
