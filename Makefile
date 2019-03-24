FLAGS=-Wall -g -mpopcnt

sudoku.exe: sudoku.o
	gcc $(FLAGS) $^ -o $@

sudoku.o: sudoku.c sudoku.h
	gcc $(FLAGS) -c sudoku.c -o $@

# Grille fausse
demo:
	./sudoku.exe grille.txt

# Sans Backtracking
demo2:
	./sudoku.exe grille2.txt

# Avec Backtracking (3)
demo3:
	./sudoku.exe grille3.txt

# Avec Backtracking mais plus dur (8)
demo4:
	./sudoku.exe grille4.txt

# Sans Backtracking 
demo5:
	./sudoku.exe grille5.txt

# Avec Backtracking mais plus dur (47)
demo6:
	./sudoku.exe grille6.txt
