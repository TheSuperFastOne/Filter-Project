import matplotlib.pyplot as plt
import numpy as np
import csv

# OLD TUFF.PY
merged_points = set()

for i in range(16):
    filename = f"../outputthreadtxts/output_thread{i}.txt"
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

# OLD TUFF.PY END

# Toggle this to choose which plot to show
show_mirrored = True  # Set to False to show original

gapdist = 1
axis = (8+gapdist)/2

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
            xs_mirrored.append(abs(axis - x))
            ys_mirrored.append(y)

fig, ax = plt.subplots(figsize=(7, 6))

if show_mirrored:
    plot_xs, plot_ys = xs_mirrored, ys_mirrored
    ax.scatter(plot_xs, plot_ys, c='red', marker='x')
    ax.set_xlabel('Mirrored X Position')
    ax.set_title('Mirrored Start Positions')
else:
    plot_xs, plot_ys = xs, ys
    ax.scatter(plot_xs, plot_ys, c='blue', marker='o')
    ax.set_xlabel('X Position')
    ax.set_title('Original Start Positions')

ax.set_ylabel('Y Position')
ax.grid(True)

active = False  # Toggle for click event

def toolbar_active(fig):
    tb = getattr(fig.canvas.manager, "toolbar", None)
    return (tb is not None) and (tb.mode != "")   # '' means no tool selected

def on_click(event):
    if not active:
        return
    if event.inaxes != ax:
        return
    # 1) don't interfere with pan/zoom/zoom-to-rect
    if toolbar_active(fig):
        return
    # 2) optionally: only respond to right-click (3) or Shift+left
    # if event.button != 3 and not (event.button == 1 and event.key == 'shift'):
    #     return

    click_x, click_y = event.xdata, event.ydata
    dists = np.hypot(np.array(plot_xs) - click_x, np.array(plot_ys) - click_y)
    idx = np.argmin(dists)
    closest_x, closest_y = plot_xs[idx], plot_ys[idx]
    print(f"Closest point: ({closest_x}, {closest_y})")

def on_key(event):
    global active
    if event.key == 'w':
        active = not active
        state = "enabled" if active else "disabled"
        print(f"Click-to-select is now {state}.")

fig.canvas.mpl_connect('button_press_event', on_click)
fig.canvas.mpl_connect('key_press_event', on_key)

plt.tight_layout()
plt.savefig("stuck_plot.png")
plt.show()


# ---- Write CSV with x,y pairs ----
with open("stuck_points.csv", "w", newline="") as csvfile:
    writer = csv.writer(csvfile)
    for x, y in zip(plot_xs, plot_ys):
        writer.writerow([x, y])

print(f"Saved {len(plot_xs)} points into stuck_points.csv")
