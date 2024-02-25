package com.example.vmm.repositories;

import com.example.vmm.models.Genome;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;
import org.springframework.stereotype.Repository;
import org.springframework.transaction.annotation.Transactional;

import java.util.List;
import java.util.Optional;

@Repository
public interface GenomeRepository extends JpaRepository<Genome, Long> {


    @Query("SELECT e FROM Genome e WHERE e.excluded = false ORDER BY e.name")
    List<Genome> findAllNotExcluded();

    @Query("SELECT e FROM Genome e WHERE e.excluded != false ORDER BY e.name")
    List<Genome> findAllExcluded();

    Optional<Genome> findByName(String name);


    @Transactional
    void deleteByName(String name);
}
