FLAGS=-Wall -g -mpopcnt

sudoku.exe: sudoku.o
	gcc $(FLAGS) $^ -o $@

sudoku.o: sudoku.c sudoku.h
	gcc $(FLAGS) -c sudoku.c -o $@

demo:
	./sudoku.exe grille.txt

demo2:
	./sudoku.exe grille2.txt
