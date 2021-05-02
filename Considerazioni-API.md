# Modellazione API Scraf

Per ottenere i voti di uno studente si potrebbe fare in due modi:

- `GET api.scraf/students/{studentId}/marks/{subjectId}`
- `GET api.scraf/students/{studentId}/subjects/{subjectId}/marks`

Credo che il secondo metodo sia migliore, in quanto permette di ottenere informazioni dettagliate sul singolo voto e sulla singola materia. Infatti, per ottenere informazioni su una materia posso fare

`GET api.scraf/students/{studentId}/subjects/{subjectId}`

mentre con la prima versione non potrei, in quanto `{subjectId}` lo uso per andare a prendere i voti di una determinata materia. Con quell'approccio non potrei neanche andarmi a prendere i dettagli di un singolo voto, cosa che invece potrei fare con la seconda aggiungendo `{markId}` alla fine. O meglio, potrei, ma andrei a rendere l'API enorme, perché una volta ottenuto l'id di un voto dovrei andare a fare una richiesta del tipo

`GET api.scraf/marks/{markId}`

e così per tutte le varie risorse (nelle API Rest per risorsa si intende tipo entità).

In poche parole, bisogna far sì che ci siano poche gerarchie ma molto profonde, nel senso di avere pochi percorsi disponibili dopo `api.scraf/` ma che essi siano molto profondi e mi permettano di ottenere tutte le informazioni che mi servono, ad esempio

- `GET api.scraf/students/{studentId}` per informazioni dettagliate su un singolo studente
- `GET api.scraf/students/{studentId}/subjects/{subjectId}` per informazioni dettagliate su una materia che fa lo studente
- `GET api.scraf/students/{studentId}/subjects/{subjectId}/marks/{markId}` per i dettagli su un singolo voto
- eccetera

Questo tipo di modellazione sembra quindi molto efficace per quanto riguarda l'ottenere informazioni relative a singole entità (voto di uno studente), ma per quanto riguarda ottenere informazioni aggregate per fare, ad esempio, grafici sull'andamento generale degli studenti di una classe di un coordinatore di classe? Bella domandina.

## Relazione studenti-insegnanti

Ogni studente ha più insegnanti, e ogni insegnante ha più studenti. Per definire questa situazione nell'API ho fatto che sotto a /students/{studentId} è presente /teachers, sulla quale si può fare una GET per ottenere la lista di insegnanti che insegnano a uno studente, e uguale per gli insegnanti. Il problema però è: come aggiungo/rimuovo insegnanti da uno studente? L'unica soluzione che mi è venuta in mente è quella di poter fare una POST su /teachers contenente solo il {teacherId} per inserire quell'insegnante nella lista di insegnanti di uno studente, e una DELETE su /teachers/{teacherId} per rimuoverlo. Per ottenere i dettagli di un insegnante o di uno studente è comunque sempre necessario fare una GET su /students/{studentId} o /teacher/{teacherId}.

(Due giorni dopo)
Ok, forse questo approccio non è proprio tanta roba... Credo vada un po' contro la filosofia REST, quindi credo farò in un altro modo. Per ottenere la lista degli studenti appartenenti a una classe dovrò fare una GET su /classes/{classId}, e otterrò un bel JSON pieno di roba, tra cui anche una lista di id degli studenti.

O forse no... Bah, sì, credo che l'approccio precedente possa funzionare, quindi credo userò quello.

## Interdipendenze delle risorse

Risorse da sistemare:

- Studente
- Insegnante
- Scuola
- Voto
- Classe
- Materia

Ogni scuola ha la sua lista di materie, non ha senso che due scuole condividano la stessa materia. Ogni materia è fortemente dipendente da una scuola. Quindi non avrò una risorsa /subjects ma bensì /school/{schoolId}/subjects. Stessa cosa per le classi.

Per gli studenti invece? Beh, è diverso, perché uno prima o poi la scuola la cambia (da superiori a università, per esempio).

I voti invece sono dipendenti dalle materie, perché ogni voto è di una e una sola materia. Ma è anche di uno studente soltanto...

Allora, devo capire che cosa deve fare Scraf per poter capire come organizzare l'API.

Prima di tutto, grafici. Quindi andamento di uno studente in tutte le materie, andamento generale di una sola materia, andamento generale della classe, andamento degli alunni di un insegnante, dell'istituto, di una materia nel contesto dell'intero istituto, e di una materia nel contesto globale. Aspe, quest'ultima cosa non ha senso, o comunque è difficile da fare, perché ogni materia è legata a un singolo istituto (e non è possibile fare altrimenti, dato che ogni scuola chiama le materie un po' come vuole).

Quindi dato che i voti sono un tipo di dato così fondamentale e legato a così tante entità diverse credo sia opportuno renderlo "globale", ovvero creare una risorsa /marks. Anzi no, vanno lasciati sotto a una materia, sennò diventa un casino. Per fare la media di tutti i voti di uno studente mi vado a prendere tutti le sue materie, tutti i voti delle varie materie e faccio la media, e così per tutto.

