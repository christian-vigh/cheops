/**************************************************************************/
/*                                                                        */
/*  DdeObj.cpp  -  Jean-Luc Lebresne, 96/06/27                            */
/*                                                                        */
/*  Implémentation de la gestion de liens DDE.                            */
/*                                                                        */
/**************************************************************************/



# include	"ddeobj.h"
# include	<stdio.h>
/**************************************************************************/
/*	DdeLink	:	Objet de gestion d'une connection.		  */
/*                                                                        */
/**************************************************************************/

DdeLink :: DdeLink ( DWORD newidinst, char * pservice )
   {
	idinst = newidinst ;	/* Récuperation de l'identifient d'instance */
				/* Génération du Handle du service	    */
	service = ::DdeCreateStringHandle ( idinst, pservice, 0 ) ;
	hconv = NULL ;		/* Initialisation du Handle de conversation */
    }

DdeLink :: ~DdeLink ( )
   {				/* Libérations et déconnection		    */
	::DdeFreeStringHandle ( idinst, service ) ;
	if  ( hconv )
	 ::DdeDisconnect ( hconv ) ;
    }

/*------------------------------------------------------------------------*/
/*	DdeLink :: Connect						  */
/*------------------------------------------------------------------------*/
/*	Tente de se connecter à l'application concernée.		  */
/*	Tente de la lancer si elle ne répond pas.			  */
/*								          */
/*  Modifié le 09.08.96 (CV)						  */
/*	J'ai rajouté le paramètre "silent" pour que Connect n'affiche pas */
/*	de MessageBox s'il est à true. Si on ouvre une MessageBox alors   */
/*      qu'un menu de Cheops est ouvert, ce dernier ne reçoit apparemment */
/*      pas le message WM_KILLFOCUS (qui est le seul évènement déterminant*/
/*      s'il doit se fermer ou pas). Moralité : le menu Cheops ouvert le  */
/*      reste (mais derrière la fen^^etre de gmenu). Quand on quitte      */
/*      gmenu, on retrouve le menu ouvert. Ca fait mauvais effet, non ?   */
/*------------------------------------------------------------------------*/
int	DdeLink :: Connect ( BOOL  silent )
   {
	char	buffer [ 200 ] ;
	HSZ	connection = DdeCreateStringHandle ( idinst, "Connection", 0 ) ;
	int	retour = TRUE ;

	hconv = DdeConnect ( idinst,service,connection,NULL);

	if  ( ! hconv )
	   {
		DdeQueryString ( idinst, service, buffer, sizeof ( buffer ), 0);
		buffer [8] = 0 ;
		strcat ( buffer, ".exe" ) ;
		WinExec ( buffer, SW_SHOW ) ;
		hconv = DdeConnect ( idinst, service, connection, NULL ) ;
		if  ( hconv == NULL  &&  ! silent )
		   {
			strcat ( buffer, " : Echec de Connection DDE." ) ;
			MessageBox ( NULL, buffer, "Système HyperAide", MB_OK ) ;
			retour = FALSE ;
		    }
	    }
	DdeFreeStringHandle ( idinst, connection ) ;
	return  ( retour ) ;
    }

/*------------------------------------------------------------------------*/
/*	DdeLink :: Execute						  */
/*------------------------------------------------------------------------*/
/*	Transmet une demande simple d'execution.			  */
/*------------------------------------------------------------------------*/
void	DdeLink :: Execute ( char * pchar )
   {
	DdeClientTransaction( pchar, strlen ( pchar ) +1, hconv, NULL, CF_TEXT	, XTYP_EXECUTE	, TIMEOUT_ASYNC, NULL) ;
    }

/*------------------------------------------------------------------------*/
/*	DdeLink :: Demande						  */
/*------------------------------------------------------------------------*/
/*	Transmet une demande et attend la reponse.			  */
/*------------------------------------------------------------------------*/
char *	DdeLink :: Demande ( char * pchar )
   {
	HSZ		request ;
	HDDEDATA	retour = NULL ;
	DWORD		cbDataLen ;

	memset ( buffer, 0 , sizeof ( buffer ) ) ;
	request = DdeCreateStringHandle ( idinst, pchar, 0 ) ;

	if  ( hconv )
	   {
		for  ( int i = 20; i  &&  ! retour; i-- )
			retour = DdeClientTransaction( 0, 0, hconv, request, CF_TEXT	, XTYP_REQUEST	, 0x10, NULL) ;

		strcpy ( buffer, DdeAccessData ( retour, & cbDataLen ) );
	    }

	DdeFreeStringHandle ( idinst, request ) ;
	return  ( buffer ) ;
    }

/*------------------------------------------------------------------------*/
/*	DdeLink :: Disconnect						  */
/*------------------------------------------------------------------------*/
/*	Déconnection du lien de conversation.				  */
/*------------------------------------------------------------------------*/
void	DdeLink :: Disconnect ( )
   {
	if  ( hconv )
	   {
		DdeDisconnect ( hconv ) ;
		hconv = NULL ;
	    }
    }

