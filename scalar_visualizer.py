import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

# Load scalar benchmark CSV
df = pd.read_csv('seal_scalar_benchmark_log.csv')

# Extract operation type
df['OpType'] = df['Operation'].apply(lambda x: x.replace("_Scalar", "").replace("_", " "))

# Set seaborn style
sns.set(style="whitegrid")

# Create plot
plt.figure(figsize=(12, 6))
sns.barplot(data=df, x="PolyModulusDegree", y="Time(ms)", hue="OpType")

plt.title("Scalar Homomorphic Operations Benchmark")
plt.xlabel("Polynomial Modulus Degree")
plt.ylabel("Execution Time (ms, log scale)")
plt.yscale("log")  # Log scale to handle small add times
plt.xticks(rotation=45)
plt.tight_layout()

# Save the plot
plt.savefig("scalar_benchmark_results.png", dpi=300, bbox_inches="tight")

# Show it
plt.show()

