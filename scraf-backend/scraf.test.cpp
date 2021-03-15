/*
 * Scraf class unit tests
 */

#include <gtest/gtest.h>
#include <scraf-backend/scraf.hpp>
#include <scraf-backend/scraf_curl.hpp>
#include <thread>

using namespace Pistache;
using namespace std::literals;
using namespace nlohmann;

class FakeDatabase {
public:
	bool begin() {
		return true;
	}

	template<typename type>
	void persist(type object) {}
};

class FakeDbTransaction {
public:
	explicit FakeDbTransaction(bool /*unused*/) {} 
	void commit() {}
};

std::uint16_t getPort() {
	static std::atomic<std::uint16_t> port {10780};
	return ++port;
}

TEST(CreateStudent, MailPasswordNameSurname) {
	const std::uint16_t port {getPort()};
	std::unique_ptr<FakeDatabase> database {std::make_unique<FakeDatabase>()};
	Http::Endpoint endpoint{{Ipv4::loopback(), Port(port)}};
	Scraf<std::unique_ptr<FakeDatabase>, FakeDbTransaction> scraf {database, endpoint, 1};

	std::jthread servingThread {[&]() {
		scraf.serve();
	}};

	ScrafCurl curl;

	curl.post(
		"localhost:" + std::to_string(port) + "/students",
		std::array{"Content-Type: application/json"sv},
		json{
			{"mail", "andrea@pappacoda.it"},
			{"password", "coconutnut1968"},
			{"name", "Andrea"},
			{"surname", "Pappacoda"}
		}.dump()
	);

	EXPECT_EQ(
		curl.getResponseCode(), 
		static_cast<long>(Http::Code::Created)
	);

	scraf.shutdown();
}

//test in cui la richiesta è effettuata senza il cognome = dovrebbe andare a buon fine
TEST(CreateStudent, MailPasswordNameWhithoutSurname) {
	const std::uint16_t port {getPort()};
	std::unique_ptr<FakeDatabase> database {std::make_unique<FakeDatabase>()};
	Http::Endpoint endpoint{{Ipv4::loopback(), Port(port)}};
	Scraf<std::unique_ptr<FakeDatabase>, FakeDbTransaction> scraf {database, endpoint, 1};

	std::jthread servingThread {[&]() {
		scraf.serve();
	}};

	ScrafCurl curl;

	curl.post(
		"localhost:" + std::to_string(port) + "/students",
		std::array{"Content-Type: application/json"sv},
		json{
			{"mail", "luca.lettini@libero.it"},
			{"password", "tuamammma68"},
			{"name", "Luca"}
		}.dump()
	);

	EXPECT_EQ(
		curl.getResponseCode(),
		static_cast<long>(Http::Code::Created)
	); //la richiesta effettuata senza il cognome dovrebbe riuscire perchè il cognome è opzionale

	scraf.shutdown();
}

// Test in cui la richiesta è effettuata senza l'email = dovrebbe non funzionare
TEST(CreateStudent, PasswordNameSurnameWhithoutEmail) {
	const std::uint16_t port {getPort()};
	std::unique_ptr<FakeDatabase> database {std::make_unique<FakeDatabase>()};
	Http::Endpoint endpoint{{Ipv4::loopback(), Port(port)}};
	Scraf<std::unique_ptr<FakeDatabase>, FakeDbTransaction> scraf {database, endpoint, 1};

	std::jthread servingThread {[&]() {
		scraf.serve();
	}};

	ScrafCurl curl;

	curl.post(
		"localhost:" + std::to_string(port) + "/students",
		std::array{"Content-Type: application/json"sv},
		json{
			{"password", "ciaociao22"},
			{"name", "Luca"},
			{"surname", "Lettini"}
		}.dump()
	);

	// La richiesta effettuata senza l'email dovrebbe dare uno status code che indica che la richiesta è incompleta
	EXPECT_EQ(
		curl.getResponseCode(),
		static_cast<long>(Http::Code::Bad_Request)
	);

	scraf.shutdown();
}

