import pandas as pd
import matplotlib.pyplot as plt
import re
import os

# === Настройки ===
csv_path = "second_bench.csv"
output_dir = "results"
os.makedirs(output_dir, exist_ok=True)

# === Загрузка и парсинг CSV ===
df = pd.read_csv(csv_path)

# Отфильтруем только интересующие нас строки (исключим header-блоки Google Benchmark)
df = df[~df["name"].str.contains("CPU|BM_") == False]

# Извлечём аргументы из имени теста (например, BM_Name/10000 -> 10000)
def extract_arg(test_name):
    match = re.search(r"/(\d+)$", test_name)
    return int(match.group(1)) if match else None

df["arg"] = df["name"].apply(extract_arg)

# Переведём время в миллисекунды
df["real_time_ms"] = df["real_time"] / 1e6
df["cpu_time_ms"] = df["cpu_time"] / 1e6

# === Группировка по тесту ===
grouped = df.groupby(df["name"].str.extract(r"^(BM_[^/]+)")[0])

# === Построение графиков ===
for name, group in grouped:
    group_sorted = group.sort_values("arg")

    plt.figure(figsize=(10, 6))
    plt.plot(group_sorted["arg"], group_sorted["real_time_ms"], marker='o', label='real_time')

    plt.title(f"Тест: {name}")
    plt.xlabel("Аргумент теста (потоки)")
    plt.ylabel("Время выполнения (мс)")
    plt.grid(True)
    plt.legend()
    plt.tight_layout()

    filename = os.path.join(output_dir, f"{name}_timing.png")
    plt.savefig(filename)
    print(f"[+] Сохранено: {filename}")
    plt.close()
