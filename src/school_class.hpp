#pragma once

#include <odb/core.hxx>
#include "school.hpp"
#include <string>
#include <memory>

#pragma db object pointer(std::shared_ptr)
class school_class {
private:
	struct PrimaryKey {
		std::string name;
		#pragma db not_null
		std::shared_ptr<school> school_fk;
	};
	#pragma db id
	PrimaryKey primaryKey;

	friend class odb::access;
	school_class() = default;
};
