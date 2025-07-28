#include <iostream>
#include <thread>
#include <chrono>

#include "SpscQueue.h"
#include "SpscQueue2.h"

constexpr int BENCHMARK_SIZE = 1'000'000'0;

template <typename T>
struct Benchmark {
	Benchmark() {
		T q;

		auto start = std::chrono::steady_clock::now();

		std::thread producer([&q] {
		  for (int i = 0; i < BENCHMARK_SIZE;) {
			  if (q.push(std::to_string(i))) {
				  ++i;
			  } else {
				  std::this_thread::yield();
			  }
		  }
		});

		std::thread consumer([&q] {
		  for (int i = 0; i < BENCHMARK_SIZE;) {
			  std::optional<std::string> res;
			  res = q.pop();
			  if (res) {
				  ++i;
			  } else {
				  std::this_thread::yield();
			  }
		  }
		});

		producer.join();
		consumer.join();

		auto end = std::chrono::steady_clock::now();
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		std::cout << "Processed " << BENCHMARK_SIZE << " items in " << ms << " ms\n";
	}
};

int main() {
	Benchmark<SpscQueue<std::string, 1 << 15>> bm1;
//	Benchmark<SpscQueue2<std::string, 1 << 15>> bm2;
	return 0;
}
