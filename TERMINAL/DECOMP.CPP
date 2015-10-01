/*
 *	Décompression de données - Coté WINDOWS
 *
 *	SAUVE Patrick
 *	Mai 1995
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "decomp.h"

TWinEnCours Wencours=NULL;



/*
 *	Fonction de création d'un descripteur :
 * 
 *	On vérifie tout d'abord qu'aucune autre fenetre n'a le meme
 * 	numéro d'identification.
 *
 *		- Initialisation de tous les champs
 *
 *		- Ouverture des fichiers Dico et Data
 *
 *		- Recuperation du nom de la fenetre qui est passée en
 *		tete du premier bloc compréssé 
 *
 *	note :	De plus on positionne correctement un pointeur sur le
 *		début des données compréssées, et on calcule la taille
 *		exacte des données compréssées. Ces 2 paramètres seront
 *		utiles au décompresseur
 */

TWinFenId *CreerDescripteur(TUnixMessage *Msg,int *erreur,
					char **pData,int * size)
{
UTEditFile 	*UData=(UTEditFile *)Msg->Data;
TWinFenId 	*Id;
char 		NomData[20],
		NomDico[20],
		Titre[20];
int  		longueur;
	
// on vérifie qu'aucune fenetre de meme numero n'existe pas
	if (Existente(UData->nbFen)) {
		*erreur=ERR_BLOC;
		return NULL;
		}
	
// allocation pour le descripteur	
	if ((Id=(TWinFenId *)malloc(sizeof(TWinFenId)))==NULL) {
		*erreur=ERR_ALLOC;
		return NULL;
		}

// on initialise les champs
	Id->nbFen=UData->nbFen;
	Id->nbBloc=UData->nbBloc;
	Id->Etat=OUVERTE;

// allocation pour les variables de decompression
	if ((Id->Var=(TVarDecompress *)malloc(sizeof(TVarDecompress)))==NULL){
		*erreur=ERR_ALLOC;
		free(Id);
		return NULL;
		}

// on genere les noms de fichier pour le dico et les données
	sprintf(NomData,"data%.3d.tmp",UData->nbFen);
	sprintf(NomDico,"dico%.3d.tmp",UData->nbFen);

	if ((Id->FichierData=fopen(NomData,"wb"))==NULL) {
		*erreur=ERR_OUV;
		free(Id->Var);
		free(Id);
		return NULL;
		}

	if ((Id->FichierDico=fopen(NomDico,"w+"))==NULL) {
		*erreur=ERR_OUV;
		fclose(Id->FichierData);
		free(Id->Var);
		free(Id);
		return NULL;
		}
	
// on recupere le nom de la fenetre en tete du bloc de données
	*size=Msg->DataSize+1-sizeof(UTEditFile);
	strcpy(Titre,UData->data);
	longueur=strlen(Titre)+1;
	*size-=longueur;
	*pData=UData->data+longueur;

// on alloue la memoire pour le nom de la fenetre
	if ((Id->NomFenetre=(char *)malloc(longueur))!=NULL)
		strcpy(Id->NomFenetre,Titre);

	Id->suiv=NULL;

	return Id;
}


/*
 *	ajouter une fenetre a la liste
 */

void AjouterEnCours(TWinFenId *Id)
{
	if (Wencours==NULL) {
		Id->suiv=Id;
		Wencours=Id;
		}
		else {
		Id->suiv=Wencours->suiv;
		Wencours->suiv=Id;
		}
}


/*
 *	fonction vérifiant qu'aucune fenetre en cours n'a le numero nb
 */

int Existente(unsigned char nb)
{
TWinFenId *pos;
int trouve=0;

	if (Wencours==NULL) return 0;
	pos=Wencours;
	do {
	  if (pos->nbFen==nb) trouve=1;
	  pos=pos->suiv;
	  }
	while ((pos!=Wencours)&&(!trouve)); 
	
	return trouve;
}

/*
 *	Fonction qui recherche le descripteur de la fenetre auquel
 *	correspond le bloc
 *	Cette fonction verifie 
 *		qu'il y a bien une fenetre associée au bloc
 *		
 *		que la fenetre est toujours ouverte
 *
 *		que le bloc recu correspond bien au bloc attendu
 *
 */

TWinFenId *ChercherEnCours(TUnixMessage *Msg,int *erreur,char **pData,int *size)
{
TWinFenId 	*pos;
UTEditFile 	*UData=(UTEditFile *)Msg->Data;

	if (Wencours==NULL) {
		*erreur=ERR_INC;
		return NULL;
		}
	
	pos=Wencours;
	
	do {
	if (pos->nbFen==UData->nbFen) 
	 if (pos->Etat==OUVERTE) {
	  if (UData->nbBloc==++pos->nbBloc) {
			*pData=UData->data;
			*size=Msg->DataSize-sizeof(UTEditFile)+1;
			return pos;
			}
		else {
			*erreur=ERR_BLOC;
			return NULL;
			}
		} else {
		*erreur=ERR_FERMEE;
		return NULL;
		}
	pos=pos->suiv;	
	}
	while (pos!=Wencours);

	*erreur=ERR_INC;
	return NULL;
}


/*
 *	Charger un dictionnaire du disque
 */

int ChargerDico(TDictionary **dict,FILE *f)
{
int i;

	rewind(f);
	for (i=0;i<TABLE_BANKS;i++) 
		if (fread(dict[i],sizeof(TDictionary),256,f)!=256)
			return ERR_LOAD_DICO;
	return 0;
}


/*
 *	Sauvegarder un dictionnaire sur disque
 */

int SauverDico(TDictionary **dict,FILE *f)
{
int i;

	rewind(f);
	for (i=0;i<TABLE_BANKS;i++)
		if (fwrite(dict[i],sizeof(TDictionary),256,f)!=256)
			return ERR_SAVE_DICO;
	return 0;
}
