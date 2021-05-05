#pragma once

#include <odb/core.hxx>
#include <odb/nullable.hxx>
#include <odb/vector.hxx>
#include <utility>

#include "concepts.hpp"

class mark;

#pragma db object
class student {
public:
	#pragma db unique
	std::string mail;
	std::string password_hash;
	std::string name;
	// Per esprimere che un oggetto può essere NULL nel database, lo dichiaro odb::nullable (o forse in futuro std::optional)
	odb::nullable<std::string> surname;
	bool is_class_president {false};
	bool is_school_president {false};
	odb::nullable<std::string> cvv_username;
	odb::nullable<std::string> cvv_password;
	odb::nullable<std::string> cvv_ident;
	odb::nullable<std::string> cvv_token;
//	#pragma db inverse(markedStudent)
//	odb::vector<std::weak_ptr<mark>> marks;

	student(std::string_view mail, std::string_view password_hash, std::string_view name)
		: mail(mail), password_hash(password_hash), name(name) {}

	template<Text text>
	student(std::string_view mail, std::string_view password_hash, std::string_view name, text surname)
		: mail(mail), password_hash(password_hash), name(name), surname(std::forward<text>(surname)) {}

	//template<Text... type>
	//student(type... arguments)
	//	: student(std::forward<type>(arguments)...) {}

	[[nodiscard]] std::int64_t getId() const {
		return id;
	}

private:
	#pragma db id auto
	std::int64_t id;	// Intero a 64 bit, grande quando un bigint SQL

	student() = default;
	friend class odb::access;
};

#ifdef ODB_COMPILER
	#include "mark.hpp"
#endif
