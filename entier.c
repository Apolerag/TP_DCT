#include "bits.h"
#include "entier.h"

/*
 * Les fonctions de ce fichier permette d'encoder et de décoder
 * des entiers en utilisant des codes statiques.
 */

/*
 * Codage d'un entier (entre 0 et 32767 inclus) en une chaine de bits
 * qui est écrite dans le bitstream.
 *
 * Le nombre est codé par la concaténation du PREFIXE et SUFFIXE
 * Le suffixe est en fait le nombre entier sauf le premier bit a 1
 * 
 * Nombre de bits |    PRÉFIXE     | nombres codés | SUFFIXE
 *       0        |       00       |      0        |
 *     	 1        |       010      |  1 (pas 0)    |
 *     	 2        |       011      |     2-3       | 2=0 3=1
 *     	 3        |      1000      |     4-7       | 4=00 5=01 6=10 7=11
 *     	 4        |      1001      |     8-15      | 8=000 ... 15=111
 *     	 5        |      1010      |    16-31      | 16=0000 ... 31=1111
 *     	 6        |      1011      |    32-63      |
 *     	 7        |      11000     |    64-127     |
 *     	 8        |      11001     |   128-255     |
 *     	 9        |      11010     |   256-511     |
 *     	 10       |      11011     |   512-1023    |
 *     	 11       |      11100     |  1024-2047    |
 *     	 12       |      11101     |  2048-4097    |
 *     	 13       |      11110     |  4096-8191    |
 *     	 14       |      111110    |  8192-16383   |
 *     	 15       |      111111    | 16384-32767   |
 *
 * Je vous conseille de faire EXIT si l'entier est trop grand.
 *
 */

static char *prefixes[] = { "00", "010", "011", "1000", "1001", "1010", "1011",
			    "11000", "11001", "11010", "11011", "11100",
			    "11101", "11110", "111110", "111111" } ;

void put_entier(struct bitstream *b, unsigned int f)
{

	int nbit = nb_bits_utile(f);
	if ( nbit > TAILLE(prefixes) )
		EXIT ;
	put_bit_string(b, prefixes[nbit]);	
	if(f > 1)
		put_bits(b, nbit-1, f);
}

/*
 * Cette fonction fait l'inverse de la précédente.
 *
 * Un implémentation propre, extensible serait d'utiliser
 * un arbre binaire comme pour le décodage d'Huffman.
 * Ou bien parcourir l'arbre des états 8 bits par 8 bits (voir le cours)
 * Mais je ne vous le demande pas
 */

unsigned int get_entier(struct bitstream *b)
{
	char str[7] = "";
	int prefixe_courant = 0;
	int trouve = 0;
	unsigned int retour ;

	
	while(!trouve)
	{
		if(get_bit(b)) strcat(str,"1");
		else strcat(str,"0");

		while(strlen(str) == strlen(prefixes[prefixe_courant]))
		{
			if(strcmp(str,prefixes[prefixe_courant]) == 0)
			{
				trouve = 1;
				break;
			}
			prefixe_courant ++;
		}
	}
	retour = get_bits(b, prefixe_courant -1);
	return pow2(prefixe_courant)/2 + retour; 
}

/*
 * Operation sur des entiers signés
 *
 * Si l'entier est signé, il est précédé d'un bit à 1:negatif et 0:positif
 * On considère que l'entier 0 est positif donc on pourra ajouter
 * 1 aux nombres négatif pour récupérer la place du zero négatif.
 *    2 --> 0 2
 *    1 --> 0 1
 *    0 --> 0 0
 *   -1 --> 1 0
 *   -2 --> 1 1
 *   -3 --> 1 2
 *
 */

void put_entier_signe(struct bitstream *b, int i)
{
	if(i < 0)
	{
		put_bit(b, 1);	
		put_entier(b, -i -1);
	}
	else 
	{
		put_bit(b, 0);	
		put_entier(b, i);
	}
}
/*

 *
 */
int get_entier_signe(struct bitstream *b)
{
	Booleen bool = get_bit(b);
	unsigned int ret = get_entier(b);
	
	if(bool)
	{
		ret = -(ret+1);
	}
	return ret;
}
