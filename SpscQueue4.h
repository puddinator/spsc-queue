#include <mutex>
#include <optional>

//https://www.linkedin.com/pulse/spsc-queue-part-2-going-atomic-sarthak-sehgal-p805c/?trackingId=nxNPBoHYTv2QLy7Xunb74A%3D%3D

// optimized memory ordering
template <typename T, size_t N>
class SpscQueue4 {
public:
	template <typename U>
	requires (std::is_assignable_v<T&, U&&>)
	bool push(U&& e) {
		size_t head = head_.load(std::memory_order_acquire);
		size_t t = tail_.load(std::memory_order_relaxed);
		size_t next = (t + 1) % N;
		if (next == head) {
			// queue is full
			return false;
		}
		buffer_[t] = std::forward<U>(e);
		tail_.store(next, std::memory_order_release);
		return true;
	}

	std::optional<T> pop() {
		size_t t = tail_.load(std::memory_order_acquire);
		size_t h = head_.load(std::memory_order_relaxed);
		if (h == t) {
			// queue is empty
			return std::nullopt;
		}
		T value = std::move(buffer_[h]);
		size_t next = (h + 1) % N;
		head_.store(next, std::memory_order_release);
		return value;
	}

private:
	alignas(64) std::array<T, N> buffer_;
	alignas(64) std::atomic<size_t> head_ = 0; // consumer reads from
	alignas(64) std::atomic<size_t> tail_ = 0; // producer writes to
};