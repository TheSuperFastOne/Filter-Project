import matplotlib.pyplot as plt

# Toggle this to choose which plot to show
show_mirrored = True  # Set to False to show original

# Read coordinates from merged_stuck_points.txt
xs, ys = [], []
xs_mirrored, ys_mirrored = [], []
with open("merged_stuck_points.txt") as f:
    for line in f:
        if "Stuck ball at" in line:
            parts = line.strip().split('(')[1].split(')')[0].split(',')
            x, y = float(parts[0]), float(parts[1])
            xs.append(x)
            ys.append(y)
            xs_mirrored.append(abs(4.25 - x))
            ys_mirrored.append(y)

fig, ax = plt.subplots(figsize=(7, 6))

if show_mirrored:
    ax.scatter(xs_mirrored, ys_mirrored, c='red', marker='x')
    ax.set_xlabel('Mirrored X Position (4.25-x)')
    ax.set_title('Mirrored Start Positions')
else:
    ax.scatter(xs, ys, c='blue', marker='o')
    ax.set_xlabel('X Position')
    ax.set_title('Original Start Positions')

ax.set_ylabel('Y Position')
ax.grid(True)

plt.tight_layout()
plt.show()