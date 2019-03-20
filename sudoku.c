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

uchar exclusivite(uchar i, uchar j) {//7 et 4
  ushort valeur = grille[i][j];
  // printf("Valeur : %d", valeur);
  ushort *lignes[TAILLE] = {0};
  ushort *colonnes[TAILLE] = {0};
  ushort *regions[TAILLE] = {0};
  uchar modifie = 0;
  casesDeLigne(i, lignes);
  casesDeColonne(j, colonnes);
  casesDeRegion(i, j, regions);
  // Si la valeur n'a pas été révelé
  if (__builtin_popcount(valeur) != 1) {
    return modifie;
  }
  // J'enleve la valeur partout sauf à l'élément lui-même
  for (int k = 0; k < TAILLE; k++) {
    if (k != i) {
      // printf(" colonnes %d ", k);
      // on applique le masque de la valeur
      *colonnes[k] &= ~(valeur);
      modifie = 1;
    }
    if (k != j) {
      // printf(" lignes %d ", k);
      // printf("Lignes %d, valeur: %d\n", k, *lignes[k]);
      // printf("Calcul %d\n", *lignes[k] & ~(valeur));
      *lignes[k] &= ~(valeur);
      modifie = 1;
      // printf("Lignes %d, valeur: %d\n", k, *lignes[k]);
    }
    if (k != ((i % 3) * 3) + (j % 3)) { // regions de 3x3 donc je retrouve l'élément
      // printf(" reg %d ", k);
      *regions[k] &= ~(valeur);
      modifie = 1;
    }
    // printf("\n");
  }
  // En theorie c'est OK
  return modifie;
}

ushort unicite(uchar i, uchar j) {
  // Si une case C peut contenir plusieurs valeurs => poids binaire > 1
  ushort valCase = grille[i][j];
  uchar modifie = 0;
  if (__builtin_popcount(grille[i][j]) > 1) {
    ushort *lignes[TAILLE] = {0};
    ushort *colonnes[TAILLE] = {0};
    ushort *regions[TAILLE] = {0};
    casesDeLigne(i, lignes);
    casesDeColonne(j, colonnes);
    casesDeRegion(i, j, regions);
    ushort bitVal, bitTest, cpt = 0;
    for(uchar n = 1; n <= TAILLE; n++)
    {
      bitVal = (valCase >> n) & 1U; // U pour unsigned, pas besoin de LL on est sur uchar
      if (bitVal) { // si 1
        // On teste pour ligne
        for(uchar k = 0; k < TAILLE; k++)
        {
          // ligne excepté lui-même
          if (k != j) {
            bitTest = (*lignes[k] >> n) & 1U;
            // printf("%d \n", bitTest);
            if (!bitTest) { // S'il n'est pas à 1
              // printf(" ligne %d, %d bit test\n", k, bitTest);
              cpt++;
            }

          }

        }
        if (cpt == 8) { // toute la ligne moins lui-même
          // On lui attribue la valeur 2^n => donc la valeur n
          grille[i][j] = 1U << n;
          modifie = 1;
          return modifie;
        }
        cpt = 0;
        // On teste pour colonne
        for(uchar k = 0; k < TAILLE; k++)
        {
          // colonne excepté lui-même
          if (k != i) {
            bitTest = (*colonnes[k] >> n) & 1U;
            if (!bitTest) { // S'il n'est pas à 1
              // printf(" colonne %d\n", k);
              cpt++;
            }

          }

        }
        if (cpt == 8) { // toute la ligne moins lui-même
          // On lui attribue la valeur 2^n => donc la valeur n
          grille[i][j] = 1U << n;
          modifie = 1;
          return modifie;
        }
        cpt = 0;
        // On teste pour region
        for(uchar k = 0; k < TAILLE; k++)
        {
          // region excepté lui-même
          if (k != ((i % 3) * 3) + (j % 3)) {
            bitTest = (*regions[k] >> n) & 1U;
            if (!bitTest) { // S'il n'est pas à 1
              // printf(" region %d\n", k);
              cpt++;
            }

          }

        }
        if (cpt == 8) { // toute la ligne moins lui-même
          // On lui attribue la valeur 2^n => donc la valeur n
          grille[i][j] = 1U << n;
          modifie = 1;
          return modifie;
        }
        cpt = 0;

      }

    }

  }
  return modifie;
} // En theorie c'est OK

