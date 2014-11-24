#include "image.h"


/*
 * Lecture d'une ligne du fichier.
 * On saute les lignes commençant par un "#" (commentaire)
 * On simplifie en considérant que les lignes ne dépassent pas MAXLIGNE
 */

void lire_ligne(FILE *f, char *ligne)
{
	do
	{
		fgets(ligne, MAXLIGNE, f);
	}
	while(ligne[0] == '#');
}

/*
 * Allocation d'une image
 */

struct image* allocation_image(int hauteur, int largeur)
{
	struct image * im;
	int i;
	ALLOUER(im,1);

	im->largeur = largeur;
	im->hauteur = hauteur;

	ALLOUER(im->pixels, hauteur);
	for (i = 0; i < hauteur; ++i)
		ALLOUER(im->pixels[i], largeur);

	return im ;
}

/*
 * Libération image
 */

void liberation_image(struct image* image)
{
	int i;
	for (i = 0; i < image->hauteur; ++i)
		free(image->pixels[i]);
	free(image->pixels);

	free(image);
}

/*
 * Allocation et lecture d'un image au format PGM.
 * (L'entête commence par "P5\nLargeur Hauteur\n255\n"
 * Avec des lignes de commentaire possibles avant la dernière.
 */

struct image* lecture_image(FILE *f)
{
	struct image *image; 
	char c;
	char ligne[MAXLIGNE] ;
	int largeur = 0, hauteur = 0;
	int i,j,k;

	//format entete
	lire_ligne(f, ligne);

	lire_ligne(f, ligne);

	//recuperation de la valeur de largeur
	i = 0;
	c = ligne[i];
	while(c >= '0' && c <= '9') {
		largeur *= 10;
		largeur += (int)(c-'0');
		i ++;
		c = ligne[i];
	}	 

	//recuperation de la valeur de hauteur
	i++;
	c = ligne[i];
	while(c >= '0' && c <= '9') {
		hauteur *= 10;
		hauteur += (int)(c-'0');
		i ++;
		c = ligne[i];
	}

	image = allocation_image(hauteur, largeur);

	//255
	lire_ligne(f, ligne);
	
	//lecture de l'image
	//k = 0;
	//lire_ligne(f, ligne);
	for (i = 0; i < image->hauteur; ++i) {
		for (j = 0; j < image->largeur; ++j) {

			image->pixels[i][j] = fgetc(f);
			/*k++;
			if(ligne[k] == '\n') {
				lire_ligne(f, ligne);
				k = 0;*/
			/*	fprintf(stderr, "iut ligne[0] %c\n",ligne[0] );
				fprintf(stderr, "iut ligne[1] %c\n",ligne[1] );
				fprintf(stderr, "iut ligne[2] %c\n",ligne[2] );
				fprintf(stderr, "iut ligne[3] %c\n",ligne[3] );*/
			//}
		}
	}
	
	return image ; 
}

/*
 * Écriture de l'image (toujours au format PGM)
 */

void ecriture_image(FILE *f, const struct image *image)
{










}
