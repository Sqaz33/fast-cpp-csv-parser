import pandas as pd
import matplotlib.pyplot as plt

# Загрузка CSV
df = pd.read_csv('benchmark.csv')

# Приведение данных
df['Lines'] = df['Lines'].astype(float)
df['cpu_time_ms'] = df['cpu_time'] / 1e6  # перевод из наносекунд в миллисекунды
df['real_time_ms'] = df['real_time'] / 1e6

# Построение графика
plt.figure(figsize=(10, 6))
plt.plot(df['Lines'], df['cpu_time_ms'], marker='o', label='CPU время (мс)', color='blue')
plt.plot(df['Lines'], df['real_time_ms'], marker='x', label='Реальное время (мс)', color='green')

plt.xscale('log')
plt.yscale('log')
plt.xlabel("Количество строк (log-scale)")
plt.ylabel("Время (мс, log-scale)")
plt.title("Производительность LineReader (нагрузочное тестирование)")
plt.grid(True, which="both", ls="--", lw=0.5)
plt.legend()
plt.tight_layout()
plt.savefig("line_reader_performance.png")
plt.show()
