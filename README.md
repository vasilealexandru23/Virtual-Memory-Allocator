<img align="right" src="https://ocw.cs.pub.ro/courses//res/sigla_sd.png" width="150" heigh="150">

**Nume: Vasile Alexandru-Gabriel**  
**Grupă: 314CA**

## Virtual Memory Allocator (Tema 1)

### Descriere:

* Tema propune implementarea unui alocator de memorie care are rolul
de a rezerva memorie, la nivel de biblioteca, traditional prin apeluri
de memorie precum malloc() sau calloc(). Acestea marchează ca fiind folosite anumite zone de memorie dintr-un pool de bytes prealocat, numit arenă. Deasemenea, alocatorul de memorie se ocupă și cu eliberarea zonelor rezervate, apelul de bibliotecă aferent fiind free().

* Descrierea implementarii comenzilor: 
	* **ALLOC_ARENA** : Alocam dinamic memoria folosita pentru structura de arena
	si pentru lista dubla inlantuita in care vom pastra nodurile, care vor contine
	structura de block_t.

	* **DEALLOC_ARENA** : Eliberam absolut toate resursele folosite de la inceputul
	programului. Parcurgem fiecare lista in parte si eliberam, pe rand, fiecare
	nod impreuna cu datele din el.

	* **ALLOC_BLOCK** : Marcam o noua zona de memorie in arena, cu datele extrase
	din inputul dat de utilizator. Verificam daca datele extrase sunt valide, iar
	in caz contrar afisam un mesaj de eroare. Mai departe, creez structura de block
	impreuna cu cea de miniblock, din interiorul ei. Cum nodurile din lista de
	blockuri sunt ordonate (dupa adresa de start), trebuie sa aflam pozitie pe care
	vom insera nodul in lista de blockuri.

	* **FREE BLOCK** :

	* **READ** :

	* **WRITE** :

	* **PMAP** : 

	* **MPROTECT** :

### Comentarii asupra temei:

* Crezi că ai fi putut realiza o implementare mai bună?
	* Se pot face diverse optimizari :
		* Adaugarea in structura de lista a unui pointer la sfarsitul listei
		(dll_node_t *tail), pentru a evita parcurgerile in scopul aflarii ultimului
		nod dintr-o lista.
		* Se puteau gasi formule pentru adaugarea in rw_buffer, dar crestea
		ambiguitatea programului si riscul la eroarea unor case-uri imprevizibile.
* Ce ai invățat din realizarea acestei teme?
	* Cum functioneaza un alocator de memorie virtual.
* Alte comentarii
	* Descrierea temei este sumara. Exista teste, in care utilizatorul poate
	da comenzi imprevizibile (incorecte), despre care nu se vorbeste in tema.
	(Nu este specificat cat de mult poate gresi un utilizator in folosirea
	programului).

### (Opțional) Resurse / Bibliografie: