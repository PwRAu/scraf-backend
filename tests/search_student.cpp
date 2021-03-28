#include <tests/common.hpp>
#include <gtest/gtest.h>
#include <scraf-backend/scraf.hpp>
#include <scraf-backend/scraf_curl.hpp>
#include <thread>

using namespace Pistache;
using namespace std::literals;
using namespace nlohmann;

TEST(GetStudents, DISABLED_GetStudentsNameLenghtLess3){
	const std::uint16_t port {getPort()};
	std::unique_ptr<FakeDatabase> database {std::make_unique<FakeDatabase>()};
	Http::Endpoint endpoint{{Ipv4::loopback(), Port(port)}};
	Scraf<std::unique_ptr<FakeDatabase>, FakeDbTransaction> scraf {database, endpoint, 1};

	std::jthread servingThread {[&]() {
		scraf.serve();
	}};

	ScrafCurl curl;

	curl.get(
		"localhost:" + std::to_string(port) + "/students?name=lu"
	);

	// La richiesta effettuata con il nome con lunghezza minore di 3 dovrebbe restituire bad request
	EXPECT_EQ(
		curl.getResponseCode(),
		static_cast<std::int32_t>(Http::Code::Bad_Request)
	);

	scraf.shutdown();
}