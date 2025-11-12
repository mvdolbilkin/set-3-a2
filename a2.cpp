#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <numeric>

// --- АЛГОРИТМЫ СОРТИРОВКИ ---

// Сортировка вставками (Insertion Sort)
void insertionSort(std::vector<int>& arr, int left, int right) {
    for (int i = left + 1; i <= right; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= left && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

// Функция слияния для Merge Sort
void merge(std::vector<int>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;

    std::vector<int> L(n1), R(n2);

    for (int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k++] = L[i++];
        } else {
            arr[k++] = R[j++];
        }
    }

    while (i < n1) {
        arr[k++] = L[i++];
    }
    while (j < n2) {
        arr[k++] = R[j++];
    }
}

// Стандартный рекурсивный Merge Sort
void standardMergeSort(std::vector<int>& arr, int left, int right) {
    if (left >= right) {
        return;
    }
    int mid = left + (right - left) / 2;
    standardMergeSort(arr, left, mid);
    standardMergeSort(arr, mid + 1, right);
    merge(arr, left, mid, right);
}

// Гибридный Merge Sort
void hybridMergeSort(std::vector<int>& arr, int left, int right, int threshold) {
    if (left >= right) {
        return;
    }
    if (right - left + 1 <= threshold) {
        insertionSort(arr, left, right);
    } else {
        int mid = left + (right - left) / 2;
        hybridMergeSort(arr, left, mid, threshold);
        hybridMergeSort(arr, mid + 1, right, threshold);
        merge(arr, left, mid, right);
    }
}


// --- ЭТАП 1: ПОДГОТОВКА ТЕСТОВЫХ ДАННЫХ ---

class ArrayGenerator {
public:
    // Генерирует массив со случайными значениями от minVal до maxVal
    static std::vector<int> generateRandomArray(int size, int minVal, int maxVal) {
        std::vector<int> arr(size);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(minVal, maxVal);
        for (int i = 0; i < size; ++i) {
            arr[i] = distrib(gen);
        }
        return arr;
    }

    // Генерирует массив, отсортированный в обратном порядке
    static std::vector<int> generateReversedArray(int size) {
        std::vector<int> arr(size);
        std::iota(arr.begin(), arr.end(), 1);
        std::reverse(arr.begin(), arr.end());
        return arr;
    }

    // Генерирует "почти" отсортированный массив
    static std::vector<int> generateAlmostSortedArray(int size, int swaps) {
        std::vector<int> arr(size);
        std::iota(arr.begin(), arr.end(), 1);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(0, size - 1);
        for (int i = 0; i < swaps; ++i) {
            std::swap(arr[distrib(gen)], arr[distrib(gen)]);
        }
        return arr;
    }
};


// --- ЭТАП 2 и 3: ЭМПИРИЧЕСКИЙ АНАЛИЗ ---

class SortTester {
private:
    // Запускает сортировку и замеряет время
    template<typename Func>
    static long long timeSort(Func sortFunc, std::vector<int>& arr) {
        auto start = std::chrono::high_resolution_clock::now();
        sortFunc(arr, 0, arr.size() - 1);
        auto elapsed = std::chrono::high_resolution_clock::now() - start;
        return std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    }

public:
    // Тестирует стандартный Merge Sort
    static double testStandardMergeSort(const std::vector<int>& sourceArr, int runs) {
        long long total_time = 0;
        for (int i = 0; i < runs; ++i) {
            std::vector<int> arr_copy = sourceArr;
            total_time += timeSort([](std::vector<int>& a, int l, int r){ standardMergeSort(a, l, r); }, arr_copy);
        }
        return static_cast<double>(total_time) / runs;
    }

    // Тестирует гибридный Merge Sort
    static double testHybridMergeSort(const std::vector<int>& sourceArr, int runs, int threshold) {
        long long total_time = 0;
        for (int i = 0; i < runs; ++i) {
            std::vector<int> arr_copy = sourceArr;
            total_time += timeSort([threshold](std::vector<int>& a, int l, int r){ hybridMergeSort(a, l, r, threshold); }, arr_copy);
        }
        return static_cast<double>(total_time) / runs;
    }
};


int main() {
    const int MAX_SIZE = 100000;
    const int RUNS_PER_TEST = 5; // Количество замеров для усреднения

    // Генерируем "большие" массивы
    auto masterRandom = ArrayGenerator::generateRandomArray(MAX_SIZE, 0, 6000);
    auto masterReversed = ArrayGenerator::generateReversedArray(MAX_SIZE);
    // Для почти отсортированного, сделаем 1% обменов от максимального размера
    auto masterAlmostSorted = ArrayGenerator::generateAlmostSortedArray(MAX_SIZE, MAX_SIZE / 100);

    // Пороги для гибридного алгоритма
    std::vector<int> thresholds = {5, 10, 20, 30, 50, 100};

    // Выводим заголовок для CSV
    std::cout << "DataType,SortType,ArraySize,Threshold,Time_us" << std::endl;

    for (int size = 500; size <= 30000; size += 500) { // Ограничим 30000 для скорости, можно поставить MAX_SIZE
        
        // --- Тестовые данные для текущего размера ---
        std::vector<int> randomArr(masterRandom.begin(), masterRandom.begin() + size);
        std::vector<int> reversedArr(masterReversed.begin(), masterReversed.begin() + size);
        std::vector<int> almostSortedArr(masterAlmostSorted.begin(), masterAlmostSorted.begin() + size);

        // --- Тестирование ---

        // Random
        double time_standard_random = SortTester::testStandardMergeSort(randomArr, RUNS_PER_TEST);
        std::cout << "Random,Standard," << size << ",0," << time_standard_random << std::endl;
        for (int th : thresholds) {
            double time_hybrid_random = SortTester::testHybridMergeSort(randomArr, RUNS_PER_TEST, th);
            std::cout << "Random,Hybrid," << size << "," << th << "," << time_hybrid_random << std::endl;
        }

        // Reversed
        double time_standard_reversed = SortTester::testStandardMergeSort(reversedArr, RUNS_PER_TEST);
        std::cout << "Reversed,Standard," << size << ",0," << time_standard_reversed << std::endl;
        for (int th : thresholds) {
            double time_hybrid_reversed = SortTester::testHybridMergeSort(reversedArr, RUNS_PER_TEST, th);
            std::cout << "Reversed,Hybrid," << size << "," << th << "," << time_hybrid_reversed << std::endl;
        }

        // Almost Sorted
        double time_standard_almost = SortTester::testStandardMergeSort(almostSortedArr, RUNS_PER_TEST);
        std::cout << "AlmostSorted,Standard," << size << ",0," << time_standard_almost << std::endl;
        for (int th : thresholds) {
            double time_hybrid_almost = SortTester::testHybridMergeSort(almostSortedArr, RUNS_PER_TEST, th);
            std::cout << "AlmostSorted,Hybrid," << size << "," << th << "," << time_hybrid_almost << std::endl;
        }
    }

    return 0;
}
