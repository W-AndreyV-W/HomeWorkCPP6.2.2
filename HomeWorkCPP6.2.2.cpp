#include <iostream>
#include <chrono>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <random>
#include <vector>
#include <windows.h>

//  Заполнение вектора случайными числами.
void vector_random(std::vector<int>& arr, int size_vector) {

	std::random_device rv;
	std::mt19937 gen(rv());
	std::uniform_int_distribution<> dist(0, size_vector);

	std::cout << "Заполнение вектора: ";

	for (int i = 0, j = 0; i < size_vector; i++, j++) {

		arr.at(i) = dist(gen);

		if (j == size_vector / 60) {

			std::cout << ">";
			j = 0;
		}
	}

	std::cout << "\n" << std::endl;
}

void summation(std::condition_variable& cond_var, std::mutex& sum, std::vector<int>& arr, std::vector<int>& veck_1, std::vector<int>& veck_2, int size_vector, int flow_num, int i) {

	int bar = size_vector / flow_num / 40;

	std::unique_lock lk(sum);
	cond_var.wait(lk, [&] {return !arr.empty(); });

	std::cout << "Номер потока: " << i + 1 << ", " << "идентификатор потока: " << std::this_thread::get_id() << "\t";

	auto start = std::chrono::steady_clock::now();

	for (int j = 0; i < size_vector; i += flow_num, j++) {

		arr.at(i) = veck_1.at(i) + veck_2.at(i);
		
		if (j == bar) {

			std::cout << ">";
			j = 0;
		}	
	}

	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<double> exe_time = end - start;

	std::cout << "\t время выполнения: " << exe_time.count() << "." << std::endl;

	lk.unlock();
}

//  Параллельное суммирование.
void addition(std::vector<int>& vect_1, std::vector <int>& vect_2, std::vector<std::thread>& flow, int size_vector, int flow_num) {

	std::condition_variable cond_var;
	std::mutex sum;

	std::vector <int> arr(size_vector);
	int j = 0;

	for (int i = 0; i < flow_num; i++) {

		flow.at(i) = std::thread(summation, std::ref(cond_var), std::ref(sum), std::ref(arr), std::ref(vect_1), std::ref(vect_2), size_vector, flow_num, i);

	}

	for (int i = 0; i < flow_num; i++) {

		if (flow.at(i).joinable()) {

			flow.at(i).join();
		}
	}
}

int main()
{
	setlocale(LC_ALL, "Russian");

	auto num_flow = std::thread::hardware_concurrency();

	//  Размер вектора (длина расчёта)
	int size = 100000000;
	//  Количество потоков
	int flow_num = 4;

	std::vector<std::thread> flow(flow_num);

	std::vector<int> vect_1(size);
	std::vector<int> vect_2(size);

	vector_random(vect_1, size);
	vector_random(vect_2, size);

	addition(vect_1, vect_2, flow, size, flow_num);
}