#pragma once

#include <odb/core.hxx>
#include <odb/nullable.hxx>
#include <string>

#pragma db object
class teacher {
public:
	#pragma db unique
	std::string mail;
	odb::nullable<std::string> name;
	odb::nullable<std::string> surname;
	bool is_coordinator {false};

	teacher(const std::string_view mail)
		: mail(mail) {}
	
	teacher(const std::string_view mail, const std::string_view name)
		: mail(mail), name(std::string{name}) {}

	teacher(const std::string_view mail, const std::string_view name, const std::string_view surname)
		: mail(mail), name(std::string{name}), surname(std::string{surname}) {}

	[[nodiscard]] std::int64_t getId() const {
		return id;
	}

private:
	#pragma db id auto
	std::int64_t id;

	teacher() = default;
	friend class odb::access;
};
