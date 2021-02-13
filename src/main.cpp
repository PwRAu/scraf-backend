/*
* TDD - Test Driven Development
* https://www.youtube.com/watch?v=RoYljVOj2H8
* 
* Andrò ad usare la Constructor Injection,
* un tipo di Dependency Injection,
* che è un caso particolare di Inversion of Control.
* In pratica, la mia classe da testare non ha più
* un controllo completo dei suoi attributi, o meglio,
* io nella classe Scraf scrivo per esempio che
* essa ha un attributo "database" di tipo "Database",
* ma faccio sì che io al posto di un oggetto Database
* gli possa passare un oggetto TestDatabase, che
* assomiglia a un Database ma che viene usato solo per i test,
* senza che la classe si accorga della differenza.
*/

#include <iostream>
#include <memory>
#include <utility>
#include <csignal>
#include "object_pool.hpp"

#include <odb/database.hxx>
#include <odb/transaction.hxx>
#include <odb/pgsql/database.hxx>
#include "school.hpp"
#include "school_odb.hpp"
#include "student.hpp"
#include "student_odb.hpp"

#include <pistache/endpoint.h>
#include <pistache/router.h>

#include <simdjson.h>

#include <sodium.h>

using namespace Pistache;

template<typename Database = odb::database, typename Endpoint = Http::Endpoint>
class Scraf {
public:
	Scraf(Database& database, Endpoint& endpoint, int threads)
		: database(database), endpoint(endpoint) {
		for (int i {0}; i < threads; ++i) {
			parserPool.push(std::make_unique<simdjson::dom::parser>());
		}
		endpoint.init(Http::Endpoint::options().threads(threads));
		Rest::Routes::Post(router, "/students", Rest::Routes::bind(&Scraf::createStudent, this));
		endpoint.setHandler(router.handler());
		endpoint.serve();
	}

private:
	void createStudent(const Rest::Request& request, Http::ResponseWriter response) {
		std::unique_ptr<simdjson::dom::parser> parser;
		if (!parserPool.waitPop(parser)) {
			std::cerr << "Error: parserPool\n";
		}
		const simdjson::dom::element parsed {parser->parse(request.body())};
		std::array<char, crypto_pwhash_STRBYTES> hashedPassword;
		const std::string_view password {parsed["password"]};
		if (crypto_pwhash_str(hashedPassword.data(), password.data(), password.length(), crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
			std::cerr << "Error: password hashing\n";
		} 
		student student {[&]() -> class student {
			std::string_view surname;
			if (parsed["surname"].get(surname)) {
				return {parsed["mail"].get_c_str().value(), hashedPassword.data(), parsed["name"].get_c_str().value()};
			}
			else {
				return {parsed["mail"].get_c_str().value(), hashedPassword.data(), parsed["name"].get_c_str().value(), surname.data()};
			}
		}()};
		{
			odb::transaction transaction(database->begin());
			database->persist(student);
			transaction.commit();
		}
		parserPool.push(std::move(parser));
		//std::cerr << parsed["mail"].get_c_str().value() << '\n' << hashedPassword.data() << '\n';

		response.send(Http::Code::Ok, "Beeella Francolino\n");
	}

private:
	Database& database;
	Endpoint& endpoint;
	Rest::Router router;
	// Pool thread-safe di parser
	ObjectPool<std::unique_ptr<simdjson::dom::parser>> parserPool;
};

void handleSignals() {
	struct sigaction sigIntHandler { .sa_flags = 0 };
	sigIntHandler.sa_handler = [](int signal) {
		std::cerr << "\nCaught signal " << signal << '\n';
		std::exit(EXIT_SUCCESS);
	};
	sigemptyset(&sigIntHandler.sa_mask);
	sigaction(SIGINT, &sigIntHandler, nullptr);
}

int main() {
	std::unique_ptr<odb::database> database {new odb::pgsql::database{"scraf", "ollareollare", "scraf_prova", "postgres.barto.paoloap.ml", 5432}};
	Http::Endpoint endpoint{{Ipv4::any(), Port(10780)}};
	handleSignals();
	Scraf scraf {database, endpoint, 1};
}
