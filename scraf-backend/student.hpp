#pragma once

#include <odb/core.hxx>
#include <odb/nullable.hxx>
#include <utility>

#include "concepts.hpp"

#pragma db object
class student {
public:
	#pragma db unique
	std::string mail;
	std::string password_hash;
	std::string name;
	// Per esprimere che un oggetto può essere NULL nel database, lo dichiaro odb::nullable (o forse in futuro std::optional)
	odb::nullable<std::string> surname;

	template<Text type>
	student(type mail)
		: mail(std::forward<type>(mail)) {}
	
	template<Text type1, Text type2>
	student(type1 mail, type2 password_hash)
		: mail(std::forward<type1>(mail)), password_hash(std::forward<type2>(password_hash)) {}
	
	template<Text type1, Text type2, Text type3>
	student(type1 mail, type2 password_hash, type3 name)
		: mail(std::forward<type1>(mail)), password_hash(std::forward<type2>(password_hash)), name(std::forward<type3>(name)) {}
	
	template<Text type1, Text type2, Text type3, Text type4>
	student(type1 mail, type2 password_hash, type3 name, type4 surname)
		: mail(std::forward<type1>(mail)), password_hash(std::forward<type2>(password_hash)), name(std::forward<type3>(name)), surname(std::forward<type4>(surname)) {}

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
