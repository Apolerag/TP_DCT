#include "matrice.h"
#include "dct.h"
#include "jpg.h"
#include "image.h"

/*
 * Calcul de la DCT ou de l'inverse DCT sur un petit carré de l'image.
 * On fait la transformation de l'image ``sur place'' c.a.d.
 * que le paramètre "image" est utilisé pour l'entrée et la sortie.
 *
 * DCT de l'image :  DCT * IMAGE * DCT transposée
 * Inverse        :  DCT transposée * I' * DCT
 */
void dct_image(int inverse, int nbe, float **image)
{
  //Création DCT
  float ** DCT;
  DCT = allocation_matrice_carree_float(nbe);
  coef_dct(nbe, DCT);

  //creation transposée
  float ** DCTtrans;
  DCTtrans = allocation_matrice_carree_float(nbe);
  transposition_matrice_carree(nbe, DCT, DCTtrans);

  //matrice temporaire
  float ** temp;
  temp = allocation_matrice_carree_float(nbe);

  if(inverse == 0) {
    produit_matrices_carrees_float(nbe, DCT, image, temp);
    produit_matrices_carrees_float(nbe, temp, DCTtrans, image);
  }
  else {
    produit_matrices_carrees_float(nbe, DCTtrans, image, temp);
    produit_matrices_carrees_float(nbe, temp, DCT, image);
  }

  //libération des matrices utilisées
  liberation_matrice_carree_float(temp, nbe);
  liberation_matrice_carree_float(DCTtrans, nbe);
  liberation_matrice_carree_float(DCT, nbe);
}

/*
 * Quantification/Déquantification des coefficients de la DCT
 * Si inverse est vrai, on déquantifie.
 * Attention, on reste en calculs flottant (en sortie aussi).
 */
void quantification(int nbe, int qualite, float **extrait, int inverse)
{
  int i, j;
  float temp;
  for(i = 0; i < nbe; i++) {
    for(j = 0; j < nbe; j++) {
      temp = 1 + (i + j + 1) * qualite;
      if (inverse == 0)  extrait[i][j] =  extrait[i][j] / temp;
      else extrait[i][j] = extrait[i][j] * temp;
    }
  }
}
/*
 * ZIGZAG.
 * On fournit à cette fonction les coordonnées d'un point
 * et elle nous donne le suivant (Toujours YX comme d'habitude)
 *
 * +---+---+---+---+     +---+---+---+
 * |00 |01 |   |   |     |   |   |   |
 * | ----/ | /---/ |     | ----/ | | |
 * |   |/  |/  |/  |     |   |/  |/| |
 * +---/---/---/---+     +---/---/-|-+
 * |10/|  /|  /|   |     |  /|  /| | |
 * | / | / | / | | |     | / | / | | |
 * | | |/  |/  |/| |     | | |/  |/  |
 * +-|-/---/---/-|-+     +-|-/---/---+
 * | |/|  /|  /| | |     | |/|  /|   |
 * | / | / | / | | |     | / | ----- |
 * |   |/  |/  |/  |     |   |   |   |
 * +---/---/---/---+     +---+---+---+
 * |  /|  /|  /|   |    
 * | /---/ | /---- |    
 * |   |   |   |   |    
 * +---+---+---+---+    
 */
static int descend = 1 ; // 0 = false et 1 = true
void zigzag(int nbe, int *y, int *x)
{
 // x = colonne y = ligne

  if(descend == 1) {
    if(*y != 0 && *x != nbe - 1) { // si non à une extremité de l'image
      *y -= 1;
      *x += 1;
    }
    else { // atteint la première *y ou la dernière *x
      descend = 0; // on remonte
      if(*y == 0) {
        if(*x != nbe - 1) *x += 1;
        else *y += 1;
      }
      else *y += 1;
    }
  }
  else {
     if(*x != 0 && *y != nbe - 1) { // si non à une extremité de l'image
      *x -= 1;
      *y += 1;
    }
    else { // atteint la première *x ou la dernière *y
      descend = 1; //on descend
      if(*x == 0) {
        if(*y != nbe - 1)
          *y += 1;
        else
          *x += 1;
      }
      else
      *x += 1;
    }
  }
  
  //*x = *x; *y = *y;
}


/*
 * Extraction d'une matrice de l'image (le résultat est déjà alloué).
 * La sous-image carrée à la position et de la taille indiquée
 * est stockée dans matrice "extrait"
 */

static void extrait_matrice(int y, int x, int nbe
			    , const struct image *entree
			    , float **extrait
			    )
 {
  int i, j ;

  for(j=0;j<nbe;j++)
    for(i=0;i<nbe;i++)
      if ( j+y < entree->hauteur && i+x < entree->largeur )
	extrait[j][i] = entree->pixels[j+y][i+x] ;
      else
	extrait[j][i] = 0 ;
 }

/*
 * Insertion d'une matrice de l'image.
 * C'est l'opération inverse de la précédente.
 */

static void insert_matrice(int y, int x, int nbe
			   , float **extrait
			   , struct image *sortie
			   )
 {
  int i, j ;

  for(j=0;j<nbe;j++)
    for(i=0;i<nbe;i++)
      if ( j+y < sortie->hauteur && i+x < sortie->largeur )
	{
	  if ( extrait[j][i] < 0 )
	    sortie->pixels[j+y][i+x] = 0 ;
	  else
	    {
	      if ( extrait[j][i] > 255 )
		sortie->pixels[j+y][i+x] = 255 ;
	      else
		sortie->pixels[j+y][i+x] = rint(extrait[j][i]) ;
	    }
	}
 }


/*
 * Compression d'une l'image :
 * Pour chaque petit carré on fait la dct et l'on stocke dans un fichier
 */
void compresse_image(int nbe, const struct image *entree, FILE *f)
 {
  static float **tmp = NULL ;
  int i, j, k ;

  if ( tmp == NULL )
    {
      tmp = allocation_matrice_carree_float(nbe) ;
    }

  for(j=0;j<entree->hauteur;j+=nbe)
    for(i=0;i<entree->largeur;i+=nbe)
      {
	extrait_matrice(j, i, nbe, entree, tmp) ;
	dct_image(0, nbe, tmp) ;
	for(k=0; k<nbe; k++)
	  assert(fwrite(tmp[k], sizeof(**tmp), nbe, f) == nbe) ;
      }
 }

/*
 * Décompression image
 * On récupère la DCT de chaque fichier, on fait l'inverse et
 * on insère dans l'image qui est déjà allouée
 */
void decompresse_image(int nbe, struct image *entree, FILE *f)
 {
  static float **tmp = NULL ;
  int i, j, k ;

  if ( tmp == NULL )
    {
      tmp = allocation_matrice_carree_float(nbe) ;
    }

  for(j=0;j<entree->hauteur;j+=nbe)
    for(i=0;i<entree->largeur;i+=nbe)
      {
	for(k=0; k<nbe; k++)
	  assert(fread(tmp[k], sizeof(**tmp), nbe, f) == nbe) ;
	dct_image(1, nbe, tmp) ;
	insert_matrice(j, i, nbe, tmp, entree) ;
      }
 }
