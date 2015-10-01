/**************************************************************************/
/*                                                                        */
/*   Dialogs.cpp  -  Christian Vigh, 94/09/19.                            */
/*                                                                        */
/*   Implémentation des classes de dialogue.                              */
/*                                                                        */
/**************************************************************************/


# include	"AllTerm.h"
# pragma	hdrstop

# include 	"HideWind.h"
# include	"UnixApp.h"







/**************************************************************************/
/**************************************************************************/
/********                                                          ********/
/********          Implémentation de la classe TRedoDialog         ********/
/********                                                          ********/
/**************************************************************************/
/**************************************************************************/

DEFINE_RESPONSE_TABLE1 ( TRedoDialog, TDialog )
	EV_COMMAND	( IDOK		, CmOk ),
	EV_COMMAND	( IDCANCEL	, CmCancel ),
END_RESPONSE_TABLE ;



BOOL	TRedoDialog :: EvInitDialog  ( HWND  hwnd )
   {
	TDialog :: EvInitDialog ( hwnd ) ;


	for  ( int  i = Device -> RedoList. Size ( ) - 1 ; i >= 0 ; i -- )
		SendDlgItemMessage ( IDC_REDO_LIST, CB_ADDSTRING, 0, 
			( LPARAM ) Device -> RedoList [i] ) ;
		
	SendDlgItemMessage ( IDC_REDO_LIST, CB_SELECTSTRING, -1,
		( LPARAM ) Device -> RedoList [ Device -> RedoList. Size ( ) - 1 ] ) ;

	return ( TRUE ) ;
    }




void	TRedoDialog :: CmOk  ( void )
   {
	GetDlgItemText ( IDC_REDO_LIST, OutputBuffer, MAX_PARAMETER_LENGTH ) ;

	if  ( SendDlgItemMessage ( IDC_REDO_LIST, CB_FINDSTRINGEXACT, 0,
			( LPARAM ) ( LPSTR ) OutputBuffer )  ==  CB_ERR )
		Device -> RedoList += OutputBuffer ;

	TDialog :: CmOk ( ) ;
    }






/**************************************************************************/
/**************************************************************************/
/********                                                          ********/
/********    Implémentation de la classe TApplicationTableDialog    ********/
/********                                                          ********/
/**************************************************************************/
/**************************************************************************/

DEFINE_RESPONSE_TABLE1 ( TApplicationTableDialog, TDialog )
	EV_COMMAND	( IDOK		, CmOk ),
	EV_COMMAND	( IDCANCEL	, CmCancel ),
	EV_COMMAND	( IDC_APP_KILL  , CmKill ),
END_RESPONSE_TABLE ;



BOOL	TApplicationTableDialog :: EvInitDialog  ( HWND  hwnd )
   {
	TDialog :: EvInitDialog ( hwnd ) ;


// Remplissage de la liste
	for  ( int  i = 0 ; i < ApplicationCount ; i ++ )
	   {
		if  ( Applications [i]  !=  NULL )
			SendDlgItemMessage ( IDC_APPLICATION_LIST, CB_ADDSTRING, 0,
			( LPARAM ) ( char far * ) Applications [i] -> GetName ( ) ) ;
	    }

	//SendDlgItemMessage ( IDC_PORT, CB_SETCURSEL, 0, 0L ) ;
	return ( TRUE ) ;
     }




void	TApplicationTableDialog :: CmOk  ( void )
   {
	register int	index 	=  ( int ) SendDlgItemMessage ( IDC_APPLICATION_LIST,
					CB_GETCURSEL, 0L ) ;

	* UserChoice = GetApplicationIndex ( index ) ;
	* Kill       = FALSE ;
    }



void	TApplicationTableDialog :: CmKill ( void )
   {
	CmOk ( ) ;
	* Kill = TRUE ;
    }



int	TApplicationTableDialog :: GetApplicationIndex  ( int  selected )
   {
	register int		i, j ;


	for  ( i = j = 0 ; i < ApplicationCount ; i ++ )
	   {
		if  ( Applications [i]  !=  NULL )
			j ++ ;

		if  ( j  ==  selected + 1 )
			return ( i ) ;
	    }

	return ( -1 ) ;
     }






/**************************************************************************/
/**************************************************************************/
/********                                                          ********/
/********    Implémentation de la classe TDosUnixTransferDialog    ********/
/********                                                          ********/
/**************************************************************************/
/**************************************************************************/

# define	DOSUNIX_TRANSFER_COMMAND	"recf"
# define	UNIXDOS_TRANSFER_COMMAND	"senf"


DEFINE_RESPONSE_TABLE1 ( TDosUnixTransferDialog, TDialog )
	EV_COMMAND	( IDOK		     , CmOk ),
	EV_COMMAND	( IDCANCEL	     , CmCancel ),
	EV_COMMAND	( IDC_TRANSFER_CHOOSE, CmChoose ),
END_RESPONSE_TABLE ;



BOOL	TDosUnixTransferDialog :: EvInitDialog  ( HWND  hwnd )
   {
	TDialog :: EvInitDialog ( hwnd ) ;

	SetDlgItemText ( IDC_DOS_FILE , DosFile ) ;
	SetDlgItemText ( IDC_UNIX_FILE, UnixFile ) ;
	CheckDlgButton ( IDC_BINARY_TRANSFER, Binary ) ;

	return ( TRUE ) ;
     }



void	TDosUnixTransferDialog :: CmChoose  ( void )
    {
	char				buffer [ MAX_PARAMETER_LENGTH ] ;
	register char *			p,
		      *			q ;
	DWORD				Flag ;


	strcpy ( buffer, DosFile ) ;
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


	Flag = ( Direction  ==  Send ) ?  OFN_FILEMUSTEXIST : 0 ;

	TOpenSaveDialog :: TData	FilenameData ( Flag |
						       OFN_PATHMUSTEXIST |
						       OFN_HIDEREADONLY,
						       "Tous les fichiers (*.*)|*.*|",
						       0, p, "*" ) ;

	strcpy ( FilenameData. FileName, q ) ;


	if  ( TFileOpenDialog ( this, FilenameData, 0,
		( Direction  ==  Send ) ?
			"Sélection du fichier à transférer" :
			"Sélection du fichier à recevoir" ).
				Execute ( )  ==  IDOK )
	   {
		strcpy ( DosFile, FilenameData. FileName ) ;
		SetDlgItemText ( IDC_DOS_FILE , DosFile ) ;
	    }
     }




void	TDosUnixTransferDialog :: CmOk  ( void )
   {
	char			buffer [ MAX_PARAMETER_LENGTH * 3 ] ;
	register char *         p 	=  DosFile ;


// Récupérer les noms de fichier
	GetDlgItemText ( IDC_DOS_FILE , DosFile , MAX_PARAMETER_LENGTH ) ;
	GetDlgItemText ( IDC_UNIX_FILE, UnixFile, MAX_PARAMETER_LENGTH ) ;

	Binary = ( IsDlgButtonChecked ( IDC_BINARY_TRANSFER ) ) ?
			TRUE : FALSE ;


// Remplacer les '\\' du fichier Dos par des '/'
	while  ( * p )
	   {
		if  ( * p  ==  '\\' )
			* p = '/' ;
		p ++ ;
	    }

// Construire la commande Unix et balancer la purée
	if  ( Direction  ==  Send )
	   {
		sprintf ( buffer, "%s %s \"%s\" \"%s\"\r", DOSUNIX_TRANSFER_COMMAND,
			( Binary ) ? "-binary" : "-text",
				DosFile, UnixFile ) ;
	    }
	else
	   {
		sprintf ( buffer, "%s %s \"%s\" \"%s\"\r", UNIXDOS_TRANSFER_COMMAND,
			( Binary ) ? "-binary" : "-text",
				UnixFile, DosFile ) ;
	    }

	Connection -> SendData ( buffer, strlen ( buffer ) ) ;


// Important...
	TDialog :: CmOk ( ) ;
    }





/**************************************************************************/
/**************************************************************************/
/********                                                          ********/
/********  Implémentation de la classe TOpenConfigurationDialog    ********/
/********                                                          ********/
/**************************************************************************/
/**************************************************************************/

DEFINE_RESPONSE_TABLE1  ( TOpenConfigurationDialog, TDialog )
	EV_LBN_DBLCLK  ( IDC_CONFIGURATION_LIST, CmOk ),
	EV_COMMAND     ( IDOK       , CmOk ),	
END_RESPONSE_TABLE ;


