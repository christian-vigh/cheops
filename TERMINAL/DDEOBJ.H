/**************************************************************************/
/*                                                                        */
/*  DdeObj.h  -  Jean-Luc Lebresne, 96/06/27                              */
/*                                                                        */
/*  Implémentation de la gestion de liens DDE.                            */
/*                                                                        */
/**************************************************************************/
# ifndef        __DDEOBJ_H__
# define	__DDEOBJ_H__

# pragma	option  -k- -N- -vi- -y- -v-
# include	<owl/owlall.h>
# pragma	option  -k. -N. -vi. -y. -v.

#include <ddeml.h>
#include <string.h>

# define MAX_LINKS_TABLE	5	/* Taille de la table interne	  */
# define DDELINK_TMP		0	/* Position réservé aux temporaire*/
# define DDELINK_HYPERAIDE	1	/* Position réservé à l'HyperAide */
# define DDELINK_LEBRAIN	2	/* Position réservé au cerveau	  */

/**************************************************************************/
/*	Classe DdeLink                                                    */
/**************************************************************************/
/*	Objet de gestion d'UNE conversation avec un autre processus	  */
/**************************************************************************/

class	DdeLink
   {
	public :
		DWORD 	idinst ;	/* Identificateur d'instance	  */
		HCONV 	hconv ;		/* Handle de conversation	  */
		HSZ	service ;	/* Handle du service associé	  */
		char	buffer [100 ] ;	/* Buffer de demande d'information*/

		DdeLink ( DWORD idinst, char * pservice ) ;
		~DdeLink ( ) ;

		int	Connect ( BOOL  silent  =  false ) ;
						/* Ouverture de conversation	  */
		void	Execute ( char * pchar ) ; /* Commande distante	  */
		char *	Demande ( char * pchar ) ; /* Commande distante	  */
		void	Disconnect ( ) ;	/* Arret de conversation  */
    } ;

/**************************************************************************/
/*	Classe DdeLinks                                                   */
/**************************************************************************/
/*	Objet de gestion DES conversations avec d'autres processus	  */
/**************************************************************************/

class	DdeLinks
   {
	public :				/* Table des liens   	  */
		DdeLink *	pLinksTable [ MAX_LINKS_TABLE ];
		HSZ		hszService ;	/* Service personnel	  */
		DWORD 		idinst ;	/* nstance		  */

		DdeLinks ( ) ;
		~DdeLinks ( ) ;

					/* Ouverture générale		  */
		int		Open  ( FARPROC lpProc, HINSTANCE hinst, char * Service  ) ;
					/* Connection d'un processus	  */
		DdeLink *	Connect ( int indice, char * Service ) ;
					/* Deconnection d'un processus	  */
		void		Disconnect ( int indice ) ;
					/* Communication avec un processus*/
		DdeLink *	Appel ( int indice ) ;
					/* Controle les demandes	  */
		int		Control ( HSZ hsz1, HSZ hsz2, char * pcontrol ) ;
					/* Fermeture du service		  */
		void		Close ( ) ;
    }  ;

/**************************************************************************/
/*	Variables Globales                                                */
/**************************************************************************/

					/* Pour Simplifier la vie	  */
extern	DdeLinks 	DdeGlobal ;

# endif
