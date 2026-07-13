# Gurobi comparison scripts

This directory contains optional scripts for solving small instances by a
Gurobi-based formulation. These scripts were used for comparison experiments.

The main kSS local-search program does **not** require Gurobi. Gurobi is needed
only if you want to run the scripts in this directory.

## Requirements

* Python 3
* Gurobi Optimizer
* Gurobi Python interface
* A valid Gurobi license

## Files

```text
lp_gurobi.sh
    Shell script for running the Gurobi-based computation.

lp_gurobi3_phylip.py
    Python script for solving instances in PHYLIP format using Gurobi.
```

## Notes

The Gurobi scripts are intended for small instances and comparison purposes.
They are not required for building or running `app/min_inc`.

The generated output files are not included in this repository.



