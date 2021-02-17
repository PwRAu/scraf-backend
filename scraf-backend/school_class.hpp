#pragma once

#include <odb/core.hxx>
#include "school.hpp"
#include <string>
#include <memory>
#include <utility>

#pragma db object
class school_class {
	#pragma db value
	struct PrimaryKey {
		std::string name;
		#pragma db points_to(school)
		std::string school_fk;
	};
	#pragma db id
	PrimaryKey primaryKey;
private:
	friend class odb::access;
	school_class() = default;
};

//#pragma db object
//class school_class {
//public:
//	[[nodiscard]] auto getId() const {
//		return std::make_pair(name, *school_fk);
//	}
//
//	void setId(std::string_view name, school school) {
//		this->name = name;
//		*school_fk = std::move(school);
//	}
//private:
//	#pragma db transient
//	school* school_fk;
//
//	#pragma db transient
//	std::string name;
//
//	#pragma db value
//	struct PrimaryKey {
//		std::string name;
//		#pragma db points_to(school)
//		std::string school_fk;
//	};
//
//	#pragma db member(id) virtual(PrimaryKey) id \
//		get(getId) \
//		set(setId)
//private:
//	friend class odb::access;
//	school_class() = default;
//};
