#include "sudoku.h"

ushort grille[TAILLE][TAILLE];

void initGrille(char* nom) {
  FILE * fichier = NULL;
  char contenu;
  uchar decalage, i = 0, j = 0;
  fichier = fopen(nom, "r");
  if (!fichier) {
    printf("Erreur à l'ouverture\n");
    exit(0);
  }
  contenu = fgetc(fichier);
  while (!feof(fichier)) {
    //printf("%c\n", contenu);
    if (contenu == ' ') j++;
    else if (contenu == '\n') {j = 0; i++;}
    else {
      //printf("%c\n", contenu);
      decalage = atoi(&contenu);
      //printf("d%d", decalage);
      if (decalage == 0) grille[i][j] = (1ULL << (TAILLE + 1)) - 2;
      else grille[i][j] = 1ULL << decalage;
    }
    contenu = fgetc(fichier);
  }
  fclose(fichier);
}

void Afficher() {
  uchar i,j,bit;
  ushort valeur;

  printf("   ");
  for (int k = 0; k < TAILLE; k++) {
    printf(" %d", k + 1);
  }
  //printf("\n");

  for (i = 0; i < TAILLE; i++) {
      if (i % TAILLE == 0)
          printf("\n");
      printf("%2u",i + 1);
      for (j = 0; j < TAILLE; j++) {
          if (j % TAILLE == 0)
              printf("  ");
          valeur = grille[i][j];
          bit = 0;
          if (__builtin_popcount(valeur) == 1) {
              while (valeur > 1UL) {
                  valeur = valeur >> 1;
                  bit++;
              }
              printf("\033[1;33m\033[40m%u \033[0m",bit);
          }
          else
            printf("\033[0;36m\033[40m%u \033[0m",__builtin_popcount(grille[i][j]));
        }
      printf("\n");
    }
}

void casesDeLigne(uchar i, ushort** lignes) {
  for (int j = 0; j < TAILLE; j++) {
    lignes[j] = &grille[i][j];
  }
}

void casesDeColonne(uchar j, ushort** colonnes) {
  for (int i = 0; i < TAILLE; i++) {
    colonnes[i] = &grille[i][j];
  }
}

// Pour calculer la région il faut prendre i et j
// et faire la division de la racine carré de la taille
// de chaque côté, pour des questions d'optimisations
// je ne calcule pas cette valeur sachant que pour cette
// instance du problème le résultat est 3
void casesDeRegion(uchar i, uchar j, ushort** regions) {
  uchar mod = 3; // le calcul devrait être ici
  uchar regi, regj;
  uchar indidep, indjdep;
  uchar cpt = 0;
  regi = i / mod; // num region en i
  regj = j / mod; // num region en j
  indidep = mod * regi; // indice i de depart de la region (haut gauche)
  indjdep = mod * regj; // indice j de depart de la region (haut gauche)
  for (int idep = indidep; idep < indidep + 3; idep++) {
    for (int jdep = indjdep; jdep < indjdep + 3; jdep++) {
      regions[cpt++] = &grille[idep][jdep]; // post inc ici
    }
  }
}

void exclusivite(uchar i, uchar j) {
  uchar valeur = grille[i][j];
  ushort *lignes[TAILLE] = {0};
  ushort *colonnes[TAILLE] = {0};
  ushort *regions[TAILLE] = {0};
  uchar regi = (i / 3);
  uchar regj = (j / 3);
  casesDeLigne(i, lignes);
  casesDeColonne(j, colonnes);
  casesDeRegion(i, j, regions);
  // Je l'enleve la valeur partout sauf à l'élément lui-même
  for (int k = 0; k < TAILLE; k++) {
    if (k != i) {
      printf(" colonnes %d ", k);
      *colonnes[k] ^= valeur; // on applique le masque de la valeur
    }
    if (k != j) {
      printf(" lignes %d ", k);
      *lignes[k] ^= valeur;
    }
    if (k != (regi * 3) + regj) { // regions de 3x3 donc je retrouve l'élément
      printf(" reg %d ", k);
      *regions[k] ^= valeur;
    }
    printf("\n");
  }
  // A corriger !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}

int main(int argc, char const *argv[]) {
  char * fichier = argv[1];
  initGrille(fichier);
  Afficher();
  ushort *lignes[TAILLE] = {0};
  casesDeLigne(7, lignes);
  printf("LIGNES ");
  for (int i = 0; i < TAILLE; i++) {
    printf("%d ", *lignes[i]);
  }
  printf("\n");
  //grille[2][5] = 3;
  ushort *colonnes[TAILLE];
  casesDeColonne(4, colonnes);
  *colonnes[2] = 9;
  Afficher();
  printf("COLONNES ");
  for (int i = 0; i < TAILLE; i++) {
    printf("%d ", *colonnes[i]);
  }
  printf("\n");
  printf("REGIONS ");
  ushort *regions[TAILLE] = {0};
  casesDeRegion(7, 4, regions);
  for (int i = 0; i < TAILLE; i++) {
    printf("%d ", *regions[i]);
  }
  printf("\n");
  exclusivite(7, 4);
  Afficher();
  return 0;
}
// _______j__
// |X_|__|__|i
// |__|__|__|
// |__|__|__|
