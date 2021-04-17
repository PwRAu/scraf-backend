#include <tests/common.hpp>
#include <gtest/gtest.h>
#include <scraf-backend/scraf.hpp>
#include <scraf-backend/scraf_curl.hpp>
#include <thread>

using namespace Pistache;
using namespace std::literals;
using namespace nlohmann;

//TEST STUDENTSSUBJECTS RICHIESTA CORRETTA

//GET
TEST(studentssubjects, GetStudentsSubjects) {
	const std::uint16_t port {getPort()};
	SCRAF_TEST_SERVER(port);

	ScrafCurl curl;

	curl.get(
		"localhost:" + std::to_string(port) + "/students/107/subjects"
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Ok)
	);
}


//TEST STUDENTSSUBJECTS RICHIESTA ERRATA

//GET NO ID
TEST(studentssubjects, GetStudentsSubjectsNoId){
	const std::uint16_t port {getPort()};
	SCRAF_TEST_SERVER(port);

	ScrafCurl curl;

	curl.get(
		"localhost:" + std::to_string(port) + "/students/subjects"
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Bad_Request)
	);
}
