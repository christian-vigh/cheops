/**************************************************************************/
/*                                                                        */
/*  Terminal.cpp  -  Christian Vigh, 94/09/12                             */
/*                   (d'après Sebastien George et Christophe Després).    */
/*                                                                        */
/*  Implémentation de la classe application.                              */
/*                                                                        */
/**************************************************************************/


# include	"AllTerm.h"
# pragma	hdrstop

# include	<toolhelp.h>
# include	"UnixApp.h"
# include	"Event.h"
# include	"ChopJet.h"
# include	"MnCommon.h"
# include	"ChopCfg.h"
# include	"ChopTree.h"
# include	"chopmenu.h"
# include	"ddeobj.h"


TTerminalApplication *	TerminalApplication ;

char			TerminalIniFileName [ 256 ] 	=  TERMINAL_INI ;



/**************************************************************************/
/*                                                                        */
/*  TTerminalApplication :: TTerminalApplication :                        */
/*      Initialiseur de la classe application.                            */
/*                                                                        */
/**************************************************************************/

TTerminalApplication :: TTerminalApplication  ( LPSTR   name )
	: TApplication ( name )
   {
    }





/**************************************************************************/
/*                                                                        */
/*   TTerminalApplication :: Run                                          */
/*      Active une instance précédente de l'application s'il y en a une.  */
/*                                                                        */
/**************************************************************************/

BOOL CALLBACK  Activator  ( HWND  Hwnd, LPARAM )
   {
// Le 2, c'est pour indiquer une activation à la souris (la meilleure)
	SendMessage ( Hwnd, WM_ACTIVATE, 2, 0L ) ;

// Ca, c'est au cas où la fenêtre serait icônifiée
	if  ( IsIconic ( Hwnd ) )
		ShowWindow ( Hwnd, SW_RESTORE ) ;
	return ( TRUE ) ;
     }




