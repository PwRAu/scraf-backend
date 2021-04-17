#include <tests/common.hpp>
#include <gtest/gtest.h>
#include <scraf-backend/scraf.hpp>
#include <scraf-backend/scraf_curl.hpp>
#include <thread>

using namespace Pistache;
using namespace std::literals;
using namespace nlohmann;

//TEST TEACHERSMARKS RICHIESTA CORRETTA

//GET
TEST(teachersmarks, GetTeachersMarks){
	const std::uint16_t port {getPort()};
	SCRAF_TEST_SERVER(port);

	ScrafCurl curl;

	curl.get(
		"localhost:" + std::to_string(port) + "/teachers/107/subjects/1/marks"
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Ok)
	);
}


//TEST TEACHERSMARKS RICHIESTA ERRATA

//GET NO ID
TEST(teachersmarks, GetTeachersMarksNoId){
	const std::uint16_t port {getPort()};
	SCRAF_TEST_SERVER(port);

	ScrafCurl curl;

	curl.get(
		"localhost:" + std::to_string(port) + "/teachers/subjects/marks"
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Bad_Request)
	);
}
