#!/usr/bin/env bash
set -euo pipefail

# 使用法:
#   ./convert_all_max.sh 入力ディレクトリ [出力ディレクトリ] [コンバータスクリプト]
# デフォルト:
#   出力ディレクトリ = 入力ディレクトリ
#   コンバータスクリプト = /home/ando/kssmiu-code/convert_max_to_phylip.py

input_dir="${1:-.}"
output_dir="${2:-$input_dir}"
converter="${3:-/home/ando/kssmiu-code/convert_max_to_phylip.py}"

mkdir -p "$output_dir"

# ファイル名にスペース等があっても安全に処理
find "$input_dir" -type f -name '*.max' -print0 | while IFS= read -r -d '' file; do
  base="$(basename "$file" .max)"
  out="$output_dir/${base}.phylip"
  echo "Converting: $file -> $out"
  python3 "$converter" "$file" "$out"
done