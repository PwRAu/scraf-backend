/*
 * Scraf class unit tests
 */

#include <gtest/gtest.h>
#include <scraf-backend/scraf.hpp>

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

class FakeRequest {
public:
	FakeRequest(const std::string_view body)
		: _body(body) {}

	[[nodiscard]] std::string body() const {
		return _body;
	}
private:
	std::string _body;
};

class FakeResponse {
public:
	void send(const Http::Code code, const std::string_view response) {
		_code = code;
		_response = response;
	}

	[[nodiscard]] std::pair<Http::Code, std::string> getResponse() const {
		return std::make_pair(_code, _response);
	}
private:
	Http::Code _code;
	std::string _response;
};

TEST(ScrafTest, MailPasswordNameSurname) {
	const FakeRequest request {nlohmann::json{
		{"mail", "andrea@pappacoda.it"},
		{"password", "coconutnut1968"},
		{"name", "Andrea"},
		{"surname", "Pappacoda"}
	}.dump()};
	FakeResponse response;
	std::unique_ptr<FakeDatabase> database {std::make_unique<FakeDatabase>()};
	Http::Endpoint endpoint{{Ipv4::any(), Port(10780)}};
	Scraf<std::unique_ptr<FakeDatabase>, FakeDbTransaction> scraf {database, endpoint, 1};
	scraf.createStudent(request, response);
	const auto[httpCode, httpResponse] {response.getResponse()};
	EXPECT_EQ(httpCode, Http::Code::Created);
}
