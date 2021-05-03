#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <scraf-backend/object_pool.hpp>

#include <odb/database.hxx>
#include <odb/transaction.hxx>
#include <odb/pgsql/database.hxx>
#include <scraf-backend/school.hpp>
#include "school_odb.hpp"
#include <scraf-backend/student.hpp>
#include "student_odb.hpp"

#include <pistache/endpoint.h>
#include <pistache/router.h>

#include <simdjson.h>
#include <nlohmann/json.hpp>

#include <sodium.h>

#include <libscrafurl/scrafurl.hpp>

using namespace Pistache;
using namespace std::chrono_literals;
using namespace std::string_view_literals;

template<typename Database = std::unique_ptr<odb::database>, typename DbTransaction = odb::transaction>
class Scraf {
public:
	Scraf(Database& database, Http::Endpoint& endpoint, int threads)
		: database(database), endpoint(endpoint) {
		endpoint.init(Http::Endpoint::options().threads(threads));
		while (threads --> 0) {
			parserPool.push(std::make_unique<simdjson::dom::parser>());
			curlPool.push(std::make_unique<Scrafurl>());
		}

		Rest::Routes::Post(router, "/students", Rest::Routes::bind(&Scraf::createStudent, this));
		Rest::Routes::Patch(router, "/students/:id", Rest::Routes::bind(&Scraf::updateStudent, this));

		endpoint.setHandler(router.handler());
	}

	void serve() {
		endpoint.serve();
		off = false;
	}

	void shutdown() {
		endpoint.shutdown();
		off = true;
	}

