#include <iostream>
#include <thread>
#include <chrono>

#include "SpscQueue.h"
#include "SpscQueue2.h"
#include "SpscQueue3.h"
#include "SpscQueue4.h"
#include "SpscQueue5.h"

constexpr int BENCHMARK_SIZE = 5'000'000;
const std::string LOREM = "orem ipsum (/ˌlɔː.rəm ˈɪp.səm/ LOR-əm IP-səm) is a dummy or placeholder text commonly used in graphic design, publishing, and web development. Its purpose is to permit a page layout to be designed, independently of the copy that will subsequently populate it, or to demonstrate various fonts of a typeface without meaningful text that could be distracting.\n Lorem ipsum is typically a corrupted version of De finibus bonorum et malorum, a 1st-century BC text by the Roman statesman and philosopher Cicero, with words altered, added, and removed to make it nonsensical and improper Latin. The first two words are the truncation of dolorem ipsum (\"pain itself\").\n Versions of the Lorem ipsum text have been used in typesetting since the 1960s, when advertisements for Letraset transfer sheets popularized it.[1] Lorem ipsum was introduced to the digital world in the mid-1980s, when Aldus employed it in graphic and word-processing templates for its desktop publishing program PageMaker. Other popular word processors, including Pages and Microsoft Word, have since adopted Lorem ipsum,[2] as have many LaTeX packages,[3][4][5] web content managers such as Joomla! and WordPress, and CSS libraries such as Semantic UI.";

template <typename T>
struct Benchmark {
	Benchmark() {
		T q;

		std::vector<std::string> data(BENCHMARK_SIZE, LOREM);

		auto start = std::chrono::steady_clock::now();

		std::thread producer([&q, &data] {
		  for (int i = 0; i < BENCHMARK_SIZE;) {
			  if (q.push(std::move(data[i]))) {
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

template <typename T>
struct BenchmarkBlocking {
	BenchmarkBlocking() {
		T q;

		std::vector<std::string> data(BENCHMARK_SIZE, LOREM);

		auto start = std::chrono::steady_clock::now();

		std::thread producer([&q, &data] {
		  for (int i = 0; i < BENCHMARK_SIZE; ++i) {
			  q.push(std::move(data[i]));
		  }
		});

		std::thread consumer([&q] {
		  for (int i = 0; i < BENCHMARK_SIZE; ++i) {
			  std::string res = q.pop();
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
	Benchmark<SpscQueue4<std::string, 1 << 15>> bm1;
	BenchmarkBlocking<SpscQueue5<std::string, 1 << 15>> bm2;
	return 0;
}
