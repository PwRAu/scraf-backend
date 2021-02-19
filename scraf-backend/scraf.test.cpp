/*
 * Scraf class unit tests
 */

#include <gtest/gtest.h>
#include <scraf-backend/scraf.hpp>
#include <cpr/cpr.h>
#include <thread>

using namespace Pistache;

class FakeDatabase {
public:
	bool begin() {
		return true;
	}

	template<typename type>
	void persist(type object) {}
};

class FakeDbTransaction {
public:
	explicit FakeDbTransaction(bool /*unused*/) {} 
	void commit() {}
};

TEST(ScrafTest, MailPasswordNameSurname) {
	constexpr std::uint16_t port {10780};
	std::unique_ptr<FakeDatabase> database {std::make_unique<FakeDatabase>()};
	Http::Endpoint endpoint{{Ipv4::loopback(), Port(port)}};
	Scraf<std::unique_ptr<FakeDatabase>, FakeDbTransaction> scraf {database, endpoint, 1};
	std::thread servingThread {[&]() {
		scraf.serve();
	}};
	EXPECT_EQ(
		cpr::Post(
			cpr::Url{"localhost:" + std::to_string(port) + "/students"},
			cpr::Header{{"Content-Type", "application/json"}},
			cpr::Body{nlohmann::json{
				{"mail", "andrea@pappacoda.it"},
				{"password", "coconutnut1968"},
				{"name", "Andrea"},
				{"surname", "Pappacoda"}
			}.dump()}
		).status_code, 
		static_cast<std::int32_t>(Http::Code::Created));
	scraf.shutdown();
	servingThread.join();
}
