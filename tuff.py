merged_points = set()

for i in range(16):
    filename = f"output_thread{i}.txt"
    try:
        with open(filename, "r") as f:
            for line in f:
                if "Stuck ball at" in line:
                    merged_points.add(line.strip())
    except FileNotFoundError:
        print(f"Warning: {filename} not found, skipping.")

with open("merged_stuck_points.txt", "w") as out:
    for line in sorted(merged_points):
        out.write(line + "\n")

print(f"Merged {len(merged_points)} unique stuck points into merged_stuck_points.txt")