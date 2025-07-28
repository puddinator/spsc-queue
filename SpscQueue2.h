#include <mutex>
#include <optional>

// perfect-forwarding for push()
template <typename T, size_t N>
class SpscQueue2 {
public:
	SpscQueue2() {}

	template <typename U>
	requires (std::is_assignable_v<T&, U&&>)
	bool push(U&& e) {
		std::lock_guard<std::mutex> lock(mtx);
		if ((tail + 1) % N == head) { // queue is full
			return false;
		}
		array[tail] = std::forward<U>(e);
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
	std::array<T, N> array;
	std::mutex mtx;
	size_t head = 0; // consumer reads from
	size_t tail = 0; // producer writes to
};