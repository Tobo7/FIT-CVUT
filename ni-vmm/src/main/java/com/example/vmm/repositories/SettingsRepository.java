package com.example.vmm.repositories;

import com.example.vmm.models.AppSettings;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.stereotype.Repository;

@Repository
public interface SettingsRepository extends JpaRepository<AppSettings, String> {

}

