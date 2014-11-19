#include "bases.h"
#include "intstream.h"
#include "rle.h"

/*
 * Avant propos sur les "intstream"
 *
 * Un "intstream" permet de stocker des entiers dans un fichier.
 * A son ouverture (faite par les programmes de test) une méthode
 * de codage de l'entier lui est associée.
 * Dans notre cas, cela sera les algorithmes développés
 * dans "entier.c" ou "sf.c"
 *
 * Il faut remarquer que deux "intstream" peuvent permettre
 * d'écrire dans le même "bitstream". Les données sont intercalées
 * dans le fichier, pour les récupérer il faut les relire en faisant
 * les lectures dans le même ordre que les écritures.
 */

/*
 * RLE : Run Length Encoding
 *
 * Version spécifique pour stocker les valeurs de la DTC.
 *
 * Au lieu de coder un entier par valeur
 * on va coder le nombre d'entiers qui sont nuls (entier positif)
 * suivi de la valeur du premier entier non nul.
 *
 * Par exemple pour coder le tableau :
 * 	5 8 0 0 4 0 0 0 0 2 1 0 0 0
 *    On stocke dans les deux "intstream"
 *      0 0     2         4 0       3      Nombres de 0
 *      5 8     4         2 1              La valeur différentes de 0
 * Comme les deux "intstream" sont stockés dans le même fichier
 * il faut absolument lire et écrire les valeurs dans le même ordre.
 *     (0,5) (0,8) (2,4) (4,2) (0,1) (3)
 */

/*
 * Stocker le tableau de flottant dans les deux "instream"
 * En perdant le moins d'information possible.
 */

void compresse(struct intstream *entier, struct intstream *entier_signe
	       , int nbe, const float *dct)
{
	int i;
	int compteur = 0;

	for (i = 0; i < nbe; ++i) {

		if(round(dct[i]) == 0) compteur ++;
		else {
			put_entier_intstream(entier, compteur);
			put_entier_intstream(entier_signe, round(dct[i]));
			compteur = 0;
		}
	}
	if(compteur != 0) put_entier_intstream(entier, compteur);

}

/*
 * Lit le tableau de flottant qui est dans les deux "instream"
 */

void decompresse(struct intstream *entier, struct intstream *entier_signe
		 , int nbe, float *dct)
{

	int i,j;
	int compteur;

	for (i = 0; i < nbe; ++i) {
		compteur = get_entier_intstream(entier);
		for(j = 0; j < compteur; j++)
			dct[i+j] = 0;
		i += j;
		if(i < nbe) dct[i] = get_entier_intstream(entier_signe);
	}

}