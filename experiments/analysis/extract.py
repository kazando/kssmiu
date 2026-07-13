import re
import sys

def extract_performance_metrics(file_path):
    metrics = []
    number = r"[-+]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][-+]?\d+)?"

    pattern = re.compile(
        rf'^\s*input:\s*(?P<input>[^,]+),\s*'
        rf'k:\s*(?P<k>\d+),\s*'
        rf'totaltime \[s\]:\s*(?P<totaltime>{number}),\s*'
        rf'#improving moves:\s*(?P<improving_moves>\d+),\s*'
        rf'#neighborhood scans:\s*(?P<neighborhood_scans>\d+),\s*'
        rf'time/iter \[ms\]:\s*(?P<time_per_iter>{number}),\s*'
        rf'ave size neighbor:\s*(?P<ave_size_neighbor>{number}),\s*'
        rf'init\. obj:\s*(?P<init_obj>{number}),\s*'
        rf'obj:\s*(?P<obj>{number}),\s*'
        rf'impr\. rate \[%\]:\s*(?P<impr_rate>{number})\s*$'
    )

    with open(file_path, 'r') as file:
        for line in file:
            match = pattern.search(line)
            if match:
                metrics.append(match.groupdict())

    return metrics

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python3 extract.py <file_path> <key1> [key2 ...]")
        sys.exit(1)

    file_path = sys.argv[1]
    selected_keys = sys.argv[2:]  # 指定されたキーのリスト
    
    metrics = extract_performance_metrics(file_path)
    if metrics:
        # 指定されたキーが全て有効か確認
        valid_keys = set(metrics[0].keys())
        invalid_keys = [k for k in selected_keys if k not in valid_keys]
        if invalid_keys:
            print(f"Error: Invalid keys: {', '.join(invalid_keys)}")
            print(f"Available keys: {', '.join(valid_keys)}")
            sys.exit(1)
            
        # ヘッダー行の出力
        print(','.join(selected_keys))
        
        # 各エントリの値を出力（選択されたキーのみ）
        for m in metrics:
            values = [str(m[key]) for key in selected_keys]
            print(','.join(values))
    else:
        print("No performance metrics found.")