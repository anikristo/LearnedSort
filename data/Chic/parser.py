import numpy as np
import matplotlib.pyplot as plt
import pandas as pd


# Constants
DATASET_NAME = "Chic"
COL_NAMES = ["Start", "Tot"]
COL_DTYPES = [np.uint64, np.float64]

for i in range(len(COL_NAMES)):
    print("Processing {}/{}".format(DATASET_NAME, COL_NAMES[i]))

    # Load data
    data = pd.read_csv(
        "{}.txt".format(COL_NAMES[i]),
        dtype=COL_DTYPES[i],
        delimiter="\n",
        header=None,
        index_col=False,
    )
    print("Data loaded into memory.")

    # Count number of unique elements
    unique_cnt = data[0].nunique()
    print(
        "{:d} unique elements ({:2.2%}).".format(
            unique_cnt, unique_cnt / data[0].shape[0]
        )
    )
    with open("uniques_cnt_{}".format(COL_NAMES[i]), "w") as f:
        f.write("{:d} ({:2.2%})".format(unique_cnt, unique_cnt / data[0].shape[0]))

    # Generate histograms
    plt.figure(figsize=(4, 4), dpi=144)
    data.hist(bins=50, grid=False)
    plt.title("Distribution of {:s}/{:s}".format(DATASET_NAME, COL_NAMES[i]))
    plt.savefig("hist_{:s}_{:s}.png".format(DATASET_NAME, COL_NAMES[i]))
    plt.clf()
    print("Histograms generated.")