//
// Initialise la boîte en adaptant les titres au contexte
// (Ouverture/modification d'une session ou d'une configuration)
//
BOOL	TOpenConfigurationDialog :: EvInitDialog  ( HWND  hwndfocus )
   {
	register BOOL		Status  = TDialog :: EvInitDialog ( hwndfocus ) ;


	UpdateListBox ( ) ;
	return  ( Status ) ;
    }



//
// Relit le fichier .Ini pour mettre à jour la liste des configurations ou des
// sessions
//
void	TOpenConfigurationDialog :: UpdateListBox  ( void )
   {
	char			List  	     [ 4096 ] ;
	register char *		p ;
	register char *		sp		=  SectionPrefix ( ) ;
	register int		length		=  strlen ( sp ) ;
	register int		i ;


	ListBox -> ClearList ( ) ;
	Configuration -> GetSectionList ( List, sizeof ( List ) ) ;

	for  ( p = List, i = 0 ; * p ; p += strlen ( p ) + 1 )
	   {
		if  ( ! strnicmp ( sp, p, length ) )
		   {
			ListBox -> AddString ( p + length ) ;
			i ++ ;
		    }
	    }

	ListBox -> SetSelIndex ( 0 ) ;
	ElementsFound ( i ) ;
     }
		



//
// Ferme la boîte de dialogue
//
void	TOpenConfigurationDialog :: CmOk  ( void )
   {
	if  ( GetCurrentSelection ( ConfigurationName ) )
		TDialog :: CmOk ( ) ;
	else
		TDialog :: CmCancel ( ) ;
     }








/**************************************************************************/
/**************************************************************************/
/********                                                          ********/
/********  Implémentation de la classe TConfigurationDialog  	   ********/
/********                                                          ********/
/**************************************************************************/
/**************************************************************************/

DEFINE_RESPONSE_TABLE1  ( TConfigurationDialog, TDialog )
	EV_COMMAND  ( IDOK		, CmOk     ),
	EV_COMMAND  ( IDC_DEFAULT	, CmDefault ),
	EV_COMMAND  ( IDC_MODIFY  	, CmModify ),
	EV_COMMAND  ( IDC_CREATE	, CmCreate ),
	EV_COMMAND  ( IDC_DELETE	, CmDelete ),
END_RESPONSE_TABLE ;




//
// CmOk : Recharge la configuration en cours si elle a été modifiée
//

void	TConfigurationDialog :: CmOk ( void )

   {
	if  ( CurrentWasModified )
	   {
		Configuration -> CloseAll ( ) ;
		Configuration -> Initialize ( ) ;
		Configuration -> Read ( ) ;
	    }

	TDialog :: CmOk ( ) ;
     }


//
// Modifie une configuration
//
void	TConfigurationDialog :: CmModify  ( void )
   {
	register char *		Name 	   =  DialogConfiguration -> Terminal ( ). Configuration ;
	int			Status ;
	BOOL			WasCurrent =  FALSE ;


	if  ( GetCurrentSelection ( Name ) )
	   {
		if  ( IsCurrentConfiguration ( Name ) )
		   {
			Status = :: MessageBox ( HWindow, "Erreur", MB_YESNOCANCEL | MB_ICONEXCLAMATION,
				"'%s' est la configuration actuellement utilisée. " 
				"La modification de ses paramètres provoquera la fermeture des connexions.\n"
				"Voulez-vous modifier cette configuration ?",
					Name ) ;

			if  ( Status  !=  IDYES )
				return ;

			:: Configuration -> CloseAll ( ) ;
			WasCurrent = TRUE ;
		    }

		DialogConfiguration -> Read ( Name ) ;

		if  ( TSessionsDialog ( this, DialogConfiguration ). Execute ( )  ==  IDOK )
		   {
			DialogConfiguration -> Write ( TRUE ) ;

			if  ( WasCurrent )
				CurrentWasModified = TRUE ;
		     }
	    }
     }
	




//
//  Supprime une configuration
//
void	TConfigurationDialog :: CmDelete  ( void )
   {
	register char *		Name 	=  DialogConfiguration -> Terminal ( ). Configuration ;
	register char *		Section ;



	if  ( GetCurrentSelection ( Name ) )
	   {
		if  ( IsCurrentConfiguration ( Name ) )
		   {
			:: MessageBox ( HWindow, "Erreur", MB_OK | MB_ICONEXCLAMATION,
				"Vous ne pouvez pas supprimer la configuration en cours." ) ;
			return ;
		    }
		       
		if  ( :: MessageBox  ( HWindow, "Suppression", MB_YESNOCANCEL,
			"Voulez-vous supprimer la configuration '%s' ?",
				Name )  ==  IDYES )
		   {
			DialogConfiguration -> Read ( Name ) ;
			Section = DialogConfiguration -> ConfigurationSectionName ( Name ) ;
			DialogConfiguration -> WriteString ( Section, NULL, NULL ) ;
			UpdateListBox ( ) ;
		    }
	     }
     }







//
//  Crée une configuration
//
void	TConfigurationDialog :: CmCreate  ( void )
   {
	char			NewOne [ MAX_PARAMETER_LENGTH ] ;
	register char *		Name 	=  DialogConfiguration -> Terminal ( ). Configuration ;


	* NewOne = 0 ;


ShootAgain :
	if  ( TInputDialog ( this, "Création d'une configuration",	
			"&Nom de la configuration", 
				NewOne, sizeof ( NewOne ) ). Execute ( )  ==  IDOK )
	   {
		if  ( ListBox -> FindExactString ( NewOne, 0 )  >=  0 )
		   {
			:: MessageBox ( HWindow, "Erreur", MB_OK | MB_ICONEXCLAMATION,
				"La configuration '%s' existe déjà !", NewOne ) ;
			goto  ShootAgain ;
		    }

		DialogConfiguration -> Initialize ( ) ;
		DialogConfiguration -> SetPreventAutoOpen ( TRUE ) ;
		DialogConfiguration -> Read ( NewOne ) ;
		strcpy ( Name, NewOne ) ;        
		DialogConfiguration -> DefaultSession    ( )       = Configuration -> DefaultSession ( );
		DialogConfiguration -> DefaultConnection ( FALSE ) = Configuration -> DefaultConnection ( FALSE ) ;
		DialogConfiguration -> DefaultConnection ( TRUE  ) = Configuration -> DefaultConnection ( TRUE  ) ;

		if  ( TSessionsDialog ( this, DialogConfiguration ). Execute ( )  ==  IDOK )
		   {
			DialogConfiguration -> Write ( TRUE ) ;
			UpdateListBox ( ) ;
		    }
	      }
     }






//
//  Définit la sélection courante comme étant la configuration par défaut
//
void	TConfigurationDialog :: CmDefault  ( void )
   {
	char			Buffer [ MAX_PARAMETER_LENGTH ] ;


	if  ( GetCurrentSelection ( Buffer ) )
		Configuration -> WriteString ( SETTINGS_SECTION, "Configuration",
			Buffer ) ;
     }






/**************************************************************************/
/**************************************************************************/
/********                                                          ********/
/********  Implémentation de la classe TSessionsDialog  	   ********/
/********                                                          ********/
/**************************************************************************/
/**************************************************************************/

DEFINE_RESPONSE_TABLE1  ( TSessionsDialog, TDialog )
	EV_COMMAND   ( IDOK		, CmOk      ),
	EV_COMMAND   ( IDC_MODIFY  	, CmModify  ),
	EV_COMMAND   ( IDC_CREATE	, CmCreate  ),
	EV_COMMAND   ( IDC_DELETE	, CmDelete  ),
END_RESPONSE_TABLE ;
				


//
// Remplit la boîte de liste avec les sessions définies pour la configuration
// actuelle
//
BOOL		TSessionsDialog :: EvInitDialog  ( HWND  focus )
   {
	char			Buffer [ MAX_PARAMETER_LENGTH ] ;
	register int		Status		=  TDialog :: EvInitDialog ( focus ) ;


	wsprintf ( Buffer, "Configuration '%s' - Modification",
		DialogConfiguration -> Terminal ( ). Configuration ) ;
	SetWindowText ( Buffer ) ;
	UpdateListBox ( ) ;
	return  ( Status ) ;
    }




//
// Met à jour la boîte de liste
//
void	TSessionsDialog :: UpdateListBox  ( void )
   {
	register int		i, count ;
	register char * 	p ;


	ListBox -> ClearList ( ) ;

	for  ( i = count = 0 ; i < MAX_SESSIONS ; i ++ )
	   {
		p = DialogConfiguration -> Terminal ( ). SessionSections [i] ;

		if  ( * p )
		   {
			ListBox -> AddString ( p ) ;
			count ++ ;
		    }
	    }

	ListBox -> SetSelIndex ( 0 ) ;
	ElementsFound ( count ) ;
     }





