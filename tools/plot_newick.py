#!/usr/bin/env python3
"""Plot Newick tree files as PNG images.

Usage:
    python3 plot_newick.py <treefile.nwk> [--output out.png] [--format png] [--dpi 150] [--size 10 10]

Examples:
    python3 plot_newick.py tree.nwk
    python3 plot_newick.py tree.nwk --output tree.png
    python3 plot_newick.py tree.nwk --format pdf --dpi 300 --size 12 8
    python3 plot_newick.py tree.nwk --show-branches
"""

import argparse
import os
import sys

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from Bio import Phylo


def plot_newick(treefile, outfile, fig_width, fig_height, dpi):
    tree = Phylo.read(treefile, "newick")
    fig = plt.figure(figsize=(fig_width, fig_height), dpi=dpi)
    ax = fig.add_subplot(1, 1, 1)
    Phylo.draw(tree, do_show=False, axes=ax, branch_labels=lambda c: "")
    ax.set_title(os.path.basename(treefile))
    fig.tight_layout()
    fig.savefig(outfile, bbox_inches="tight")
    plt.close(fig)
    print(f"Wrote {outfile}")


def parse_args():
    parser = argparse.ArgumentParser(
        description="Read Newick tree files and render them as image files."
    )
    parser.add_argument("treefile", help="Input Newick file")
    parser.add_argument(
        "--output", "-o", help="Output image file. Default: <treefile>.<format>", default=None
    )
    parser.add_argument(
        "--format", "-f", help="Output image format (png, pdf, svg, etc.)", default="png"
    )
    parser.add_argument(
        "--dpi", type=int, default=150, help="Image resolution in dots per inch"
    )
    parser.add_argument(
        "--size",
        nargs=2,
        type=float,
        metavar=("WIDTH", "HEIGHT"),
        default=(10.0, 10.0),
        help="Figure size in inches (width height)"
    )
    parser.add_argument(
        "--show-branches",
        action="store_true",
        help="Show branch labels in the rendered tree"
    )
    return parser.parse_args()


if __name__ == "__main__":
    args = parse_args()
    if not os.path.isfile(args.treefile):
        print(f"Error: file not found: {args.treefile}", file=sys.stderr)
        sys.exit(1)

    root, _ = os.path.splitext(args.treefile)
    outfile = args.output or f"{root}.{args.format.lstrip('.') }"

    try:
        tree = Phylo.read(args.treefile, "newick")
    except Exception as e:
        print(f"Error reading Newick file: {e}", file=sys.stderr)
        sys.exit(1)

    fig = plt.figure(figsize=(args.size[0], args.size[1]), dpi=args.dpi)
    ax = fig.add_subplot(1, 1, 1)
    branch_labels = (lambda c: c.branch_length) if args.show_branches else (lambda c: "")
    Phylo.draw(tree, do_show=False, axes=ax, branch_labels=branch_labels)
    ax.set_title(os.path.basename(args.treefile))
    fig.tight_layout()
    fig.savefig(outfile, bbox_inches="tight")
    plt.close(fig)
    print(f"Wrote {outfile}")
