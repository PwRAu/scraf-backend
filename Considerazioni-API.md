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

