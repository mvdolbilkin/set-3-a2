import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import sys

# --- Чтение и подготовка данных ---
try:
    df = pd.read_csv('results.csv')
except FileNotFoundError:
    print("Ошибка: Файл 'results.csv' не найден.")
    print("Пожалуйста, сначала запустите C++ программу для генерации данных.")
    sys.exit()

# Устанавливаем стиль для графиков
sns.set_theme(style="whitegrid")
data_types = df['DataType'].unique()

# ==============================================================================
# Этап 2: Эмпирический анализ стандартного алгоритма MERGE SORT
# ==============================================================================
print("--- Генерация графиков для Этапа 2 (Стандартный Merge Sort) ---")

# Фильтруем данные, оставляя только стандартную сортировку
standard_df = df[df['SortType'] == 'Standard'].copy()

for data_type in data_types:
    plt.figure(figsize=(14, 8))
    
    # Выбираем данные для конкретного типа
    subset = standard_df[standard_df['DataType'] == data_type]
    
    ax = sns.lineplot(data=subset, x='ArraySize', y='Time_us', marker='o', label='Standard Merge Sort')
    
    ax.set_title(f'Этап 2: Стандартный Merge Sort\nТип данных: "{data_type}"', fontsize=18, pad=20)
    ax.set_xlabel('Размер массива', fontsize=14)
    ax.set_ylabel('Среднее время (микросекунды)', fontsize=14)
    ax.legend()
    
    filename = f'chart_stage2_{data_type}.png'
    plt.tight_layout()
    plt.savefig(filename)
    plt.close() # Закрываем фигуру, чтобы освободить память
    print(f'График сохранен: {filename}')


# ==============================================================================
# Этап 3: Эмпирический анализ гибридного алгоритма MERGE+INSERTION SORT
# ==============================================================================
print("\n--- Генерация графиков для Этапа 3 (Гибридный Merge Sort) ---")

# Фильтруем данные, оставляя только гибридную сортировку
hybrid_df = df[df['SortType'] == 'Hybrid'].copy()

for data_type in data_types:
    plt.figure(figsize=(14, 8))
    
    # Выбираем данные для конкретного типа
    subset = hybrid_df[hybrid_df['DataType'] == data_type]
    
    # Создаем красивую подпись для легенды
    subset['Legend'] = subset['Threshold'].apply(lambda th: f'th = {int(th)}')
    
    ax = sns.lineplot(
        data=subset,
        x='ArraySize',
        y='Time_us',
        hue='Legend', # Разные линии для каждого порога
        palette='viridis',
        linewidth=2
    )

    ax.set_title(f'Этап 3: Гибридный Merge+Insertion Sort\nТип данных: "{data_type}"', fontsize=18, pad=20)
    ax.set_xlabel('Размер массива', fontsize=14)
    ax.set_ylabel('Среднее время (микросекунды)', fontsize=14)
    ax.legend(title='Порог (Threshold)')
    
    filename = f'chart_stage3_{data_type}.png'
    plt.tight_layout()
    plt.savefig(filename)
    plt.close() # Закрываем фигуру
    print(f'График сохранен: {filename}')

