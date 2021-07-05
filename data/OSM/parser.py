import numpy as np
import matplotlib.pyplot as plt

# Constants
DATASET_NAME = "OSM"
COLUMN_NAME = "Cell_IDs"

# Load data
data = np.fromfile(DATASET_NAME, dtype=np.uint64)
print("Data loaded into memory.")

# Remove the size info
data = data[1:]

# Count number of unique elements
unique_cnt = len(np.unique(data))
print("{:d} unique elements ({:2.2%}).".format(unique_cnt, unique_cnt / data.shape[0]))
with open("uniques_cnt", "w") as f:
    f.write("{:d} ({:2.2%})".format(unique_cnt, unique_cnt / data.shape[0]))

# Cleanup
np.random.shuffle(data)
print("Data cleaned.")

# Save to file
with open("{:s}.txt".format(COLUMN_NAME), "w") as f:
    fmt = "\n".join(["%u"] * data.shape[0])
    flat_data = fmt % tuple(data.ravel())
    f.write(flat_data)
print("Data saved to text format.")

# Generate histograms
plt.figure(figsize=(4, 4), dpi=144)
plt.hist(data, bins=50)
plt.title("Distribution of {:s}/{:s}".format(DATASET_NAME, COLUMN_NAME))
plt.savefig("hist_{:s}_{:s}.png".format(DATASET_NAME, COLUMN_NAME))
plt.clf()
print("Histograms generated.")
