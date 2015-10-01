/**************************************************************************/
/*                                                                        */
/*  TermWin.cpp  -  Christian Vigh, 94/09/12.                             */
/*                  (d'après Sébastien George et Christophe Després).     */
/*                                                                        */
/*  Implémentation de la classe TTerminalWindow.                          */
/*                                                                        */
/**************************************************************************/


# include	"AllTerm.h"
# pragma	hdrstop

# include	<stdarg.h>
# include	"UnixApp.h"
# include	"Print.h"
# include	"TextFile.h"
# include	"ChopJet.h"
# include	"EditFile.h"
# include	"ChopCfg.h"

# if	defined ( LEBRAIN )
# include	"ddeobj.h"
# endif


TTerminalWindow *	TerminalWindow		=  NULL ;
TDisplayDevice *	Device			=  NULL ;
TUnixApplicationTable *	ApplicationTable	=  NULL ;
THelp *			Help			=  NULL ;
TPrinter *  		Printer			=  NULL ;



DEFINE_RESPONSE_TABLE1 ( TTerminalWindow, TDecoratedFrame )

# if 	STANDALONE
	EV_COMMAND	  ( IDM_File_Open	        , IDMFileOpen ),
# endif

	EV_COMMAND 	  ( IDM_File_Connect		, IDMFileConnect ),
	EV_COMMAND 	  ( IDM_File_Disconnect	 	, IDMFileDisconnect ),
	EV_COMMAND	  ( IDM_File_Add_Session	, IDMFileAddSession ),
	EV_COMMAND 	  ( IDM_File_Remove_Session	, IDMFileRemoveSession ),
	EV_COMMAND 	  ( IDM_File_Log		, IDMFileLog ),
	EV_COMMAND 	  ( IDM_File_Transfer		, IDMFileTransfer ),
	EV_COMMAND 	  ( IDM_File_Receive            , IDMFileReceive ),
	EV_COMMAND 	  ( IDM_File_Save_Configuration , IDMFileSaveConfiguration ),
	EV_COMMAND 	  ( IDM_File_Printer_Config	, IDMFilePrinterConfig ),
	EV_COMMAND	  ( IDM_File_Print_Page		, IDMFilePrintPage ),
	EV_COMMAND 	  ( IDM_File_Print_Continuous	, IDMFilePrintContinuous ),
	EV_COMMAND 	  ( IDM_File_Quit	        , IDMFileQuit ),
	EV_COMMAND 	  ( IDM_Edit_Copy		, IDMEditCopy ),
	EV_COMMAND 	  ( IDM_Edit_Paste		, IDMEditPaste ),
	EV_COMMAND 	  ( IDM_Edit_Redo		, IDMEditRedo ),
	EV_COMMAND 	  ( IDM_Edit_Select_All		, IDMEditSelectAll ),
	EV_COMMAND 	  ( IDM_Options_Display_All     , IDMOptionsDisplayAll ),
	EV_COMMAND 	  ( IDM_Options_Function_Keys   , IDMOptionsDisplayFunctionKeys ),
	EV_COMMAND 	  ( IDM_Options_Configuration	, IDMOptionsConfiguration ),
	EV_COMMAND	  ( IDM_Options_Session_Defaults, IDMOptionsSessionDefaults ),
	EV_COMMAND_AND_ID ( IDM_Options_Network_Defaults, IDMOptionsConnectionDefaults ),
	EV_COMMAND_AND_ID ( IDM_Options_Serial_Defaults , IDMOptionsConnectionDefaults ),
	EV_COMMAND	  ( IDM_Options_Global		, IDMOptionsGlobal ),
	EV_COMMAND 	  ( IDM_Lambda_Shortcut	 	, IDMLambdaShortcut ),
	EV_COMMAND	  ( IDM_Maintenance_Unfreeze    , IDMMaintenanceUnfreeze ),
	EV_COMMAND 	  ( IDM_Maintenance_Objects	, IDMMaintenanceObjects ),
	EV_COMMAND	  ( IDM_Help_About		, IDMHelpAbout ),
	EV_COMMAND	  ( IDM_Help_Index		, IDMHelpIndex ),
	EV_COMMAND	  ( IDM_Window_Next_Session	, IDMWindowNextSession ),
	EV_COMMAND	  ( IDM_Window_Previous_Session	, IDMWindowPreviousSession ),
	EV_COMMAND	  ( IDM_Window_Session_List     , IDMWindowSessionList ),
	EV_COMMAND_AND_ID ( IDM_Window_Session_Base +  0, EvSessionCommand ),
	EV_COMMAND_AND_ID ( IDM_Window_Session_Base +  1, EvSessionCommand ),
	EV_COMMAND_AND_ID ( IDM_Window_Session_Base +  2, EvSessionCommand ),
	EV_COMMAND_AND_ID ( IDM_Window_Session_Base +  3, EvSessionCommand ),
	EV_COMMAND_AND_ID ( IDM_Window_Session_Base +  4, EvSessionCommand ),
	EV_COMMAND_AND_ID ( IDM_Window_Session_Base +  5, EvSessionCommand ),
	EV_COMMAND_AND_ID ( IDM_Window_Session_Base +  6, EvSessionCommand ),
	EV_COMMAND_AND_ID ( IDM_Window_Session_Base +  7, EvSessionCommand ),
	EV_COMMAND_AND_ID ( IDM_Window_Session_Base +  8, EvSessionCommand ),
	EV_COMMAND_AND_ID ( IDM_Window_Session_Base +  9, EvSessionCommand ),
	EV_COMMAND_AND_ID ( IDM_Window_Session_Base + 10, EvSessionCommand ),
	EV_COMMAND_AND_ID ( IDM_Window_Session_Base + 11, EvSessionCommand ),
	EV_COMMAND_AND_ID ( IDM_Window_Session_Base + 12, EvSessionCommand ),
	EV_COMMAND_AND_ID ( IDM_Window_Session_Base + 13, EvSessionCommand ),
	EV_COMMAND_AND_ID ( IDM_Window_Session_Base + 14, EvSessionCommand ),
	EV_COMMAND_AND_ID ( IDM_Window_Session_Base + 15, EvSessionCommand ),
	EV_WM_CHAR,
	EV_WM_KEYDOWN,
	EV_WM_SYSKEYDOWN,
	EV_WM_LBUTTONDBLCLK,
	EV_WM_NCLBUTTONDBLCLK,
	EV_WM_LBUTTONDOWN,
	EV_WM_LBUTTONUP,
	EV_WM_RBUTTONDOWN,
	EV_WM_MOUSEMOVE,
	EV_WM_INITMENUPOPUP,
	EV_WM_SYSCOLORCHANGE,
	EV_COMMAND_ENABLE ( IDM_File_Connect		, CommandEnable ),
	EV_COMMAND_ENABLE ( IDM_File_Disconnect	 	, CommandEnable ),
	EV_COMMAND_ENABLE ( IDM_File_Add_Session	, CommandEnable ),
	EV_COMMAND_ENABLE ( IDM_File_Remove_Session	, CommandEnable ),
	EV_COMMAND_ENABLE ( IDM_File_Save_Configuration , CommandEnable ),
	EV_COMMAND_ENABLE ( IDM_File_Transfer		, CommandEnable ),
	EV_COMMAND_ENABLE ( IDM_File_Receive		, CommandEnable ),
	EV_COMMAND_ENABLE ( IDM_File_Print_Page		, CommandEnable ),
	EV_COMMAND_ENABLE ( IDM_File_Print_Continuous	, CommandEnable ),
	EV_COMMAND_ENABLE ( IDM_Edit_Paste		, CommandEnable ),
	EV_COMMAND_ENABLE ( IDM_Edit_Copy		, CommandEnable ),
	EV_COMMAND_ENABLE ( IDM_Edit_Redo		, CommandEnable ),
	EV_COMMAND_ENABLE ( IDM_Edit_Select_All		, CommandEnable ),   
	EV_COMMAND_ENABLE ( IDM_Options_Function_Keys	, CommandEnable ),
	EV_COMMAND_ENABLE ( IDM_Options_Configuration   , CommandEnable ),
	EV_COMMAND_ENABLE ( IDM_Options_Serial_Defaults , CommandEnable ),
	EV_COMMAND_ENABLE ( IDM_Options_Network_Defaults, CommandEnable ),
