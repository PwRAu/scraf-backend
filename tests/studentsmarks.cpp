#include <tests/common.hpp>
#include <gtest/gtest.h>
#include <scraf-backend/scraf.hpp>
#include <libscrafurl/scrafurl.hpp>
#include <thread>

using namespace Pistache;
using namespace std::literals;
using namespace nlohmann;

//TEST STUDENTSMARKS RICHIESTA CORRETTA

//GET
TEST(studentsmarks, GetStudentsMarks){
	const std::uint16_t port {getPort()};
	SCRAF_TEST_SERVER(port);

	Scrafurl curl;

	curl.get(
		"localhost:" + std::to_string(port) + "/students/107/subjects/1/marks"
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Ok)
	);
}


//TEST STUDENTSMARKS RICHIESTA ERRATA

//GET NO ID
TEST(studentsmarks, GetStudentsMarksNoId){
	const std::uint16_t port {getPort()};
	SCRAF_TEST_SERVER(port);

	Scrafurl curl;

	curl.get(
		"localhost:" + std::to_string(port) + "/students/subjects/marks"
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Bad_Request)
	);
}
