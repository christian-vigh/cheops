/*
*	Module de compression et transmission de message
*
*	SAUVE Patrick
*	Mai 1995
*
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "const.h"
#include "compression.h"
#include "bitio.h"
#include "uwcommon.h"
#include "unixwin.h"


// variable globale
TEnCours encours=NULL;


/*
 *	fonction d'initialisation pour la variable globale
 *	A appeler une fois avant le premier appel à EditerEcran
 */

void InitCompression()
{
	encours=NULL;
}

/*
 *	fonction principale à appeler pour editer un fichier à l'ecran
 */

int UWFEditFile(char *nomfen,char *nomfichier)
{
FILE *input;
TIdFile *pIdent;
int n;
long taille;

// on commence par essayer d'ouvrir le fichier d'entrée
	
	input = fopen(nomfichier,"rb");
	if (input == NULL) {
		Transmission(ERR_OUV);
		return ERR_OUV;
		}


// on verifie que toutes les allocations sont correctes
	
	if ((pIdent=Init_ID(nomfen))==NULL) {
		Transmission(ERR_ALLOC);
		return ERR_ALLOC;
		}


// ouverture du BIT_FILE de sortie
	
	if ((pIdent->fichier=OpenOutputBitFile(pIdent->nomFichierSortie))
			== NULL) {
				Transmission(ERR_ECRIT);
				return ERR_ECRIT;
				}


// on ecrit le nom de la fenetre en tete du premier bloc
	
	EcrireNomFen(nomfen,pIdent->fichier);


// on compresse le fichier et on verifie que tout ce passe bien
// il peut y avoir deux types d'erreur pendant la compression :
//	- probleme d'allocation pour le dictionnaire
//	- probleme d'écriture sur le disque du fichier compressé
	
	taille=file_size(input);
	n=Compresser(pIdent,input,taille);
	

// on referme le fichier de données initiales
	
	fclose(input);


// on referme le fichier temporaire
	
	CloseOutputBitFile(pIdent->fichier);
	pIdent->fichier==NULL;
	
	
// on ouvre le fichier temporaire en lecture
	
	if ((pIdent->Lecture=fopen(pIdent->nomFichierSortie,"rb"))==NULL)
		return ERR_OUV;

	
// on vérifie que la compression c'est bien passée

	if (n==ERR_ALLOC || n==ERR_ECRIT) {
		Transmission(ERR_COMP);
		return ERR_COMP;
		}


// on ajoute le nouveau fichier en tete de file
	
	InsererTete(pIdent);

	
// on commence la transmission
	
	return Transmission(0);

}

/*
 *	initialiser le descripteur associé au nouveau fichier
 *	cette fonction n'ouvre pas le BIT_FILE de sortie
 */

TIdFile *Init_ID(char *nom)
{
TIdFile 	*pId;
char 		nomTmp[15];

	if ((pId=malloc(sizeof(TIdFile)))!=NULL) {
		pId->nbFen=newNum();
		pId->dernierBloc=0;
		pId->indice=NULL;
		pId->suiv=NULL;
		if ((pId->nomFen=malloc(strlen(nom)+1))!=NULL)
			strcpy(pId->nomFen,nom);
		newNom(nomTmp,pId->nbFen);
		if ((pId->nomFichierSortie=malloc(strlen(nomTmp)+1))!=NULL)
			strcpy(pId->nomFichierSortie,nomTmp);
		} 
	return pId;
}


/*
 *	cherche un numéro de fenetre libre 
 *	on marque tous les numéros pris en parcourant la file circulaire
 *	et on prend le plus petit disponible
 */

unsigned char newNum()
{
char 		occupe[255];
int 		i;
TIdFile 	*pos=encours;

	for (i=0;i<255;i++) occupe[i]=0;
	if (pos==NULL) return 1;
	  else {
	  do {
	    occupe[pos->nbFen]=1;
	    pos=pos->suiv;
	    }
	    while (pos!=encours);
	  for (i=1;occupe[i];i++) ;
	  return (unsigned char)i;
	  }
}

/*
 *	Ajoute le nom de la fenetre avec le premier bloc de données compréssées
 */

void EcrireNomFen(char *nom,BIT_FILE *f)
{
	f->pacifier_counter=strlen(nom)+1;
	while(*nom) putc(*nom++,f->file);
	putc(*nom,f->file);
}


/*
 *	genere un nom de fichier temporaire de la forme compXXX.tmp
 *	aucune vérification n'est effectué pour savoir si ce fichier existe déjà
 */

void newNom(char *nom,unsigned char n)
{int i=(int)n;
	sprintf(nom,"comp%.3d.tmp",i);
}


/*
 *	fonctions de base sur la liste circulaire
 */

// avance d'un cran dans la file

void Suivant()
{
	if (encours) encours=encours->suiv;
}


// inserer en tete de la file

int InsererTete(TIdFile *Id)
{
	if (encours) {
	  Id->suiv=encours->suiv;
	  encours->suiv=Id;
	  return 0;
	  }
	  else {
	    Id->suiv=Id;
	    encours=Id;
	    return 0;
	    }
}


