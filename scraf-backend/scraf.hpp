#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <scraf-backend/object_pool.hpp>

#include <odb/database.hxx>
#include <odb/transaction.hxx>
#include <odb/pgsql/database.hxx>
#include "school.hpp"
#include "school_odb.hpp"
#include "student.hpp"
#include "student_odb.hpp"

#include <pistache/endpoint.h>
#include <pistache/router.h>

#include <simdjson.h>
#include <nlohmann/json.hpp>

#include <sodium.h>

using namespace Pistache;

template<typename Database = odb::database, typename Endpoint = Http::Endpoint>
class Scraf {
public:
	Scraf(Database& database, Endpoint& endpoint, int threads)
		: database(database), endpoint(endpoint) {
		for (int i {0}; i < threads; ++i) {
			parserPool.push(std::make_unique<simdjson::dom::parser>());
		}
		endpoint.init(Http::Endpoint::options().threads(threads));
		Rest::Routes::Post(router, "/students", Rest::Routes::bind(&Scraf::createStudent, this));
		endpoint.setHandler(router.handler());
		endpoint.serve();
	}

private:
	void createStudent(const Rest::Request& request, Http::ResponseWriter response) {
		std::unique_ptr<simdjson::dom::parser> parser;
		if (!parserPool.waitPop(parser)) {
			std::cerr << "Error: parserPool\n";
		}
		const simdjson::dom::element parsed {parser->parse(request.body())};
		std::array<char, crypto_pwhash_STRBYTES> hashedPassword;
		const std::string_view password {parsed["password"]};
		if (crypto_pwhash_str(hashedPassword.data(), password.data(), password.length(), crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
			std::cerr << "Error: password hashing\n";
		} 
		student student {[&]() -> class student {
			std::string_view surname;
			if (parsed["surname"].get(surname)) {
				return {parsed["mail"].get_c_str().value(), hashedPassword.data(), parsed["name"].get_c_str().value()};
			}
			else {
				return {parsed["mail"].get_c_str().value(), hashedPassword.data(), parsed["name"].get_c_str().value(), surname.data()};
			}
		}()};
		{
			odb::transaction transaction(database->begin());
			database->persist(student);
			transaction.commit();
		}
		parserPool.push(std::move(parser));
		response.send(Http::Code::Created, nlohmann::json{{"id", student.getId()}}.dump());
	}

private:
	Database& database;
	Endpoint& endpoint;
	Rest::Router router;
	// Pool thread-safe di parser
	ObjectPool<std::unique_ptr<simdjson::dom::parser>> parserPool;
};
