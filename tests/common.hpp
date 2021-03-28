#pragma once

#include <atomic>
#include <cstdint>

class FakeDatabase {
public:
	bool begin() {
		return true;
	}

	template<typename type>
	void persist(type object) {}
};

class FakeDbTransaction {
public:
	explicit FakeDbTransaction(bool /*unused*/) {} 
	void commit() {}
};

std::uint16_t getPort() {
	static std::atomic<std::uint16_t> port {10780};
	return ++port;
}
