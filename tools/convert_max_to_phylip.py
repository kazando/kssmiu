import sys 


def read_max_file(filename):
    with open(filename, 'r') as f:
        lines = f.readlines()

    # 1行目からnを取得
    for line in lines:
        if line.startswith('p max'):
            _, _, n, _ = line.strip().split()
            n = int(n)
            break

    # n x n の下三角行列（対角成分0）を初期化
    matrix = [[0.0 for _ in range(i + 1)] for i in range(n)]

    # データ部分を読み込む
    for line in lines:
        if line.startswith('a'):
            _, i, j, value = line.strip().split()
            i = int(i) - 1  # 0-indexed
            j = int(j) - 1
            value = float(value)
            # 下三角部分に格納（i > j）
            if i > j:
                matrix[i][j] = value
            else:
                matrix[j][i] = value

    return matrix

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python convert_max_to_phylip.py <input.max> <output.phylip>")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]

    matrix = read_max_file(input_file)
    n = len(matrix)

    with open(output_file, 'w') as f:
        f.write(f"{n}\n")
        for i in range(n):
            f.write(f"Seq{i+1} ")  # シーケンス名
            for j in range(n):
                if i == j:
                    f.write("0.0 ")
                elif i > j:
                    f.write(f"{matrix[i][j]} ")
                else:
                    f.write(f"{matrix[j][i]} ")
            f.write("\n")