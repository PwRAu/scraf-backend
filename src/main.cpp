#include <iostream>
#include <memory>
#include <csignal>

#include <odb/database.hxx>
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

		// Credo che questo sia il modo in cui bisogna lavorare con l'ODB. 
		// Per lavorare su un oggetto mi serve il suo id.
		// Per poter ottenere l'id devo fare un paio di controlli;
		// se l'oggetto sul quale voglio lavorare esiste già nel database dovrò prendere l'id già esistente,
		// se invece l'oggetto non esiste devo crearlo e poi usare l'id assegnato.
		// L'id non dev'essere modificato dal mio programma (una chiave primaria è immutabile)
		// quindi lo dichiaro const. Dichiarandolo const però ho un problema:
		// come faccio a sapere se l'oggetto esiste già nel db prima ancora di crearlo?
		// Normalmente creerei studentPappacodaId vuoto, faccio tutti i controlli
		// e poi assegno il valore corretto alla variabile, ma facendo così dovrei dire addio a const.
		// Fortunatamente in C++ esistono le lambda, ovvero funzioni usa e getta che avevo già spiegato
		// in un commit che mannaggia a git non so dove caz sia finito... Vabbè, tanto non ho niente da fare, vero Fauster?
		//
		// Una lambda è una funzione usa e getta che posso dichiarare così:
		// [*captures*](*parameters*) {*body*}
		//
		// Le captures sono le variabili che la lambda può "catturare" dall'esterno;
		// in questo caso metto &, che sta a significare che la lambda può accedere a tutte le variabili esterne
		// per riferimento, quindi senza copie inutili e potenzialmente dannose. 
		// Se volessi dire esplicitamente che la lambda può accedere solamente alla variabile database
		// scriverei [&database].
		//
		// In parameters posso passare dei parametri come farei normalmente con una funzione,
		// che boh ok va bene si può fare ma a me non è mai servito
		// perché sveglia C++ ci sono già le captures che fanno la stessa indentica fucking cosa, quindi lascio vuoto.
		//
		// Infine in body scrivo il corpo della lamda, ez.
		// In questo caso nel corpo ci scrivo l'if che controlla se l'oggetto esiste già e agisce di conseguenza,
		// così che posso ritornare l'id direttamente durante l'inizializzazione e non dire addio al bellissimo const.
		// Ah, non so se l'ho già detto ma in C++ ci sono 48173 modi diversi per inizializzare una variabile, tipo:
		// std::string paolo("nazionale");
		// std::string paolo = "nazionale";
		// std::string paolo = std::string("nazionale");
		// std::string paolo = {"nazionale"};
		// std::string paolo {"nazionale"};
		// Per una serie di motivi che non ho voglia di spiegare la migliore inizializzazione è l'ultima,
		// quella con {"..."} e senza l'uguale. Quindi abituati a usare quella. Sempre. Solo per l'inizializzazione.
		// Anche il classicissimo for va scritto con {}, quindi sarebbe
		// for (int i {0}; i < nv; ++i) {
		//     ...
		// }
		// So che viene naturale scrivere int i = 0, ma è meglio usare {0}.
		const std::int64_t studentPappacodaId {[&]() {
			std::unique_ptr<student> studentResult {database->query_one<student>(odb::query<student>::mail == "andrea@pappacoda.it")};
			if (!studentResult) {
				student pappacoda {"andrea@pappacoda.it", "Andrea", "Pappacoda"};
				return database->persist(pappacoda);
			}
			else {
				return studentResult->getId();
			}
		}()};

		std::cerr << "L'id dello studente con la mail andrea@pappacoda.it è " << studentPappacodaId << '\n';

		// In pratica preparo prima una serie di robe da fare, ma non le faccio,
		// e poi con transaction.commit() le faccio effettivamente.
		// Questo mi permette di fare più cose in una sola volta, migliorando le prestazioni.
		transaction.commit();



		// Aggiornare un oggetto conoscendo già l'id
		{
			odb::transaction transactionn {database->begin()};
			std::unique_ptr<student> studentPappacoda {database->load<student>(studentPappacodaId)};
			studentPappacoda->name = "Tachi";
			database->update(*studentPappacoda);
			std::cout << "Ora il nome dello studente con id " << studentPappacoda->getId() << " è \"" << studentPappacoda->name.get() << "\"\n";
			transactionn.commit();
		}



		// Aggiornare un oggetto solo senza conoscere l'id
		{
			odb::transaction transactionn {database->begin()};
			std::unique_ptr<student> studentPappacoda {database->query_one<student>(odb::query<student>::mail == "andrea@pappacoda.it")};
			if (studentPappacoda) {
				studentPappacoda->name = "Andrea";
				database->update(*studentPappacoda);
				std::cout << "Ora il nome dello studente con id " << studentPappacoda->getId() << " è \"" << studentPappacoda->name.get() << "\"\n";
			}
			else {
				std::cout << "Non esiste nessuno studente con la mail \"andrea@pappacoda.it\"\n";
			}
			transactionn.commit();
		}
	}

	handleSignals();

	Http::listenAndServe<HelloHandler>(Address(Ipv4::any(), Port(10780)), Http::Endpoint::options().threads(1));
}
