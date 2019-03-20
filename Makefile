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

# Avec Backtracking (4)
demo3:
	./sudoku.exe grille3.txt

# Avec Backtracking mais plus dur (15)
demo4:
	./sudoku.exe grille4.txt
