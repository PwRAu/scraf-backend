#include <tests/common.hpp>
#include <gtest/gtest.h>
#include <scraf-backend/scraf.hpp>
#include <scraf-backend/scraf_curl.hpp>
#include <thread>

using namespace Pistache;
using namespace std::literals;
using namespace nlohmann;

//TEST TEACHERID RICHIESTA CORRETTA

//GET
TEST(teacherid, GetTeachersId){
	const std::uint16_t port {getPort()};
	SCRAF_TEST_SERVER(port);

	ScrafCurl curl;

	curl.get(
		"localhost:" + std::to_string(port) + "/teachers/107"
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Ok)
	);
}

//PATCH
TEST(teacherid, PatchTeachersId){
	const std::uint16_t port {getPort()};
	SCRAF_TEST_SERVER(port);

	ScrafCurl curl;

	curl.patch(
		"localhost:" + std::to_string(port) + "/teachers/107",
		"Content-Type: application/json",
		json{
			{"teacherid", "107"}
		}.dump()
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Ok)
	);
}

//DELETE
TEST(teacherid, DeleteTeachersId){
	const std::uint16_t port {getPort()};
	SCRAF_TEST_SERVER(port);

	ScrafCurl curl;

	curl.deletee(
		"localhost:" + std::to_string(port) + "/teachers/107"
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Ok)
	);
}


//TEST TEACHERID RICHIESTA ERRATA




//GET no id
TEST(teacherid, GetTeachersIdNoId){
	const std::uint16_t port {getPort()};
	SCRAF_TEST_SERVER(port);

	ScrafCurl curl;

	curl.get(
		"localhost:" + std::to_string(port) + "/teachers"
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Bad_Request)
	);
}

//PATCH NO ID
TEST(teacherid, PatchTeachersIdNoId){
	const std::uint16_t port {getPort()};
	SCRAF_TEST_SERVER(port);

	ScrafCurl curl;

	curl.patch(
		"localhost:" + std::to_string(port) + "/teachers",
		"Content-Type: application/json",
		json{
		}.dump()
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Bad_Request)
	);
}

//DELETE NO ID
TEST(teacherid, DelteTeachersIdNoId){
	const std::uint16_t port {getPort()};
	SCRAF_TEST_SERVER(port);

	ScrafCurl curl;

	curl.deletee(
		"localhost:" + std::to_string(port) + "/teachers"
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Bad_Request)
	);
}
