#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <random>
#include <vector>
#include <windows.h>

//  Заполнение вектора случайными числами.
void vector_random(std::vector<int>& arr, int size_vector) {

	std::random_device rv;
	std::mt19937 gen(rv());
	std::uniform_int_distribution<> dist(0, size_vector);

	for (int i = 0, j = 0; i < size_vector; i++, j++) {

		arr.at(i) = dist(gen);

		if (j == size_vector / 30) {

			std::cout << ">";
			j = 0;
		}
	}
}

void summation(std::mutex& sum, std::vector<int>& arr, std::vector<int>& veck_1, std::vector<int>& veck_2, int size_vector, int flow_num, int i) {

	int bar = size_vector / flow_num / 40;

	COORD coord;
	coord.X = 0;
	coord.Y = i + 3;

	sum.lock();

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
	std::cout << "    " << i + 1 << "\t\t" << std::this_thread::get_id() << "\t [";

	sum.unlock();

	auto start = std::chrono::steady_clock::now();

	for (int j = 0, k = 0; i < size_vector; i += flow_num, j++) {

		arr.at(i) = veck_1.at(i) + veck_2.at(i);
		
		if (j == bar) {

			coord.X = k + 27;

			sum.lock();
			
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
			std::cout << ">";

			sum.unlock();

			j = 0;
			k++;
		}	
	}

	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<double> exe_time = end - start;

	coord.X = 67;
	
	sum.lock();
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
	std::cout << "] \t" << exe_time.count();

	sum.unlock();
}

//  Параллельное суммирование.
void addition(std::vector<int>& vect_1, std::vector <int>& vect_2, std::vector<std::thread>& flow, int size_vector, int flow_num) {

	std::mutex sum;

	std::vector <int> arr(size_vector);
	int j = 0;

	std::cout << "# потока   идентификатор \t\t\t\t\t     время выполнения \n";

	for (int i = 0; i < flow_num; i++) {

		flow.at(i) = std::thread(summation, std::ref(sum), std::ref(arr), std::ref(vect_1), std::ref(vect_2), size_vector, flow_num, i);
	}

	for (int i = 0; i < flow_num; i++) {

		if (flow.at(i).joinable()) {

			flow.at(i).join();
		}
	}

	std::cout << std::endl;
}

int main()
{
	setlocale(LC_ALL, "Russian");

	//  Размер вектора (длина расчёта)
	int size = 100000000;
	//  Количество потоков
	int flow_num = 4;

	std::vector<std::thread> flow(flow_num);

	std::vector<int> vect_1(size);
	std::vector<int> vect_2(size);

	std::cout << "Заполнение векторов: ";

	std::thread vt_1 (vector_random, std::ref(vect_1), size);
	std::thread vt_2 (vector_random, std::ref(vect_2), size);

	vt_1.join();
	vt_2.join();

	std::cout << "\n" << std::endl;

	addition(vect_1, vect_2, flow, size, flow_num);
}