int	TTerminalApplication :: Run ( void )
   {

// Si TERMINAL.EXE a déjà été lancé, on se contente de lui redonner la main
	if  ( hPrevInstance )
	   {
		TASKENTRY	TaskEntry ;
		register BOOL	Status ;



		TaskEntry. dwSize = sizeof ( TASKENTRY ) ;
		Status = TaskFirst ( & TaskEntry ) ;

		while  ( Status )
		   {
			if  ( TaskEntry. hInst  ==  hPrevInstance )
			   {
				EnumTaskWindows ( TaskEntry. hTask, ( WNDENUMPROC )  Activator, 0L ) ;
				return ( 0 ) ;   			     
			     }

			Status = TaskNext ( & TaskEntry ) ;
		     }


	// On ne devrait normalement pas arriver ici.
		:: MessageBox ( NULL, "Information", MB_OK,
			"Vous ne pouvez pas lancer plus d'un exemplaire de cette application." ) ;
		return ( 0 ) ;
	    }

	return ( TApplication :: Run ( ) ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TTerminalApplication :: InitMainWindow  :                             */
/*      Crée la fenêtre principale de l'application.                      */
/*                                                                        */
/**************************************************************************/

void	TTerminalApplication :: InitMainWindow  ( void )
   {
// Création de la fenêtre principale
	TerminalWindow	= new  TTerminalWindow ( "Terminal" ) ;
	MainWindow      = TerminalWindow ;

	TerminalWindow -> SetIcon ( this, ICON_TERMINAL ) ;
	TerminalWindow -> AssignMenu ( MENU_TERMINAL ) ;

	TerminalWindow -> StatusBar =  new  TStatusBar ( 0,
						 TGadget :: Recessed,
						 TStatusBar :: CapsLock   |
						 TStatusBar :: NumLock    |
						 TStatusBar :: ScrollLock |
						 TStatusBar :: Overtype ) ;
	TerminalWindow -> StatusBar -> Insert ( 
		* new TTextGadget ( 0, TGadget :: Recessed,
			TTextGadget :: Center, 7 ) ) ;

	TerminalWindow -> StatusBar [0]. SetText ( "Pas encore connecté..." ) ;
	TerminalWindow -> Insert ( * TerminalWindow -> StatusBar, TDecoratedFrame :: Bottom ) ;

    }




/**************************************************************************/
/*                                                                        */
/*  TTerminalApplication :: ScanParameterList -                           */
/*      Récupère les arguments de la ligne de commande.                   */
/*                                                                        */
/**************************************************************************/

void	TTerminalApplication :: ScanParameterList ( int  argc, char **  argv )
   {
	while  ( -- argc )
	   {
		argv = & argv [1] ;

		if  ( ! stricmp ( argv [0], "-INI" )  ||
		      ! stricmp ( argv [0], "-I"   ) )
		   {
			if  ( argv [1]  ==  NULL )
				:: MessageBox ( 0, "Terminal: Erreur de ligne de commande", MB_OK | MB_ICONEXCLAMATION,
					"L'option '-INI' doit etre suivie d'un argument." ) ;
			else
			   {
				argc -- ;
				argv = & argv [1] ;
				strcpy ( TerminalIniFileName, argv [0] ) ;
			     }
		     }
		else if  ( argv [0] [0]  ==  '+'  ||  argv [0] [0]  ==  '-' )
			:: MessageBox ( 0, "Terminal: Erreur de ligne de commande", MB_OK | MB_ICONEXCLAMATION,
				"Option '%s' incorrecte.", argv [0] ) ;
		else  // nom de fichier
			:: MessageBox ( 0, "Terminal: Erreur de ligne de commande", MB_OK | MB_ICONEXCLAMATION,
				"Les noms de fichier ne sont pas admis sur la ligne de commande." ) ;
	     }

// Chercher si un suffixe a été spécifié pour le nom de fichier .INI
	register char *		p 	=  TerminalIniFileName +
					   strlen ( TerminalIniFileName ) - 1 ;
	register bool		found	=  false ;


	while  ( p  >=  TerminalIniFileName  &&  strchr ( "\\/:", * p )  ==  NULL )
	   {
		if  ( * p  ==  '.' )
		   {
			found = true ;
			break ;
		    }

		p -- ;
	     }

	if  ( ! found )
		strcat ( TerminalIniFileName, ".INI" ) ;
       }





/**************************************************************************/
/*                                                                        */
/*  TTerminalApplication :: IdleAction                                    */
/*      Permet, entre deux messages, de scruter la liaison afin de        */
/*      recevoir ceux provenant d'Unix plus rapidement.                   */
/*                                                                        */
/**************************************************************************/

int	TTerminalApplication :: IdleAction ( long int  IdleCount )
   {
// Appel de l'IdleAction de TApplication
	TApplication :: IdleAction  ( IdleCount ) ;

// Ne rien faire si :
//	. La configuration n'est pas encore créée
//	. IdleCount est différent de 0 (car dans ce cas, cela signifie
//	  que IdleAction a été appelée IdleCount fois depuis le dernier
//	  message traité). Ceci permet de ne pas trop monopoliser le système.
	if  ( ! Configuration )
		return ( 0 ) ;


// Récupération de données éventuelles 
	int		receive_count = 0 ;


	for  ( int  i = 0 ; i < Configuration -> GetSessionCount ( ) ; i ++ )
	   {
		if  ( Configuration -> IsOpened ( i ) )
		   {		
			TConnection *   ic  =  Configuration -> Connection ( i ). Connection ;

			if  ( typeid ( * ic )  ==  typeid ( TSerialConnection ) )
			   {
				do
				   {
					ic -> Receive ( ) ;
					receive_count ++ ;
				    }
				while  ( ic -> DataReceived ( ) ) ;
			     }
			else
			   {
				while  ( ic -> DataReceived ( ) )
				   {
					ic -> Receive ( ) ;
					receive_count ++ ;
				    }
			     }
		     }
	     }



// Si on répond FALSE, TApplication :: MessageLoop appel WaitMessage
// Du coup, les écrans Cheops marchent beaucoup moins bien...
	return ( receive_count ) ;
     }


/**************************************************************************/
/*                                                                        */
/*  Callback DDE.                                                  	  */
/*                                                                        */
/**************************************************************************/

# if	defined ( LEBRAIN )
HDDEDATA FAR PASCAL _export DdeCallback ( UINT 		iType,
					  UINT 		/* iFmt */,
					  HCONV 	/* hConv */,
					  HSZ 		hsz1,
					  HSZ 		hsz2,
					  HDDEDATA 	hData,
					  DWORD 	/* dwData1 */,
					  DWORD 	/* dwData2 */ )
   {
	char *	pchar,
		buffer [ 250 ],
		le_brain [ 80 ] ;


	switch  ( iType )
	   {
	     case XTYP_CONNECT :
	     case XTYP_CONNECT_CONFIRM :
		if  ( ! DdeGlobal. Control ( hsz1, hsz2, "Connection" ) )
			return  ( FALSE ) ;
		break ;
	     case XTYP_EXECUTE :
		if  ( ! TheCheopsMenuWindowObject )
		   {
			MessageBox ( NULL, "Le menu Graphique n'a pas été lancé." ,"TERMINAL", MB_ICONSTOP | MB_OK ) ;
			break ;
		    }

		DdeGetData ( hData, buffer, sizeof ( buffer ), 0 ) ;
		pchar = buffer ;
		if  ( ! strncmpi ( buffer, "Exec", 4 ) )	/* Demande d'ouverture d'un écran ou d'un traitement	*/
		   {
			for  ( pchar = buffer; * pchar  &&  * pchar  !=  ':'; pchar ++ ) ;
			if  ( * pchar )
			   {
				* pchar = 0 ;
				EventQueue -> Enqueue (
					TheCheopsMenuWindowObject -> Application -> ApplicationId,
					TheCheopsMenuWindowObject -> ObjectId,
					TheCheopsMenuWindowObject -> Application -> ProcessId,
					WM_MENU_EVENT, MNF_EXECUTE,
					( LPARAM ) pchar,
					strlen ( pchar + 1 ) + 2 ) ;

				sprintf ( le_brain, "Utilise %s_exe", pchar +1 ) ;
				DdeGlobal. Appel ( DDELINK_LEBRAIN ) -> Connect ( ) ;
				DdeGlobal. Appel ( DDELINK_LEBRAIN ) -> Execute ( le_brain ) ;
				DdeGlobal. Appel ( DDELINK_LEBRAIN ) -> Disconnect ( ) ;
			    }
		    }
		if  ( ! strncmpi ( buffer, "Menu", 4 ) )	/* Demande d'ouverture d'un menu	*/
		   {
			for  ( pchar = buffer; * pchar  &&  * pchar  !=  ':'; pchar ++ ) ;
			if  ( * pchar ++ )
			   {
				int	status, size ;

				TheCheopsMenuWindowObject -> SetFocus ( ) ;	/* On garde le Focus pour nous.	*/
				TheCheopsMenuWindowObject -> ProcessMenuFunction ( MNF_OPEN_MENU, pchar, status ,size ) ;
			    }
		    }
		break;
	     case XTYP_WILDCONNECT :
		return ( NULL ) ;
	      }
	return ( ( HDDEDATA ) TRUE ) ;
    }
# endif


/**************************************************************************/
/*                                                                        */
/*  Programme principal.                                                  */
/*                                                                        */
/**************************************************************************/

int	OwlMain  ( int   argc , char **   argv  )
	{
	register int		status ;

# if 	STATIC_COLOR_DLL
	HINSTANCE	hInstance  =  LoadLibrary ( "STATICOL.DLL" ) ;
# endif

	SetMessageQueue ( 120 ) ;
	LoadLibrary ( "BWCC.DLL" ) ;
	
	TerminalApplication = new TTerminalApplication ( "Terminal Unix" ) ;

	TBIVbxLibrary	vbxlib ;

# if	defined ( LEBRAIN )
	if  ( DdeGlobal. Open (( FARPROC ) DdeCallback, ::Module -> GetInstance (),"Terminal" ) )
	   {
		DdeGlobal. Connect ( DDELINK_HYPERAIDE, "HyperAide" ) ;
		DdeGlobal. Connect ( DDELINK_LEBRAIN, "LeBrain" ) ;
# endif

		TerminalApplication -> ScanParameterList ( argc, argv ) ;
		status = TerminalApplication -> Run ( ) ;

# if	defined ( LEBRAIN )
	    }
	DdeGlobal. Close ( ) ;
# endif


# if	STATIC_COLOR_DLL
	FreeLibrary ( hInstance ) ;
# endif


	delete  TerminalApplication ;

	return ( status ) ;
	}
