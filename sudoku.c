#include "sudoku.h"

ushort grille[TAILLE][TAILLE];
ushort *lignes[TAILLE];
ushort *colonnes[TAILLE];
ushort *regions[TAILLE];

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
    if (contenu == ' ') j++;
    else if (contenu == '\n') {j = 0; i++;}
    else {
      decalage = atoi(&contenu);
      // Si valeur à 0 alors on initialise les bits de 9 à 1
      // (1ULL << TAILLE + 1) - 1 permet d'avoir tout les bits de TAILLE à 0 à 1
      // encore -1 pour enlever le dernier bit
      if (decalage == 0) grille[i][j] = (1ULL << (TAILLE + 1)) - 2;
      // Sinon on le decale de la valeur
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

void casesDeLigne(uchar i) {
  for (int j = 0; j < TAILLE; j++) {
    lignes[j] = &grille[i][j];
  }
}

void casesDeColonne(uchar j) {
  for (int i = 0; i < TAILLE; i++) {
    colonnes[i] = &grille[i][j];
  }
}

// Pour calculer la région il faut prendre i et j
// et faire la division de la racine carré de la taille
// de chaque côté, pour des questions d'optimisations
// je ne calcule pas cette valeur sachant que pour cette
// instance du problème le résultat est 3
void casesDeRegion(uchar i, uchar j) {
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

uchar exclusivite(uchar i, uchar j) {
  /*
  Si une valeur à été trouvée en case i, j alors on supprime cette valeur des autres
  blocs où se trouve cette valeur
  */
  ushort valeur = grille[i][j];
  uchar modifie = 0;
  ushort avantModif;
  casesDeLigne(i);
  casesDeColonne(j);
  casesDeRegion(i, j);
  // Si la valeur n'a pas été révelé
  if (__builtin_popcount(valeur) != 1) {
    return modifie;
  }
  // J'enleve la valeur partout sauf à l'élément lui-même
  for (int k = 0; k < TAILLE; k++) {
    if (k != i) {
      // on applique le masque de la valeur
      avantModif = *colonnes[k];
      *colonnes[k] &= ~(valeur);
      if (avantModif != *colonnes[k]) {
        modifie = 1;
      }
    }
    if (k != j) {
      avantModif = *lignes[k];
      *lignes[k] &= ~(valeur);
      if (avantModif != *lignes[k]) {
        modifie = 1;
      }
    }
    if (k != ((i % 3) * 3) + (j % 3)) { // regions de 3x3 donc je retrouve l'élément
      avantModif = *regions[k];
      *regions[k] &= ~(valeur);
      if (avantModif != *regions[k]) {
        modifie = 1;
      }
    }
  }
  return modifie;
}

ushort unicite(uchar i, uchar j) {
  /*
  Si la case i, j est la seule à pouvoir contenir une valeur alors on lui attribue
  */
  // Si une case C peut contenir plusieurs valeurs => poids binaire > 1
  ushort valCase = grille[i][j];
  uchar modifie = 0;
  if (__builtin_popcount(valCase) > 1) {
    casesDeLigne(i);
    casesDeColonne(j);
    casesDeRegion(i, j);
    ushort bitVal, bitTest, cptLigne = 0, cptColonne = 0, cptRegion = 0;
    ushort avantModif;
    for(uchar n = 1; n <= TAILLE; n++)
    {
      // J'applique un masque pour recuperer la valeur du bit
      bitVal = (valCase >> n) & 1U; // U pour unsigned, pas besoin de LL on est sur uchar
      if (bitVal) { // si 1
        // On teste pour ligne
        for(uchar k = 0; k < TAILLE; k++)
        {
          // ligne excepté lui-même
          if (k != j) {
            bitTest = (*lignes[k] >> n) & 1U;
            if (!bitTest) { // S'il n'est pas à 1
              cptLigne++;
            }
          }
          // colonne excepté lui-même
          if (k != i) {
            bitTest = (*colonnes[k] >> n) & 1U;
            if (!bitTest) { // S'il n'est pas à 1
              cptColonne++;
            }
          }
          // region excepté lui-même
          if (k != ((i % 3) * 3) + (j % 3)) {
            bitTest = (*regions[k] >> n) & 1U;
            if (!bitTest) { // S'il n'est pas à 1
              cptRegion++;
            }
          }
        }
        // toute la ligne/colonne/region moins lui-même
        if (cptLigne == 8 || cptColonne == 8 || cptRegion == 8) {
          // On lui attribue la valeur 2^n => donc la valeur n
          avantModif = grille[i][j];
          grille[i][j] = 1U << n;
          if (avantModif != grille[i][j]) {
            modifie = 1;
          }
          return modifie;
        }
        cptLigne = 0;
        cptColonne = 0;
        cptRegion = 0;
      }
    }
  }
  return modifie;
}

uchar parite(uchar i, uchar j) {
  /*
  Si un couple de case ne peuvent contenir que les 2 mêmes valeurs alors on supprime
  ces valeurs du bloc qui contient ces 2 cases
  */
  ushort valeur = grille[i][j];
  uchar modifie = 0;
  ushort avantModif;
  casesDeLigne(i);
  casesDeColonne(j);
  casesDeRegion(i, j);
  // Verification ligne
  for (uchar k = 0; k < TAILLE; k++) {
    if (k != j) {
      // si la case actuelle a le même couple de valeur qu'une autre de la ligne
      // et que la case n'a que 2 valeurs possibles
      if (valeur == *lignes[k] && __builtin_popcount(valeur) == 2) {
        for (uchar l = 0; l < TAILLE; l++) {
          // J'applique un masque pour enlever ces 2 valeurs
          if (l != j && l != k) {
            avantModif = *lignes[l];
            *lignes[l] &= ~(valeur);
            if (avantModif != *lignes[l]) {
              modifie = 1;
            }
          }
        }
      }
    }
    // Verification colonne
    if (k != i) {
      // si la case actuelle a le même couple de valeur qu'une autre de la colonne
      // et que la case n'a que 2 valeurs possibles
      if (valeur == *colonnes[k] && __builtin_popcount(valeur) == 2) {
        for (uchar l = 0; l < TAILLE; l++) {
          // J'applique un masque pour enlever ces 2 valeurs
          if (l != i && l != k) {
            avantModif = *colonnes[l];
            *colonnes[l] &= ~(valeur);
            if (avantModif != *colonnes[l]) {
              modifie = 1;
            }
          }
        }
      }
    }
    // Verification region
    if (k != ((i % 3) * 3) + (j % 3)) {
      // si la case actuelle a le même couple de valeur qu'une autre de la region
      // et que la case n'a que 2 valeurs possibles
      if (valeur == *regions[k] && __builtin_popcount(valeur) == 2) {
        for (uchar l = 0; l < TAILLE; l++) {
          // J'applique un masque pour enlever ces 2 valeurs
          if ((l != ((i % 3) * 3) + (j % 3)) && l != k) {
            avantModif = *regions[l];
            *regions[l] &= ~(valeur);
            if (avantModif != *regions[l]) {
              modifie = 1;
            }
          }
        }
      }
    }
  }
  return modifie;
}

short bouclerRegles(ushort * posi, ushort * posj) {
  short poidsFaible, poids;
  uchar continuer, modifie, modifie2, modifie3;
  do {
    continuer = 0;
    modifie = 0;
    modifie2 = 0;
    modifie3 = 0;
    poidsFaible = 0;
    for (uchar i = 0; i < TAILLE; i++) {
      for (uchar j = 0; j < TAILLE; j++) {
        modifie = unicite(i, j);
        modifie2 = exclusivite(i, j);
        modifie3 = parite(i, j);
        if ((modifie || modifie2 || modifie3) && !continuer) {
          continuer = 1;
        }
        poids = __builtin_popcount(grille[i][j]);
        // Si le poids est égale à 0, la grille n'a pas de solution
        if (!poids) {
          return 0;
        }
        // Si poids > 1 et soit poidsFaible pas initialisé soit poidsFaible plus élévé
        if ((!poidsFaible && poids > 1) || (poidsFaible > poids && poids > 1)) {
          poidsFaible = poids;
          *posi = i;
          *posj = j;
        }
      }
    }
    // Afficher();
  } while(continuer);
  if (!poidsFaible) {
    poidsFaible = -1;
  }
  return poidsFaible;
}

void copieGrille(ushort origine[TAILLE][TAILLE], ushort copie[TAILLE][TAILLE]) {
  for(int i = 0; i < TAILLE; i++)
  {
    for(int j = 0; j < TAILLE; j++)
    {
      copie[i][j] = origine[i][j];
    }
    
  }
  
}

uchar sudoku(uchar * compteur) {
  uchar trouver = 0;
  ushort posi = 0, posj = 0;
  short retour = 0;
  ushort valeur;
  ushort copie[TAILLE][TAILLE];

  retour = bouclerRegles(&posi, &posj);
  // Si la grille n'a pas de solution on renvoie 0
  if (!retour) {
    return 0;
  }
  
  // Si le sudoku n'est pas terminé
  if (retour > 0) {
    // On cherche/applique les valeurs possibles du poids binaire le plus faible
    valeur = grille[posi][posj];
    for (uchar k = 1; k <= TAILLE; k++) {
      if (trouver) {
        break;
      }
      // On parcourt les bits de valeur
      if (valeur & (1U << k)) { // Si cette valeur est possible
        
        copieGrille(grille, copie);
        grille[posi][posj] = 1U << k;
        trouver = sudoku(compteur);
        *compteur += trouver;
        if (!trouver) {
          copieGrille(copie, grille);
        }
      }
    }
  }
  else {
    trouver = 1;
  }
  return trouver;
}

int main(int argc, char const *argv[]) {
  char * fichier = (char *) argv[1]; // Je cast pour enlever le warning a la compilation
  uchar compteur = 0, trouver;
  initGrille(fichier);
  Afficher();
  trouver = sudoku(&compteur);
  if (trouver) {
    Afficher();
    if (compteur > 0) {
      printf("Solution trouvée après %2u retour(s) en arrière\n", compteur);
    }
    else
    {
      printf("Solution trouvée en appliquant les règles EUP seulement\n");
    }
  }
  else
  {
    printf("Grille fausse !\n");
  }
  return 0;
}
