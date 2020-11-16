#include "dmitigr/pgfe/conversions_api.hpp"
#include <iostream>
#include <memory>
#include <pistache/endpoint.h>
#include <dmitigr/pgfe.hpp>

using namespace Pistache;
namespace pgfe = dmitigr::pgfe;
//using pgfe = dmitigr::pgfe;

class HelloHandler : public Http::Handler {
public:
	// I should define the clone() function,
	// but since this http handler is really simple I can use the Pistache macro that does that for me
	HTTP_PROTOTYPE(HelloHandler)

	void onRequest([[maybe_unused]] const Http::Request& request, Http::ResponseWriter response) override {
		response.send(Http::Code::Ok, "Luca suca\n");
	}
};

int main() {
	pgfe::Connection connection {pgfe::Connection_options{pgfe::Communication_mode::net}.net_hostname("localhost").database("scraf").username("tachi").password("pone666")};
	connection.connect();
	//connection.perform("INSERT INTO tabella (payload) VALUES ('soos')");

	Http::listenAndServe<HelloHandler>(Address(Ipv4::any(), Port(10780)), Http::Endpoint::options().threads(1));
	// La funzione che c'è sopra è una scorciatoia per le righe di sotto
	// Address address(Ipv4::any(), Port(10780));
	// auto httpOptions = Http::Endpoint::options().threads(1);
	// Http::Endpoint server(address);
	// server.init(httpOptions);
	// server.setHandler(Http::make_handler<HelloHandler>());
	// server.serve();
	connection.disconnect();
}
