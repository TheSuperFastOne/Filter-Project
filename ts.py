import matplotlib.pyplot as plt

# Read coordinates from output.txt
xs, ys = [], []
xs_mirrored, ys_mirrored = [], []
with open("output.txt") as f:
    for line in f:
        if "Stuck ball at" in line:
            parts = line.strip().split('(')[1].split(')')[0].split(',')
            x, y = float(parts[0]), float(parts[1])
            xs.append(x)
            ys.append(y)
            xs_mirrored.append(abs(4.25 - x))
            ys_mirrored.append(y)

fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 6), sharey=True)

# Original
ax1.scatter(xs, ys, c='blue', marker='o')
ax1.set_xlabel('X Position')
ax1.set_ylabel('Y Position')
ax1.set_title('Original Start Positions')
ax1.grid(True)

# Mirrored
ax2.scatter(xs_mirrored, ys_mirrored, c='red', marker='x')
ax2.set_xlabel('Mirrored X Position (4.25-x)')
ax2.set_title('Mirrored Start Positions')
ax2.grid(True)

plt.tight_layout()
plt.show()