// 
// Retourne l'index d'une session par rapport à son nom
//
int	TSessionsDialog :: SessionIndex  ( char *  Name )
   {
	register char *		p ;


	for  ( int  i = 0 ; i < MAX_SESSIONS ; i ++ )
	   {
		p = DialogConfiguration -> Terminal ( ). SessionSections [i] ;

		if  ( ! stricmp ( p, Name ) )
			return ( i ) ;
	     }

	:: MessageBox ( HWindow, "Erreur interne", MB_OK | MB_ICONEXCLAMATION,
		"TSessionsDialog :: SessionIndex ( %s ) retourne -1 !!!",
			Name ) ;
	return ( -1 ) ;
     }



//
// Modification des paramètres d'une session
//
void	TSessionsDialog :: CmModify  ( void )
   {
	char		SessionName [ MAX_PARAMETER_LENGTH ] ;
	register int	i ;	


	if  ( GetCurrentSelection ( SessionName ) )
	   {
		i = SessionIndex ( SessionName ) ;

		if  ( i >= 0 )
		   {
			if  ( TSessionParametersDialog ( this, 
				( DialogConfiguration -> Connection ( i ). NetworkLink ) ?
					IDD_SESSION_PARAMETERS_NETWORK : IDD_SESSION_PARAMETERS_SERIAL,
				DialogConfiguration, 
				& DialogConfiguration -> Connection ( i ),
				& DialogConfiguration -> Session ( i ), i, SessionName ). Execute ( )  ==  IDOK )
			   {
				DialogConfiguration -> WriteSessionParameters ( SessionName,
					DialogConfiguration -> Session ( i ), FALSE ) ;
				DialogConfiguration -> WriteConnectionParameters ( SessionName,
					DialogConfiguration -> Connection ( i ), 
					DialogConfiguration -> Connection ( i ). NetworkLink,
					FALSE ) ;
			    }
		     }
	    }
     }



//
// Suppression d'une session.
// La section définissant les paramètres de la session n'est pas supprimée,
// seulement sa référence dans la section définissant la configuration.
// Cependant, si la touche SHIFT est enfoncée, la section de la session sera
// entièrement supprimée.
//

void	TSessionsDialog :: CmDelete  ( void )
   {
	char			SessionName [ MAX_PARAMETER_LENGTH ] ;
	char			Entry       [ MAX_PARAMETER_LENGTH ] ;
	char			Buffer      [ MAX_PARAMETER_LENGTH ] ;
	register char *         p ;
	register char *		Name		=  DialogConfiguration -> Terminal ( ). Configuration ;
	register char *		ConfSection     =  DialogConfiguration -> ConfigurationSectionName ( Name ) ;



	if  ( GetCurrentSelection ( SessionName ) )
	   {
		if  ( :: MessageBox ( HWindow, "Suppression", MB_YESNOCANCEL,
			"Voulez-vous supprimer la session '%s' de la configuration '%s' ?",
				SessionName, Name )  !=  IDYES )
			return ;

	// Libérer le slot
		DialogConfiguration -> RemoveSession ( SessionName ) ;


	// Effacer la section concernant cette session si la touche SHIFT est 
	// appuyée
		if  ( GetKeyState ( VK_SHIFT )  &  0x8000 )
			DialogConfiguration -> WriteString ( 
				DialogConfiguration -> SessionSectionName ( SessionName ),
					NULL, NULL ) ;


	// Rechercher l'entrée dans la définition de la configuration
		DialogConfiguration -> GetSectionItems ( ConfSection, Buffer, sizeof ( Buffer ) ) ;

		for  ( p = Buffer ; * p ; p += strlen ( p ) + 1 )
		   {
			DialogConfiguration -> ReadString ( ConfSection, p, "",
				Entry, sizeof ( Entry ), FALSE ) ;

			if  ( ! stricmp ( Entry, SessionName ) )
			   {
				DialogConfiguration -> WriteString ( ConfSection,
					p, NULL ) ;
				break ;
			     }
		     }


	// Mettre à jour la liste déroiulante
		UpdateListBox ( ) ;
	     }
      }






//
//  Crée une nouvelle session pour cette configuration
//
void	TSessionsDialog :: CmCreate  ( void )
   {
	char			SessionName [ MAX_PARAMETER_LENGTH ] ;
	int			slot  =  DialogConfiguration -> FindFreeSlot ( ) ;
	TConnectionParameters   cp ;
	TSessionParameters 	sp ;



	if  ( slot  ==  -1 )
	   {
		:: MessageBox ( HWindow, "Dépassement de capacité", MB_OK | MB_ICONEXCLAMATION,
			"Vous avez défini trop de sessions pour la configuration '%s'."
			"Pour créer une nouvelle session, vous devrez auparavant en supprimer.",
				DialogConfiguration -> Terminal ( ). Configuration ) ;
		return ;
	    }

	cp   = DialogConfiguration -> DefaultConnection ( ) ;
	sp   = DialogConfiguration -> DefaultSession ( ) ;


	if  ( TSessionParametersDialog ( this, IDD_SESSION_PARAMETERS,
			DialogConfiguration, & cp, & sp, -1, SessionName, ListBox ). 
				Execute ( )  ==  IDOK )
	   {
		slot = DialogConfiguration -> AddSession ( SessionName, cp, & sp ) ;

		DialogConfiguration -> WriteSessionParameters ( SessionName,
			DialogConfiguration -> Session ( slot ), FALSE ) ;
		DialogConfiguration -> WriteConnectionParameters ( SessionName,
			DialogConfiguration -> Connection ( slot ), 
			DialogConfiguration -> Connection ( slot ). NetworkLink,
			FALSE ) ;
		UpdateListBox ( ) ;
	      }
     }





/**************************************************************************/
/*                                                                        */
/*   TSessionsDialog :: CmOk  -                                           */
/*      Réarrange la table des sessions conformément à l'ordre inscrit    */
/*      dans la listebox.                                                 */
/*                                                                        */
/**************************************************************************/

void	TSessionsDialog :: CmOk  ( void )
   {
	char				ListElement [ MAX_PARAMETER_LENGTH ] ;
	char				Buffer 	    [ MAX_PARAMETER_LENGTH ] ;
	register int			i, j,
					count	=  ListBox -> GetCount ( ) ;
	register char *			section ;


// Suppression de la section Configuration entière
	section = DialogConfiguration -> ConfigurationSectionName ( 
			DialogConfiguration -> Terminal ( ). Configuration ) ;
	DialogConfiguration -> WriteString ( section, NULL, NULL ) ;


// Synchronisation des sessions avec les éléments de la boîte de dialogue
	for  ( i = 0 ; i < count ; i ++ )
	   {
		ListBox -> GetString ( ListElement, i ) ;
		j = SessionIndex ( ListElement ) ;

		if  ( i  !=  j )
			DialogConfiguration -> SwapSessions ( i, j ) ;

// (ré-)écriture de l'entrée "Session" dans la configuration
		wsprintf ( Buffer, "Session%d", i + 1 ) ;
		DialogConfiguration -> WriteString ( section, Buffer,
			ListElement ) ;
	    }

	TDialog :: CmOk ( ) ;
     }

		




/**************************************************************************/
/**************************************************************************/
/********                                                          ********/
/********  Implémentation de la classe TSessionsParametersDialog   ********/
/********                                                          ********/
/**************************************************************************/
/**************************************************************************/

# define	MAX_PORTS		4


DEFINE_RESPONSE_TABLE1  ( TSessionParametersDialog, TDialog )
	EV_COMMAND  	   ( IDOK		, CmOk     	),
	EV_COMMAND_AND_ID  ( IDC_NETWORK_TYPE	, CmLinkType    ),
	EV_COMMAND_AND_ID  ( IDC_SERIAL_TYPE	, CmLinkType    ),
	EV_COMMAND	   ( IDC_PREFERENCES    , CmPreferences ),
	EV_EN_CHANGE       ( IDC_SESSION_NAME	, CmChanged     ),
END_RESPONSE_TABLE ;                                    




/**************************************************************************/
/*                                                                        */
/*   TSessionParametersDialog :: EvInitDialog                             */
/*      Met disable le contrôle "Nom de session".                         */
/*      Met "disable" le type de liaison si la boîte de dialogue est      */
/*      IDD_SESSION_PARAMETERS.                                           */
/*                                                                        */
/**************************************************************************/

