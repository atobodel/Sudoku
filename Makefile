FLAGS=-Wall -g -mpopcnt

sudoku.exe: sudoku.o
	gcc $(FLAGS) $^ -o $@

sudoku.o: sudoku.c sudoku.h
	gcc $(FLAGS) -c sudoku.c -o $@

demo:
	./sudoku.exe grille.txt

# Sans Backtracking
demo2:
	./sudoku.exe grille2.txt

# Avec Backtracking
demo3:
	./sudoku.exe grille3.txt
