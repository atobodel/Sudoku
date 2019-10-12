# Solveur de Sudoku

Ce programme résout n'importe quelle grille de Sudoku de taille 9x9

## Fonctionnement

Il utilise les vecteurs caractéristiques pour représenter les chiffres dans le Sudoku.
Il applique les règles EUP (exclusivité, unicité et parité) pour dévoiler un maximum de case à chaque tour et pour éviter les récursions inutiles.
Pour les points d'indécisions, un backtracking est utilisé pour tester les valeurs qui peuvent être prises.

## Données

Les données du problème à résoudre seront écrit dans un fichier .txt dans lequel une matrice 9x9 contenant des chiffres allant de 0 à 9 seront écrit et seront séparés par un espace.
Les chiffres de 1 à 9 représentent une valeur de la case en question.
Le chiffre 0 représentent une case vide.

Exemple:

7 1 4 0 0 3 0 0 0

3 6 8 0 4 1 0 5 0

5 0 0 0 6 0 1 0 3

0 5 0 0 3 4 0 0 0

9 4 0 1 7 2 8 3 5

0 0 0 6 9 5 0 0 0

0 0 5 3 8 0 0 0 7

0 0 0 5 0 0 3 0 9

0 0 0 4 0 0 5 0 8

Pour tester le programme, des grilles "démos" sont fournis.

## Exécution

Compilation:

make

Test des grilles fournies:

Grille fausse

make demo

Résolution uniquement avec les règles EUP

make demo2

Résolution avec 3 retours en arrière

make demo3

Résolution avec 8 retours en arrière

make demo4

Résolution uniquement avec les règles EUP

make demo5

Résolution avec 47 retours en arrière (grille vide)

make demo6

Test avec un fichier .txt formaté:

./sudoku.exe fichier.txt

## Affichage

Par défaut, seul la grille de départ et la grille résolue sont affichées sur la sortie standard.
Pour afficher à chaque étape de résolution des règles EUP, vous devez décommenter la ligne Afficher() dans la fonction bouclerRegles et commenter celle du main.