BOOL	TSessionParametersDialog :: EvInitDialog  ( HWND  focus )
   {
	char		Buffer [ BUFSIZ ] ;
	register int	Status = TDialog :: EvInitDialog ( focus ) ;
	register int	i ;


// Titre de la fenêtre
	if  ( DialogId  !=  IDD_SESSION_PARAMETERS )
	   {
		if  ( SessionName  ==  NULL )
			wsprintf ( Buffer, "Paramètres par défaut pour les liaisons %s",
				( DialogId  ==  IDD_SESSION_PARAMETERS_SERIAL ) ?
					"série" : "réseau" ) ;
		else
			wsprintf ( Buffer, "Paramètres de la session '%s' (en liaison %s)",
				SessionName,
				( DialogId  ==  IDD_SESSION_PARAMETERS_SERIAL ) ?
					"série" : "réseau" ) ;
	     }
	else
		strcpy ( Buffer, "Création d'une session" ) ;

	SetWindowText ( Buffer ) ;

				    

// Boîte initiale : on met le bouton OK en disable, tant que le type de laison
// n'a pas été choisi
	if  ( DialogId  ==  IDD_SESSION_PARAMETERS ) 
	   {
		EnableControl ( IDOK, FALSE ) ;
		return ( Status ) ;
	    }


	SetDlgItemText ( IDC_SESSION_NAME, ( SessionName  ==  NULL ) ?
				"(paramètres par défaut)" : SessionName ) ;

	EnableControl ( IDC_SESSION_NAME, FALSE ) ;


// Ne pas mettre de bouton "Préférences" s'il s'agit d'une configuration par
// défaut
	if  ( SessionName  ==  NULL )
		:: ShowWindow ( GetDlgItem ( IDC_PREFERENCES ), SW_HIDE ) ;


// Dans tous les autres cas, il s'agit de la boîte de dialogue appropriée au type
// de liaison; il faut donc empêcher l'utilisateur de le changer
	EnableControl ( IDC_NETWORK_TYPE, FALSE ) ;
	EnableControl ( IDC_SERIAL_TYPE , FALSE ) ;

// Initialisation du type de liaison
	CheckRadioButton ( IDC_NETWORK_TYPE, IDC_SERIAL_TYPE, 	
		( LocalConnectionParameters -> NetworkLink ) ?
			IDC_NETWORK_TYPE : IDC_SERIAL_TYPE ) ;

// Echo local
	CheckDlgButton ( IDC_LOCAL_ECHO, ( LocalSessionParameters -> LocalEcho ) ?
						1 : 0 ) ;

// Initialisations pour le réseau
	if  ( LocalConnectionParameters -> NetworkLink )
	   {
		SetDlgItemText ( IDC_SERVER_NAME, 
			LocalConnectionParameters -> Network. ServerName ) ;

		wsprintf ( Buffer, "%u", LocalConnectionParameters -> Network. Delay ) ;
		SetDlgItemText ( IDC_DELAY, Buffer ) ;
	    }

// Initialisations pour la liaison série
	else
	   {

	// Ajout des noms de ports série
		for  ( i = 0 ; i < MAX_PORTS ; i ++ )
		   {
			wsprintf ( Buffer, "COM%d", i + 1 ) ;
			Port-> AddString ( Buffer ) ;
		    }

		Port -> SetSelString ( LocalConnectionParameters -> Serial. PortName, 0 ) ;


	// Ajout des différentes vitesses supportées
		Speed -> AddString ( "300" ) ;
		Speed -> AddString ( "600" ) ;		
		Speed -> AddString ( "1200" ) ;
		Speed -> AddString ( "2400" ) ;
		Speed -> AddString ( "4800" ) ;
		Speed -> AddString ( "9600" ) ;
		Speed -> AddString ( "19200" ) ;
		Speed -> AddString ( "38400" ) ;

		Speed -> SetSelString ( SpeedToString ( 
				LocalConnectionParameters -> Serial. Speed ), 0 ) ;

	// Parité
		switch ( LocalConnectionParameters -> Serial. Parity )
		  {
			case	NOPARITY   : i = IDC_PARITY_NONE ; break ;
			case	ODDPARITY  : i = IDC_PARITY_ODD  ; break ;
			case	EVENPARITY : i = IDC_PARITY_EVEN ; break ;
		   }
		
		CheckRadioButton ( IDC_PARITY_NONE, IDC_PARITY_ODD, i ) ;

	// Nombre de bits de données
		CheckRadioButton ( IDC_DATA_BITS_8, IDC_DATA_BITS_5,
			IDC_DATA_BITS_8 + 8 - 
				LocalConnectionParameters -> Serial. DataBits ) ;

	// Nombre de bits de stop
		switch ( LocalConnectionParameters -> Serial. StopBits )
		   {
			case	ONE5STOPBITS : i = IDC_STOP_BIT_15 ; break ;
			case	TWOSTOPBITS  : i = IDC_STOP_BIT_2  ; break ;
			case	ONESTOPBIT   : i = IDC_STOP_BIT_1  ; break ;
		    }

		CheckRadioButton ( IDC_STOP_BIT_1, IDC_STOP_BIT_2, i ) ;
	    }

	return ( Status ) ;
    }



/**************************************************************************/
/*                                                                        */
/*  TSessionParametersDialog :: CmPreferences                             */
/*      Ouvre la boîte de dialogue "Préférences".                         */
/*                                                                        */
/**************************************************************************/

void	TSessionParametersDialog :: CmPreferences ( void )
   {
	TPreferencesDialog ( this, LocalSessionParameters ). Execute ( ) ;
    }



/**************************************************************************/
/*                                                                        */
/*   TSessionParametersDialog :: CmOk                                     */
/*      Récupère les infos de la boîte de dialogue.                       */
/*                                                                        */
/**************************************************************************/

void	TSessionParametersDialog :: CmOk  ( void )
   {
	char			Buffer [ MAX_PARAMETER_LENGTH ] ;
	BOOL			IsNetwork ;
	register int		Status ;


// Va falloir créer une boîte de dialogue appropriée au type de liaison
	if  ( DialogId  ==  IDD_SESSION_PARAMETERS )
	   {
		IsNetwork = IsDlgButtonChecked ( IDC_NETWORK_TYPE ) ;
		GetDlgItemText ( IDC_SESSION_NAME, SessionName, MAX_PARAMETER_LENGTH ) ;

	// Chercher si la session n'existe pas déjà
		if  ( ParentListBox  &&
				ParentListBox -> FindExactString ( SessionName, 0 )  >=  0 )
		   {
			:: MessageBox ( HWindow, "Erreur", MB_OK | MB_ICONEXCLAMATION,
				"La session %s existe déjà !", SessionName ) ;
			:: SetFocus ( GetDlgItem ( IDC_SESSION_NAME ) ) ;
			SendDlgItemMessage ( IDC_SESSION_NAME, EM_SETSEL, 0, 0xFFFF0000L ) ;
			return ;
		    }

		* LocalConnectionParameters = :: Configuration -> DefaultConnection ( IsNetwork ) ;
		* LocalSessionParameters    = :: Configuration -> DefaultSession ( ) ;	

		Status    = TSessionParametersDialog ( this,
				( IsNetwork ) ?  
					IDD_SESSION_PARAMETERS_NETWORK : 
					IDD_SESSION_PARAMETERS_SERIAL,
				DialogConfiguration,
				LocalConnectionParameters,
				LocalSessionParameters,
				SessionIndex,
				SessionName ). Execute ( ) ;

		if  ( Status  ==  IDOK )
			TDialog :: CmOk ( ) ;
		else
			TDialog :: CmCancel ( ) ;

		return ;
	    }		


// Echo local
	LocalSessionParameters -> LocalEcho = IsDlgButtonChecked ( IDC_LOCAL_ECHO ) ;


// Paramètres réseau
	if  ( LocalConnectionParameters -> NetworkLink )
	   {

	// Récupération du nom du serveur
		GetDlgItemText ( IDC_SERVER_NAME, 
			LocalConnectionParameters -> Network. ServerName,
				MAX_PARAMETER_LENGTH ) ;

	// Délai
		LocalConnectionParameters -> Network. Delay = GetDlgItemInt ( IDC_DELAY,
								NULL, FALSE ) ;
	      }
// Paramètres série
	else
	   {

	// Nom du port (salut !)
	// (pouf-pouf)
	// Le fond de l'air est frais...
		GetDlgItemText ( IDC_PORT, LocalConnectionParameters -> Serial. PortName,
			MAX_PARAMETER_LENGTH ) ;

	// Vitesse
		GetDlgItemText ( IDC_SPEED, Buffer, sizeof ( Buffer ) ) ;
		LocalConnectionParameters -> Serial. Speed = StringToSpeed ( Buffer ) ;
		
	// Parité
		if  ( IsDlgButtonChecked ( IDC_PARITY_NONE ) )
			LocalConnectionParameters -> Serial. Parity = NOPARITY ;
		else if  ( IsDlgButtonChecked ( IDC_PARITY_EVEN ) )
			LocalConnectionParameters -> Serial. Parity = EVENPARITY ;
		if  ( IsDlgButtonChecked ( IDC_PARITY_ODD ) )
			LocalConnectionParameters -> Serial. Parity = ODDPARITY ;

	// Bits de données
		if  ( IsDlgButtonChecked ( IDC_DATA_BITS_8 ) )
			LocalConnectionParameters -> Serial. DataBits = 8 ;
		else if  ( IsDlgButtonChecked ( IDC_DATA_BITS_7 ) )
			LocalConnectionParameters -> Serial. DataBits = 7 ;
		else if  ( IsDlgButtonChecked ( IDC_DATA_BITS_6 ) )
			LocalConnectionParameters -> Serial. DataBits = 6 ;
		else if  ( IsDlgButtonChecked ( IDC_DATA_BITS_5 ) )
			LocalConnectionParameters -> Serial. DataBits = 5 ;

	// Bits de stop
		if  ( IsDlgButtonChecked ( IDC_STOP_BIT_1 ) )
			LocalConnectionParameters -> Serial. StopBits = ONESTOPBIT ;
		else if  ( IsDlgButtonChecked ( IDC_STOP_BIT_15 ) )
			LocalConnectionParameters -> Serial. StopBits = ONE5STOPBITS ;
		else if  ( IsDlgButtonChecked ( IDC_STOP_BIT_2 ) )
			LocalConnectionParameters -> Serial. DataBits = TWOSTOPBITS ;
	   }

	TDialog :: CmOk ( ) ;
    }		       






