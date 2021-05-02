#pragma once

#include <atomic>
#include <cstdint>

class FakeDatabase {
public:
	bool begin() {
		return true;
	}

	void update(auto /**/) {}

	template<typename type>
	type* load(auto /*id*/) { return nullptr; }

	template<typename type>
	void persist(type object) {}
};

class FakeDbTransaction {
public:
	explicit FakeDbTransaction(bool /*unused*/) {} 
	void commit() {}
};

inline std::uint16_t getPort() {
	static std::atomic<std::uint16_t> port {10780};
	return ++port;
}

#define SCRAF_TEST_SERVER(port)\
	std::unique_ptr<FakeDatabase> database {std::make_unique<FakeDatabase>()};\
	Http::Endpoint endpoint{{Ipv4::loopback(), Port(port)}};\
	Scraf<std::unique_ptr<FakeDatabase>, FakeDbTransaction> scraf {database, endpoint, 1};\
	std::jthread servingThread {[&]() {\
		scraf.serve();\
	}}
