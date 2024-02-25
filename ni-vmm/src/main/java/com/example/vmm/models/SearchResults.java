package com.example.vmm.models;

import java.util.List;

public class SearchResults {

    long elapsedTime;

    List<SearchResult> results;


    public SearchResults(long elapsedTime, List<SearchResult> results) {
        this.elapsedTime = elapsedTime;
        this.results = results;
    }

    public long getElapsedTime() {
        return elapsedTime;
    }

    public List<SearchResult> getResults() {
        return results;
    }
}
