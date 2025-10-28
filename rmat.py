import numpy as np

# Attempt to read the 2D gamma-gamma matrix from the uploaded .mat file (Radware format)
mat_file_path = "2-6.mat"

# Radware .mat files often use int16, big-endian with no header (or small header)
# Try reading as big-endian int16 assuming it's a square 2D matrix
with open(mat_file_path, 'rb') as f:
    data = f.read()

# Try interpreting entire content as int16 big-endian
raw_array = np.frombuffer(data, dtype='>i2')

# Try to infer matrix dimensions (square matrix: sqrt(N))
from math import isqrt

n_elements = raw_array.size
n_dim = isqrt(n_elements)

# Check if it forms a perfect square
if n_dim * n_dim == n_elements:
    matrix = raw_array.reshape((n_dim, n_dim))
else:
    matrix = None

matrix_clipped = np.clip(matrix, 0, None)

n_dim, matrix.shape if matrix is not None else "Invalid shape"

import matplotlib.pyplot as plt

# Example gate: project along Y axis where X is in channels 1200 to 1300
x_gate_start = 1200
x_gate_end = 1300

# Extract and sum the rows corresponding to the gate (i.e., gate on X)
projection = matrix[x_gate_start:x_gate_end+1, :].sum(axis=0)

# Plot the 1D projected spectrum
plt.figure(figsize=(10, 5))
plt.plot(projection)
plt.title(f"Gamma-Gamma Gated Spectrum (X = {x_gate_start} to {x_gate_end})")
plt.xlabel("Channel")
plt.ylabel("Counts")
plt.grid(True)
plt.tight_layout()
plt.show()


# Clip negative values to zero for visualization
matrix_visual = np.clip(matrix, 0, None)

# Plot the full 2D matrix as an image
plt.figure(figsize=(8, 6))
plt.imshow(matrix_visual, cmap='inferno', norm='linear', origin='lower')
plt.title("Full Gamma-Gamma Coincidence Matrix (4096×4096)")
plt.xlabel("Channel")
plt.ylabel("Channel")
plt.colorbar(label="Counts")
plt.tight_layout()
plt.show()
