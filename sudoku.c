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

void casesDeRegion(uchar i, uchar j, ushort** regions) {
  // a voir
}

int main(int argc, char const *argv[]) {
  char * fichier = argv[1];
  initGrille(fichier);
  Afficher();
  //printf("%d, %d\n", grille[2][3], grille[6]);
  ushort *lignes[TAILLE] = {0};
  casesDeLigne(7, lignes);
  for (int i = 0; i < TAILLE; i++) {
    printf("%d\n", *lignes[i]);
  }
  //grille[2][5] = 3;
  ushort *colonnes[TAILLE];
  casesDeColonne(4, colonnes);
  *colonnes[2] = 9;
  Afficher();
  for (int i = 0; i < TAILLE; i++) {
    printf("%d\n", *colonnes[i]);
  }
  return 0;
}