/**************************************************************************/
/*                                                                        */
/*  TSessionParameters :: CmLinkType                                      */
/*      Met "Enable" le bouton OK si l'utilisateur a saisi quelque chose  */
/*      pour le nom de session.                                           */
/*                                                                        */
/**************************************************************************/

void	TSessionParametersDialog :: CmLinkType  ( WPARAM   /* id */ )
   {
	if  ( DialogId  !=  IDD_SESSION_PARAMETERS )
		return ;


	char	Buffer [ MAX_PARAMETER_LENGTH ] ;

	GetDlgItemText ( IDC_SESSION_NAME, Buffer, sizeof ( Buffer ) ) ;

	if  ( * Buffer )
		EnableControl ( IDOK, TRUE ) ;
	else
		EnableControl ( IDOK, FALSE ) ;
    }



/**************************************************************************/
/*                                                                        */
/*   TSessionParameters :: CmChanged                                      */
/*      Met "Enable" le bouton OK si le type de liaison a été choisi.     */
/*                                                                        */
/**************************************************************************/

void	TSessionParametersDialog :: CmChanged ( void )
   {
	if  ( DialogId  !=  IDD_SESSION_PARAMETERS )
		return ;


	char		Buffer [ MAX_PARAMETER_LENGTH ] ;


	GetDlgItemText ( IDC_SESSION_NAME, Buffer, sizeof ( Buffer ) ) ;

	if  ( * Buffer  &&  ( IsDlgButtonChecked ( IDC_NETWORK_TYPE )  ||
			      IsDlgButtonChecked ( IDC_SERIAL_TYPE ) ) )
		EnableControl ( IDOK, TRUE ) ;
	else
		EnableControl ( IDOK, FALSE ) ;
    }





/**************************************************************************/
/**************************************************************************/
/********                                                          ********/
/********  Implémentation de la classe TPreferencesDialog.         ********/
/********                                                          ********/
/**************************************************************************/
/**************************************************************************/

DEFINE_RESPONSE_TABLE1 ( TPreferencesDialog, TDialog )
	EV_COMMAND	( IDOK 		      , CmOk ),
	EV_COMMAND 	( IDC_BACKGROUND_COLOR, CmBackground ),
	EV_COMMAND	( IDC_FOREGROUND_COLOR, CmForeground ),
	EV_COMMAND	( IDC_CHOOSE_FONT     , CmChooseFont ),
	EV_COMMAND	( IDC_CHOOSE_EMULATION, CmChooseEmulation ),
	EV_WM_CTLCOLOR,
END_RESPONSE_TABLE ;



/**************************************************************************/
/*                                                                        */
/*  TPreferencesDialog :: EvCtlColor                                      */
/*      Redessine le contrôle statique IDC_EXAMPLE avec les couleurs de   */
/*      fond et de texte qui ont été choisies.                            */
/*                                                                        */
/**************************************************************************/

HBRUSH	TPreferencesDialog :: EvCtlColor  ( HDC  	hDC,
					    HWND	ChildHwnd,
					    UINT	Type )
   {
	HBRUSH		hBrush  =  TDialog :: EvCtlColor ( hDC, ChildHwnd, Type ) ;


	if  ( ChildHwnd  ==  GetDlgItem ( IDC_EXAMPLE ) )
	   {
		SetTextColor ( hDC, Parameters. ForegroundColor ) ;
		SetBkColor   ( hDC, Parameters. BackgroundColor ) ;
	    }

	return  ( hBrush ) ;
     }





/**************************************************************************/
/*                                                                        */
/*   TPreferencesDialog :: ResetFont                                      */
/*      Redéfinit la fonte du contrôle IDC_EXAMPLE.                       */
/*                                                                        */
/**************************************************************************/

void	TPreferencesDialog :: ResetFont ( void ) 
   {
	char			Buffer [ MAX_PARAMETER_LENGTH ] ;
	register HFONT		SavedFont	=  ExampleFont ;


	ExampleFont = CreateFont  ( Parameters. FontHeight,
				   Parameters. FontWidth,
				   0, 0,
				   FW_NORMAL,
				   0,
				   0,
				   0,
				   DEFAULT_CHARSET,
				   OUT_DEFAULT_PRECIS,
				   CLIP_DEFAULT_PRECIS,
				   DEFAULT_QUALITY,
				   DEFAULT_PITCH | FF_DONTCARE,
				   Parameters. FontName ) ;

	SendDlgItemMessage ( IDC_EXAMPLE, WM_SETFONT, ( WPARAM ) ExampleFont,
				MAKELONG ( TRUE, 0 ) ) ;
	
	if  ( SavedFont  !=  NULL )
		DeleteObject ( SavedFont ) ;

	SetDlgItemText ( IDC_FONT_NAME, Parameters. FontName ) ;

// La hauteur de la police est exprimée en unités logiques; il faut la convertir
// en unités physiques pour obtenir la taille telle qu'elle est affichée par
// ChooseFontDialog
	TWindowDC	WindowDC ( HWindow ) ;
	int		Pixels		=  WindowDC. GetDeviceCaps ( LOGPIXELSY ) ;

	wsprintf ( Buffer, "%d", 
		( ( - Parameters. FontHeight * 72 ) + 
			( Pixels / 2 ) ) / Pixels ) ;
	SetDlgItemText ( IDC_FONT_HEIGHT, Buffer ) ;
     }




/**************************************************************************/
/*                                                                        */
/*   TDialog :: CmOk                                                      */
/*      Récupère les paramètres et les recopie dans le pointeur sur le    */
/*      TSessionParameters.                                               */
/*                                                                        */
/**************************************************************************/

void	TPreferencesDialog :: CmOk  ( void )
   {

// Titre de la fenêtre
	GetDlgItemText ( IDC_WINDOW_TITLE, Parameters. WindowTitle,
			sizeof ( Parameters. WindowTitle ) ) ;

// Nom et taille de la police
	GetDlgItemText ( IDC_FONT_NAME, Parameters. FontName, 
			sizeof ( Parameters. FontName ) ) ;


// Emulation
	GetDlgItemText ( IDC_EMULATION, Parameters. Emulation, 
			sizeof ( Parameters. Emulation ) ) ;

// Jeu de caractères
	if  ( IsDlgButtonChecked ( IDC_OEM_CHARSET ) )
		Parameters. HostCharSet = OemCharSet ;
	else if  ( IsDlgButtonChecked ( IDC_ANSI_CHARSET ) )
		Parameters. HostCharSet = AnsiCharSet ;

// C'est tout
	* ReturnParameters = Parameters ;
	TDialog :: CmOk ( ) ;
    }




