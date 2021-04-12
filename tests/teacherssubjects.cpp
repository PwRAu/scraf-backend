#include <tests/common.hpp>
#include <gtest/gtest.h>
#include <scraf-backend/scraf.hpp>
#include <scraf-backend/scraf_curl.hpp>
#include <thread>

using namespace Pistache;
using namespace std::literals;
using namespace nlohmann;

//TEST TEACHERSSUBJECTS RICHIESTA CORRETTA

//GET
TEST(teacherssubjects, GetTeachersSubjects){
	const std::uint16_t port {getPort()};
	std::unique_ptr<FakeDatabase> database {std::make_unique<FakeDatabase>()};
	Http::Endpoint endpoint{{Ipv4::loopback(), Port(port)}};
	Scraf<std::unique_ptr<FakeDatabase>, FakeDbTransaction> scraf {database, endpoint, 1};

	std::jthread servingThread {[&]() {
		scraf.serve();
	}};

	ScrafCurl curl;

	curl.get(
		"localhost:" + std::to_string(port) + "/teachers/107/subjects"
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Ok)
	);

	scraf.shutdown();
}

//POST
TEST(teacherssubjects, PostTeachersSubjects){
	const std::uint16_t port {getPort()};
	std::unique_ptr<FakeDatabase> database {std::make_unique<FakeDatabase>()};
	Http::Endpoint endpoint{{Ipv4::loopback(), Port(port)}};
	Scraf<std::unique_ptr<FakeDatabase>, FakeDbTransaction> scraf {database, endpoint, 1};

	std::jthread servingThread {[&]() {
		scraf.serve();
	}};

	ScrafCurl curl;

	curl.post(
		"localhost:" + std::to_string(port) + "/teachers/107/subjects",
		"Content-Type: application/json",
		json{
			{"teacherid", "107"},
			{"subjectid", "1"}
		}.dump()
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Created)
	);

	scraf.shutdown();
}


//TEST TEACHERSSUBJECTS RICHIESTA ERRATA

//GET NO ID
TEST(teacherssubjects, GetTeachersSubjectsNoId){
	const std::uint16_t port {getPort()};
	std::unique_ptr<FakeDatabase> database {std::make_unique<FakeDatabase>()};
	Http::Endpoint endpoint{{Ipv4::loopback(), Port(port)}};
	Scraf<std::unique_ptr<FakeDatabase>, FakeDbTransaction> scraf {database, endpoint, 1};

	std::jthread servingThread {[&]() {
		scraf.serve();
	}};

	ScrafCurl curl;

	curl.get(
		"localhost:" + std::to_string(port) + "/teachers/subjects"
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Bad_Request)
	);

	scraf.shutdown();
}

//POST NO ID

TEST(teacherssubjects, PostTeachersSubjectsNoId){
	const std::uint16_t port {getPort()};
	std::unique_ptr<FakeDatabase> database {std::make_unique<FakeDatabase>()};
	Http::Endpoint endpoint{{Ipv4::loopback(), Port(port)}};
	Scraf<std::unique_ptr<FakeDatabase>, FakeDbTransaction> scraf {database, endpoint, 1};

	std::jthread servingThread {[&]() {
		scraf.serve();
	}};

	ScrafCurl curl;

	curl.post(
		"localhost:" + std::to_string(port) + "/teachers/subjects",
		"Content-Type: application/json",
		json{
		}.dump()
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Created)
	);

	scraf.shutdown();
}

