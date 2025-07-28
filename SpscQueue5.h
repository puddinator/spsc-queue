#include <mutex>
#include <optional>

// blocking with mutex
template <typename T, size_t N>
class SpscQueue5 {
public:
	template <typename U>
	requires (std::is_assignable_v<T&, U&&>)
	void push(U&& e) {
		std::unique_lock<std::mutex> lock(mtx);
		cv.wait(lock, [this] {return (tail_ + 1) % N != head_;});
		buffer_[tail_] = std::forward<U>(e);
		tail_ = (tail_ + 1) % N;
		cv.notify_one();
	}

	T pop() {
		std::unique_lock<std::mutex> lock(mtx);
		cv.wait(lock, [this] {return head_ != tail_;});
		T value = std::move(buffer_[head_]);
		head_ = (head_ + 1) % N;
		cv.notify_one();
		return value;
	}

private:
	std::array<T, N> buffer_;
	std::mutex mtx;
	std::condition_variable cv;
	size_t head_ = 0; // consumer reads from
	size_t tail_ = 0; // producer writes to
};