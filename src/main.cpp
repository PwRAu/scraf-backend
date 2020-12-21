#include <iostream>
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
	Http::listenAndServe<HelloHandler>(Address(Ipv4::any(), Port(10780)), Http::Endpoint::options().threads(1));
}
