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
#include <csignal>

#include <odb/database.hxx>
#include <odb/pgsql/database.hxx>

#include <pistache/endpoint.h>

#include <scraf-backend/scraf.hpp>

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
