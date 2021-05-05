
/*#pragma once

#include <odb/core.hxx>
#include <odb/vector.hxx>

class student;

#pragma db object
class mark {
public:
	std::string name;
	std::shared_ptr<student> markedStudent;


	[[nodiscard]] std::int64_t getId() const {
		return id;
	}

private:
	#pragma db id auto
	std::int64_t id;	// Intero a 64 bit, grande quando un bigint SQL

	mark() = default;
	friend class odb::access;
};

#ifdef ODB_COMPILER
	#include "student.hpp"
#endif
*/