//test in cui la richiesta è effettuata senza la password = dovrebbe non funzionare
TEST(CreateStudent, MailNameSurnameWhithoutPassword) {
	const std::uint16_t port {getPort()};
	std::unique_ptr<FakeDatabase> database {std::make_unique<FakeDatabase>()};
	Http::Endpoint endpoint{{Ipv4::loopback(), Port(port)}};
	Scraf<std::unique_ptr<FakeDatabase>, FakeDbTransaction> scraf {database, endpoint, 1};

	std::jthread servingThread {[&]() {
		scraf.serve();
	}};

	ScrafCurl curl;

	curl.post(
		"localhost:" + std::to_string(port) + "/students",
		std::array{"Content-Type: application/json"sv},
		json{
			{"mail", "lettilu22@gmail.com"},
			{"name", "Marco"},
			{"surname", "IlSupremo"}
		}.dump()
	);

	// La richiesta effettuata senza la password dovrebbe dare uno status code che indica che la richiesta è incompleta
	EXPECT_EQ(
		curl.getResponseCode(),
		static_cast<long>(Http::Code::Bad_Request)
	);

	scraf.shutdown();
}

//test in cui la richiesta è effettuata senza il nome = dovrebbe non funzionare
TEST(CreateStudent, MailPasswordSurnameWhithoutName) {
	const std::uint16_t port {getPort()};
	std::unique_ptr<FakeDatabase> database {std::make_unique<FakeDatabase>()};
	Http::Endpoint endpoint{{Ipv4::loopback(), Port(port)}};
	Scraf<std::unique_ptr<FakeDatabase>, FakeDbTransaction> scraf {database, endpoint, 1};

	std::jthread servingThread {[&]() {
		scraf.serve();
	}};

	ScrafCurl curl;

	curl.post(
		"localhost:" + std::to_string(port) + "/students",
		std::array{"Content-Type: application/json"sv},
		json{
			{"mail", "lucalettini54@gmail.com"},
			{"password", "marcopolo22"},
			{"surname", "IlSupremo"}
		}.dump()
	);

	// La richiesta effettuata senza il nome dovrebbe dare uno status code che indica che la richiesta è incompleta
	EXPECT_EQ(
		curl.getResponseCode(),
		static_cast<std::int32_t>(Http::Code::Bad_Request)
	);

	scraf.shutdown();
}


TEST(GetStudents, GetStudentsNameLenghtLess3){
	const std::uint16_t port {getPort()};
	std::unique_ptr<FakeDatabase> database {std::make_unique<FakeDatabase>()};
	Http::Endpoint endpoint{{Ipv4::loopback(), Port(port)}};
	Scraf<std::unique_ptr<FakeDatabase>, FakeDbTransaction> scraf {database, endpoint, 1};

	std::jthread servingThread {[&]() {
		scraf.serve();
	}};

	ScrafCurl curl;

	curl.get(
		"localhost:" + std::to_string(port) + "/students?name=lu"
	);

	// La richiesta effettuata con il nome con lunghezza minore di 3 dovrebbe restituire bad request
	EXPECT_EQ(
		curl.getResponseCode(),
		static_cast<std::int32_t>(Http::Code::Bad_Request)
	);

	scraf.shutdown();
}

/* Malisssssimo Luca, i test li devi fare in base a quello che dev'essere fatto,
 * non quello che è stato implementato finora;
 * non ti deve interessare di quello che c'è a livello di implementazione.
TEST(PutStudents, PutMethodOnApi){
	const std::uint16_t port {getPort()};
	std::unique_ptr<FakeDatabase> database {std::make_unique<FakeDatabase>()};
	Http::Endpoint endpoint{{Ipv4::loopback(), Port(port)}};
	Scraf<std::unique_ptr<FakeDatabase>, FakeDbTransaction> scraf {database, endpoint, 1};
	std::jthread servingThread {[&]() {
		scraf.serve();
	}};
	EXPECT_EQ(
		cpr::Put(
			cpr::Url{"localhost:" + std::to_string(port) + "/students"},
			cpr::Header{{"Content-Type", "application/json"}},
			cpr::Body{json{
				{"mail", "andrea@pappacoda.it"},
				{"password", "coconutnut1968"},
				{"name", "Andrea"},
				{"surname", "Pappacoda"}
			}.dump()}
		).status_code, 
		static_cast<std::int32_t>(Http::Code::Method_Not_Allowed)); //la richiesta effettuata con Put non dovrebbe essere permessa perchè il metodo non è ancora stato implementato
	scraf.shutdown();
}
*/
