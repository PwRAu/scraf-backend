#include <iostream>
#include <memory>

#include <odb/database.hxx>
#include <odb/forward.hxx>
#include <odb/transaction.hxx>
#include <odb/pgsql/database.hxx>
#include "school.hpp"
#include "school_odb.hpp"

#include <pistache/endpoint.h>

using namespace Pistache;

class HelloHandler : public Http::Handler {
public:
	// I should define the clone() function,
	// but since this http handler is really simple I can use the Pistache macro that does that for me
	HTTP_PROTOTYPE(HelloHandler)

	void onRequest([[maybe_unused]] const Http::Request& request, Http::ResponseWriter response) override {
		//db_connection.perform([&response](auto&& row) {
		//	auto schoolCode {pgfe::to<std::string>(row[0])};
		//	response.send(Http::Code::Ok, "La tua scuola è " + schoolCode + '?');
		//}, "SELECT code FROM school WHERE name LIKE '%Cobianchi%'");
	}
};

int main() {
	std::unique_ptr<odb::database> database(new odb::pgsql::database("scraf", "ollareollare", "scraf_prova", "postgres.barto.paoloap.ml", 5432));
	{
		// Creo degli oggetti scuola
		school cobianchi("VBIS00700V", "IS L. Cobianchi");
		school stein("VAIS01200Q", "E. Stein");

		// Creo la transazione, ovvero l'oggetto che si occuperà di inviare i cambiamenti al database
		odb::transaction transaction(database->begin());
		// I controlli con # vengono eseguiti prima della compilazione, e mi permettono di compilare del codice
		// in base a cose come se sto compilando il programma in Debug o Release;
		// se l'#if è false, il codice al suo interno viene ignorato.
		// In questo caso, sto dicendo "se NDEBUG non è definito, abilita il tracciamento dell'SQL";
		// NDEBUG viene definito automaticamente quando sto compilando in Release,
		// quindi questo codice sarà presente solo in non non-debug, quindi debug.
		#ifndef NDEBUG
		transaction.tracer(odb::stderr_tracer);
		#endif

		// Dico a ODB di rendere persistenti (salvare nel db) le scuole cobianchi e stein,
		// e poi faccio un commit dei cambiamenti, ovvero dico a ODB di salvarli effettivamente nel database.
		// In pratica preparo prima una serie di robe da fare, ma non le faccio,
		// e poi con transaction.commit() le faccio effettivamente.
		//  Questo mi permette di fare più cose in una sola volta, migliorando le prestazioni.
		database->persist(cobianchi);
		database->persist(stein);
		transaction.commit();
	}
	Http::listenAndServe<HelloHandler>(Address(Ipv4::any(), Port(10780)), Http::Endpoint::options().threads(1));
}
