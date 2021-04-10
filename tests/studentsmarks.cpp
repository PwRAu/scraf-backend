#include <tests/common.hpp>
#include <gtest/gtest.h>
#include <scraf-backend/scraf.hpp>
#include <scraf-backend/scraf_curl.hpp>
#include <thread>

using namespace Pistache;
using namespace std::literals;
using namespace nlohmann;

//TEST STUDENTSMARKS RICHIESTA CORRETTA

//GET
TEST(StudentsMarks, GetStudentsMarks){
	const std::uint16_t port {getPort()};
	std::unique_ptr<FakeDatabase> database {std::make_unique<FakeDatabase>()};
	Http::Endpoint endpoint{{Ipv4::loopback(), Port(port)}};
	Scraf<std::unique_ptr<FakeDatabase>, FakeDbTransaction> scraf {database, endpoint, 1};

	std::jthread servingThread {[&]() {
		scraf.serve();
	}};

	ScrafCurl curl;

	curl.get(
		"localhost:" + std::to_string(port) + "/students/{studentId}/subjects/{subjectId}/marks?studentid=107&subjectid=1"
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Ok)
	);

	scraf.shutdown();
}


//TEST STUDENTSMARKS RICHIESTA ERRATA

//GET NO ID
TEST(StudentsMarks, GetStudentsMarksNoId){
	const std::uint16_t port {getPort()};
	std::unique_ptr<FakeDatabase> database {std::make_unique<FakeDatabase>()};
	Http::Endpoint endpoint{{Ipv4::loopback(), Port(port)}};
	Scraf<std::unique_ptr<FakeDatabase>, FakeDbTransaction> scraf {database, endpoint, 1};

	std::jthread servingThread {[&]() {
		scraf.serve();
	}};

	ScrafCurl curl;

	curl.get(
		"localhost:" + std::to_string(port) + "/students/{studentId}/subjects/{subjectId}/marks"
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Bad_Request)
	);

	scraf.shutdown();
}
