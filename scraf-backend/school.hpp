#pragma once

// Includo l'header dell'ODB
#include <odb/core.hxx>
#include <string>
#include <utility>
#include <memory>
#include <vector>

#include "concepts.hpp"

class school_class;
// Dico che la classe school è un oggetto del database
#pragma db object
class school {
public:
	// Dico che code è l'id (PRIMARY KEY) della school;
	// potrei dire db id auto per dire che l'id è AUTOINCREMENT
	#pragma db id
	std::string code;
	std::string name;

	//#pragma db inverse(primaryKey.school_fk)
	//std::vector<std::weak_ptr<school_class>> classes;

	//school(std::string code, std::string name) 
	//	: code(std::move(code)), name(std::move(name)) {}

	// Questo costruttore storto è il modo più efficiente per creare un costruttore
	// a cui passi due strighe che vengono assegnate ai due attributi std::string.
	// È quaasi la stessa cosa di quello commentato qui sopra, ma meglio.
	//
	// Riga 1: questo costruttore è un template, ovvero è valido per 
	// ogni tipo, nel senso che posso mettere variabili di qualiasi tipo
	// al suo interno e va bene lo stesso.
	// Ceh va bene lo stesso un paio di balle, perché se metto, che ne so,
	// un intero, va tutto a p**, perché proverei ad assegnare un intero 
	// a un attributo (code o name) di tipo string. 
	// Quindi mi serve un modo per dire che questa funzione va bene per tutti i tipi
	// di testo, e in C++ ci sono std::string, char[], eccetera.
	// Fortunatamente c'è un modo semplice per farlo, ovvero usare i Concept.
	// I concept sono stati appena introdotti in C++20, quindi su VSCode
	// i colori sono un po' buggati, però funzionano perfettamente. 
	// I concept mi permettono di creare un "concetto".
	// Il concetto che vado a creare lo si va poi ad usare per, ad esempio,
	// dire quali tipi accetta la mia funzione.
	// Nel nostro caso, ho creato un concept Text, che rappresenta tutti i tipi di testo;
	// questo concept dice che un Text, per essere tale, dev'essere convertibile ad std::string. ez
	// I concept li vado poi ad usare nei template. In questo caso dico che type1 e type2 sono due Text.
	// Tutti questi controlli vengono fatti durante la compilazione, quindi nessun impatto sulle performance.
	//
	// Riga 2: inizio a scrivere il classico costruttore, in cui prendo i due parametri di tipo type1 e type2.
	//
	// Riga 3: Ecco qui arriva una parte in cui ti dovrei spiegare un casino di roba e non ho né voglia né spazio,
	// quindi riassumo dicendo che std::forward prende il code che ho passato come parametro e lo butta
	// nell'attributo code della classe, e uguale con name, senza fare copie e massimizzando le prestazioni.
	// La sintassi dei : dopo ) e prima di {} è strana, lo so, ma è il modo di C++ di dire che
	// il parametro passato nel costruttore va messo nell'attributo della classe senza operazioni particolari
	// e senza copie aggiuntive.
	// Onestamente std::forward non l'ho ben capito neanch'io, cioè probabilmente l'ho capito ieri ma me ne sono
	// già dimenticato perché è bello storto.
	template<Text type1, Text type2>
	school(type1 code, type2 name)
		: code(std::forward<type1>(code)), name(std::forward<type2>(name)) {}

private:
	// Creo il costruttore di default, di cui ODB ha bisogno;
	// non deve essere necessariamente privato, ma deve esserci.
	// È conveniente metterlo privato così che all'interno del codice
	// non sia possibile instanziare un oggetto school senza code e name,
	// cosa sbagliata in quanto nel database sono salvati come NOT NULL
	school() = default;

	// Rendo odb::access una classe "amica" di school, così che ODB possa accedere ai campi privati;
	// se la classe ha tutti i membri e il costruttore di default public non è necessario (per chiarezza penso sia meglio metterlo)
	friend class odb::access;
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

#ifdef ODB_COMPILER
	#include "school_class.hpp"
#endif
