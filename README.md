# kSSMIU

This repository contains a C implementation of local-search algorithms for the
LP minimum-increase ultrametric tree problem. The main executable is `min_inc`,
which reads a distance matrix in PHYLIP format and applies the kSS local search
with parameter `k`.

The repository also includes small sample instances, scripts for running batches
of experiments, scripts for extracting performance metrics from raw logs, and
programs for generating synthetic input instances.

## Directory structure

```text
.
├── app/
│   └── min_inc.c                  # Main program
├── src/
│   ├── common/                    # Basic data structures and utilities
│   │   ├── list.c, list.h
│   │   ├── matrix.c, matrix.h
│   │   ├── tree.c, tree.h
│   │   ├── linkage.c, linkage.h
│   │   └── stack.c, stack.h
│   └── kss/
│       ├── kss.c                  # kSS local-search implementation
│       └── kss.h
├── experiments/
│   ├── scripts/                   # Batch execution scripts
│   ├── analysis/                  # Log extraction scripts
│   ├── instance_generation/       # Programs for generating instances
│   └── gurobi/                    # Optional Gurobi-based comparison scripts
├── data/
│   └── sample/                    # Small sample input instances
├── results/
│   ├── raw/                       # Raw experimental logs, if included
│   └── figure/                    # Processed data and figures
├── tools/                         # Auxiliary conversion and plotting tools
└── Makefile
```

## Requirements

The core program requires:

* A C compiler such as `gcc`
* `make`
* A POSIX-like shell environment

The analysis and auxiliary scripts may additionally require:

* Python 3
* Gnuplot, for regenerating figures from `.gp` scripts
* Gurobi, only for the optional scripts in `experiments/gurobi/`

Gurobi is not required to build or run the main kSS local-search program.

## Build

From the repository root, run:

```bash
make
```

This builds the main executable:

```text
app/min_inc
```

To remove generated object files and executables, run:

```bash
make clean
```

To build the instance-generation programs, run:

```bash
make experiments
```

This builds the following programs under `experiments/instance_generation/`:

```text
make_euclidean
make_random
perturbedUM
```

## Usage

The main program has the following form:

```bash
./app/min_inc [l1|l2] <input file> <k> [tree output file]
```

The argument `[11|l2]` selects the objective parameter. If it is omitted, `l1`
is used by default.

Examples:

```bash
./app/min_inc data/sample/euclid100_1.phylip 1
```

```bash
./app/min_inc l1 data/sample/euclid100_1.phylip 10
```

```bash
./app/min_inc l2 data/sample/euclid100_1.phylip 10
```

To also output the resulting tree in Newick format, provide an output file as
the final argument:

```bash
./app/min_inc l1 data/sample/euclid100_1.phylip 10 output_tree.nwk
```

## Output format

For each run, `min_inc` prints one comma-separated line of performance metrics.
A typical output line is:

```text
input: data/sample/euclid100_1.phylip, k: 1, totaltime [s]: 0.001234, #improving moves: 12, #neighborhood scans: 13, time/iter [ms]: 0.094923, ave size neighbor: 123.000000, init. obj: 168282.257814, obj: 164048.308213, impr. rate [%]: 2.515981
```

The fields are:

| Field                 | Meaning                                                           |
| --------------------- | ----------------------------------------------------------------- |
| `input`               | Input file name                                                   |
| `k`                   | Algorithm parameter                                               |
| `totaltime [s]`       | Total running time in seconds                                     |
| `#improving moves`    | Number of improving moves performed                               |
| `#neighborhood scans` | Number of neighborhood scans                                      |
| `time/iter [ms]`      | Average time per neighborhood scan in milliseconds                |
| `ave size neighbor`   | Average neighborhood size                                         |
| `init. obj`           | Objective value of the initial solution                           |
| `obj`                 | Objective value of the final solution                             |
| `impr. rate [%]`      | Improvement rate, computed as `(init_obj - obj) / init_obj * 100` |

## Sample data

This repository contains only small sample instances under:

```text
data/sample/
```

They are intended for checking that the code builds and runs correctly. Larger
experimental datasets are not included in the main repository.

The sample files are:

```text
data/sample/euclid100_1.phylip
data/sample/random100_1.phylip
data/sample/pertUM100_1.phylip
```

## Running batch experiments

The directory `experiments/scripts/` contains shell scripts for running `min_inc`
on multiple input instances and parameter values.

For example:

```bash
./experiments/scripts/kss2phylip.sh 1 euclid
```

or:

```bash
./experiments/scripts/kss3phylip.sh 2 pertUM
```

The first argument is the objective parameter, either `1` or `2`. The second
argument is the instance type, one of:

```text
euclid
random
pertUM
```

These batch scripts assume that the corresponding full instance files have
already been generated under `data/euclid/`, `data/random/`, or `data/pertUM/`.
To generate them, first run `make experiments` and then use
`experiments/instance_generation/make_instances.sh`. 

The scripts use paths relative to the repository root internally, so they can be
executed from any working directory. 

## Extracting results from logs

The script `experiments/analysis/extract.py` extracts selected fields from raw
`min_inc` output logs.

Example:

```bash
python3 experiments/analysis/extract.py \
  results/raw/kss2phylip-l1-euc.txt \
  input k totaltime obj impr_rate
```

This prints a CSV table with the selected columns.

Available keys are:

```text
input
k
totaltime
improving_moves
neighborhood_scans
time_per_iter
ave_size_neighbor
init_obj
obj
impr_rate
```

## Generating input instances

The directory `experiments/instance_generation/` contains programs for generating
synthetic instances.

Build them with:

```bash
make experiments
```

Examples:

```bash
./experiments/instance_generation/make_euclidean 100 1 output.phylip
```

```bash
./experiments/instance_generation/make_random 100 1 output.phylip
```

```bash
./experiments/instance_generation/perturbedUM 100 0.1 1 output.phylip
```

The script `make_instances.sh` can be used to generate multiple instances:

```bash
./experiments/instance_generation/make_instances.sh euclid
```

```bash
./experiments/instance_generation/make_instances.sh random
```

```bash
./experiments/instance_generation/make_instances.sh pertUM
```

## Results and figures

The directory `results/figure/` contains processed experimental data and figure
files used for reporting results. Raw logs, if included, are placed under
`results/raw/`.

The Gnuplot scripts in `results/figure/` can be used to regenerate the PDF
figures, for example:

```bash
gnuplot results/figure/fig1_euclidean_improvement_l1.gp
```

## Optional Gurobi scripts

The directory `experiments/gurobi/` contains scripts for Gurobi-based comparison
experiments. These scripts are optional and require a working Gurobi installation
and license.

They are not needed for compiling or running `app/min_inc`.

## Notes

* Generated executables and object files are excluded from version control by
  `.gitignore`.
* The main repository is intended to remain lightweight. Large generated datasets
  and extensive raw logs should be managed separately from the source code.
* The `legacy/` directory, if present in a local working copy, contains auxiliary or
  legacy programs and is not required for the main kSS implementation.

## Citation

If you use this code in academic work, please cite the accompanying paper. The
citation information will be added here when the final bibliographic details are
available.
