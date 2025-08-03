import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# Load vector benchmark CSV
df = pd.read_csv('seal_benchmark_log.csv')

# Clean operation names for better readability
df['OpType'] = df['Operation'].apply(lambda x: x.replace("_", " "))

# Set seaborn style
sns.set(style="whitegrid")

# Create the plot
plt.figure(figsize=(12, 6))
sns.barplot(data=df, x="PolyModulusDegree", y="Time(ms)", hue="OpType")

# Labels and formatting
plt.title("Vector Homomorphic Operations Benchmark")
plt.xlabel("Polynomial Modulus Degree")
plt.ylabel("Execution Time (ms, log scale)")
plt.yscale("log")
plt.xticks(rotation=45)
plt.tight_layout()

# Save and show the plot
plt.savefig("vector_benchmark_results.png", dpi=300, bbox_inches="tight")
plt.show()
