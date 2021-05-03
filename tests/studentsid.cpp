#include <tests/common.hpp>
#include <gtest/gtest.h>
#include <scraf-backend/scraf.hpp>
#include <libscrafurl/scrafurl.hpp>
#include <thread>

using namespace Pistache;
using namespace std::literals;
using namespace nlohmann;

//TEST STUDENTSID RICHIESTA CORRETTA

//GET
TEST(studentsid, GetStudentsId){
	const std::uint16_t port {getPort()};
	SCRAF_TEST_SERVER(port);

	Scrafurl curl;

	curl.get(
		"localhost:" + std::to_string(port) + "/students/107"
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Ok)
	);
}

//PATCH
TEST(studentsid, PatchStudentsId){
	const std::uint16_t port {getPort()};
	SCRAF_TEST_SERVER(port);

	Scrafurl curl;

	curl.patch(
		"localhost:" + std::to_string(port) + "/students/107",
		json{
			{"studentid", "107"}
		}.dump(),
		"Content-Type: application/json"
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Ok)
	);
}

//DELETE
TEST(studentsid, DeleteStudentsId){
	const std::uint16_t port {getPort()};
	SCRAF_TEST_SERVER(port);

	Scrafurl curl;

	curl.deletee(
		"localhost:" + std::to_string(port) + "/students/107"
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Ok)
	);
}


//TEST STUDENTSID RICHIESTA ERRATA




//GET no id
TEST(studentsid, GetStudentsIdNoId){
	const std::uint16_t port {getPort()};
	SCRAF_TEST_SERVER(port);

	Scrafurl curl;

	curl.get(
		"localhost:" + std::to_string(port) + "/students"
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Bad_Request)
	);
}

//PATCH NO ID
TEST(studentsid, PatchStudentsIdNoId){
	const std::uint16_t port {getPort()};
	SCRAF_TEST_SERVER(port);

	Scrafurl curl;

	curl.patch(
		"localhost:" + std::to_string(port) + "/students",
		json{
		}.dump(),
		"Content-Type: application/json"
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Bad_Request)
	);
}

//DELETE NO ID
TEST(studentsid, DeleteStudentsIdNoId){
	const std::uint16_t port {getPort()};
	SCRAF_TEST_SERVER(port);

	Scrafurl curl;

	curl.deletee(
		"localhost:" + std::to_string(port) + "/students"
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Bad_Request)
	);
}