END_RESPONSE_TABLE ;



DEFINE_RESPONSE_TABLE1  ( TTerminalWindowClient, TWindow )
	EV_MESSAGE ( WM_CHAR   		, EvChar          ),
	EV_MESSAGE ( WM_KEYDOWN		, EvKeyDown       ),
	EV_MESSAGE ( WM_SYSKEYDOWN      , EvSysKeyDown    ),
	EV_MESSAGE ( WM_LBUTTONDBLCLK	, EvLButtonDblClk ),
	EV_MESSAGE ( WM_LBUTTONDOWN     , EvLButtonDown   ),
	EV_MESSAGE ( WM_LBUTTONUP	, EvLButtonUp	  ),
	EV_MESSAGE ( WM_RBUTTONDOWN     , EvRButtonDown   ),
	EV_MESSAGE ( WM_MOUSEMOVE       , EvMouseMove     ),
	EV_MESSAGE ( WM_DATARECEIVED	, EvDataReceived ),
	EV_MESSAGE ( WM_SENDDATA	, EvSendData ),
	EV_MESSAGE ( WM_UNIXMESSAGE     , EvUnixMessage ),
	EV_WM_SETFOCUS,
	EV_WM_KILLFOCUS,
END_RESPONSE_TABLE ;                                        





/**************************************************************************/
/*                                                                        */
/*  TTerminalWindowClient :: ~TTerminalWindowClient                       */
/*      Libère les objets connexion et device.                            */
/*      Attention ! il est trait important de supprimer Device APRES la   */
/*      destruction de la fenêtre et non avant (comme c'était le cas,     */
/*      puisque Device était supprimé dans le destructeur de              */
/*      TConfiguration).                                                  */
/*      Si on le supprime avant, on obtient un beau plantage...           */
/*                                                                        */
/**************************************************************************/

TTerminalWindowClient :: ~TTerminalWindowClient ( )
   {
    }


/**************************************************************************/
/*                                                                        */
/*  Gestion du caret.                                                     */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindowClient :: EvSetFocus  ( HWND ) 
   {
	if  ( MyDevice )
		MyDevice -> ManageCaret ( TRUE ) ;
    }

		    
void	TTerminalWindowClient :: EvKillFocus  ( HWND  gained ) 
   {
	if  ( MyDevice )
		MyDevice -> ManageCaret ( FALSE ) ;
	Parent -> HoldFocusHWnd ( HWindow, gained ) ;
     }




/**************************************************************************/
/*                                                                        */
/*  TTerminalWindowClient :: Paint                                        */
/*      Redessine la fenêtre.                                             */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindowClient :: Paint ( TDC &, BOOL, TRect & )
   {
	if  ( MyDevice ) 
		MyDevice -> UpdateScreen ( TRect ( 0, 0, 0, 0 ) ) ;
    }




/**************************************************************************/
/*                                                                        */
/*   TTerminalWindowClient :: EvDataReceived :                            */
/*      Réceptionne tous les messages indiquant que des données ont été   */
/*      reçues.                                                           */
/*                                                                        */
/**************************************************************************/

LRESULT	  TTerminalWindowClient :: EvDataReceived  ( WPARAM  , LPARAM )
   {
	char 			buffer [ MAX_PARAMETER_LENGTH ] ;
	register int		i ;
	register char *		p ;

	

	do
	   {
		i = MyConnection -> ReadData ( buffer, MAX_PARAMETER_LENGTH ) ;

		if  ( i )
		   {
			MyDevice -> Write ( buffer, i ) ;

			if  ( MyDevice -> GetContinuousPrint ( ) )
			   {
				p = buffer ;


				while  ( i )
				   {
					MyDevice -> PrintCharacter ( * p ) ;
					p ++, i -- ;
				    }
			    }
		     }
	    }  while  ( i ) ;

	MyDevice -> UpdateCursorPosition ( ) ;

	return ( 0 ) ;
    }




/**************************************************************************/
/*                                                                        */
/*   TTerminalWindowClient :: EvSendData                                  */
/*      Gère le message WM_SENDDATA, qui est envoyé à chaque fois qu'un   */
/*      caractère doit être émis.                                         */
/*      Si lparam est NULL, wparam contient le code du caractère à        */
/*      envoyer.                                                          */
/*      Dans le cas contraire, lparam pointe sur l'adresse d'une chaîne   */
/*      de caractères à émettre, et wparam contient sa longueur.          */
/*                                                                        */
/**************************************************************************/

LRESULT	  TTerminalWindowClient :: EvSendData  ( WPARAM  wparam, LPARAM  lparam )
   {
	char			buffer [ 2 ] ;
	register char *		p ;
	TDisplayDevice *	Device      = MyDevice ;
	TConnection *		Connection  = MyConnection ;



	if  ( lparam  ==  NULL )
	   {
		buffer [0] = ( char ) wparam ;
		buffer [1] = 0 ;
		Connection -> SendData ( buffer, 1 ) ;
		p = buffer ;
	    }
	else
	    {
		p = ( char * ) lparam ;
		Connection -> SendData ( p, wparam ) ;
	     }


	if  ( Device -> GetContinuousPrint ( ) )
	   {
		while  ( * p )
		   {
			Device -> PrintCharacter ( * p ) ;
			p ++ ;
		    }
	    }

	return ( 0L ) ;
     }





/**************************************************************************/
/*                                                                        */
/*  TTerminalWindowClient :: EvUnixMessage                                */
/*      Gère les messages sortants (wparam = FALSE) ou entrants (wparam = */
/*      TRUE).                                                            */
/*      lparam contient un pointeur sur une copie du message ( du type    */
/*      TUnixMessage). Il est de la responsabilité du destinataire de     */
/*      libérer la mémoire allouée pour le message.                       */
/*                                                                        */
/**************************************************************************/

LRESULT		TTerminalWindowClient :: EvUnixMessage  ( WPARAM  wparam, LPARAM  lparam )
   {
// Messages entrants (en provenance d'Unix)
	if  ( wparam )
		ApplicationTable -> HandleMessage ( ( TUnixMessage * ) lparam,
			MyConnection ) ;


	return ( 0L ) ;
	 }






/**************************************************************************/
/*                                                                        */
/*  TTerminalWindow :: TTerminalWindow  :                                 */
/*      Initialiseur de la classe.                                        */
/*                                                                        */
/**************************************************************************/

