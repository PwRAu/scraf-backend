#include <tests/common.hpp>
#include <gtest/gtest.h>
#include <scraf-backend/scraf.hpp>
#include <libscrafurl/scrafurl.hpp>
#include <thread>

using namespace Pistache;
using namespace std::literals;
using namespace nlohmann;

//TEST TEACHERSSUBJECTS RICHIESTA CORRETTA

//GET
TEST(teacherssubjects, GetTeachersSubjects){
	const std::uint16_t port {getPort()};
	SCRAF_TEST_SERVER(port);

	Scrafurl curl;

	curl.get(
		"localhost:" + std::to_string(port) + "/teachers/107/subjects"
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Ok)
	);
}

//POST
TEST(teacherssubjects, PostTeachersSubjects){
	const std::uint16_t port {getPort()};

	Scrafurl curl;

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
}


//TEST TEACHERSSUBJECTS RICHIESTA ERRATA

//GET NO ID
TEST(teacherssubjects, GetTeachersSubjectsNoId){
	const std::uint16_t port {getPort()};
	SCRAF_TEST_SERVER(port);

	Scrafurl curl;

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
	SCRAF_TEST_SERVER(port);

	Scrafurl curl;

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
}
