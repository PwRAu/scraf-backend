#include <odb/forward.hxx>
#include <string>
// Includo l'header dell'ODB
#include <odb/core.hxx>

// Dico che la classe School è un oggetto del database
#pragma db object
class School {
public:
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
	School() = default;

	// Rendo odb::access una classe "amica" di School, così che ODB possa accedere ai campi privati;
	// se la classe ha tutti i membri e il costruttore di default public non è necessario (per chiarezza penso sia meglio metterlo)
	friend class odb::access;

	// Dico che code è l'id (PRIMARY KEY) della School;
	// potrei dire db id auto per dire che l'id è AUTOINCREMENT
	#pragma db id
	std::string code;
	std::string name;
};