TTerminalWindow :: TTerminalWindow  ( char *  			title )
	:  TDecoratedFrame ( 0, title, 0, TRUE )
   {
	Attr. AccelTable  =  ACCELERATORS_TERMINAL ;
	Attr. Style      &=  ~( WS_MAXIMIZEBOX | WS_VISIBLE ) ;
	Attr. X		  =  CW_USEDEFAULT ;
	Attr. Y		  =  CW_USEDEFAULT ;

	InSelection 	  =  FALSE ;
	Printer		  =  new  TPrinter ;
	LastPosition. x	  =  20 ;
	LastPosition. y   =  20 ;
    }




/**************************************************************************/
/*                                                                        */
/*  TTerminalWindow :: ~TTerminalWindow                                   */
/*      Libère les ressources allouées dynamiquement, désinstalle le      */
/*      hook destiné à attraper les demandes d'aide (Ctrl-F1) et signale  */
/*      à l'aide en ligne de Windows qu'on en a fini avec elle.           */
/*                                                                        */
/**************************************************************************/

TTerminalWindow :: ~TTerminalWindow ( )
   {
	delete   ApplicationTable ;
	delete   Printer ;
	delete   Help ;
	delete   Configuration ;
	delete	 CheopsConfiguration ;
	delete   StatusBar ;
    }




/**************************************************************************/
/*                                                                        */
/*   TTerminalWindow :: CleanupWindow -                                   */
/*      Enlève toute fenêtre cliente de terminal car sinon, elle et ses   */
/*      filles sera détruite. Or il ne faut pas, car il existe encore     */
/*      des objets qui y font référence via un pointeur.                  */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindow :: CleanupWindow  ( void )
   {
	TDecoratedFrame :: CleanupWindow ( ) ;
    }



/**************************************************************************/
/*                                                                        */
/*   TTerminalWindow :: SetupWindow -                                     */
/*      Initialise le gestionnaire d'écran.                               */
/*                                                                        */
/**************************************************************************/

BOOL	TTerminalWindow :: Create  ( void )
   {
	return  ( TDecoratedFrame :: Create ( ) ) ;
    }


