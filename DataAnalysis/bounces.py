#!/usr/bin/env python3
"""
bounces.py
----------
Reads bounce counts from output_thread_bounces0-15.txt,
plots a histogram (1 bin per integer), saves it as a PNG,
and writes a frequency CSV.
"""

from pathlib import Path
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from collections import Counter

# ---- 1) Locate files ----
folder = Path("../outputthreadtxts")
files = [folder / f"output_thread_bounces{i}.txt" for i in range(16)]

# ---- 2) Extract all numbers ----
bounces = []
for f in files:
    with f.open() as fh:
        for line in fh:
            s = line.strip()
            if s:
                bounces.append(int(s))

if not bounces:
    raise SystemExit("No bounce values found!")

# ---- 3) Histogram (integer bins) ----
min_v, max_v = min(bounces), 200
bins = np.arange(min_v - 0.5, max_v + 1.5, 1)

plt.figure(figsize=(10, 6), dpi=150)
plt.hist(bounces, bins=bins, edgecolor="black", color="steelblue")
plt.xlabel("Bounce Count")
plt.ylabel("Frequency")
plt.title("Bounce Histogram (1 bin per integer)")
plt.grid(axis="y", alpha=0.3)

# ✅ don't set plt.xticks(...) — let Matplotlib decide
plt.tight_layout()
plt.savefig("bounces_hist.png")
print("Saved histogram -> bounces_hist.png")

# ---- 4) Save frequency CSV ----
freqs = Counter(bounces)
df = pd.DataFrame(sorted(freqs.items()), columns=["Bounce", "Frequency"])
df.to_csv("bounces_freq.csv", index=False)
print("Saved frequencies -> bounces_freq.csv")
