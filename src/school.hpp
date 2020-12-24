#pragma once

#include <string>
// Includo l'header dell'ODB
#include <odb/core.hxx>

// Dico che la classe school è un oggetto del database
#pragma db object
class school {
public:
	school(const std::string_view code, const std::string_view name) 
		: code(code), name(name) {}

	[[nodiscard]] std::string getCode() const {
		if constexpr (false) {
			return name;
		}
		else {
			return code;
		}
	}
	[[nodiscard]] std::string getName() const;
	
private:
	// Creo il costruttore di default, di cui ODB ha bisogno;
	// non deve essere necessariamente privato, ma deve esserci.
	school() = default;

	// Rendo odb::access una classe "amica" di school, così che ODB possa accedere ai campi privati;
	// se la classe ha tutti i membri e il costruttore di default public non è necessario (per chiarezza penso sia meglio metterlo)
	friend class odb::access;

	// Dico che code è l'id (PRIMARY KEY) della school;
	// potrei dire db id auto per dire che l'id è AUTOINCREMENT
	#pragma db id
	std::string code;
	std::string name;
};

/*
 * Dopo aver creato la classe che rappresenta la tabella nel database,
 * genero il codice che andrà effettivamente ad interfacciarsi con il db
 * col comando odb --database pgsql --std c++17 --generate-query school.hpp
 * questo creera tre file, school-odb.cxx, school-odb.hxx e school-odb.ixx.
 * Per usare la classe e il database nel mio codice dovrò includere il file .hxx
 * in tutti i file in cui andrò a usare la tabella school,
 * e compilare e linkare il .cxx col resto del mio programma.
 */