// supprime l'élément en tete de file
// il faut s'assurer que la file n'est pas déja vide et faire
// attention lorsqu'il ne reste qu'un élément (le 1er est aussi le dernier)

void SupprimerTete()
{
TIdFile *cel;

	if (encours) {
	  cel=encours->suiv;
	  if (encours==cel) 
		encours=NULL;
  	     else encours->suiv=cel->suiv; 
	DetruireID(cel);
	}
}


/*
 *	libere la memoire allouée pour la premiere cellule des indices
 */

void LibererCelluleId(TIdFile *pos)
{
TIndice *tmp;

	if (pos->indice!=NULL) {
		  tmp=pos->indice;
		  pos->indice=tmp->suiv;
		  free(tmp);
		  }
}

/*
 *	fonction s'occupant de liberer tous les champs dynamiques de pIdFile
 */

void DetruireID(TIdFile *id)
{
char command[20];

// fermeture du fichier temporaire ouvert en Lecture
	fclose(id->Lecture);

// on efface le fichier temporaire
	sprintf(command,"rm %s",id->nomFichierSortie);
	system(command);
	
// on libere les parties dynamiques
	free(id->nomFichierSortie);
	free(id->nomFen);
	while (id->indice) LibererCelluleId(id);
	free(id);
}


/*
 *	calcul la longueur d'un fichier. Le fichier doit etre ouvert.
 *	Il sera remis à 0 apres l'appel de cet fonction
 */

long file_size(FILE *f)
{
long nb;
	
	rewind(f);
	fseek(f,0L,SEEK_END);
	nb=ftell(f);
	rewind(f);
	return nb;
}

// envoyer un message d'erreur
void EnvoyerMsgErr(int erreur)
{
	printf("Erreur %d\n",erreur);
}

/*****************************************************************************
*
*            fonction qui envoie un bloc de données sur la liaison
*            série 
*
******************************************************************************/

// #define UWF_EDIT_FILE 15


// int WIN_UWFEditFile(TUnixMessage *msg);

int Transmission(int erreur)
{
TUnixMessage		Message;
UTEditFile		*Params;
WTEditFile		*Result;
register int		Status;
register integer16 	Size=sizeof(UTEditFile)-1;
TIdFile 		*Id;
int			taille;
char			*tmp;

	if (erreur==0) {

// on vérifie qu'il y a encore des bloc à envoyer
	if (encours!=NULL) Id=encours->suiv;
			else return 0;

// on recupere la taille du bloc de données compréssées
	taille=Id->indice->taille;

// on calcul la taille des parametres du message sortant
	Size+=taille;

// on alloue la mémoire
	if ((Params=(UTEditFile *)malloc(Size))==NULL) {
		errno=ENOMEM;
		return (-1);
		}

// Initialisation des parametres du message sortant
	Params->nbFen=Id->nbFen;
	if (Id->indice->suiv==NULL) Params->fin=FIN_FICHIER;
		else Params->fin=0;
	Params->nbBloc=++(Id->dernierBloc);
	tmp=Params->data;

// recopie du bloc de données compressées
	fseek(Id->Lecture,Id->indice->deb,SEEK_SET);
	while (taille--) *(tmp++)=(char)getc(Id->Lecture);

// construction du message
	memset(&Message,0,sizeof(Message));
	winmsg_build_message(&Message, UWF_EDIT_FILE,Params,Size);

//       envoie du message
       Status=winmsg_send_receive(&Message,&Message,0);
       free(Params);
 
	
//	Status=WIN_UWFEditFile(&Message);
//	free(Params);

// vérification du résultat de la transmission 
	if (Status!=UWS_OK)
		return (-1);
	
// Vérification de la validité du resultat
	Result=(WTEditFile *) Message.Data;

	switch (Result->code) {
		case T_OK : LibererCelluleId(Id);Suivant();break;
		case ERR_BLOC :
		case ERR_INC  :
		case ERR_FERMEE   : 
		case FIN_FICHIER      :	
		default	      :	SupprimerTete();break;
		}

	winmsg_free_message(&Message);

	}
	else {

// calcul de la taille du message d'erreur
	Size+=1;

// on alloue la mémoire
	if ((Params=(UTEditFile *)malloc(Size))==NULL) {
		errno=ENOMEM;
		return (-1);
		}

// intialisation des parametres sortants
	Params->nbFen=0;
	Params->nbBloc=(char)erreur;

// construction du message
	memset(&Message,0,sizeof(Message));
	winmsg_build_message(&Message,UWF_EDIT_FILE,Params,Size);

// envoie du message
	Status=winmsg_send_receive(&Message,&Message,0);
	free(Params);

// vérification du résultat de la transmission
	if (Status!=UWS_OK)
		return (-1);

	winmsg_free_message(&Message);
	
	}

	return (encours!=NULL);
}

// **************************************************************************

