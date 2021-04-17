/**
 * The ObjectPool class.
 * Provides a wrapper around a basic stack to provide thread safety.
 */
#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <stack>
#include <utility>

template<typename type> class ObjectPool {
public:
	~ObjectPool(){
		invalidate();
	}

	/**
	 * Attempt to get the first value in the stack.
	 * Returns true if a value was successfully written to the out parameter, false otherwise.
	 */
	bool tryPop(type* out){
		std::scoped_lock lock {_mutex};
		if (_stack.empty() || !_valid) {
			return false;
		}
		*out = std::move(_stack.top());
		_stack.pop();
		return true;
	}

	/**
	 * Get the first value in the stack.
	 * Will block until a value is available unless clear is called or the instance is destructed.
	 * Returns true if a value was successfully written to the out parameter, false otherwise.
	 */
	bool waitPop(type* out) {
		std::unique_lock<std::mutex> lock {_mutex};
		_condition.wait(lock, [this]() {
			return !_stack.empty() || !_valid;
		});
		/*
		 * Using the condition in the predicate ensures that spurious wakeups with a valid
		 * but empty stack will not proceed, so only need to check for validity before proceeding.
		 */
		if (!_valid) {
			return false;
		}
		*out = std::move(_stack.top());
		_stack.pop();
		return true;
	}

	/// Push a new value onto the stack.
	void push(type value) {
		std::scoped_lock lock {_mutex};
		_stack.push(std::move(value));
		_condition.notify_one();
	}

	/// Check whether or not the stack is empty.
	bool empty() const {
		std::scoped_lock lock {_mutex};
		return _stack.empty();
	}

	/// Clear all items from the stack.
	void clear() {
		std::scoped_lock lock {_mutex};
		while (!_stack.empty()) {
			_stack.pop();
		}
		_condition.notify_all();
	}

	/**
	 * Invalidate the stack.
	 * Used to ensure no conditions are being waited on in waitPop when
	 * a thread or the application is trying to exit.
	 * The stack is invalid after calling this method and it is an error
	 * to continue using a stack after this method has been called.
	 */
	void invalidate() {
		std::scoped_lock lock {_mutex};
		_valid = false;
		_condition.notify_all();
	}

	/// Returns whether or not this stack is valid.
	bool isValid() const {
		std::scoped_lock lock {_mutex};
		return _valid;
	}

private:
	std::atomic_bool _valid {true};
	mutable std::mutex _mutex;
	std::stack<type> _stack;
	std::condition_variable _condition;
};