/**************************************************************************/
/*                                                                        */
/*   TPreferencesDialog :: CmChooseFont                                   */
/*      Permet de choisir la police de caractères.                        */
/*                                                                        */
/**************************************************************************/

void	TPreferencesDialog :: CmChooseFont ( void )
   {
	static BOOL				FirstTime 	=  TRUE ;
	static TChooseFontDialog :: TData       FontData ;


// Initialisation des données
	if  ( FirstTime )
	   {
		FirstTime 		=  FALSE ;

		FontData. DC		=  0 ;
		FontData. Flags         =  CF_FORCEFONTEXIST |
					   CF_ANSIONLY |
					   CF_SCREENFONTS |
					   CF_INITTOLOGFONTSTRUCT ;
		FontData. Color		=  Parameters. ForegroundColor ;
		FontData. FontType	=  SCREEN_FONTTYPE ;
	     }

	memset ( & FontData. LogFont, 0, sizeof ( FontData. LogFont ) ) ;

	FontData. LogFont. lfHeight		=  Parameters. FontHeight ;
	FontData. LogFont. lfWidth 		=  Parameters. FontWidth ;
	FontData. LogFont. lfCharSet		=  DEFAULT_CHARSET ;
	FontData. LogFont. lfOutPrecision       =  OUT_DEFAULT_PRECIS ;
	FontData. LogFont. lfClipPrecision	=  CLIP_DEFAULT_PRECIS ;
	FontData. LogFont. lfQuality		=  DEFAULT_QUALITY ;
	FontData. LogFont. lfPitchAndFamily	=  DEFAULT_PITCH | FF_DONTCARE ;
	strcpy ( FontData. LogFont. lfFaceName, Parameters. FontName ) ;


// On balance la boiboite
	if  ( TChooseFontDialog ( this, FontData, 0, 
			"Choix de la police de caractères" ). Execute ( )  ==  IDOK )
	   {	
		strcpy ( Parameters. FontName, 
				FontData. LogFont. lfFaceName ) ;
		Parameters. FontHeight = FontData. LogFont. lfHeight ;
		Parameters. FontWidth  = FontData. LogFont. lfWidth ;
		ResetFont ( ) ;
	    }
     }




/**************************************************************************/
/*                                                                        */
/*   TPreferencesDialog :: CmColor                                        */
/*      Permet de choisir la couleur de texte et de fond.                 */
/*                                                                        */
/*                                                                        */
/**************************************************************************/

void	TPreferencesDialog :: CmColor  ( WPARAM  Id )
   {
	TChooseColorDialog :: TData	ColorData ;
	register COLORREF *		Color ;
	TColor				CustomColors [ 16 ] ;
	register char *			Title ;
	HWND				ExampleHwnd ;


// Initialisations en fonction du type de couleur (fond ou texte)
	if  ( Id  ==  IDC_FOREGROUND_COLOR )
	   {
		Color  =  & Parameters. ForegroundColor ;
		Title  =  "Choix de la couleur de texte" ;
	    }
	else
	   {
		Color  =  & Parameters. BackgroundColor ;
		Title  =  "Choix de la couleur de fond" ;
	    }

// Initialisations nécessaires à TChooseColorDialog
	memset ( CustomColors, 0, sizeof ( CustomColors ) ) ;

	ColorData. Flags	=  CC_RGBINIT | CC_PREVENTFULLOPEN ;
	ColorData. Color	=  * Color ;
	ColorData. CustColors 	=  CustomColors ;

// Exécution 
	if  ( TChooseColorDialog ( this, ColorData, 0, Title ). Execute ( )  ==  IDOK )
	   {
		* Color = ColorData. Color ;
		ExampleHwnd = GetDlgItem ( IDC_EXAMPLE ) ;
		:: InvalidateRect ( ExampleHwnd, NULL, TRUE ) ;
		:: UpdateWindow ( ExampleHwnd ) ;
	    }
     }




/**************************************************************************/
/*                                                                        */
/*  TPreferencesDialog :: CmChooseEmulation                               */
/*      Permet de choisir l'émulation de terminal.                        */
/*                                                                        */
/**************************************************************************/

void	TPreferencesDialog :: CmChooseEmulation  ( void )
   {
	char			Buffer  	[ MAX_PARAMETER_LENGTH ] ;
	char			Filename        [ MAX_PARAMETER_LENGTH ] ;
	char			Directory 	[ MAX_PARAMETER_LENGTH ] ;
	register char *		p,
		      * 	q ;


// Récupération du répertoire de l'émulation
	strcpy ( Buffer, Parameters. Emulation ) ;
	p = Buffer + strlen ( Buffer ) - 1 ;

	while  ( p >= Buffer  &&  strchr ( "\\/:", * p )  !=  NULL )
		p -- ;

	if  ( p < Buffer )		// Pas de chemin dans le nom
	   {
		p = "" ;
		q = Buffer ;
	    }
	else
	   {
		* p = 0 ;
		q   = p + 1 ;
		p   = Buffer ;
	    }

	strcpy ( Filename, q ) ;
	strcpy ( Directory, p ) ;


// Initialisation des données pour la boiboite
	TOpenSaveDialog :: TData	FilenameData ( OFN_FILEMUSTEXIST |
						       OFN_PATHMUSTEXIST |
						       OFN_HIDEREADONLY,
						       "Emulations (*.trm)|*.TRM|Tous les fichiers (*.*)|*.*|",
						       0, p, "trm" ) ;

	FilenameData. FileName		=  Filename ;
	FilenameData. InitialDir	=  Directory ;


// Et zou
	if  ( TFileOpenDialog ( this, FilenameData, 0, "Choix d'une émulation de terminal" ).
			Execute ( )  ==  IDOK )
		strcpy ( Parameters. Emulation, FilenameData. FileName ) ;
     }			




/**************************************************************************/
/*                                                                        */
/*   TPreferencesDialog :: EvInitDialog                                   */
/*      Initialise les contrôles.                                         */
/*                                                                        */
/**************************************************************************/

BOOL	TPreferencesDialog :: EvInitDialog  ( HWND  focus )
   {
	char			Buffer [ MAX_PARAMETER_LENGTH ] ;
	register int		Status = TDialog :: EvInitDialog ( focus ) ;



// Titre de la fenêtre
	SetDlgItemText ( IDC_WINDOW_TITLE, Parameters. WindowTitle ) ;

// Nom et taille de la police
	SetDlgItemText ( IDC_FONT_NAME, Parameters. FontName ) ;

	wsprintf ( Buffer, "%d", - Parameters. FontHeight ) ;
	SetDlgItemText ( IDC_FONT_HEIGHT, Buffer ) ;

// Emulation
	SetDlgItemText ( IDC_EMULATION, Parameters. Emulation ) ;

// Jeu de caractères
	CheckRadioButton ( IDC_OEM_CHARSET, IDC_ANSI_CHARSET,
		( Parameters. HostCharSet  ==  OemCharSet ) ?
			IDC_OEM_CHARSET : IDC_ANSI_CHARSET ) ;

// C'est tout
	ResetFont ( ) ;
	return ( Status ) ;
    }





/**************************************************************************/
/**************************************************************************/
/********                                                          ********/
/********  Implémentation de la classe TExtendedListBox 	   ********/
/********                                                          ********/
/**************************************************************************/
/**************************************************************************/


DEFINE_RESPONSE_TABLE1  ( TExtendedListBox, TListBox )
	EV_WM_LBUTTONDOWN,
	EV_WM_LBUTTONUP,
	EV_WM_MOUSEMOVE,
END_RESPONSE_TABLE ;




/**************************************************************************/
/*                                                                        */
/*  TExtendedListBox :: EvLButtonDown                                     */
/*      . Sauvegarde le curseur de la fenêtre                             */
/*      . Installe le curseur CURSOR_LISTBOX                              */
/*      . Capture les messages de la souris                               */
/*      . Récupère l'élément sélectionné.                                 */
/*                                                                        */
/**************************************************************************/

void	TExtendedListBox :: EvLButtonDown  ( uint  flags, TPoint &  pos )
   {
// Ne rien faire si l'interface "étendue" n'est pas active.
	TListBox :: EvLButtonDown ( flags, pos ) ;

	if  ( ! ExtendedInterfaceEnabled )
		return ; 

	ButtonPressed = TRUE ;


// Récupérer l'index de l'élément sélectionné
	SelectedElement = GetSelIndex ( ) ;


// Installer le nouveau curseur à la place
	SavedCursor   = :: GetCursor ( ) ;
	ListBoxCursor = :: LoadCursor ( * GetModule ( ), ( LPCSTR ) CURSOR_LISTBOX ) ;
	:: SetCursor ( ListBoxCursor ) ;

// Maintenant, pour la boîte, tout se passera comme si le bouton avait été
// relaché, ce qui évitera de déplacer la sélection
	TListBox :: EvLButtonUp ( flags, pos ) ;

// Capturer les messages souris 
	SetCapture ( ) ;
     }








