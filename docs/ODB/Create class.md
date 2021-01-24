# Creazione classi

ODB è usata per creare un collegamento tra oggetti e tabelle di un database, in modo da poter interagire con gli oggetti salvati nel DB senza dover eseguire delle query SQL. È quindi un livello di astrazione.

La mappatura tra tabella e oggetto viene fatta attraverso alcune macro (ovvero righe di codice che iniziano con `#`) di ODB. In particolare, le macro di ODB iniziano tutte con `#pragma db` seguito da quello che devo dire ad ODB. 

Per esempio, facciamo che ho una tabella studente che contiente un id automaticamente assegnato dal database, una mail che dev'essere univoca per ogni utente, e un nome e un cognome che l'utente può decidere di inserire per poterlo visualizzare nel suo profilo. In termini di SQL:

```sql
CREATE TABLE studente (
    id integer GENERATED AS IDENTITY PRIMARY KEY,
    mail varchar(100) UNIQUE,
    name varchar(30) NULL,
    surname varchar(30) NULL
)
```

Per tradurre questa tabella in una classe dovrò prima di tutto includere l'header `<odb/core.hxx>`, 