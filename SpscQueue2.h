#include <mutex>
#include <optional>

// perfect-forwarding for push()
template <typename T, size_t N>
class SpscQueue2 {
public:
	template <typename U>
	requires (std::is_assignable_v<T&, U&&>)
	bool push(U&& e) {
		std::lock_guard<std::mutex> lock(mtx);
		if ((tail_ + 1) % N == head_) { // queue is full
			return false;
		}
		buffer_[tail_] = std::forward<U>(e);
		tail_ = (tail_ + 1) % N;
		return true;
	}

	std::optional<T> pop() {
		std::lock_guard<std::mutex> lock(mtx);
		if (head_ == tail_) { // queue is empty
			return std::nullopt;
		}
		T value = std::move(buffer_[head_]);
		head_ = (head_ + 1) % N;
		return value;
	}

private:
	std::array<T, N> buffer_;
	std::mutex mtx;
	size_t head_ = 0; // consumer reads from
	size_t tail_ = 0; // producer writes to
};