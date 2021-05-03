#include <tests/common.hpp>
#include <gtest/gtest.h>
#include <scraf-backend/scraf.hpp>
#include <libscrafurl/scrafurl.hpp>
#include <thread>

using namespace Pistache;
using namespace std::literals;
using namespace nlohmann;

//TEST TEACHERS RICHIESTA CORRETTA

//GET
TEST(teachers, GetTeachers) {
	const std::uint16_t port {getPort()};
	SCRAF_TEST_SERVER(port);

	Scrafurl curl;

	curl.get(
		"localhost:" + std::to_string(port) + "/teachers?name=Mirella"
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Ok)
	);
}

//POST
TEST(teachers, PostTeachers){
	const std::uint16_t port {getPort()};
	SCRAF_TEST_SERVER(port);

	Scrafurl curl;

	curl.post(
		"localhost:" + std::to_string(port) + "/teachers",
		json{
			{"mail", "fausto.bartolucci@edu.cobianchi.it"},
			{"password", "CBC349aa"},
			{"name", "Fausto"},
			{"surname", "Bartolucci"}
		}.dump(),
		"Content-Type: application/json"
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Created)
	);
}


//TEST TEACHERS RICHIESTA ERRATA

//GET LENGHTLESS3

TEST(teachers, GetTeachersLenghtLess3){
	const std::uint16_t port {getPort()};
	SCRAF_TEST_SERVER(port);

	Scrafurl curl;

	curl.get(
		"localhost:" + std::to_string(port) + "/teachers?name=Fa"
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Bad_Request)
	);
}

//GET NO NAME

TEST(teachers, GetTeachersNoName){
	const std::uint16_t port {getPort()};
	SCRAF_TEST_SERVER(port);

	Scrafurl curl;

	curl.get(
		"localhost:" + std::to_string(port) + "/teachers"
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Bad_Request)
	);
}

//POST

//NO COGNOME

TEST(teachers, PostTeachersNoSurname){
	const std::uint16_t port {getPort()};
	SCRAF_TEST_SERVER(port);

	Scrafurl curl;

	curl.post(
		"localhost:" + std::to_string(port) + "/teachers",
		json{
			{"mail", "fausto.bartolucci@edu.cobianchi.it"},
			{"password", "CBC349aa"},
			{"name", "Fausto"}
		}.dump(),
		"Content-Type: application/json"
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Bad_Request)
	);
}

//NO EMAIL
TEST(teachers, PostTeachersNoEmail) {
	const std::uint16_t port {getPort()};
	SCRAF_TEST_SERVER(port);

	Scrafurl curl;

	curl.post(
		"localhost:" + std::to_string(port) + "/teachers",
		json{
			{"password", "CBC349aa"},
			{"name", "Fausto"},
			{"surname", "Bartolucci"}
		}.dump(),
		"Content-Type: application/json"
	);

	// La richiesta effettuata senza l'email dovrebbe dare uno status code che indica che la richiesta è incompleta
	EXPECT_EQ(
		curl.getResponseCode(),
		static_cast<long>(Http::Code::Bad_Request)
	);
}

//NO PASSWORD

TEST(teachers, PostTeachersNoPassword) {
	const std::uint16_t port {getPort()};
	SCRAF_TEST_SERVER(port);

	Scrafurl curl;

	curl.post(
		"localhost:" + std::to_string(port) + "/teachers",
		json{
			{"mail", "fausto.bartolucci@edu.cobianchi.it"},
			{"name", "Fausto"},
			{"surname", "Bartolucci"}
		}.dump(),
		"Content-Type: application/json"
	);

	// La richiesta effettuata senza la password dovrebbe dare uno status code che indica che la richiesta è incompleta
	EXPECT_EQ(
		curl.getResponseCode(),
		static_cast<long>(Http::Code::Bad_Request)
	);
}


// NO NAME

TEST(teachers, PostTeachersNoName) {
	const std::uint16_t port {getPort()};
	SCRAF_TEST_SERVER(port);

	Scrafurl curl;

	curl.post(
		"localhost:" + std::to_string(port) + "/teachers",
		json{
			{"mail", "fausto.bartolucci@edu.cobianchi.it"},
			{"password", "CBC349aa"},
			{"surname", "Bartolucci"}
		}.dump(),
		"Content-Type: application/json"
	);

	// La richiesta effettuata senza il nome dovrebbe dare uno status code che indica che la richiesta è incompleta
	EXPECT_EQ(
		curl.getResponseCode(),
		static_cast<std::int32_t>(Http::Code::Bad_Request)
	);
}
