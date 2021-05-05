#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <scraf-backend/object_pool.hpp>
#include <regex>

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
using json = nlohmann::json;

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

		Rest::Routes::Get  (router, "/students", Rest::Routes::bind(&Scraf::searchStudent, this));
		Rest::Routes::Post (router, "/students", Rest::Routes::bind(&Scraf::createStudent, this));
		Rest::Routes::Get  (router, "/students/:id", Rest::Routes::bind(&Scraf::getStudentDetails, this));
		Rest::Routes::Patch(router, "/students/:id", Rest::Routes::bind(&Scraf::updateStudent, this));
		Rest::Routes::Get  (router, "/students/:id/subjects", Rest::Routes::bind(&Scraf::getStudentSubjects, this));

		Rest::Routes::Post (router, "/schools", Rest::Routes::bind(&Scraf::createSchool, this));

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
				if (std::string_view surname; parsed["surname"].get(surname) == simdjson::SUCCESS) {
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
			response.send(Http::Code::Created, json{{"id", student.getId()}}.dump());
		}
		catch (const simdjson::simdjson_error& exception) {
			response.send(Http::Code::Bad_Request, json{{"message", std::string{"Error parsing the JSON: "} + exception.what()}}.dump());
		}
		catch (const std::exception& exception) {
			response.send(Http::Code::Bad_Gateway, json{{"message", exception.what()}}.dump());
		}
		catch (...) {
			response.send(Http::Code::Bad_Gateway, json{{"message", "An error occured"}}.dump());
		}
		parserPool.push(std::move(parser));
	}

	void searchStudent(const Rest::Request& request, Http::ResponseWriter response) {
		try {
			json responseBody {json::array()};
			{
				DbTransaction transaction(database->begin());
				odb::result<student> result(database->template query<student>("levenshtein_less_equal('" + request.query().get("name").value() + "', concat(name, surname), 7) < 7")); 
				for (const auto& student : result) {
					responseBody.push_back({{"id", student.getId()}, {"name", student.name + (student.surname.null() ? "" : student.surname.get())}});
				}
				transaction.commit();
			}
			response.send(Http::Code::Ok, responseBody.dump());
		}
		catch (const std::exception& exception) {
			response.send(Http::Code::Bad_Gateway, json{{"message", exception.what()}}.dump());
		}
		catch (...) {
			response.send(Http::Code::Bad_Gateway, json{{"message", "An error occured"}}.dump());
		}
	}

	/*
	 * Devo restituire nome cognome ecc e una lista di id voti e 
	 */
	void getStudentDetails(const Rest::Request& request, Http::ResponseWriter response) {
		try {
			DbTransaction transaction(database->begin());
			std::unique_ptr<student> student_ {database->template load<student>(request.param(":id").as<std::int64_t>())};
			transaction.commit();
			if (student_->surname.null()) {
				response.send(Http::Code::Ok,
					json{
						{"mail", student_->mail},
						{"name", student_->name},
						{"is_class_president", student_->is_class_president},
						{"is_school_president", student_->is_school_president}
					}
				);
			}
			else {
				response.send(Http::Code::Ok,
					json{
						{"mail", student_->mail},
						{"name", student_->name},
						{"surname", student_->surname.get()},
						{"is_class_president", student_->is_class_president},
						{"is_school_president", student_->is_school_president}
					}
				);
			}
		}
		catch (const std::exception& exception) {
			response.send(Http::Code::Bad_Gateway, json{{"message", exception.what()}}.dump());
		}
		catch (...) {
			response.send(Http::Code::Bad_Gateway, json{{"message", "An error occured"}}.dump());
		}
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
			{
				DbTransaction transaction(database->begin());
				std::unique_ptr<student> currentStudent {database->template load<student>(request.param(":id").as<std::int64_t>())};
				std::string_view text;
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
				if (parsed["is_class_president"].get(boolean) == simdjson::SUCCESS && currentStudent->is_class_president != boolean) {
					currentStudent->is_class_president = boolean;
					needsUpdate = true;
				}
				if (parsed["is_school_president"].get(boolean) == simdjson::SUCCESS && currentStudent->is_school_president != boolean) {
					currentStudent->is_school_president = boolean;
					needsUpdate = true;
				}
				if (parsed["cvv_username"].get(text) == simdjson::SUCCESS) {
					if (std::string_view text2; parsed["cvv_password"].get(text2) == simdjson::SUCCESS) {
						curl->post(
							"https://web.spaggiari.eu/rest/v1/auth/login",
							json{{"uid", text}, {"pass", text2}}.dump(),
							"Content-Type: application/json", "User-Agent: zorro/1.0", "Z-Dev-ApiKey: +zorro+"
						);
						if (curl->getResponseCode() != static_cast<long>(Http::Code::Ok)) {
							throw std::logic_error("ClasseViva login failed");
						}
						const simdjson::dom::element parsedCvv {parser->parse(curl->getResponseBody())};
						currentStudent->cvv_username = std::string{text};
						currentStudent->cvv_password = std::string{text2};
						currentStudent->cvv_ident = std::regex_replace(parsedCvv["ident"].get_c_str().value(), std::regex("[^0-9]*([0-9]+).*"), std::string("$1"));
						currentStudent->cvv_token = std::string{parsedCvv["token"].get_string().value()};
						needsUpdate = true;
					}
					else {
						throw std::logic_error("When changing one ClasseViva credential you must provide both of them");
					}
				}
				if (needsUpdate) {
					database->update(*currentStudent);
					transaction.commit();
				}
			}
		}
		catch (const simdjson::simdjson_error& exception) {
			response.send(Http::Code::Bad_Request, json{{"message", std::string{"Error parsing the JSON: "} + exception.what()}}.dump());
		}
		catch (const std::exception& exception) {
			response.send(Http::Code::Bad_Gateway, json{{"message", exception.what()}}.dump());
		}
		catch (...) {
			response.send(Http::Code::Bad_Gateway, json{{"message", "An error occured"}}.dump());
		}
		parserPool.push(std::move(parser));
		curlPool.push(std::move(curl));
	}

	/*
	 * Per gestire l'autenticazione posso fare una richiesta col token memorizzato,
	 * e se ottengo 401 (unauthorized) ne richiedo uno nuovo
	 */
	void getStudentSubjects(const Rest::Request& request, Http::ResponseWriter response) {
		std::unique_ptr<simdjson::dom::parser> parser;
		if (!parserPool.waitPop(&parser)) {
			throw std::runtime_error("Error getting the JSON parser. Try again later");
		}
		std::unique_ptr<Scrafurl> curl;
		if (!curlPool.waitPop(&curl)) {
			throw std::runtime_error("Error getting the curl handle. Try again later");
		}
		try {
			DbTransaction transaction(database->begin());
			std::unique_ptr<student> currentStudent {database->template load<student>(request.param(":id").as<std::int64_t>())};
			curl->get(
				"https://web.spaggiari.eu/rest/v1/students/" + currentStudent->cvv_ident.get() + "/subjects?ffilter=subjects(id,description)",
				"Content-Type: application/json", "User-Agent: zorro/1.0", "Z-Dev-ApiKey: +zorro+", std::string{"Z-Auth-Token: " + currentStudent->cvv_token.get()}.c_str()
			);
			// Token expired, I get a new one
			if (curl->getResponseCode() == static_cast<long>(Http::Code::Unauthorized)) {
				curl->post(
					"https://web.spaggiari.eu/rest/v1/auth/login",
					json{{"uid", currentStudent->cvv_username.get()}, {"pass", currentStudent->cvv_password.get()}}.dump(),
					"Content-Type: application/json", "User-Agent: zorro/1.0", "Z-Dev-ApiKey: +zorro+"
				);
				if (curl->getResponseCode() != static_cast<long>(Http::Code::Ok)) {
					throw std::logic_error("ClasseViva login failed");
				}
				const simdjson::dom::element parsed {parser->parse(curl->getResponseBody())};
				currentStudent->cvv_token = std::string{parsed["token"].get_string().value()};
				curl->get(
					"https://web.spaggiari.eu/rest/v1/students/" + currentStudent->cvv_ident.get() + "/subjects?ffilter=subjects(id,description)",
					"Content-Type: application/json", "User-Agent: zorro/1.0", "Z-Dev-ApiKey: +zorro+", std::string{"Z-Auth-Token: " + std::string{parsed["token"].get_string().value()}}.c_str()
				);
				transaction.commit();
			}
			response.send(Http::Code::Ok, curl->getResponseBody());
		}
		catch (const std::exception& exception) {
			response.send(Http::Code::Bad_Gateway, json{{"message", exception.what()}}.dump());
		}
		catch (...) {
			response.send(Http::Code::Bad_Gateway, json{{"message", "An error occured"}}.dump());
		}
		parserPool.push(std::move(parser));
		curlPool.push(std::move(curl));
	}

	void createSchool(const Rest::Request& request, Http::ResponseWriter response) {
		std::unique_ptr<simdjson::dom::parser> parser;
		if (!parserPool.waitPop(&parser)) {
			throw std::runtime_error("Error getting the JSON parser. Try again later");
		}
		try {
			const simdjson::dom::element parsed {parser->parse(request.body())};
			school school {parsed["code"].get_string(), parsed["name"].get_string()};
			{
				DbTransaction transaction(database->begin());
				database->persist(school);
				transaction.commit();
			}
			response.send(Http::Code::Created, json{{"id", school.getCode()}}.dump());
		}
		catch (const simdjson::simdjson_error& exception) {
			response.send(Http::Code::Bad_Request, json{{"message", std::string{"Error parsing the JSON: "} + exception.what()}}.dump());
		}
		catch (const std::exception& exception) {
			response.send(Http::Code::Bad_Gateway, json{{"message", exception.what()}}.dump());
		}
		catch (...) {
			response.send(Http::Code::Bad_Gateway, json{{"message", "An error occured"}}.dump());
		}
		parserPool.push(std::move(parser));
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