/**************************************************************************/
/*                                                                        */
/*  TExtendedListBox :: EvLButtonUp                                       */
/*      Déplace l'élément initialement sélectionné                        */
/*                                                                        */
/*                                                                        */
/**************************************************************************/

void	TExtendedListBox :: EvLButtonUp  ( uint  flags, TPoint &  pos )
   {
	int			Destination ;
	char			Buffer [ MAX_PARAMETER_LENGTH ] ;


// Ne rien faire si l'interface "étendue" n'est pas active.
	TListBox :: EvLButtonUp ( flags, pos ) ;


	if  ( ! ExtendedInterfaceEnabled )
		return ; 

	ButtonPressed = FALSE ;


// Récupérer l'index de l'élément sélectionné
	Destination = GetSelIndex ( ) ;


// Restaurer l'ancien curseur 
	:: SetCursor ( SavedCursor ) ;
	:: DestroyCursor ( ListBoxCursor ) ;


// Ne plus capturer les messages souris
	ReleaseCapture ( ) ;

// Déplacer l'élément
	if  ( Destination  ==  SelectedElement )
		return ;

	GetString ( Buffer, SelectedElement ) ;	// Récupérer la chaîne
	DeleteString  ( SelectedElement ) ;	// la supprimer
	InsertString ( Buffer, Destination ) ;  // et l'insérer à la position choisie
     }




/**************************************************************************/
/**************************************************************************/
/********                                                          ********/
/********  Implémentation de la classe TChooseSessionDialog        ********/
/********                                                          ********/
/**************************************************************************/
/**************************************************************************/


DEFINE_RESPONSE_TABLE1  ( TChooseSessionDialog, TDialog )
	EV_COMMAND       ( IDOK		 	, CmOk ),
	EV_LBN_DBLCLK    ( IDC_SESSION_LIST	, CmOk ),
	EV_COMMAND       ( IDC_CHOOSE_SESSION	, CmOk ),
	EV_COMMAND       ( IDCANCEL        	, CmCancel ),
	EV_COMMAND       ( IDC_CLOSE_SESSION	, CmClose ),
	EV_COMMAND       ( IDC_OPEN_SESSION	, CmOpen ),
	EV_LBN_SELCHANGE ( IDC_SESSION_LIST     , CmSelChange ),
END_RESPONSE_TABLE ;






/**************************************************************************/
/*                                                                        */
/*   TChooseSessionDialog :: SetupWindow                                  */
/*      Redimensionne la fenêtre de manière à ce que la liste affiche     */
/*      tous les sessions actives, en adaptant sa taille selon le nombre  */
/*      de sessions.                                                      */
/*                                                                        */
/**************************************************************************/

BOOL  TChooseSessionDialog :: EvInitDialog  ( HWND   hwnd )
   {
	char *			SessionNames [ MAX_SESSIONS ] ;	
	register char *		p ;
	register int		i, j ;
	register BOOL		Status ;
	


	Status = TDialog :: EvInitDialog ( hwnd ) ;


// Récupération des noms de sessions
	memset ( SessionNames, 0, sizeof ( SessionNames ) ) ;

	for  ( i = 0 ; i < MAX_SESSIONS ; i ++ )
	   {
		if  ( Configuration -> SessionState ( i )  !=  TConfiguration :: Free )
		   {
			p = Configuration -> Session ( i ). WindowTitle ;

			if  ( ! * p )
				p = Configuration -> Terminal ( ). SessionSections [i] ;

			SessionNames [i] = p ;
		     }
	     }



// Ajout des noms de session à la boiboite
	ListBox -> ClearList ( ) ;

	for  ( i = 0 ; i < MAX_SESSIONS ; i ++ )
	   {
		if  ( SessionNames [i]  !=  NULL )
		   {
			j = ListBox -> AddString   ( SessionNames [i] ) ;
			ListBox -> SetItemData ( j, ( uint32 ) i ) ;  // Stocker le no de session
		    }
	    }

	ListBox -> SetSelIndex ( 0 ) ;
	CmSelChange ( ) ;

	return ( Status ) ;
    }






/**************************************************************************/
/*                                                                        */
/*  TChooseSessionDialog :: CmOk                                          */
/*      Récupère le no de session actuellement sélectionnée.              */
/*                                                                        */
/**************************************************************************/

void  TChooseSessionDialog :: CmOk  ( void ) 
   {
	register int		Index  =  ListBox -> GetSelIndex ( ) ;
	register int		SessionIndex ;


	if  ( Index  >=  0 )
	   {
		SessionIndex = ( int ) ListBox -> GetItemData ( Index ) ;
		* SessionId = SessionIndex ;
	    }

	TDialog :: CmOk ( ) ;
     }




/**************************************************************************/
/*                                                                        */
/*   TChooseSessionDialog :: Open, Close                                  */
/*      Ouvre/ferme la session actuellement sélectionnée dans la liste.   */
/*                                                                        */
/**************************************************************************/

void	TChooseSessionDialog :: CmOpen  ( void )
   {
	register int		Index  =  ListBox -> GetSelIndex ( ) ;
	register int		SessionIndex, OldSession ;


	if  ( Index  >=  0 )
	   {
		OldSession   = Configuration -> GetCurrentSession ( ) ;
		Configuration -> SetPreventAutoOpen ( TRUE ) ;

		SessionIndex = ( int ) ListBox -> GetItemData ( Index ) ;
		Configuration -> SetCurrentSession ( SessionIndex ) ;

		TerminalWindow -> IDMFileConnect ( ) ;

		Configuration -> SetCurrentSession ( OldSession ) ;
		Configuration -> SetPreventAutoOpen ( FALSE ) ;
		CmSelChange ( ) ;
	    }
     }
	




void	TChooseSessionDialog :: CmClose  ( void )
   {
	register int		Index  =  ListBox -> GetSelIndex ( ) ;
	register int		SessionIndex, OldSession ;


	if  ( Index  >=  0 )
	   {
		OldSession   = Configuration -> GetCurrentSession ( ) ;
		Configuration -> SetPreventAutoOpen ( TRUE ) ;

		SessionIndex = ( int ) ListBox -> GetItemData ( Index ) ;
		Configuration -> SetCurrentSession ( SessionIndex ) ;

		TerminalWindow -> IDMFileDisconnect ( ) ;

		Configuration -> SetCurrentSession ( OldSession ) ;
		Configuration -> SetPreventAutoOpen ( FALSE ) ;
		CmSelChange ( ) ;
	    }
	
     }




/**************************************************************************/
/*                                                                        */
/*  TChooseSessionDialog :: CmSelChange                                   */
/*      Change l'état des boutons Ouvrir et Fermer selon l'état de la     */
/*      session actuellement sélectionnée.                                */
/*                                                                        */
/**************************************************************************/

void  TChooseSessionDialog :: CmSelChange  ( void ) 
   {
	register int		Index  =  ListBox -> GetSelIndex ( ) ;
	register int		SessionIndex ;
	register BOOL		OpenState, CloseState ;


	if  ( Index  >=  0 )
	   {
		SessionIndex = ( int ) ListBox -> GetItemData ( Index ) ;

		if  ( Configuration -> SessionState ( SessionIndex )  ==
				TConfiguration :: Created )
		   {
			OpenState  = TRUE ;
			CloseState = FALSE ;
		    }
		else
		   {
			OpenState  = FALSE ;
			CloseState = TRUE ;
		    }

		:: EnableWindow ( GetDlgItem ( IDC_OPEN_SESSION  ), OpenState ) ;
		:: EnableWindow ( GetDlgItem ( IDC_CLOSE_SESSION ), CloseState ) ;
	      }
       }





/**************************************************************************/
/**************************************************************************/
/********                                                          ********/
/********  Implémentation de la classe TGeneralOptionsDialog       ********/
/********                                                          ********/
/**************************************************************************/
/**************************************************************************/


DEFINE_RESPONSE_TABLE1  ( TGeneralOptionsDialog, TDialog )
	EV_COMMAND      ( IDOK	 	, CmOk ),
	EV_COMMAND 	( IDCANCEL	, CmCancel ),
END_RESPONSE_TABLE ;




