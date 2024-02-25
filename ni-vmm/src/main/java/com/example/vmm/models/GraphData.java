package com.example.vmm.models;

public class GraphData {

    int[] seq1;
    int[] seq2;
    Integer[] seq1Aligned;
    Integer[] seq2Aligned;

    public GraphData(int[] seq1, int[] seq2, Integer[] seq1Aligned, Integer[] seq2Aligned) {
        this.seq1 = seq1;
        this.seq2 = seq2;
        this.seq1Aligned = seq1Aligned;
        this.seq2Aligned = seq2Aligned;
    }

    public int[] getSeq1() {
        return seq1;
    }

    public int[] getSeq2() {
        return seq2;
    }

    public Integer[] getSeq1Aligned() {
        return seq1Aligned;
    }

    public Integer[] getSeq2Aligned() {
        return seq2Aligned;
    }
}
