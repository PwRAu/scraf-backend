#include <tests/common.hpp>
#include <gtest/gtest.h>
#include <scraf-backend/scraf.hpp>
#include <scraf-backend/scraf_curl.hpp>
#include <thread>

using namespace Pistache;
using namespace std::literals;
using namespace nlohmann;

//TEST STUDENTS RICHIESTA CORRETTA

//GET
TEST(Students, GetStudents){
	const std::uint16_t port {getPort()};
	std::unique_ptr<FakeDatabase> database {std::make_unique<FakeDatabase>()};
	Http::Endpoint endpoint{{Ipv4::loopback(), Port(port)}};
	Scraf<std::unique_ptr<FakeDatabase>, FakeDbTransaction> scraf {database, endpoint, 1};

	std::jthread servingThread {[&]() {
		scraf.serve();
	}};

	ScrafCurl curl;

	curl.get(
		"localhost:" + std::to_string(port) + "/students",
		"Content-Type: application/json",
		json{
			{"name", "Andrea"}
		}.dump()
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Ok)
	);

	scraf.shutdown();
}

//POST
TEST(Students, PostStudents){
	const std::uint16_t port {getPort()};
	std::unique_ptr<FakeDatabase> database {std::make_unique<FakeDatabase>()};
	Http::Endpoint endpoint{{Ipv4::loopback(), Port(port)}};
	Scraf<std::unique_ptr<FakeDatabase>, FakeDbTransaction> scraf {database, endpoint, 1};

	std::jthread servingThread {[&]() {
		scraf.serve();
	}};

	ScrafCurl curl;

	curl.post(
		"localhost:" + std::to_string(port) + "/students",
		"Content-Type: application/json",
		json{
			{"mail", "andrea@pappacoda.it"},
			{"password", "coconutnut1968"},
			{"name", "Andrea"},
			{"surname", "Pappacoda"}
		}.dump()
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Created)
	);

	scraf.shutdown();
}


//TEST STUDENTS RICHIESTA ERRATA