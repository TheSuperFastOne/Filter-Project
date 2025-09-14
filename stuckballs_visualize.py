import matplotlib.pyplot as plt

# Read coordinates from output.txt
xs, ys = [], []
with open("output.txt") as f:
    for line in f:
        if "Stuck ball at" in line:
            parts = line.strip().split('(')[1].split(')')[0].split(',')
            x, y = float(parts[0]), float(parts[1])
            xs.append(x)
            ys.append(y)

plt.figure(figsize=(8, 6))
plt.scatter(xs, ys, c='red', marker='o', label='Stuck Ball Start')
plt.xlabel('X Position')
plt.ylabel('Y Position')
plt.title('Starting Positions of Stuck Balls')
plt.grid(True)
plt.legend(loc='center left', bbox_to_anchor=(1, 0.5))  # Move legend outside right
plt.tight_layout()
plt.show()
