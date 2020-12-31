#include <iostream>
#include <memory>

#include <odb/database.hxx>
#include <odb/forward.hxx>
#include <odb/transaction.hxx>
#include <odb/pgsql/database.hxx>
#include "school.hpp"
#include "school_odb.hpp"
#include "student.hpp"
#include "student_odb.hpp"

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

		// Voglio stampare a schermo il codice della scuola il cui nome equivale a "IS L. Cobianchi".
		// (Da destra a sinistra) faccio una query al database e gli chiedo di restituirmi tutte le scuole
		// il cui nome sia uguale a "IS L. Cobianchi", e mi salvo il risultato della query in una variabile result.
		// Il tipo di result dev'essere un odb::result dell'oggetto che sto cercando, in questo caso school,
		// e così anche la query, che è una query di tipo school.
		odb::result<school> result {database->query<school>(odb::query<school>::name == "IS L. Cobianchi")};
		// Una volta aver ottenuto il risultato non posso stamparlo, perché non ho la certezza di aver ottenuto un singolo
		// oggetto school, in quando name non è UNIQUE o PRIMARY KEY. 
		// Per questo motivo, odb::result è una specie di array.
		// Per iterare attraverso l'array dei risultati uso un range based for loop, ovvero un for che itera su un range;
		// quindi quello che faccio è creare un for che per ad ogni iterazione su result crea un oggetto chiamato school,
		// e dato che il tipo di school non mi interessa metto auto e lascio che sia il compilatore a mettere il tipo al posto mio.
		// In più metto una & a fianco al tipo così che non venga effettivamente creato un oggetto ad ogni iterazione,
		// ma che l'oggetto sia un riferimento a quello contenuto in result, evitando copie inutili,
		// e metto const perché non ho bisogno di modificare school (è raccomandato mettere sempre const se possibile).
		// Infine, faccio l'output di del code di ogni oggetto school in result, e in questo esempio so già che in realtà ce n'è solo 1.
		for (const auto& school : result) {
			std::cout << "Il codice del Cobianchi è " << school.code << '\n';
		}

		student pappacoda {"andrea@pappacoda.it", "Andrea", "Pappacoda"};
		std::cerr << pappacoda.name.get() << '\n';

		// In pratica preparo prima una serie di robe da fare, ma non le faccio,
		// e poi con transaction.commit() le faccio effettivamente.
		// Questo mi permette di fare più cose in una sola volta, migliorando le prestazioni.
		transaction.commit();
	}
	Http::listenAndServe<HelloHandler>(Address(Ipv4::any(), Port(10780)), Http::Endpoint::options().threads(1));
}
