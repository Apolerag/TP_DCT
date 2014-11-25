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
	int i,j;

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
	for (i = 0; i < image->hauteur; ++i)
		fread (image->pixels[i] ,sizeof(char),image->largeur,f);
		
	
	return image ; 
}

/*
 * Écriture de l'image (toujours au format PGM)
 */

void ecriture_image(FILE *f, const struct image *image)
{
	int i;
	fprintf(f, "P5\n%d %d\n255\n", image->largeur, image->hauteur);

	for(i = 0; i < image->hauteur; i++) {
		fwrite(image->pixels[i] , sizeof(char), image->largeur, f);
	}

}	
