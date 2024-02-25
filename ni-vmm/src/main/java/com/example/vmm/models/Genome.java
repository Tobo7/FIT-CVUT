

package com.example.vmm.models;

import jakarta.persistence.*;

import java.io.Serializable;

@Entity
public class Genome  {
    @Id
    @GeneratedValue
    private Long id;

    @Column(unique = true)
    public String name;

    @Column(columnDefinition = "TEXT")
    private String sequence;

    @Column(columnDefinition=" bool default false not null")
    private boolean excluded;

    public Genome() {
    }
    public Genome(String name) {
        this.name = name;
    }
    public Genome(String name, String sequence) {
        this.name = name;
    }

    public Long getId() {
        return id;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getSequence() {
        return sequence;
    }

    public void setSequence(String sequence) {
        this.sequence = sequence;
    }

    public boolean isExcluded() {
        return excluded;
    }

    public void setExcluded(boolean excluded) {
        this.excluded = excluded;
    }
}
