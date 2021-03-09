/*
 * Scraf class unit tests
 */

#include <gtest/gtest.h>
#include <scraf-backend/scraf.hpp>
#include <cpr/cpr.h>
#include <thread>

using namespace Pistache;

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

TEST(ScrafTest, MailPasswordNameSurname) {
	const std::uint16_t port {getPort()};
	std::unique_ptr<FakeDatabase> database {std::make_unique<FakeDatabase>()};
	Http::Endpoint endpoint{{Ipv4::loopback(), Port(port)}};
	Scraf<std::unique_ptr<FakeDatabase>, FakeDbTransaction> scraf {database, endpoint, 1};
	std::thread servingThread {[&]() {
		scraf.serve();
	}};
	EXPECT_EQ(
		cpr::Post(
			cpr::Url{"localhost:" + std::to_string(port) + "/students"},
			cpr::Header{{"Content-Type", "application/json"}},
			cpr::Body{nlohmann::json{
				{"mail", "andrea@pappacoda.it"},
				{"password", "coconutnut1968"},
				{"name", "Andrea"},
				{"surname", "Pappacoda"}
			}.dump()}
		).status_code, 
		static_cast<std::int32_t>(Http::Code::Created));
	scraf.shutdown();
	servingThread.join();
}


//test in cui la richiesta è effettuata senza il cognome = dovrebbe andare a buon fine
TEST(ScrafTest, MailPasswordNameWhithoutSurname) {
	const std::uint16_t port {getPort()};
	std::unique_ptr<FakeDatabase> database {std::make_unique<FakeDatabase>()};
	Http::Endpoint endpoint{{Ipv4::loopback(), Port(port)}};
	Scraf<std::unique_ptr<FakeDatabase>, FakeDbTransaction> scraf {database, endpoint, 1};
	std::thread servingThread {[&]() {
		scraf.serve();
	}};
	EXPECT_EQ(
		cpr::Post(
			cpr::Url{"localhost:" + std::to_string(port) + "/students"},
			cpr::Header{{"Content-Type", "application/json"}},
			cpr::Body{nlohmann::json{
				{"mail", "luca.lettini@libero.it"},
				{"password", "tuamammma68"},
				{"name", "Luca"}
			
			}.dump()}
		).status_code, 
		static_cast<std::int32_t>(Http::Code::Created)); //la richiesta effettuata senza il cognome dovrebbe riuscire perchè il cognome è opzionale
	scraf.shutdown();
	servingThread.join();
}

//test in cui la richiesta è effettuata senza l'email = dovrebbe non funzionare
TEST(ScrafTest, PasswordNameSurnameWhithoutEmail) {
	const std::uint16_t port {getPort()};
	std::unique_ptr<FakeDatabase> database {std::make_unique<FakeDatabase>()};
	Http::Endpoint endpoint{{Ipv4::loopback(), Port(port)}};
	Scraf<std::unique_ptr<FakeDatabase>, FakeDbTransaction> scraf {database, endpoint, 1};
	std::thread servingThread {[&]() {
		scraf.serve();
	}};
	EXPECT_EQ(
		cpr::Post(
			cpr::Url{"localhost:" + std::to_string(port) + "/students"},
			cpr::Header{{"Content-Type", "application/json"}},
			cpr::Body{nlohmann::json{
				{"password", "ciaociao22"},
				{"name", "Luca"},
				{"surname", "Lettini"}
			}.dump()}
		).status_code, 
		static_cast<std::int32_t>(Http::Code::Not_Acceptable)); //la richiesta effettuata senza l'email dovrebbe dare uno status code che indica che la richiesta è incompleta
	scraf.shutdown();											//lo status code corrispondente al NOT_ACCEPTABLE è il 406
	servingThread.join();
}

//test in cui la richiesta è effettuata senza la password = dovrebbe non funzionare
TEST(ScrafTest, MailNameSurnameWhithoutPassword) {
	const std::uint16_t port {getPort()};
	std::unique_ptr<FakeDatabase> database {std::make_unique<FakeDatabase>()};
	Http::Endpoint endpoint{{Ipv4::loopback(), Port(port)}};
	Scraf<std::unique_ptr<FakeDatabase>, FakeDbTransaction> scraf {database, endpoint, 1};
	std::thread servingThread {[&]() {
		scraf.serve();
	}};
	EXPECT_EQ(
		cpr::Post(
			cpr::Url{"localhost:" + std::to_string(port) + "/students"},
			cpr::Header{{"Content-Type", "application/json"}},
			cpr::Body{nlohmann::json{
				{"mail", "lettilu22@gmail.com"},
				{"name", "Marco"},
				{"surname", "IlSupremo"}
			}.dump()}
		).status_code, 
		static_cast<std::int32_t>(Http::Code::Not_Acceptable)); //la richiesta effettuata senza la password dovrebbe dare uno status code che indica che la richiesta è incompleta
	scraf.shutdown();											//lo status code corrispondente al NOT_ACCEPTABLE è il 406
	servingThread.join();
}

//test in cui la richiesta è effettuata senza il nome = dovrebbe non funzionare
TEST(ScrafTest, MailPasswordSurnameWhithoutName) {
	const std::uint16_t port {getPort()};
	std::unique_ptr<FakeDatabase> database {std::make_unique<FakeDatabase>()};
	Http::Endpoint endpoint{{Ipv4::loopback(), Port(port)}};
	Scraf<std::unique_ptr<FakeDatabase>, FakeDbTransaction> scraf {database, endpoint, 1};
	std::thread servingThread {[&]() {
		scraf.serve();
	}};
	EXPECT_EQ(
		cpr::Post(
			cpr::Url{"localhost:" + std::to_string(port) + "/students"},
			cpr::Header{{"Content-Type", "application/json"}},
			cpr::Body{nlohmann::json{
				{"mail", "lucalettini54@gmail.com"},
				{"password", "marcopolo22"},
				{"surname", "IlSupremo"}
			}.dump()}
		).status_code, 
		static_cast<std::int32_t>(Http::Code::Not_Acceptable)); //la richiesta effettuata senza il nome dovrebbe dare uno status code che indica che la richiesta è incompleta
	scraf.shutdown();											//lo status code corrispondente al NOT_ACCEPTABLE è il 406
	servingThread.join();
}
