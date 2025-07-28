#include <mutex>
#include <optional>

// lock free
template <typename T, size_t N>
class SpscQueue3 {
public:
	template <typename U>
	requires (std::is_assignable_v<T&, U&&>)
	bool push(U&& e) {
		if ((tail_ + 1) % N == head_) { // queue is full
			return false;
		}
		buffer_[tail_] = std::forward<U>(e);
		tail_ = (tail_ + 1) % N;
		return true;
	}

	std::optional<T> pop() {
		if (head_ == tail_) { // queue is empty
			return std::nullopt;
		}
		T value = std::move(buffer_[head_]);
		head_ = (head_ + 1) % N;
		return value;
	}

private:
	std::array<T, N> buffer_;
	std::atomic<size_t> head_ = 0; // consumer reads from
	std::atomic<size_t> tail_ = 0; // producer writes to
};