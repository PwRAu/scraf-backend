#include <iostream>
#include <pistache/endpoint.h>
#include <dmitigr/pgfe.hpp>

using namespace Pistache;
namespace pgfe = dmitigr::pgfe;

static pgfe::Connection db_connection {pgfe::Connection_options{pgfe::Communication_mode::net}.net_hostname("postgres.barto.paoloap.ml").database("scraf").username("scraf").password("ollareollare")};

class HelloHandler : public Http::Handler {
public:
	// I should define the clone() function,
	// but since this http handler is really simple I can use the Pistache macro that does that for me
	HTTP_PROTOTYPE(HelloHandler)

	HelloHandler() {
		db_connection.connect();
	}

	~HelloHandler() override {
		db_connection.disconnect();
	}

	void onRequest([[maybe_unused]] const Http::Request& request, Http::ResponseWriter response) override {
		db_connection.perform([&response](auto&& row) {
			auto schoolCode {pgfe::to<std::string>(row[0])};
			response.send(Http::Code::Ok, "La tua scuola è " + schoolCode + '?');
		}, "SELECT code FROM school WHERE name LIKE '%Cobianchi%'");
	}
};

int main() {
	//pgfe::Connection connection {pgfe::Connection_options{pgfe::Communication_mode::net}.net_hostname("postgres.barto.paoloap.ml").database("scraf").username("scraf").password("ollareollare")};
	//connection.connect();
	//connection.perform("INSERT INTO tabella (payload) VALUES ('soos')");

	Http::listenAndServe<HelloHandler>(Address(Ipv4::any(), Port(10780)), Http::Endpoint::options().threads(1));
	// La funzione che c'è sopra è una scorciatoia per le righe di sotto
	// Address address(Ipv4::any(), Port(10780));
	// auto httpOptions = Http::Endpoint::options().threads(1);
	// Http::Endpoint server(address);
	// server.init(httpOptions);
	// server.setHandler(Http::make_handler<HelloHandler>());
	// server.serve();
}