uchar parite(uchar i, uchar j) {
  ushort valeur = grille[i][j];
  uchar modifie = 0;
  // printf("Valeur : %d", valeur);
  ushort *lignes[TAILLE] = {0};
  ushort *colonnes[TAILLE] = {0};
  ushort *regions[TAILLE] = {0};
  casesDeLigne(i, lignes);
  casesDeColonne(j, colonnes);
  casesDeRegion(i, j, regions);
  // Verification ligne
  for (uchar k = 0; k < TAILLE; k++) {
    if (k != j) {
      // si la case actuelle a le même couple de valeur qu'une autre de la ligne
      // et que la case n'a que 2 valeurs possibles
      if (valeur == *lignes[k] && __builtin_popcount(valeur) == 2) {
        for (uchar l = 0; l < TAILLE; l++) {
          // J'applique un masque pour enlever ces 2 valeurs
          if (l != j && l != k) {
            *lignes[l] &= ~(valeur);
            modifie = 1;
          }
        }
      }
    }
  }
  // Verification colonne
  for (uchar k = 0; k < TAILLE; k++) {
    if (k != i) {
      // printf("CAW CAW!!!!!!! %d vs %d \n", *colonnes[k], valeur);
      // si la case actuelle a le même couple de valeur qu'une autre de la colonne
      // et que la case n'a que 2 valeurs possibles
      if (valeur == *colonnes[k] && __builtin_popcount(valeur) == 2) {
        // printf("COW COW\n");
        for (uchar l = 0; l < TAILLE; l++) {
          // J'applique un masque pour enlever ces 2 valeurs
          if (l != i && l != k) {
            *colonnes[l] &= ~(valeur);
            modifie = 1;
            // printf("CAW CAW\n");
          }
        }
      }
    }
  }
  // Verification region
  for (uchar k = 0; k < TAILLE; k++) {
    if (k != ((i % 3) * 3) + (j % 3)) {
      // si la case actuelle a le même couple de valeur qu'une autre de la region
      // et que la case n'a que 2 valeurs possibles
      if (valeur == *regions[k] && __builtin_popcount(valeur) == 2) {
        for (uchar l = 0; l < TAILLE; l++) {
          // J'applique un masque pour enlever ces 2 valeurs
          if ((l != ((i % 3) * 3) + (j % 3)) && l != k) {
            *regions[l] &= ~(valeur);
            modifie = 1;
          }
        }
      }
    }
  }
  return modifie;
} // Douteux

short bouclerRegles() {
  // a descendre eventuellement dans le do
  short poidsFaible, poids;
  uchar continuer, modifie;
  do {
    continuer = 0;
    modifie = 0;
    poidsFaible = 0;
    for (uchar i = 0; i < TAILLE; i++) {
      for (uchar j = 0; j < TAILLE; j++) {
        modifie = unicite(i, j);
        printf("%d\n", modifie);
        if (modifie && !continuer) {
          continuer = modifie;
        }
        modifie = exclusivite(i, j);
        printf("%d\n", modifie);
        if (modifie && !continuer) {
          continuer = modifie;
        }
        modifie = parite(i, j);
        printf("%d\n", modifie);
        if (modifie && !continuer) {
          continuer = modifie;
        }
        poids = __builtin_popcount(grille[i][j]);
        if ((!poidsFaible && poids > 1) || (poidsFaible > poids && poids > 1)) {
          poidsFaible = poids;
        }
      }
    }
    Afficher();
    //return;
  } while(continuer);
  if (!poidsFaible) {
    poidsFaible = -1;
  }
  return poidsFaible;
}

int main(int argc, char const *argv[]) {
  char * fichier = (char *) argv[1]; // Je cast pour enlever le warning a la compilation
  initGrille(fichier);
  Afficher();
  // ushort *lignes[TAILLE] = {0};
  // casesDeLigne(7, lignes);
  // printf("LIGNES ");
  // for (int i = 0; i < TAILLE; i++) {
  //   printf("%d ", *lignes[i]);
  // }
  // printf("\n");
  // //grille[2][5] = 3;
  // ushort *colonnes[TAILLE];
  // casesDeColonne(4, colonnes);
  // //*colonnes[2] = 9;
  // //Afficher();
  // printf("COLONNES ");
  // for (int i = 0; i < TAILLE; i++) {
  //   printf("%d ", *colonnes[i]);
  // }
  // printf("\n");
  // printf("REGIONS ");
  // ushort *regions[TAILLE] = {0};
  // casesDeRegion(7, 4, regions);
  // for (int i = 0; i < TAILLE; i++) {
  //   printf("%d ", *regions[i]);
  // }
  // printf("\n");
  // printf("Valeur de 7, 8 : %d\n", grille[7][8]);
  // exclusivite(7, 4);
  // printf("Valeur de 7, 8 : %d\n", grille[7][8]);
  // Afficher();
  // unicite(7,8);
  // Afficher();
  // grille[1][1] = 12; // 2 choix
  // grille[1][2] = 12; // 2 choix
  // grille[1][3] = 18; // 2 choix
  // Afficher();
  // parite(1, 2);
  Afficher();
  bouclerRegles();
  // unicite(4, 2); // marche
  Afficher();
  // exclusivite(4, 2);
  // Afficher();

  return 0;
}
// _______j__
// |X_|__|__|i
// |__|__|__|
// |__|__|__|
// NOM.tar.gz pour mercredi
