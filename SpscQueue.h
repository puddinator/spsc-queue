#include <mutex>
#include <optional>

template <typename T, size_t N>
class SpscQueue {
public:
	SpscQueue() {}

	bool push(const T& e) {
		std::lock_guard<std::mutex> lock(mtx);
		if ((tail + 1) % N == head) { // queue is full
			return false;
		}
		array[tail] = e;
		tail = (tail + 1) % N;
		return true;
	}

	std::optional<T> pop() {
		std::lock_guard<std::mutex> lock(mtx);
		if (head == tail) { // queue is empty
			return std::nullopt;
		}
		T value = std::move(array[head]);
		head = (head + 1) % N;
		return value;
	}

private:
//	T array[N];
	std::array<T, N> array;
	std::mutex mtx;
	int head = 0; // consumer reads from
	int tail = 0; // producer writes to
};