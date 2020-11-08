#include <iostream>
#include <memory>
#include <pistache/endpoint.h>

using namespace Pistache;

class HelloHandler : public Http::Handler {
public:
	// I should define the clone() function,
	// but since this http handler is really simple I can use the Pistache macro that does that for me
	HTTP_PROTOTYPE(HelloHandler)

	void onRequest(const Http::Request& request, Http::ResponseWriter response) override {
		response.send(Http::Code::Ok, "Luca suca");
	}
};

int main() {
	Address address(Ipv4::any(), Port(10780));
	auto httpOptions = Http::Endpoint::options().threads(1);
	Http::Endpoint server(address);
	server.init(httpOptions);
	server.setHandler(std::make_shared<HelloHandler>());
	server.serve();
	// Or I can simply use Http::listenAndServe<HelloHandler>("*:10780", options) that sets up everything for me
}
