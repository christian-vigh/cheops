
/*
 *	Compression de donées- Déclaration des constantes
 *
 *	SAUVE Patrick
 *	Mai 1995
 *
 */

#ifndef __DECOMP_H__
#define __DECOMP_H__


// code des erreurs renvoyées

#define OK		0
#define  T_OK		0
#define ERREUR		1
#define ERR_OUV		2
#define ERR_ALLOC	3
#define ERR_ECRIT	4
#define ERR_COMP	5
#define ERR_DCOMP	6
#define ERR_SAVE_DICO	7
#define ERR_LOAD_DICO	8
#define ERR_FERMEE	9
#define ERR_BLOC	10
#define ERR_INC		11
#define FIN_FICHIER	12


#define OUVERTE		1
#define FERMEE		0

// ***********************************************************************
//         Constantes se rapportant à l'algorithme de compression
//************************************************************************


// Nombre maximum de bits sur lequel sera codé une reference au dico

#define BITS                       15


// Nombre de code d'éléments maxi du dico

#define MAX_CODE                   32767     //( ( 1 << BITS ) - 1 )



// Taille réelle du dico 
// le dico est un peu grand que prévue afin d'ameliorer la fonction
// de hachage

#define TABLE_SIZE                 35023L


// Nombre de page du dico.
// A cause de la barriere des 64Ko sous DOS obligation de decouper le dico

#define TABLE_BANKS                137       //( ( TABLE_SIZE >> 8 ) + 1 )


// definition des codes spéciaux

#define END_OF_STREAM              259	 // code de fin de fichier
#define BUMP_CODE                  257	 // changement de codage 9->10 bits etc
#define FLUSH_CODE                 258   // vider le dico
#define FIRST_CODE                 260   // Premier code dispo pour le codage
#define FIN_BLOC		   256   // code de fin de bloc



// compartiment libre dans le dico

#define UNUSED                     -1


// pendant la compression si les données restantes à compresser sont
// inferieiur au seuil fixé elles sont ajoutées au dernier bloc.
// Cela évite d'avoir des petits blocs supllémentaires à renvoyer

#define SEUIL			   4096


// Taille (approximative) d'un bloc

#define TAILLE_BLOC		   4096




/*
 *	Compression de données - définition des types 
 *
 */





// ***************************************************************************
//                Types propres à la compression-décompression
//****************************************************************************


// dictionnaire utilisé pour la compression et la decompression

typedef struct {
    int 	code_value;
    int 	parent_code;
    char 	character;
} TDictionary;


// Le dictionnaire a une taille superieur a 64Ko. Il sera donc alloué en 
// plusieurs blocs. Cette macro a pour role d'acceder directement au 
// bon élément du dictionnaire

#define DICT( i ) dict[ i >> 8 ][ i & 0xff ]



// sert à stocker le contenu de variables importantes pour
// pouvoir reprendre la decompression de chaque bloc

typedef struct {
	unsigned int 	next_code;
	int 		current_code_bits;
	unsigned int 	old_code;
	int 		reprise;
	} TVarDecompress;


// ****************************************************************************
//                 Types utilisés coté WINDOWS
// ****************************************************************************

typedef struct bidule {
	unsigned char	nbFen;
	unsigned char	nbBloc;
	integer16	ObjectId;
	char		*NomFenetre;
	FILE		*FichierDico;
	FILE		*FichierData;
	int		Etat;
	long		Taille;
	TVarDecompress  *Var;
	struct bidule   *suiv;
	} TWinFenId;



/*
 *	bitio.h
 *
 */

typedef struct bit_file {
	FILE	*file;
	unsigned char mask;
	int	rack;
	int	pacifier_counter;
	} BIT_FILE;

typedef struct {
	char *data;
	int size;
	unsigned char mask;
	int rack;
	} BIT_INPUT;

BIT_FILE	*OpenOutputBitFile( char *name);
int		OutputBits(BIT_FILE *bit_file,unsigned long code, int count);
int		InputBits(BIT_INPUT *bit_file, int bit_count,unsigned int *ret);
int		CloseOutputBitFile(BIT_FILE *bit_file);

typedef TWinFenId *TWinEnCours;



int WIN_UWFEditFile(TUnixMessage *Msg);

TWinFenId *CreerDescripteur(TUnixMessage *Msg,int *err,char **pdata,int *size);

void AjouterEnCours(TWinFenId *Id);

int Existente(unsigned char nb);

TWinFenId *ChercherEnCours(TUnixMessage *Msg,int *err,char **pdata,int *size);

int ChargerDico(TDictionary **dict,FILE *f);

int SauverDico(TDictionary **dict,FILE *f);

int Decompresser(TWinFenId *Id,char *data,int size);


#endif
