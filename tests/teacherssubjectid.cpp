#include <tests/common.hpp>
#include <gtest/gtest.h>
#include <scraf-backend/scraf.hpp>
#include <scraf-backend/scraf_curl.hpp>
#include <thread>

using namespace Pistache;
using namespace std::literals;
using namespace nlohmann;

//TEST TEACHERSSUBJECTID RICHIESTA CORRETTA

//DELETE
TEST(teacherssubjectid, DeleteTeachersSubject){
	const std::uint16_t port {getPort()};
	SCRAF_TEST_SERVER(port);

	ScrafCurl curl;

	curl.deletee(
		"localhost:" + std::to_string(port) + "/teachers/107/subjects/1"
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Ok)
	);
}


//TEST TEACHERSSUBJECTID RICHIESTA ERRATA

//DELETE NO ID
TEST(teacherssubjectid, DeleteTeachersSubjectNoId){
	const std::uint16_t port {getPort()};
	SCRAF_TEST_SERVER(port);

	ScrafCurl curl;

	curl.deletee(
		"localhost:" + std::to_string(port) + "/teachers/subjects"
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Bad_Request)
	);
}
