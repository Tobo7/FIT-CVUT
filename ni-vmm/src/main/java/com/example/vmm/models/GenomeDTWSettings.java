

package com.example.vmm.models;


public class GenomeDTWSettings {

    int AWeight;
    int CWeight;
    int GWeight;
    int TWeight;

    Integer sakoeChibaValue;

    Integer maxSequenceLength;


    public GenomeDTWSettings(int AWeight, int CWeight, int GWeight, int TWeight, Integer sakoeChibaValue, Integer maxSequenceLength) {
        this.AWeight = AWeight;
        this.CWeight = CWeight;
        this.GWeight = GWeight;
        this.TWeight = TWeight;
        this.sakoeChibaValue = sakoeChibaValue;
        this.maxSequenceLength = maxSequenceLength;
    }

    public int getAWeight() {
        return AWeight;
    }

    public int getCWeight() {
        return CWeight;
    }

    public int getGWeight() {
        return GWeight;
    }

    public int getTWeight() {
        return TWeight;
    }

    public Integer getSakoeChibaValue() {
        return sakoeChibaValue;
    }

    public Integer getMaxSequenceLength() {
        return maxSequenceLength;
    }

    public void setSakoeChibaValue(Integer sakoeChibaValue) {
        this.sakoeChibaValue = sakoeChibaValue;
    }

    public GenomeDTWSettings clone() {
        return new GenomeDTWSettings(AWeight,CWeight,GWeight, TWeight, sakoeChibaValue, maxSequenceLength);
    }
}