/**************************************************************************/
/*                                                                        */
/*  TGeneralOptionsDialog :: EvInitDialog                                 */
/*      Initialise les contrôles.                                         */
/*                                                                        */
/**************************************************************************/

BOOL	TGeneralOptionsDialog :: EvInitDialog  ( HWND  hwnd )
   {
	char			Buffer [ 32 ] ;
	register int		Status 	= TDialog :: EvInitDialog ( hwnd ) ;


	CheckDlgButton ( IDC_ALARM_BEEP     , Configuration -> Terminal ( ). AlarmBeep ) ;
	CheckDlgButton ( IDC_AUTO_OPEN      , Configuration -> Terminal ( ). AutoOpen ) ;
	CheckDlgButton ( IDC_CONFIRM_ON_EXIT, Configuration -> Terminal ( ). ConfirmOnExit ) ;
	CheckDlgButton ( IDC_FF_ON_HARDCOPY , Configuration -> Terminal ( ). FormFeedOnHardcopy ) ;
	CheckDlgButton ( IDC_FRAME_HARDCOPY , Configuration -> Terminal ( ). FrameHardcopy ) ;

# if	defined ( CHEOPS )
	CheckDlgButton ( IDC_OLD_FUNCTION_KEYS, Configuration -> Terminal ( ). OldFunctionKeys ) ;
# endif

	wsprintf ( Buffer, "%d", Configuration -> Terminal ( ). MinRedoSize ) ;
	SetDlgItemText ( IDC_MIN_REDO_SIZE, Buffer ) ;

	wsprintf ( Buffer, "%d", Configuration -> Terminal ( ). MenuAutoOpenDelay ) ;
	SetDlgItemText ( IDC_AUTOOPEN_DELAY, Buffer ) ;

	return ( Status ) ;
     }




/**************************************************************************/
/*                                                                        */
/*  TGeneralOptionsDialog :: CmOk                                         */
/*      Récupère les valeurs des contrôles.                               */
/*                                                                        */
/**************************************************************************/

void	TGeneralOptionsDialog :: CmOk  ( void )
   {
	char			Buffer [ 32 ] ;


	Configuration -> Terminal ( ). AlarmBeep	=
				IsDlgButtonChecked ( IDC_ALARM_BEEP ) ;
	Configuration -> Terminal ( ). AutoOpen		=
				IsDlgButtonChecked ( IDC_AUTO_OPEN ) ;
	Configuration -> Terminal ( ). ConfirmOnExit	=
				IsDlgButtonChecked ( IDC_CONFIRM_ON_EXIT ) ;
	Configuration -> Terminal ( ). FormFeedOnHardcopy =
				IsDlgButtonChecked ( IDC_FF_ON_HARDCOPY ) ;
	Configuration -> Terminal ( ). FrameHardcopy =
				IsDlgButtonChecked ( IDC_FRAME_HARDCOPY ) ;

# if  defined ( CHEOPS )
	Configuration -> Terminal ( ). OldFunctionKeys  =  
				IsDlgButtonChecked ( IDC_OLD_FUNCTION_KEYS ) ;
# endif

	
	
	GetDlgItemText ( IDC_MIN_REDO_SIZE, Buffer, sizeof ( Buffer ) ) ;
	Configuration -> Terminal ( ). MinRedoSize = atoi ( Buffer ) ;

	GetDlgItemText ( IDC_AUTOOPEN_DELAY, Buffer, sizeof ( Buffer ) ) ;
	Configuration -> Terminal ( ). MenuAutoOpenDelay = atoi ( Buffer ) ;

	Configuration -> SetDirty ( TRUE ) ;
	TDialog :: CmOk ( ) ;
     }





/**************************************************************************/
/**************************************************************************/
/********                                                          ********/
/********  Implémentation de la classe TObjectListDialog           ********/
/********                                                          ********/
/**************************************************************************/
/**************************************************************************/


DEFINE_RESPONSE_TABLE1  ( TObjectListDialog, TDialog )
	EV_COMMAND      ( IDOK	 	, CmOk ),
	EV_COMMAND 	( IDCANCEL	, CmCancel ),
END_RESPONSE_TABLE ;



/**************************************************************************/
/*                                                                        */
/*  TObjectListDialog :: CmOk -                                           */
/*      Ferme la boîte.                                                   */
/*                                                                        */
/**************************************************************************/

void	TObjectListDialog :: CmOk  ( void ) 
   {
	TDialog :: CmOk ( ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TObjectListDialog :: EvInitDialog -                                   */
/*      Initialisae la listbox avec la liste des objets encore alloués.   */
/*                                                                        */
/**************************************************************************/

BOOL	TObjectListDialog :: EvInitDialog  ( HWND  focus )
   {
	char			Buffer  [ 256 ] ;
	char			AppName [ 256 ],
				ObjName [ 256 ] ;
	register BOOL           Status 			=  TDialog :: EvInitDialog  ( focus ) ;
	register int		ObjectCount             =  0,
				ApplicationCount	=  0 ;
	register int		i, j ;
	TUnixApplication **	Apps 			=  ApplicationTable -> ApplicationTable ;
	TWindow *		Window ;



	for  ( i = 0 ; i < ApplicationTable -> ApplicationTableSize ; i ++ )
	   {
		if  ( ! Apps [i] )
			continue ;

		strcpy ( AppName, Apps [i] -> ApplicationName ) ;
		ApplicationCount ++ ;

		for  ( j = 0 ; j < MAX_APPLICATION_OBJECTS ; j ++ )
		   {
			Window = Apps [i] -> ApplicationObjects [j] ;

			if  ( ! Window )
				continue ;

			strcpy ( ObjName, typeid ( * Window ). name ( ) ) ;
			ObjectCount ++ ;

			wsprintf ( Buffer, "[%s]\t%s", AppName, ObjName ) ;
			ObjectList -> AddString ( Buffer ) ;
		     }
	      }


	wsprintf ( Buffer, "%d application(s), %d objet(s).", 
		ApplicationCount, ObjectCount ) ;
	SendDlgItemMessage ( IDC_OBJECT_COUNT, WM_SETTEXT, 0,
		( LPARAM ) Buffer ) ;

	return ( Status ) ;
    }
				



/**************************************************************************/
/**************************************************************************/
/********                                                          ********/
/********  Implémentation de la classe TPrintingDialog             ********/
/********                                                          ********/
/**************************************************************************/
/**************************************************************************/

# define	MAX_SLIDER_TICKS		100


DEFINE_RESPONSE_TABLE1  ( TPrintingDialog, TDialog )
	EV_COMMAND 	( IDCANCEL	, CmCancel ),
END_RESPONSE_TABLE ;



/**************************************************************************/
/*                                                                        */
/*  TPrintDialog :: EvInitDialog -                                        */
/*      Initialise la boite de dialogue et crée le slider.                */
/*                                                                        */
/**************************************************************************/

BOOL	TPrintingDialog :: EvInitDialog  ( HWND   hwnd )
   {
	BOOL			Status		=  TDialog :: EvInitDialog ( hwnd ) ;
	HWND			EditHwnd        =  GetDlgItem ( IDC_PRINTING_SLIDER ) ;
	TRect			EditRect ;
	TPoint			Origin ;


// Récupérer la dimension du controle EDIT pour caser le slider par dessus
	:: GetWindowRect ( EditHwnd, & EditRect ) ;
	Origin. x = EditRect. left ;
	Origin. y = EditRect. top ;
	ScreenToClient ( Origin ) ;

// Créer le slider
	Slider = new  TGauge ( this, "", IDC_PRINTING_SLIDER + 100,
			Origin. x, Origin. y, 
			EditRect. Width ( ), EditRect. Height ( ),
			true, 2 ) ;
	Slider -> SetRange ( 0, MAX_SLIDER_TICKS ) ;
	Slider -> SetLed ( 4, 80 ) ;
	Slider -> Create ( ) ;
	Slider -> Show ( SW_SHOW ) ;

	return  ( Status ) ;
    }

	

/**************************************************************************/
/*                                                                        */
/*  TPrintDialog :: AnimateSlider -                                       */
/*      Positionne la jauge à la valeur suivante.                         */
/*                                                                        */
/**************************************************************************/

void	TPrintingDialog :: AnimateSlider ( )
  {
	if  ( CurrentValue + 1  ==  AnimateEvery )
	   {
		CurrentValue = 0 ;
		Slider -> SetValue ( ( Slider -> GetValue ( ) + 1 ) % MAX_SLIDER_TICKS ) ;
		Slider -> UpdateWindow ( ) ;	// Pourquoi est-ce nécessaire ????
	     }
	else
		CurrentValue ++ ;
   }
