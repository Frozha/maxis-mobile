import numpy as np
import imageio.v2 as imageio
import re

# ---------------- CONFIG ----------------
log_path = "../logs/unmod/vis/50x50.vis.log"
gif_path = "../output/visualizations/mis_simulation_50x50_visualizer.gif"
frame_duration = 0.01   # seconds per frame
pause_seconds = 2

# ------------ COLOR PALETTE -------------
# Indexed palette (GIF-friendly)
CHAR_TO_IDX = {
    'r': 0,
    'g': 1,
    'b': 2,
    '.': 3,
    'x': 4
}

# RGB palette (max 256 colors)
PALETTE = np.array([
    [214,  39,  40],   # red
    [ 44, 160,  44],   # green
    [ 31, 119, 180],   # blue
    [255, 255, 255],   # empty
    [170, 170, 170],   # boundary
], dtype=np.uint8)

# ---------------- PARSE LOG ----------------
with open(log_path) as f:
    lines = f.read().splitlines()

rows, cols = map(int, lines[0].split())
rest = "\n".join(lines[1:])

states_raw = re.findall(r"\[\[(.*?)\]\]", rest, flags=re.DOTALL)

# ---------------- GIF STREAM ----------------
with imageio.get_writer(
    gif_path,
    mode="I",
    duration=5,#frame_duration,
    loop=0
) as writer:

    last_rgb = None

    for step, s in enumerate(states_raw):
        grid = np.zeros((rows, cols), dtype=np.uint8)

        row_strings = s.split("][")
        for i, row in enumerate(row_strings):
            for j, ch in enumerate(row):
                grid[i, j] = CHAR_TO_IDX.get(ch, 3)

        # Convert indexed grid → RGB image
        rgb = PALETTE[grid]
        writer.append_data(rgb)
        last_rgb = rgb

    # ---- pause at end ----
    pause_frames = int(pause_seconds / frame_duration)
    for _ in range(pause_frames):
        writer.append_data(last_rgb)

print("Saved GIF to:", gif_path)