/**************************************************************************/
/*	DdeLinks	:	Objet de gestion des connection. 	  */
/*      	et les appels personnels.                                 */
/**************************************************************************/

DdeLinks :: DdeLinks ( )
   {
	memset ( pLinksTable, 0, sizeof ( pLinksTable ) ) ;
	idinst = 0 ;
    }

DdeLinks :: ~DdeLinks ( )
   {
	Close ( ) ;
    }

/*------------------------------------------------------------------------*/
/*	DdeLinks :: Open						  */
/*------------------------------------------------------------------------*/
/*	Connection de la base au système DDE				  */
/*------------------------------------------------------------------------*/
int	DdeLinks :: Open ( FARPROC lpProc, HINSTANCE hinst, char * pService )
   {
	FARPROC	pfnDdeCallback ;

	pfnDdeCallback = MakeProcInstance ( lpProc, hinst ) ;
	if  ( DdeInitialize ( & idinst, ( PFNCALLBACK ) pfnDdeCallback, CBF_SKIP_CONNECT_CONFIRMS | CBF_SKIP_REGISTRATIONS | CBF_SKIP_UNREGISTRATIONS | CBF_SKIP_DISCONNECTS , 0L ) )
	   {
		MessageBox ( NULL, "Enregistrement DDE impossible",pService, MB_ICONEXCLAMATION | MB_OK ) ;
		return  ( FALSE ) ;
	    }
	hszService = DdeCreateStringHandle ( idinst, pService, 0 ) ;
	DdeNameService ( idinst, hszService, NULL, DNS_REGISTER ) ;

	return  ( TRUE ) ;
    }

/*------------------------------------------------------------------------*/
/*	DdeLinks :: Connect						  */
/*------------------------------------------------------------------------*/
/*	Initialisation d'un lien de conversation avec un service.	  */
/*------------------------------------------------------------------------*/
DdeLink *	DdeLinks :: Connect ( int indice, char * pService )
   {
	DdeLink *	plink ;

				/* Allocation de l'objet de gestion	  */
	plink = new DdeLink ( idinst, pService ) ;

				/* Placement dans la table		  */
	if  ( indice  >=  0  &&  indice  < MAX_LINKS_TABLE )
		pLinksTable [ indice ] = plink ;

	return  ( plink ) ;
    }

/*------------------------------------------------------------------------*/
/*	DdeLinks :: Disconnect						  */
/*------------------------------------------------------------------------*/
/*	Initialisation d'un lien de conversation avec un service.	  */
/*------------------------------------------------------------------------*/
void	DdeLinks :: Disconnect ( int indice )
   {
				/* Placement dans la table		  */
	if  ( indice  >=  0  &&  indice  < MAX_LINKS_TABLE )
		delete pLinksTable [ indice ] ;

	pLinksTable [ indice ] = NULL ;
    }

/*------------------------------------------------------------------------*/
/*	DdeLinks :: Appel						  */
/*------------------------------------------------------------------------*/
/*	Fournit l'objet demandé pour communiquer.			  */
/*------------------------------------------------------------------------*/
DdeLink *	DdeLinks :: Appel ( int indice )
   {
	if  ( indice  >=  0  &&  indice  < MAX_LINKS_TABLE )
		return  ( pLinksTable [ indice ] ) ;

	return  ( NULL ) ;
    }

/*------------------------------------------------------------------------*/
/*	DdeLinks :: Control						  */
/*------------------------------------------------------------------------*/
/*	Controle l'identification à la demande de service		  */
/*------------------------------------------------------------------------*/
int	DdeLinks :: Control ( HSZ hsz1, HSZ hsz2, char * pcontrol )
   {
	char	buffer [ 200 ] ;
				/* Controle le destinataire		  */
	if  ( DdeCmpStringHandles(hszService, hsz2) )
		return  FALSE ;
				/* controle le service			  */
	DdeQueryString ( idinst, hsz1, buffer, sizeof ( buffer ), 0 ) ;
	if  ( strcmpi ( buffer, pcontrol ) )
		return ( FALSE ) ;

	return  ( TRUE ) ;
    }

/*------------------------------------------------------------------------*/
/*	DdeLinks :: Close						  */
/*------------------------------------------------------------------------*/
/*	Fermeture des objets et désalocations.				  */
/*------------------------------------------------------------------------*/
void	DdeLinks :: Close ( )
   {
	int	i ;			/* Comteur pour le tableau	  */

	if  ( idinst )			/* Déconnection officielle	  */
	   {
		DdeNameService ( idinst, hszService, NULL, DNS_UNREGISTER ) ;
		DdeFreeStringHandle ( idinst, hszService ) ;
		DdeUninitialize ( idinst ) ;
	    }

					/* Libération Links finale	  */
	for  ( i = 0; i  <  MAX_LINKS_TABLE; i ++ )
		if  ( pLinksTable [i] )
			delete pLinksTable [i] ;
	memset ( pLinksTable, 0, sizeof ( pLinksTable ) ) ;
    }

/**************************************************************************/
/*	DdeLinks DdeGlobal objet de communication 			  */
/*                                                                        */
/**************************************************************************/
DdeLinks	DdeGlobal ;
