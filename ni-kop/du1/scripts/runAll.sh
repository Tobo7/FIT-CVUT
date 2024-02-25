#!/usr/bin/env bash

./scripts/runDataset.sh gsat2 ./data/uf20-71/ $1 $2
./scripts/runDataset.sh probsat ./data/uf20-71/ $1 $2

./scripts/runDataset.sh gsat2 ./data/uf20-81/ $1 $2
./scripts/runDataset.sh probsat ./data/uf20-81/ $1 $2

./scripts/runDataset.sh gsat2 ./data/uf20-91/ $1 $2
./scripts/runDataset.sh probsat ./data/uf20-91/ $1 $2

./scripts/runDataset.sh gsat2 ./data/uf50-218/ $1 $2
./scripts/runDataset.sh probsat ./data/uf50-218/ $1 $2

./scripts/runDataset.sh gsat2 ./data/uf75-325/ $1 $2
./scripts/runDataset.sh probsat ./data/uf75-325/ $1 $2