	~Scraf() {
		if (!off) {
			endpoint.shutdown();
		}
	}

private:
	void createStudent(const Rest::Request& request, Http::ResponseWriter response) {
		std::unique_ptr<simdjson::dom::parser> parser;
		if (!parserPool.waitPop(&parser)) {
			throw std::runtime_error("Error getting the JSON parser. Try again later");
		}
		try {
			const simdjson::dom::element parsed {parser->parse(request.body())};
			std::array<char, crypto_pwhash_STRBYTES> hashedPassword;
			const std::string_view password {parsed["password"]};
			if (crypto_pwhash_str(hashedPassword.data(), password.data(), password.length(), crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
				throw std::runtime_error("Error hashing the password. Try again later");
			}
			student student {[&]() -> class student {
				std::string_view surname;
				if (parsed["surname"].get(surname) == simdjson::SUCCESS) {
					return {parsed["mail"].get_string(), hashedPassword.data(), parsed["name"].get_string(), surname.data()};
				}
				else {
					return {parsed["mail"].get_string(), hashedPassword.data(), parsed["name"].get_string()};
				}
			}()};
			{
				DbTransaction transaction(database->begin());
				database->persist(student);
				transaction.commit();
			}
			response.send(Http::Code::Created, nlohmann::json{{"id", student.getId()}}.dump());
		}
		catch (const simdjson::simdjson_error& exception) {
			response.send(Http::Code::Bad_Request, nlohmann::json{{"message", std::string{"Error parsing the JSON: "} + exception.what()}}.dump());
		}
		catch (const std::exception& exception) {
			response.send(Http::Code::Bad_Gateway, nlohmann::json{{"message", exception.what()}}.dump());
		}
		catch (...) {
			response.send(Http::Code::Bad_Gateway, nlohmann::json{{"message", "An error occured"}}.dump());
		}
		parserPool.push(std::move(parser));
	}

	void updateStudent(const Rest::Request& request, Http::ResponseWriter response) {
		std::unique_ptr<simdjson::dom::parser> parser;
		if (!parserPool.waitPop(&parser)) {
			throw std::runtime_error("Error getting the JSON parser. Try again later");
		}
		std::unique_ptr<Scrafurl> curl;
		if (!curlPool.waitPop(&curl)) {
			throw std::runtime_error("Error getting the curl handle. Try again later");
		}
		try {
			const simdjson::dom::element parsed {parser->parse(request.body())};
			std::int64_t id {request.param(":id").as<std::int64_t>()};
			{
				DbTransaction transaction(database->begin());
				std::unique_ptr<student> currentStudent {database->template load<student>(id)};
				std::string_view text;
				std::string_view text2;
				bool boolean;
				bool needsUpdate {false};
				if (parsed["mail"].get(text) == simdjson::SUCCESS && currentStudent->mail != text) {
					currentStudent->mail = text;
					needsUpdate = true;
				}
				if (parsed["name"].get(text) == simdjson::SUCCESS && currentStudent->name != text) {
					currentStudent->name = text;
					needsUpdate = true;
				}
				if (parsed["surname"].get(text) == simdjson::SUCCESS && currentStudent->surname.get() != text) {
					currentStudent->surname = std::string{text};
					needsUpdate = true;
				}
				if (parsed["password"].get(text) == simdjson::SUCCESS) {
					std::array<char, crypto_pwhash_STRBYTES> hashedPassword;
					if (crypto_pwhash_str(hashedPassword.data(), text.data(), text.length(), crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE) != 0) {
						throw std::runtime_error("Error hashing the password. Try again later");
					}
					currentStudent->password_hash = hashedPassword.data();
					needsUpdate = true;
				}
				if (parsed["is_class_president"].get(boolean) == simdjson::SUCCESS && currentStudent->is_class_president.get() != boolean) {
					currentStudent->is_class_president = boolean;
					needsUpdate = true;
				}
				if (parsed["is_school_president"].get(boolean) == simdjson::SUCCESS && currentStudent->is_school_president.get() != boolean) {
					currentStudent->is_school_president = boolean;
					needsUpdate = true;
				}
				if (parsed["spaggiari_username"].get(text) == simdjson::SUCCESS) {
					if (parsed["spaggiari_password"].get(text2) == simdjson::SUCCESS) {
						curl->post(
							"https://web.spaggiari.eu/rest/v1/auth/login",
							nlohmann::json{{"uid", text}, {"pass", text2}}.dump(),
							"Content-Type: application/json", "User-Agent: zorro/1.0", "Z-Dev-ApiKey: +zorro+"
						);
						if (curl->getResponseCode() != static_cast<long>(Http::Code::Ok)) {
							throw std::logic_error("Spaggiari login failed");
						}
						currentStudent->spaggiari_username = std::string{text};
						currentStudent->spaggiari_password = std::string{text2};
						needsUpdate = true;
					}
					else {
						throw std::logic_error("When changing one Spaggiari credential you must provide both of them");
					}
				}
				else if (parsed["spaggiari_password"].get(text) == simdjson::SUCCESS && parsed["spaggiari_username"].get(text2) != simdjson::SUCCESS) {
					throw std::logic_error("When changing one Spaggiari credential you must provide both of them");
				}
				if (needsUpdate) {
					database->update(*currentStudent);
					transaction.commit();
				}
			}
		}
		catch (const simdjson::simdjson_error& exception) {
			response.send(Http::Code::Bad_Request, nlohmann::json{{"message", std::string{"Error parsing the JSON: "} + exception.what()}}.dump());
		}
		catch (const std::logic_error& exception) {
			response.send(Http::Code::Bad_Request, nlohmann::json{{"message", exception.what()}}.dump());
		}
		catch (const std::exception& exception) {
			response.send(Http::Code::Bad_Gateway, nlohmann::json{{"message", exception.what()}}.dump());
		}
		catch (...) {
			response.send(Http::Code::Bad_Gateway, nlohmann::json{{"message", "An error occured"}}.dump());
		}
		parserPool.push(std::move(parser));
		curlPool.push(std::move(curl));
	}

private:
	Database& database;
	Http::Endpoint& endpoint;
	Rest::Router router;
	// Pool thread-safe di parser
	ObjectPool<std::unique_ptr<simdjson::dom::parser>> parserPool;
	ObjectPool<std::unique_ptr<Scrafurl>> curlPool;
	bool off {true};
};