Non ha senso rendere accessibili le materie da uno studente, in quanto ogni studente di una determinata classe ha le stesse materie degli altri; ha più senso andare a cercare le materie di uno studente nella lista di materie di una determinata classe. È importante ricordare che le materie però non sono specifiche di una classe ma di un istituto, e una stessa materia può essere condivisa tra più classi. Stessa cosa per gli insegnanti; nonostante un insegnante possa appartenere a più classi, ottenere una lista delle materie seguirebbe lo stesso approccio.

Mi sono accorto che avere teachers sotto a /students/{studentId} non ha molto senso, in quanto un insegnante non è assegnato a un singolo studente ma a una intera classe, quindi per ottenere la lista di insegnanti di uno studente è meglio ottenere la classe dello studente e da lì ottenere la lista degli insegnanti assegnati alla classe.

Adesso però c'è un altro problemino. Se da /students/{studentId} non posso accedere alle materie perché posso andarmele a prendere dalla classe, come faccio ad ottenere una lista dei suoi voti? Non posso (credo), quindi devo per forza avere /subjects/{subjectId}/marks sotto a /students/{studentId} (e /teachers).

In /students la risorsa subjects può essere di sola lettura (solo GET), mentre in /teachers no, in quanto un insegnante insegna in diverse classi, e le può cambiare.

In /students non ho anche /{classId} in quanto è un po' inutile, dato che uno studente apparteiene a soltanto una classe. Ha senso avere delle sottorisorse soltanto quando ce ne possono essere diverse istanze (materie).

## Struttura API

Prendiamo come esempio /students. /students rappresenta la risorsa degli studenti. È una risorsa "libera" (non dipende da altri). Facendo una GET su di essa è possibile effettuare una ricerca degli studenti in base al nome, ottenendo un id, facendo una POST può essere creato un nuovo studente. Per ottenere dei dettagli su un determinato studente basta fare una GET su /students/{studentId}, per aggiornarne qualche campo una PATCH e per eliminarlo definitivamente una DELETE. Per ottenere una lista dei suoi insegnanti si può fare una GET su /students/{studentId}/teachers, che restituisce una lista di id. Non è possibile ottenere i dettagli di un insegnante da /students/{studentId}/teachers/{teachersId}, ma è necessario prendere l'id ottenuto dalla GET precedente e fare una GET sul percorso della risorsa "originale", che in questo caso è /teachers/{teacherId}. Oltre a poter ottenere una lista di insegnanti di un determinato studente è possibile aggiungere un insegnante a quella lista facendo una POST con l'id dell'insegnante da aggiungere su /students/{studentId}/teachers, ed è anche possibile eliminare un determinato insegnante dalla lista con una DELETE su /students/{studentId}/teachers/{teacherId}. Utilizzare questo approccio è però sconsigliato, ed è meglio ottenere una lista degli insegnanti

Un id "cronologico" per le classi permette di mantenere uno storico dei dati, utile per le scuole, e semplifica la migrazione degli studenti alle nuove classi, in quanto non è necessario cancellare nessuno da una classe, ma basta crearne una nuova e spostare all'interno gli studenti. Un'altra cosa da considerare sono le bocciature o i cambi di classe. Prima dell'inizio dell'anno dovrebbe essere richiesto agli utenti di inserire la nuova classe; non è possibile fare un calcolo del tipo "nuova_classe = (anno_precedente + 1) + sezione_precedente" perché ad esempio la sezione potrebbe cambiare dalla seconda alla terza e così via.

La registrazione di uno studente segue questo flusso:

- Lo studente inserisce la sua mail, il suo nome ecc
- Lo studente inserisce il suo registro
- Il backend invia una serie di dati "suggeriti" al frontend, tipo scuola e classe, presi dal registro
- Allo studente viene richiesto di confermare i dati, altrimenti di cercare la scuola manualmente
- Viene fatta una POST con l'id dello studente su /schools/{schoolId}/students per aggiungerlo alla scuola
- Lo studente seleziona la sua classe, e se non esiste la crea (in formato simil-5AITI, no "quinta a informatica", controllo sulla lunghezza e magari che il primo carattere sia un numero. Sarebbe comodo fare un controllo con una regex che controlla che vengano inseriti soltanto numeri e lettere maiuscole)
- Viene effettuata una POST con il nome della classe su /schools/{schoolId}/classes, e viene restituito l'id, che sarà qualcosa del tipo nomeclasse-anno, ad esempio 5AITI-2020 (questo passaggio solo se la classe non esiste già)
- Viene aggiunto lo studente alla classe con una POST con il suo id sulla classe in questione

## Implementazione

### Richieste HTTP

Dato che Scraf è multithreaded posso creare una pool di `Scrafurl` condivisa tra i thread, in modo da riciclare il curl handle, così da migliorare notevolmente la velocità delle richieste, dato che libcurl si occuperà di mantenere aperta la connessione ai server Spaggiari
