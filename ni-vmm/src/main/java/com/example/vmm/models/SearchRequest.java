package com.example.vmm.models;

public class SearchRequest {

    private Long genomeId;
    int AWeight;
    int CWeight;
    int GWeight;
    int TWeight;
    Integer sakoeChibaValue;
    Integer maxSequenceLength;

    public GenomeDTWSettings getDTWSettings() {
        return new GenomeDTWSettings(AWeight,CWeight,GWeight,TWeight,sakoeChibaValue, maxSequenceLength);
    }

    public Long getGenomeId() {
        return genomeId;
    }

    public void setGenomeId(Long genomeId) {
        this.genomeId = genomeId;
    }


    public void setAWeight(int AWeight) {
        this.AWeight = AWeight;
    }


    public void setCWeight(int CWeight) {
        this.CWeight = CWeight;
    }


    public void setGWeight(int GWeight) {
        this.GWeight = GWeight;
    }


    public void setTWeight(int TWeight) {
        this.TWeight = TWeight;
    }


    public void setSakoeChibaValue(Integer sakoeChibaValue) {
        this.sakoeChibaValue = sakoeChibaValue;
    }

    public void setMaxSequenceLength(Integer maxSequenceLength) {
        this.maxSequenceLength = maxSequenceLength;
    }
}