void	TTerminalWindow :: SetupWindow ( void )
    {
	TConfiguration :: TReadStatus		Status ;
	int					Answer ;


// La configuration
	Configuration 		= new  TConfiguration ;
	CheopsConfiguration	= new  TCheopsConfiguration ;

// Créer 
	TDecoratedFrame :: SetupWindow ( ) ;
	TerminalWindow   = this ;
	ApplicationTable = new  TUnixApplicationTable ;


// Initialiser l'aide en ligne
	Help = new  THelp ( this ) ;
	InitializeHelp ( ) ;


ShootAgain :
	Status = Configuration -> Read ( ) ;


	switch  ( Status )
	   {

	// Erreur : TERMINAL.INI n'a pas été trouvé
		case	TConfiguration :: NoIniFile :
			Answer = :: MessageBox ( NULL, "Erreur", MB_YESNOCANCEL | MB_ICONSTOP | MB_TASKMODAL,
				"Le fichier de paramètrage (.INI) nécessaire au lancement"
				" de cette application est introuvable. "
				"Voulez-vous créer un fichier de configuration avec des paramètres par défaut ?\n"
				"(vous devrez ensuite définir au moins une session)" ) ;

			if  ( Answer  ==  IDYES )
			   {
				if  ( Configuration -> CreateDefaultINIFile ( )  && 
				      Configuration -> CreateDefaultEmulation ( ) )
					goto  ShootAgain ;

			    }

			throw  TXAbort ( TXAbort :: NoIniFile, 
				"Le fichier de configuration n'existe pas." ) ;



	// Erreur : pas de configuration par défaut
		case	TConfiguration :: NoConfiguration :
			Answer = :: MessageBox ( NULL, "Erreur", MB_YESNOCANCEL | MB_ICONSTOP | MB_TASKMODAL,
				"Votre fichier de paramètrage (%s) ne contient aucune"
				" configuration par défaut; voulez-vous la définir ?",
					Configuration -> Terminal ( ). IniFileName ) ;

			if  ( Answer  ==  IDYES )
			   {
				Configuration -> Terminal ( ). LambdaUserShortcut = TRUE ;
				if  ( TConfigurationDialog ( NULL ). Execute ( )  ==  IDOK )
					Configuration -> Write ( ) ;
				Configuration -> Terminal ( ). LambdaUserShortcut = FALSE ;
				goto  ShootAgain ;
			     }

			throw  TXAbort ( TXAbort :: NoConfiguration,
					"Aucune configuration n'est définie." ) ;


	// Erreur : pas de session définie pour la configuration par défaut
		case	TConfiguration :: NoSession :
			Answer = :: MessageBox ( NULL, "Erreur", MB_YESNOCANCEL | MB_ICONSTOP | MB_TASKMODAL,
				"Aucune session n'est définie pour la configuration '%s'."
				" Désirez-vous en définir une ?",
					Configuration -> Terminal ( ). Configuration ) ;

			if  ( Answer  ==  IDYES )
			   {
				if  ( TSessionsDialog ( this, Configuration ). Execute ( )  ==  IDOK )
					Configuration -> Write ( ) ;
				goto  ShootAgain ;
			    }

			throw  TXAbort ( TXAbort :: NoSession,
				"Aucune session n'est définie pour la configuration en cours." ) ;
	    }



	SetText ( ) ;


// Replacer la fenêtre à la dernière position sauvegardée
	SetWindowPos ( 0, Configuration -> Terminal ( ). LastX,
		Configuration -> Terminal ( ). LastY, 0, 0,
			SWP_NOSIZE | SWP_NOZORDER ) ;


// ajuster la position de la fenêtre si celle-ci est plus large que l'écran
	TRect		Rect = GetWindowRect ( ) ;
	int		x    = Rect. left, 
			y    = Rect. top ;
	BOOL		move = FALSE ;


	if  ( Rect. Width ( ) > GetSystemMetrics ( SM_CXSCREEN ) )
	   {
		x = - GetSystemMetrics ( SM_CXFRAME ) ;
		move = TRUE ;
	    }


	if  ( Rect. Height ( )  >  GetSystemMetrics ( SM_CYSCREEN ) )
	   {
		y = - GetSystemMetrics ( SM_CYFRAME ) ;
		move = TRUE ;
	    }

	if  ( move )
		SetWindowPos ( 0, x, y,	0, 0, SWP_NOSIZE | SWP_NOZORDER ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TTerminalWindow :: Destroy -                                          */
/*      Détruit la fenêtre.                                               */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindow :: Destroy  ( int  i )
   {
	TDecoratedFrame :: Destroy ( i ) ;
    }




/**************************************************************************/
/*                                                                        */
/*   TTerminalWindow :: CanClose  :                                       */
/*      Détermine si on peut quitter l'application.                       */
/*                                                                        */
/**************************************************************************/

BOOL	TTerminalWindow :: CanClose  ( void )
   {
	register int		i, found, status ;
	register char *		p ;



// Vérifier si des sessions sont encore actives
	for  ( i = found = 0 ; i < MAX_SESSIONS ; i ++ )
	   {
		if  ( Configuration -> IsOpened ( i ) )
			found ++ ;
	    }

// Si oui, demander la confirmation
	if  ( found )
	   {
		if  ( found  >  1 )
			p = "s" ;
		else
			p = "" ;

		if  ( Configuration -> Terminal ( ). ConfirmOnExit )
			status = :: MessageBox ( HWindow, "Quitter", MB_YESNOCANCEL,
							"Il y a encore %d session%s active%s.\n"
							"Désirez-vous quand même quitter l'application ?", 
								found, p, p )  ==  IDYES ;
		else
			status = TRUE ;

		if ( status )
			Configuration -> CloseAll ( ) ;
		else
			return ( FALSE ) ;
	    }
	else
		status = TRUE ;

	if  ( status )
	   {
		TRect		Rect 		= GetWindowRect ( ) ;
		register int	cxScreen	= GetSystemMetrics ( SM_CXSCREEN ),
				cyScreen	= GetSystemMetrics ( SM_CYSCREEN ) ;
		char		buffer [ 32 ] ;


	// Vérifier que les coordonnées sont OK (lorsque la fenêtre est réduite
	// sous forme d'icône, par exemple, il se passe des choses bizarres...)
		if  ( Rect. left  <  cxScreen )
		   {
			Configuration -> Terminal ( ). LastX = Rect. left ;
			wsprintf ( buffer, "%d", Configuration -> Terminal ( ). LastX ) ;
			Configuration -> WriteString ( SETTINGS_SECTION,
					"LastPositionX", buffer ) ;
		    }


		if  ( Rect. top  <  cyScreen )
		   {
			Configuration -> Terminal ( ). LastY = Rect. top ;
			wsprintf ( buffer, "%d", Configuration -> Terminal ( ). LastY ) ;
			Configuration -> WriteString ( SETTINGS_SECTION,
					"LastPositionY", buffer ) ;
		     }
	     }


	if  ( Configuration -> IsDirty ( ) )
	   {
		if  ( :: MessageBox ( HWindow, "Configuration", MB_YESNO,
					 "La configuration a été modifiée.\n"
					 "Désirez-vous l'enregistrer ?" ) ==  IDYES )
			Configuration -> Write ( ) ;
	    }


	return ( status ) ;
    }







# if	STANDALONE

/**************************************************************************/
/*                                                                        */
/*  TTerminalWindow :: IDMFileOpen -                                      */
/*      Ouvre un fichier de texte dans une TEditWindow.                   */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindow :: IDMFileOpen  ( void )
   {
// Initialisation des données pour la boiboite
	static TOpenSaveDialog :: TData	  FilenameData ( OFN_PATHMUSTEXIST |
							 OFN_HIDEREADONLY  |
							 OFN_FILEMUSTEXIST,
						       "Fichiers texte (*.txt)|*.TXT|Tous les fichiers (*.*)|*.*|",
						       0, "", "txt" ) ;

// Et zou
ShootAgain :
	if  ( TFileOpenDialog ( this, FilenameData, 0, "Ouverture d'un fichier" ).
			Execute ( )  ==  IDOK )
	   {
		TEditWindow *  ew = new  TEditWindow ( FilenameData. FileName ) ;

		HCURSOR	        OldCursor  =  :: GetCursor ( ),
				NewCursor  =  :: LoadCursor  ( * GetModule ( ), IDC_WAIT ) ;

		:: SetCursor ( NewCursor ) ;
		ew -> UpdateText ( ) ;
		:: SetCursor ( OldCursor ) ;


		ew -> Create ( ) ;
		ew -> Show   ( SW_SHOW ) ;
	     }
      }

# endif		/*  STANDALONE  */





/**************************************************************************/
/*                                                                        */
/*  IDMFileConnect                                                        */
/*      Connecte la session en cours.                                     */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindow :: IDMFileConnect  ( void )
   {
	if  ( Configuration -> Open ( )  <  0 )
		:: MessageBox ( HWindow, "Erreur de communication",
			MB_OK | MB_ICONEXCLAMATION,
				"La session %s n'a pu être ouverte.",
					Configuration -> SessionName ( ) ) ;
	else if  ( ! Configuration -> Terminal ( ). LastExitWasOk )
	   {
		Configuration -> Terminal ( ). LastExitWasOk = TRUE ;
		//IDMMaintenanceUnfreeze ( ) ;
	    }
    }



/**************************************************************************/
/*                                                                        */
/*   IDMFileDisconnect :                                                  */
/*      Ferme la session en cours.                                        */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindow :: IDMFileDisconnect  ( void )
   {
	Configuration -> Close ( ) ;
	Device -> SetContinuousPrint ( FALSE ) ;
	SetStatusBarText ( 1, 0 ) ;
    }                                                                 




/**************************************************************************/
/*                                                                        */
/*  IDMFileAddSession -                                                   */
/*      Ajoute une session.                                               */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindow :: IDMFileAddSession  ( void )
   {
	TConfiguration *	DialogConfiguration  = new  TConfiguration ;
	char			Buffer [ MAX_PARAMETER_LENGTH ] ;


	DialogConfiguration -> SetPreventAutoOpen ( TRUE ) ;
	DialogConfiguration -> DefaultSession ( ) =
			Configuration -> DefaultSession ( ) ;
	DialogConfiguration -> DefaultConnection ( ) =
			Configuration -> DefaultConnection ( ) ;
	strcpy ( Buffer, "Nouvelle session" ) ;


	if  ( TSessionParametersDialog ( this, IDD_SESSION_PARAMETERS,
			DialogConfiguration,
			& DialogConfiguration -> Connection ( 0 ),
			& DialogConfiguration -> Session ( 0 ),
			0, Buffer ). Execute ( )  ==  IDOK )
	   {
		DialogConfiguration -> SetPreventAutoOpen ( FALSE ) ;
		DialogConfiguration -> Session ( 0 ). LocalSession = TRUE ;
		Configuration -> AddSession (
			Buffer,
			DialogConfiguration -> Connection ( 0 ),
			& DialogConfiguration -> Session ( 0 ) ) ;
	     }

	delete  DialogConfiguration ;
    }





/**************************************************************************/
/*                                                                        */
/*  IDMFileRemoveSession -                                                */
/*      Supprime une session définie manuellement.                        */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindow :: IDMFileRemoveSession  ( void )
   {
// Session ajoutée manuellement : activer la suivante et supprimer cette 
// session "manuelle"
	if  ( Configuration -> Session ( ). LocalSession )
	   {
		register int		id 	=  Configuration -> GetCurrentSession ( ) ;


		Configuration -> SetNextSession ( ) ;

		if  ( id  !=  Configuration -> GetCurrentSession ( ) )
			Configuration -> RemoveSession ( id ) ;
	     }
     }






/**************************************************************************/
/*                                                                        */
/*  TTerminalWindow :: IDMOptionsDisplayAll                               */
/*      Indique d'afficher/ne pas afficher tous les caractères.           */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindow :: IDMOptionsDisplayAll  ( void )
   {
	register BOOL		Display		=  Device -> IsTransparent ( ) ;


	CheckMenuItem ( GetMenu ( ), IDM_Options_Display_All,
		MF_BYCOMMAND | ( ( Display ) ? MF_UNCHECKED : MF_CHECKED ) ) ;
	Device -> SetTransparent ( ! Display ) ;
    }


    


/**************************************************************************/
/*                                                                        */
/*  TTerminalWindow :: IDMOptionsDisplayFunctionKeys                      */
/*      Indique d'afficher/ne pas afficher les pavés de touches.          */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindow :: IDMOptionsDisplayFunctionKeys  ( void )
   {
	register BOOL		Display		=  ! Device -> FunctionKeypad -> 
							IsActive( ) ;


	if ( Display )
		Device -> FunctionKeypad -> Enable ( ) ;
	else
		Device -> FunctionKeypad -> Disable ( ) ;
    }       




/**************************************************************************/
/*                                                                        */
/*  TTerminalWindow :: IDMFileQuit :                                      */
/*      Quitte l'application.                                             */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindow :: IDMFileQuit  ( void )
   {
   CloseWindow ( ) ; 
	}




/**************************************************************************/
/*                                                                        */
/*  TTerminalWindow :: IDMFileSaveConfiguration :                         */
/*      Sauvegarde la configuration dans le fichier .INI .                */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindow :: IDMFileSaveConfiguration  ( void )
   {
	Configuration -> Write ( ) ;
    }

    


/**************************************************************************/
/*                                                                        */
/*  TTerminalWindow :: IDMFileLog :                                       */
/*      Permet d'enregistrer les caractères affichés dans un fichier.     */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindow :: IDMFileLog  ( void )
   {
	char				buffer [ MAX_PARAMETER_LENGTH ] ;
	register char *			p,
		      *			q ;


	if  ( Configuration -> LogFileFP  !=   NULL )
	   {
		fclose ( Configuration -> LogFileFP ) ;
		Configuration -> LogFileFP = NULL ;
		ModifyMenu ( GetMenu ( ), IDM_File_Log,
			MF_BYCOMMAND | MF_UNCHECKED | MF_STRING,
				IDM_File_Log, "&Historique...\tAlt-H" ) ;
		:: MessageBox ( HWindow, "Historique", MB_OK,
			"L'enregistrement dans le fichier historique est terminé." ) ;
		return ;
	     }



ShootAgain :
	strcpy ( buffer, Configuration -> LogFileName ) ;
	p = buffer + strlen ( buffer ) - 1 ;

	while  ( p >= buffer  &&  strchr ( "/\\:", * p )  !=  NULL )
		p -- ;

	if  ( p < buffer )
	   {
		 p = "" ;
		 q = buffer ;
	    }
	else
	   {
		* p  = 0 ;
		q = p + 1 ;
		p = buffer ;
	    }


	TOpenSaveDialog :: TData	FilenameData ( OFN_PATHMUSTEXIST |
						       OFN_HIDEREADONLY,
						       "Historiques (*.log)|*.log|Tous les fichiers (*.*)|*.*|",
						       0, p, "log" ) ;

	strcpy ( FilenameData. FileName, q ) ;


	if  ( TFileOpenDialog ( this, FilenameData, 0, "Création du fichier historique." ).
			Execute ( )  ==  IDOK )
	   {
		strcpy ( Configuration -> LogFileName, FilenameData. FileName ) ;

		if  ( ( Configuration -> LogFileFP = fopen ( Configuration -> LogFileName, "wb" ) )  ==  NULL )
		   {
			sprintf ( buffer, "Impossible de créer le fichier %s.\n"
					  "Voulez-vous recommencer ?",
						Configuration -> LogFileName ) ;
			if  ( :: MessageBox ( HWindow, "Erreur de création",
				MB_YESNO | MB_ICONEXCLAMATION,
				"Impossible de créer le fichier %s.\nVoulez-vous recommencer ?",
				   Configuration -> LogFileName )  ==  IDYES )
				goto  ShootAgain ;
		     }
	     }

	if  ( Configuration -> LogFileFP  !=  NULL )
	   {
		sprintf ( buffer, "&Historique vers le fichier %s",
			Configuration -> LogFileName ) ;
		ModifyMenu ( GetMenu ( ), IDM_File_Log,
			MF_BYCOMMAND | MF_CHECKED | MF_STRING,
				IDM_File_Log, buffer ) ;
	     }
     }







/**************************************************************************/
/*                                                                        */
/*  IDMOptionsCOnfiguration :                                             */
/*      Gestion des configurations.                                       */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindow :: IDMOptionsConfiguration  ( void )
   {
	TConfigurationDialog ( this ). Execute ( ) ;
    }



/**************************************************************************/
/*                                                                        */
/*  TTerminalWindow :: IDMOptionsSessionDefaults                          */
/*      Permet de définir les paramètres par défaut pour les sessions.    */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindow :: IDMOptionsSessionDefaults  ( void )
   {
	register TSessionParameters *	Params =  & Configuration -> DefaultSession ( ) ;


	if  ( TPreferencesDialog ( this, Params ). Execute ( )  ==  IDOK )
		Configuration -> SetDirty ( TRUE ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TTerminalWindow :: IDMOptionsConnectionDefaults                       */
/*      Permet de définir les paramètres par défaut pour les liaisons     */
/*      série ou réseau.                                                  */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindow :: IDMOptionsConnectionDefaults  ( WPARAM  id )
   {
	register TConnectionParameters *	Connection ;
	register TSessionParameters *		Session ;


	if  ( id  ==  IDM_Options_Network_Defaults )
		Connection = & Configuration -> DefaultConnection ( TRUE ) ;
	else
		Connection = & Configuration -> DefaultConnection ( FALSE ) ;

	Session    = & Configuration -> DefaultSession ( ) ;
	

	if  ( TSessionParametersDialog ( this, 
		( id  ==  IDM_Options_Network_Defaults ) ?
			IDD_SESSION_PARAMETERS_NETWORK : IDD_SESSION_PARAMETERS_SERIAL,
		   Configuration, Connection, Session, 0, NULL ). Execute ( )  ==  IDOK )
		Configuration -> SetDirty ( TRUE ) ;
     }




/**************************************************************************/
/*                                                                        */
/*  TTerminalWindow :: SetStatusBarText                                   */
/*      Change le texte de la barre d'état, à l'index spécifié.           */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindow :: SetStatusBarText ( int  	index, 
					      char *  	message, 
					      ... )
   {
	char		Buffer [ MAX_PARAMETER_LENGTH ] ;
	va_list		args ;
   


	va_start ( args, message ) ;
	wvsprintf ( Buffer, message, args ) ;
	va_end ( args ) ;

	( ( TTextGadget * ) ( * StatusBar ) [index] ) -> SetText ( Buffer ) ;
     }

	


/**************************************************************************/
/*                                                                        */
/*  TTerminalWindow :: UpdateConnectionText                               */
/*      Remet à jour le texte de la barre d'état en fonction de l'état de */
/*      la connexion.                                                     */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindow :: UpdateConnectionText  ( TConfiguration :: TSessionState  state )
   {
	switch ( state )
	   {
		case	TConfiguration :: Created :
			SetStatusBarText ( 0, "Connexion fermée." ) ;
			break ;
	
		case	TConfiguration :: Opened :
			SetStatusBarText ( 0, "Connexion établie." ) ;
			break ;
	     }
     }






/**************************************************************************/
/*                                                                        */
/*  TTerminalWindow :: EvChar                                             */
/*      Envoie les frappes du clavier vers la liaison.                    */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindow :: EvChar  ( UINT  c, UINT, UINT )
    {
	char		buffer [2] ;
	UINT		Flag 	=  REGULARCHAR ;
	

	if  ( GetKeyState ( VK_SHIFT ) & 0x8000 )
		Flag |= SHIFT ;
	if  ( GetKeyState ( VK_CONTROL ) &  0x8000 )
		Flag |= CONTROL ;
	if  ( GetKeyState ( VK_MENU ) &  0x8000 )
		Flag |= ALT ;

	if  ( ( c < ' '  ||  c > '~' )  &&  Device -> ProcessKey ( c, Flag ) )
	   {
		Device -> RedoList += c ;
		return ;
	    }


	* buffer = c ;
	Device -> RedoList += c ;
	Device -> LocalToHostCharSet ( buffer, 1 ) ;
	Connection -> SendData ( buffer, 1 ) ;


	if  ( SessionParameters -> LocalEcho )
	   {
		Device -> Write ( buffer, 1 ) ;
	    }
     }




/**************************************************************************/
/*                                                                        */
/*  TTerminalWindow :: EvKeyDown, EvSysKeyDown :                          */
/*      Envoie la séquence d'échappement correspondant à une touche de    */
/*      fonction.                                                         */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindow :: EvSysKeyDown ( UINT   virtkey, UINT  repcnt, UINT  flags )
   {
	GotKey ( TRUE, virtkey, repcnt, flags ) ;
    }

void	TTerminalWindow :: EvKeyDown ( UINT   virtkey, UINT  repcnt, UINT  flags )
   {
	GotKey ( FALSE, virtkey, repcnt, flags ) ;
    }


    
void	TTerminalWindow :: GotKey    ( BOOL  	syskey,
				       UINT   	virtkey, 
				       UINT  	repcnt, 
				       UINT  	flags )
   {
	UINT	Flag 	=  0 ;

	if  ( GetKeyState ( VK_SHIFT ) & 0x8000 )
		Flag |= SHIFT ;
	if  ( GetKeyState ( VK_CONTROL ) &  0x8000 )
		Flag |= CONTROL ;
	if  ( GetKeyState ( VK_MENU ) &  0x8000 )
		Flag |= ALT ;

	Connection -> SetLastTick ( ) ;

	if  ( Device -> ProcessKey ( virtkey, Flag ) )
		return ;

	if  ( syskey )
		TDecoratedFrame :: EvSysKeyDown ( virtkey, repcnt, flags ) ;
	else
		TDecoratedFrame :: EvKeyDown    ( virtkey, repcnt, flags ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  TTerminalWindow :: EvLButtonDblClk                                    */
/*      Entraine le réajustement de la taille de la fenêtre si elle a été */
/*      modifiée.                                                         */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindow :: EvLButtonDblClk  ( UINT, TPoint & )
   {
	Device -> AdjustWindowSize ( ) ;
    }




/**************************************************************************/
/*                                                                        */
/*   TTerminalWindow :: EvLButtonDown, EvLButtonUp, EvMouseMove :         */
/*      Gestion de la sélection.                                          */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindow :: EvLButtonDown  ( UINT, TPoint &  where )
   {
	SetCapture ( ) ;
	Device -> StartSelection  ( where ) ;
    }


void	TTerminalWindow :: EvLButtonUp   ( UINT, TPoint &  where )
   {
	ReleaseCapture ( ) ;
	Device -> ExpandSelection ( where, FALSE ) ;

	if  ( Device -> GetSelectionSize ( )  ==  2 )
		Device -> StopSelection ( ) ;
    }



void	TTerminalWindow :: EvMouseMove  ( UINT  modflags, TPoint &  where )
   {
	if  ( ! Device -> ExpandSelection ( where, TRUE ) )
		TDecoratedFrame :: EvMouseMove ( modflags, where ) ;
    }
    


void	TTerminalWindow :: EvRButtonDown  ( UINT  modflags, TPoint &  where )
   {
	TDecoratedFrame :: EvRButtonDown ( modflags, where ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TTerminalWindow :: CommandEnable                    		  */
/*      Activation de certaines commandes de menu.                        */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindow :: CommandEnable  ( TCommandEnabler &  ce )
    {
	char		Buffer [ MAX_PARAMETER_LENGTH ] ;
	BOOL		flag ;


	switch  ( ce. Id )
	   {
		case	IDM_Edit_Paste :
			flag	=  IsClipboardFormatAvailable ( CF_TEXT ) ;
			ce. Enable ( flag ) ;
			break ;


		case	IDM_Edit_Copy :
			flag    =  Device -> GetSelectionSize ( )  !=  0 ;
			ce. Enable ( flag ) ;
			break ;

		case	IDM_File_Print_Continuous :
			ce. SetCheck ( ( Device -> GetContinuousPrint ( ) ) ?  
						TCommandEnabler :: Checked :
						TCommandEnabler :: Unchecked ) ;

		case	IDM_File_Print_Page :
			ce. Enable ( Configuration -> IsOpened ( ) ) ;
			break ;

		case	IDM_Edit_Redo :
			ce. Enable ( Configuration -> HasAccess ( ) ) ;
			break ;

		case	IDM_Options_Function_Keys :
			flag = Device -> FunctionKeypad -> IsActive ( ) ;
			ce. SetCheck ( ( flag ) ?  TCommandEnabler :: Checked :
						   TCommandEnabler :: Unchecked ) ;
			break ;


		case	IDM_File_Save_Configuration :
			ce. Enable ( Configuration -> HasAccess ( ) ) ;
			break ;

		case	IDM_File_Connect :
			wsprintf ( Buffer, "Ouvrir la session %s",
				Configuration -> SessionTitle ( ) ) ;
			ce. SetText ( Buffer ) ;
			ce. Enable ( ! Configuration -> IsOpened ( ) ) ;
			break ;

		case	IDM_File_Disconnect :
			wsprintf ( Buffer, "Fermer la session %s",
				Configuration -> SessionTitle ( ) ) ;
			ce. SetText ( Buffer ) ;
			ce. Enable ( Configuration -> IsOpened ( ) ) ;
			break ;


		case	IDM_File_Add_Session :
			ce. Enable ( Configuration -> HasAccess ( ) ) ;
			break ;


		case	IDM_File_Remove_Session :
			wsprintf ( Buffer, "Supprimer la session %s",
				Configuration -> SessionTitle ( ) ) ;
			ce. SetText ( Buffer ) ;
			ce. Enable ( Configuration -> HasAccess ( )  &&
				     Configuration -> Session ( ). LocalSession ) ;
			break ;


		case	IDM_File_Transfer :
			ce. Enable ( Configuration -> HasAccess ( ) ) ;
			break ;

		case	IDM_File_Receive :
			ce. Enable ( Configuration -> HasAccess ( ) ) ;
			break ;

		case	IDM_Options_Configuration :
		case	IDM_Options_Serial_Defaults :
		case	IDM_Options_Network_Defaults :
			ce. Enable ( Configuration -> HasAccess ( ) ) ;
			break ;
	    }                       
    }





/**************************************************************************/
/*                                                                        */
/*   TTerminalWindow :: EvSessionCommand                                  */
/*      Intercepte les commandes liées aux sessions (définies dans le     */
/*      menu Fenêtre).                                                    */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindow :: EvSessionCommand  ( uint  id )
   {
	if  ( id  >=  IDM_Window_Session_Base    &&
		id  <   IDM_Window_Session_Base + MAX_SESSIONS )
		IDMWindowSetCurrentSession ( id - IDM_Window_Session_Base ) ;
     }





/**************************************************************************/
/*                                                                        */
/*  TTerminalWindow :: IDMWindowNextSession,                              */
/*                     IDMWindowPreviousSession,                          */
/*                     IDMWindowSetCurrentSession,                        */
/*                     IDMWindowSessionList                               */
/*      Gestion des sessions.                                             */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindow :: IDMWindowNextSession ( void )
   {
	Configuration -> SetNextSession ( ) ;
    }


void	TTerminalWindow :: IDMWindowPreviousSession ( void )
   {
	Configuration -> SetPreviousSession ( ) ;
    }


void	TTerminalWindow :: IDMWindowSetCurrentSession  ( WPARAM  id )
   {
	Configuration -> SetCurrentSession  ( id ) ;
    }



void	TTerminalWindow :: IDMWindowSessionList ( void )
   {
	int		SessionId ;


	if  ( Configuration -> GetSessionCount ( )  >  1 )
	   {
		if  ( TChooseSessionDialog ( this, & SessionId ). Execute ( )  ==  IDOK )
			IDMWindowSetCurrentSession ( SessionId ) ;
	    }
    }




/**************************************************************************/
/*                                                                        */
/*  TTerminalWindow :: IDMOptionsGlobal                                   */
/*      Ouvre la boîte de dialogue qui permet de définir les paramètres   */
/*      généraux.                                                         */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindow :: IDMOptionsGlobal  ( void )
   {
	TGeneralOptionsDialog ( this ). Execute ( ) ;
    }




/**************************************************************************/
/*                                                                        */
/*   TTerminalWindow :: EvInitMenuPopup                                   */
/*      Construit le menu Fenêtre en fonction des sessions actuellement   */
/*      définies.                                                         */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindow :: EvInitMenuPopup  ( HMENU   	Menu,
					      uint    	Index,
					      BOOL	IsSysMenu )
   {
	register int		i, count ;
	register char *		Title ;



// On détermine si c'est bien le menu qui nous intéresse
	i = GetMenuItemID  ( Menu, 0 ) ;

	if  ( IsSysMenu  ||  i  !=   IDM_Window_Next_Session )
	   {
		TDecoratedFrame :: EvInitMenuPopup ( Menu, Index, IsSysMenu ) ;
		return ;
	    }


// Etat des éléments Session suivante/précédente
	i =  ( Configuration -> GetSessionCount ( )  >  1 ) ?  MF_ENABLED : MF_GRAYED ;
	EnableMenuItem ( Menu, IDM_Window_Next_Session    , i | MF_BYCOMMAND ) ;
	EnableMenuItem ( Menu, IDM_Window_Previous_Session, i | MF_BYCOMMAND ) ;
	

// Effacement de tous les éléments de ce menu (on va le reconstruire)
	count  =  GetMenuItemCount ( Menu ) ;

	for  ( i = 2 ; i < count ; i ++ )
		DeleteMenu ( Menu, 2, MF_BYPOSITION ) ;

// Ajout des éléments fixes
	AppendMenu ( Menu, MF_SEPARATOR, 0, 0 ) ;

// Ajout des sessions
	for  ( i = 0 ; i < MAX_SESSIONS ; i ++ )
	   {
		if  ( Configuration -> SessionState ( i )  !=  TConfiguration :: Free )
		   {
			Title = Configuration -> Session ( i ). WindowTitle ;

			if  ( ! * Title )
				Title = Configuration -> Terminal ( ). SessionSections [i] ;

			AppendMenu ( Menu, MF_STRING | MF_ENABLED, 
				IDM_Window_Session_Base + i,
					Title ) ;
		     }
	      }


// On laisse faire TDecoratedFrame pour qu'il appelle notre commandEnable,
// qui décidera de l'état des 2 premiers éléments du menu.
	DrawMenuBar ( ) ;
     }






/**************************************************************************/
/*                                                                        */
/*  TTerminalWindow :: IDMEditCopy, IdmEditPaste :       		  */
/*      Gestion du presse-papiers.                                        */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindow :: IDMEditCopy  ( void )
   {
	register int		size  	=  Device -> GetSelectionSize ( ) ;


	if  ( size  &&  :: OpenClipboard ( HWindow ) )
	   {
		register char *		buffer 	   =  new char [ size + 1 ] ;
		register int		real_size  =  Device -> GetSelection ( buffer ) ;
		register void far *	memstr ;
		HANDLE			hmem       =  GlobalAlloc ( GMEM_DDESHARE,
								real_size + 1 ) ;

		if  ( ! hmem )
		   {
			delete [ ]  buffer ;
			:: MessageBox ( HWindow, "Erreur", MB_OK | MB_ICONEXCLAMATION,
					"Pas assez de mémoire pour copier la sélection." ) ;
			return ;
		    }


		memstr = GlobalLock ( hmem ) ;
		:: EmptyClipboard ( ) ;

		memcpy ( memstr, buffer, real_size + 1 ) ;
		GlobalUnlock ( hmem ) ;

		:: SetClipboardData ( CF_TEXT, hmem ) ;
		:: CloseClipboard ( ) ;

		delete [] buffer ;
		Device -> StopSelection ( ) ;
	     }
      }




void	TTerminalWindow :: IDMEditPaste  ( void )
   {
	if  ( IsClipboardFormatAvailable ( CF_TEXT )  &&
		:: OpenClipboard ( HWindow ) )
	   {
		HANDLE			hmem       =  :: GetClipboardData ( CF_TEXT ) ;
		char far *		memstr ;


		memstr = ( char * ) GlobalLock ( hmem ) ;


		if  ( memstr  !=  NULL )
		   {
			register char far *	p	=  memstr,
					  *     q 	=  memstr ;
			register int		i ;

			while  ( * p )
			   {
				if  ( ( * p  ==  '\r'  &&  * ( p + 1 )  ==  '\n' ) ||
				      ( * p  ==  '\n'  &&  * ( p + 1 )  ==  '\r' ) )
				   {
					* q ++ = '\r' ;
					p += 2 ;
				    }
				else
					* q ++ = * p ++ ;
			     }
			* q = 0 ;
			i = lstrlen ( memstr ) ;

			Device -> LocalToHostCharSet ( memstr, i ) ;
			Connection -> SendHugeData ( memstr, i ) ;
			Device -> HostToLocalCharSet ( memstr, i ) ;
			GlobalUnlock ( hmem ) ;
		     }

		:: CloseClipboard ( ) ;
	     }
   }





/**************************************************************************/
/*                                                                        */
/*   TTerminalWindow :: IDMEditSelectAll                                  */
/*      Sélectionne le contenu de la fenêtre entière.                     */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindow :: IDMEditSelectAll  ( void )
   {
	register int		x  =  Device -> CharacterWidth  * Device -> ScreenWidth,
				y  =  Device -> CharacterHeight * Device -> ScreenHeight ;


	Device -> StartSelection ( TPoint ( 0, 0 ) ) ;
	Device -> ExpandSelection ( TPoint ( x - 1, y - 1 ), FALSE ) ;
     }




/**************************************************************************/
/*                                                                        */
/*  TTerminalWindow :: IDMFileTransfer, IDMFileReceive                    */
/*      Transfère un fichier de Windows vers Unix (et vice-versa).        */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindow :: IDMFileTransfer  ( void )
   {
	TDosUnixTransferDialog   TransferDialog ( this, TDosUnixTransferDialog :: Send ) ;


	TransferDialog. Execute ( ) ;
     }



void	TTerminalWindow :: IDMFileReceive  ( void )
   {
	TDosUnixTransferDialog   TransferDialog ( this, TDosUnixTransferDialog :: Receive ) ;


	TransferDialog. Execute ( ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TTerminalWindow :: IDMFilePrinterConfig                               */
/*      Permet de choisir l'imprimante.                                   */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindow :: IDMFilePrinterConfig  ( void )
   {
	Printer -> Setup ( this ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TTerminalWindow :: IDMFilePrintPage                                   */
/*      Imprime la page-écran.                                            */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindow :: IDMFilePrintPage ( void )
   {
	if  ( ! Device -> InPrintMode )
		Device -> PrintPage ( ) ;
	else
		:: MessageBox ( HWindow, "Problème d'impression", MB_OK,
			"Une impression en provenance du système distant est déjà "
			"en cours sur l'imprimante '%s'; réessayez d'imprimer cette "
			"page-écran plus tard.",
				Printer -> GetSetup ( ). GetOutputName ( ) ) ;
    }




/**************************************************************************/
/*                                                                        */
/*   TTerminalWindow :: IDMFilePrintContinuous                            */
/*      Met la session en mode d'impression continu.                      */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindow :: IDMFilePrintContinuous  ( void )
   {
	Device -> SetContinuousPrint ( -1 ) ;
    } 




/**************************************************************************/
/*                                                                        */
/*  TTerminalWindow :: IDMEditRedo                                        */
/*      Ouvre une boîte de dialogue permettant de choisir une commande    */
/*      parmi celles déjà saisies.                                        */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindow :: IDMEditRedo  ( void )
   {
	char		buffer [ MAX_PARAMETER_LENGTH ] ;

	
	if  ( TRedoDialog ( this, buffer ). Execute ( )  ==  IDOK )
	   {
		strcat ( buffer, "\r" ) ;
		Connection -> SendData ( buffer, strlen ( buffer ) ) ;
	    }
    }




/**************************************************************************/
/*                                                                        */
/*  TTerminalWindow :: CmAbout                                            */
/*      Affiche la boîte de dialogue "à propose de".                      */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindow :: IDMHelpAbout  ( void )
   {
	TAboutDialog ( this ). Execute ( ) ;
    }


void	TTerminalWindow :: IDMHelpIndex  ( void )
   {
# if	defined ( LEBRAIN )
	DdeGlobal. Appel ( DDELINK_HYPERAIDE ) -> Connect ( ) ;
	DdeGlobal. Appel ( DDELINK_HYPERAIDE ) -> Execute ( "Ouvrir:Helpindx.htm" ) ;
	DdeGlobal. Appel ( DDELINK_HYPERAIDE ) -> Disconnect ( ) ;
# endif
    }

/**************************************************************************/
/*                                                                        */
/*  TTerminalWindow :: IDMMaintenanceUnfreeze -                           */
/*      Tente de débloquer la connexion en cours (si l'utilisateur a      */
/*      éteint son micro sans quitter Cheops graphique, par exemple).     */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindow :: IDMMaintenanceUnfreeze ( void )
   {
	static char	sync [ ]  =  { SYNC_CHAR, '\n', '\n' } ;


// Sur réception de SYNC_CHAR, comdaemon va réémettre son dernier message.
// Comme TERMINAL a été relancé, aucune application n'est connectée pour lui.
// A réception du message de comdaemon, il va donc retourner une réponse
// avec comme numéro de fonction UWF_NULL_FUNCTION, provoquant ainsi l'arrêt
// de comdaemon
	Connection -> SendData ( sync, sizeof ( sync ) ) ;
     }




/**************************************************************************/
/*                                                                        */
/*  TTerminalWindow :: IDMMaintenanceObjects -                            */
/*      Affiche la liste des objets encore alloué pour les applications.  */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindow :: IDMMaintenanceObjects ( void )
   {
	TObjectListDialog ( this ). Execute ( ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TTerminalWindow :: Paint                                              */
/*      Redessine la fenêtre.                                             */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindow :: Paint ( TDC &  dc, BOOL  erase, TRect &  rect )
   {
	TDecoratedFrame :: Paint ( dc, erase, rect ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TTerminalWindow :: EvNCLButtonDblClk                                  */
/*      Positionne la fenêtre Terminal dans le coin supérieur gauche de   */
/*      l'écran, en faisant en sorte que les bordures haute et gauche     */
/*      soient en dehors de l'écran, si la touche shift est appuyée.      */
/*      Sinon, n'effectue le repositionnement qu'horizontalement.         */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindow :: EvNCLButtonDblClk  ( uint, TPoint & )
   {
	register int		x	=  - GetSystemMetrics ( SM_CXFRAME ),
				y ;

	if  ( GetKeyState ( VK_SHIFT ) & 0x8000 )
		y = - GetSystemMetrics ( SM_CYFRAME ) ;
	else
		y = GetWindowRect ( ). top ;


	SetWindowPos ( NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER ) ;
    }






/**************************************************************************/
/*                                                                        */
/*  TTerminalWindow :: EvSysColorChange                                   */
/*      Remet à jour les couleurs de texte et de fond après  changement   */
/*      de la palette système.                                            */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindow :: EvSysColorChange  ( void )
   {
	register int			i ;
	register TDisplayDevice *       Device ;
	COLORREF			NewFore		=  GetSysColor ( COLOR_WINDOWTEXT ),
					NewBack		=  GetSysColor ( COLOR_WINDOW ) ;



// Pour chaque session définie...
	for  ( i = 0 ; i < MAX_SESSIONS ; i ++ )
	   {
		if  ( Configuration -> SessionState ( i )  ==  TConfiguration :: Free )
			continue ;

	// ... mettre à jour le texte de la mémoire écran
		Device = Configuration -> Connection ( i ). Device ;

		Device -> SysColorChange ( Configuration -> Session ( ). ForegroundColor,
					   Configuration -> Session ( ). BackgroundColor,
						NewFore, NewBack ) ;

		Configuration -> Session ( i ). BackgroundColor = NewBack ;
		Configuration -> Session ( i ). ForegroundColor = NewFore ;
	     }

// Rafraîchir l'écran du terminal
	Device = Configuration -> Connection ( ). Device ;
	Device -> UpdateScreen ( 0, 0, 0, 0 ) ;
    }






/**************************************************************************/
/*                                                                        */
/*  TTerminalWindow ::ComputeNextPosition                                 */
/*      Calcule la nouvelle position par défaut d'une fenêtre.            */
/*                                                                        */
/**************************************************************************/

void	TTerminalWindow :: ComputeNextPosition  ( TWindow *  win )
   {
	if  ( win  &&  win -> HWindow )
	   {
		register int	xFrame	 =  GetSystemMetrics ( SM_CXFRAME ),
				yFrame	 =  GetSystemMetrics ( SM_CYFRAME ),
				yCaption =  GetSystemMetrics ( SM_CYCAPTION ),
				yMenu	 =  GetSystemMetrics ( SM_CYMENU ) ;
		register int	x	 =  yCaption + xFrame,
				y	 =  yFrame + yCaption + ( yMenu / 2 ) ;
		TRect		Rect ;



		win -> GetWindowRect ( Rect ) ;

		if  ( LastPosition. x + x + Rect. Width ( )   >  
					GetSystemMetrics ( SM_CXSCREEN )  ||
		      LastPosition. y + y + Rect. Height ( )  >  
					GetSystemMetrics ( SM_CYSCREEN ) )
		   {
			LastPosition. x = 20 ;
			LastPosition. y = 20 ;
		    }
		else
		   {
			LastPosition. x += x ;
			LastPosition. y += y ;
		    }

		win -> SetWindowPos ( 0, LastPosition. x, LastPosition. y,
				0, 0, SWP_NOSIZE | SWP_NOZORDER ) ;
	      }
        }

		
