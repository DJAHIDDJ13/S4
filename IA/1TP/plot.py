import matplotlib.pyplot as pp
import pandas as pd

data = pd.read_csv("output.csv", delimiter=":", names=["with_ab", "depth",
    "avg_moves", "avg_time", "avg_nodes"])

print(data)
