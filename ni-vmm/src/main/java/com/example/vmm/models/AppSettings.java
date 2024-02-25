

package com.example.vmm.models;

import jakarta.persistence.Entity;
import jakarta.persistence.Id;

@Entity
public class AppSettings {
    @Id
    private String id;

    public String value;

    public String getId() {
        return id;
    }

    public void setId(String id) {
        this.id = id;
    }

    public String getValue() {
        return value;
    }

    public void setValue(String value) {
        this.value = value;
    }

    public String toString() {
        return getValue();
    }
}
