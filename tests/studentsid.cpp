#include <tests/common.hpp>
#include <gtest/gtest.h>
#include <scraf-backend/scraf.hpp>
#include <scraf-backend/scraf_curl.hpp>
#include <thread>

using namespace Pistache;
using namespace std::literals;
using namespace nlohmann;

//TEST STUDENTSID RICHIESTA CORRETTA

//GET
TEST(studentsid, GetStudentsId){
	const std::uint16_t port {getPort()};
	std::unique_ptr<FakeDatabase> database {std::make_unique<FakeDatabase>()};
	Http::Endpoint endpoint{{Ipv4::loopback(), Port(port)}};
	Scraf<std::unique_ptr<FakeDatabase>, FakeDbTransaction> scraf {database, endpoint, 1};

	std::jthread servingThread {[&]() {
		scraf.serve();
	}};

	ScrafCurl curl;

	curl.get(
		"localhost:" + std::to_string(port) + "/students/107"
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Ok)
	);

	scraf.shutdown();
}

//PATCH
TEST(studentsid, PatchStudentsId){
	const std::uint16_t port {getPort()};
	std::unique_ptr<FakeDatabase> database {std::make_unique<FakeDatabase>()};
	Http::Endpoint endpoint{{Ipv4::loopback(), Port(port)}};
	Scraf<std::unique_ptr<FakeDatabase>, FakeDbTransaction> scraf {database, endpoint, 1};

	std::jthread servingThread {[&]() {
		scraf.serve();
	}};

	ScrafCurl curl;

	curl.patch(
		"localhost:" + std::to_string(port) + "/students/107"
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Ok)
	);

	scraf.shutdown();
}

//DELETE
TEST(studentsid, DeleteStudentsId){
	const std::uint16_t port {getPort()};
	std::unique_ptr<FakeDatabase> database {std::make_unique<FakeDatabase>()};
	Http::Endpoint endpoint{{Ipv4::loopback(), Port(port)}};
	Scraf<std::unique_ptr<FakeDatabase>, FakeDbTransaction> scraf {database, endpoint, 1};

	std::jthread servingThread {[&]() {
		scraf.serve();
	}};

	ScrafCurl curl;

	curl.delete(
		"localhost:" + std::to_string(port) + "/students/107"
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Ok)
	);

	scraf.shutdown();
}


//TEST STUDENTSID RICHIESTA ERRATA




//GET no id
TEST(studentsid, GetStudentsIdNoId){
	const std::uint16_t port {getPort()};
	std::unique_ptr<FakeDatabase> database {std::make_unique<FakeDatabase>()};
	Http::Endpoint endpoint{{Ipv4::loopback(), Port(port)}};
	Scraf<std::unique_ptr<FakeDatabase>, FakeDbTransaction> scraf {database, endpoint, 1};

	std::jthread servingThread {[&]() {
		scraf.serve();
	}};

	ScrafCurl curl;

	curl.get(
		"localhost:" + std::to_string(port) + "/students"
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Bad_Request)
	);

	scraf.shutdown();
}

//PATCH NO ID
TEST(studentsid, PatchStudentsIdNoId){
	const std::uint16_t port {getPort()};
	std::unique_ptr<FakeDatabase> database {std::make_unique<FakeDatabase>()};
	Http::Endpoint endpoint{{Ipv4::loopback(), Port(port)}};
	Scraf<std::unique_ptr<FakeDatabase>, FakeDbTransaction> scraf {database, endpoint, 1};

	std::jthread servingThread {[&]() {
		scraf.serve();
	}};

	ScrafCurl curl;

	curl.patch(
		"localhost:" + std::to_string(port) + "/students",
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Bad_Request)
	);

	scraf.shutdown();
}

//DELETE NO ID
TEST(studentsid, DeleteStudentsIdNoId){
	const std::uint16_t port {getPort()};
	std::unique_ptr<FakeDatabase> database {std::make_unique<FakeDatabase>()};
	Http::Endpoint endpoint{{Ipv4::loopback(), Port(port)}};
	Scraf<std::unique_ptr<FakeDatabase>, FakeDbTransaction> scraf {database, endpoint, 1};

	std::jthread servingThread {[&]() {
		scraf.serve();
	}};

	ScrafCurl curl;

	curl.delete(
		"localhost:" + std::to_string(port) + "/students"
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Bad_Request)
	);

	scraf.shutdown();
}
