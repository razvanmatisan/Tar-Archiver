
## Descrierea solutiei

In functia main s-a folosit un while pentru a introduce de la tastatura comenzi pana la primirea comenzii "exit".
Drept output, utilizatorul primeste mesaje ce tin de valabilitatea comenzii/daca a fost executata cu succes.

Taskul 1 - Comanda "Create":
- pentru implementarea comenzii am folosit functiie check_create, solve_create, read_files si read_usermap care completau arhiva propriu zisa cu headerele si continuturile fisierelor.
- pentru implementarea acestor patru functii am avut nevoie de alte functii ajutatoare precum: counter_words, complete_mode, conversion_in_octal, conversion_in_decimal, nr_digit, checksum, copy_hour, concat_zeros si concat_bzeros --> denumiri intuitive/explicate in cod.
- am citit linie cu linie fisierul files.txt, iar cuvintele aflate pe pozitii specifice corespundeau unui buffer din archive.header.
- am citit linie cu linie fisierul usermap.txt cand am aflat username-ul fisierului ce a trebuit arhivat pentru a afla uid si gid.
- cand headerul a fost completat, l-am copiat in fisierul arhivat (urmat de bytes nuli pana la completarea a 512 bytes). In continuare, am copiat continutul fisierului.
- am procedat la fel pana la terminarea fisierelor. La finalul arhivei, am completat cu 512 bytes nuli.

Taskul 2 - Comanda "List":
- m-am folosit de functia solve_list.
- am folosit un while care imi citea la inceput un header si care imi muta cursorul, citind continutul fisierului corespunzator headerului, inainte de urmatorul header.
- m-am folosit de size-ul fisierului pentru a sti pana unde sa mut cursorul si sa gasesc urmatorul header.
- din headere am gasit numele fisierelor arhivate, pe care ulterior le-am listat.

Taskul 3 - Comanda "Extract":
- m-am folosit de functia solve_extract.
- am folosit un while care imi citea la inceput un header si care imi muta cursorul, citind continutul fisierului corespunzator headerului, inainte de urmatorul header.
- m-am folosit de size-ul fisierului pentru a sti pana unde sa mut cursorul si sa gasesc urmatorul header.
- daca numele fisierului din header este cel din comanda, atunci continutul fisierului, impreuna cu headerul, au fost copiate in fisierul extras.

      
         
