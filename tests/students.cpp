#include <tests/common.hpp>
#include <gtest/gtest.h>
#include <scraf-backend/scraf.hpp>
#include <libscrafurl/scrafurl.hpp>
#include <thread>

using namespace Pistache;
using namespace std::literals;
using namespace nlohmann;

//TEST STUDENTS RICHIESTA CORRETTA

//GET
TEST(students, GetStudents){
	const std::uint16_t port {getPort()};
	SCRAF_TEST_SERVER(port);

	Scrafurl curl;

	curl.get(
		"localhost:" + std::to_string(port) + "/students?name=Andrea"
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Ok)
	);
}

//POST
TEST(students, PostStudents){
	const std::uint16_t port {getPort()};
	SCRAF_TEST_SERVER(port);
	
	Scrafurl curl;

	curl.post(
		"localhost:" + std::to_string(port) + "/students",
		json{
			{"mail", "andrea@pappacoda.it"},
			{"password", "coconutnut1968"},
			{"name", "Andrea"},
			{"surname", "Pappacoda"}
		}.dump(),
		"Content-Type: application/json"
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Created)
	);
}

//POST WITHOUT SURNAME

TEST(students,PostStudentsNoSUrname) {
	const std::uint16_t port {getPort()};
	SCRAF_TEST_SERVER(port);

	Scrafurl curl;

	curl.post(
		"localhost:" + std::to_string(port) + "/students",
		json{
			{"mail", "luca.lettini@libero.it"},
			{"password", "tuamammma68"},
			{"name", "Luca"}
		}.dump(),
		"Content-Type: application/json"
	);

	EXPECT_EQ(
		curl.getResponseCode(),
		static_cast<long>(Http::Code::Created)
	); //la richiesta effettuata senza il cognome dovrebbe riuscire perchè il cognome è opzionale
}


//TEST STUDENTS RICHIESTA ERRATA

//GET LENGHTLESS3

TEST(students, GetStudentsLenghtLess3){
	const std::uint16_t port {getPort()};
	SCRAF_TEST_SERVER(port);

	Scrafurl curl;

	curl.get(
		"localhost:" + std::to_string(port) + "/students?name=Lu"
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Bad_Request)
	);
}

//GET NO NAME

TEST(students, GetStudentsNoName){
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



//NO EMAIL
TEST(students, PostStudentsNoEmail) {
	const std::uint16_t port {getPort()};
	SCRAF_TEST_SERVER(port);

	Scrafurl curl;

	curl.post(
		"localhost:" + std::to_string(port) + "/students",
		json{
			{"password", "ciaociao22"},
			{"name", "Luca"},
			{"surname", "Lettini"}
		}.dump(),
		"Content-Type: application/json"
	);

	// La richiesta effettuata senza l'email dovrebbe dare uno status code che indica che la richiesta è incompleta
	EXPECT_EQ(
		curl.getResponseCode(),
		static_cast<long>(Http::Code::Bad_Request)
	);

	scraf.shutdown();
}

//NO PASSWORD

TEST(students, PostStudentsNoPassword) {
	const std::uint16_t port {getPort()};
	SCRAF_TEST_SERVER(port);

	Scrafurl curl;

	curl.post(
		"localhost:" + std::to_string(port) + "/students",
		json{
			{"mail", "lettilu22@gmail.com"},
			{"name", "Marco"},
			{"surname", "IlSupremo"}
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

TEST(students, PostStudentsNoName) {
	const std::uint16_t port {getPort()};
	SCRAF_TEST_SERVER(port);

	Scrafurl curl;

	curl.post(
		"localhost:" + std::to_string(port) + "/students",
		json{
			{"mail", "lucalettini54@gmail.com"},
			{"password", "marcopolo22"},
			{"surname", "IlSupremo"}
		}.dump(),
		"Content-Type: application/json"
	);

	// La richiesta effettuata senza il nome dovrebbe dare uno status code che indica che la richiesta è incompleta
	EXPECT_EQ(
		curl.getResponseCode(),
		static_cast<std::int32_t>(Http::Code::Bad_Request)
	);
}
