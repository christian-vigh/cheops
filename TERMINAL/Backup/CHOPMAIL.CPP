/**************************************************************************/
/*                                                                        */
/*   ChopMail.cpp  -  Christian Vigh, 95/11/15.                           */
/*                                                                        */
/*   Implémentation des classes mail.                                     */
/*                                                                        */
/**************************************************************************/


# include	"AllTerm.h"
# pragma	hdrstop

# if 	! STANDALONE 
# include	"UnixApp.h"
# include	"Event.h"
# endif

# include	"MNCommon.h"
# include	"ChopCfg.h"
# include	"ChopJet.h"
# include	"ChopTree.h"
# include	"ChopMenu.h"
# include	"ChopUp.h"
# include	"ChopMail.h"




# define	MAIL_SEQUENCE			"\001\001\001\001\n"





/**************************************************************************/
/**************************************************************************/
/********                                                          ********/
/********    Classe ControlBar pour relayer les notifications.     ********/
/********                                                          ********/
/**************************************************************************/
/**************************************************************************/

class	TRelayColorControlBar 	:  public  TColorControlBar
   {
	public :
		TRelayColorControlBar  	( TWindow *		parent,
					  TTileDirection        dir   =  Horizontal,
					  TFont *		font  =  new  TCheopsWindowFont ( 8 ) )
			: TColorControlBar ( parent, dir, font )
		   { }


	// Relaye les messages WM_COMMAND vers le parent
		virtual LRESULT  DefWindowProc  ( uint  msg, WPARAM  wp, LPARAM  lp )
		   {
			if  ( msg  ==  WM_COMMAND )
				Parent -> SendMessage ( WM_COMMAND, wp, lp ) ;
			
			return  ( TColorControlBar :: DefWindowProc ( msg, wp, lp ) ) ;
		     }
     } ;





/**************************************************************************/
/**************************************************************************/
/********                                                          ********/
/********          Implémentation de la classe TCheopsMailDialog.  ********/
/********                                                          ********/
/**************************************************************************/
/**************************************************************************/

DEFINE_RESPONSE_TABLE1 ( TCheopsMailDialog, TDecoratedFrame )
	EV_COMMAND ( IDC_MAIL_CLOSE	, CmOk 		),
END_RESPONSE_TABLE ;



/**************************************************************************/
/*                                                                        */
/*  Constructeur.                                                         */
/*                                                                        */
/**************************************************************************/

TCheopsMailDialog :: TCheopsMailDialog  ( TCheopsMenuWindow *	chopmenu,
					  TMDIClient *		client,
					  char *		title )
	: TDecoratedFrame ( client, title, 0 )
   {
	Attr. Style  =  WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_BORDER |
			WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX |
			WS_SYSMENU ;

	Attr. AccelTable = TResId ( "MAIL_ACCEL" ) ;

	Attr. X = Attr. Y = CW_USEDEFAULT ;
	Attr. H = 275 ;
	Attr. W = 530 ;

	EditClient = new  TEdit ( 0, IDC_MAIL_TEXT, 0, 0, 100, 100, 0, true ) ;
	EditClient -> Attr. Style = WS_CHILD | ES_AUTOHSCROLL | ES_AUTOVSCROLL |
				    WS_TABSTOP | ES_WANTRETURN |
				    ES_LEFT | ES_MULTILINE | WS_VISIBLE |
				    WS_VSCROLL | WS_HSCROLL ;
	CheopsMenuWindow = chopmenu ;

	EnableKBHandler ( ) ;
    }
		
					  



/**************************************************************************/
/*                                                                        */
/*  Destructeur.                                                          */
/*                                                                        */
/**************************************************************************/

TCheopsMailDialog :: ~TCheopsMailDialog  ( )
   {
    }




/**************************************************************************/
/*                                                                        */
/*  TCheopsMailDialog :: Create -                                         */
/*      Crée la fenêtre.                                                  */
/*                                                                        */
/**************************************************************************/

BOOL	TCheopsMailDialog :: Create (  void )
   {
	register BOOL		Status ;

// Création de la barre d'icônes
	ControlBar = new  TRelayColorControlBar ( this, TGadgetWindow :: Horizontal,
				new  TGadgetWindowFont ( 8 ) ) ;
	ControlBar -> SetHintMode ( TControlBar :: NoHints ) ;

// Insérer les autres éléments
	InsertControlBarItems ( * ControlBar ) ;

// Insérer un gadget séparateur "wide as possible"
	ControlBar -> Insert ( * new  TShrinkSeparatorGadget ( 6 ) ) ;
		

// "Fermer" 
	TTextButtonGadget *	Close =  new  TTextButtonGadget ( IDC_MAIL_CLOSE ) ;

	Close -> SetShrinkWrap ( TRUE, TRUE ) ;
	Close -> SetText ( "&Fermer" ) ;
	ControlBar -> Insert ( * Close ) ;


// Créer la barre d'état 
	StatusBar = new  TStatusBar ( this ) ;

// Insérer tout ça
	Insert ( * ControlBar, TDecoratedFrame :: Top ) ;
	Insert ( * StatusBar , TDecoratedFrame :: Bottom ) ;

// Créer
	Status = TDecoratedFrame :: Create ( ) ;
	SetClientWindow ( EditClient ) ;

	return  ( Status ) ;
    }

	



/**************************************************************************/
/*                                                                        */
/*  TCheopsMailDialog :: PreProcessMsg -                                  */
/*      Appelle le PreProcessMsg de TDecoratedFrame, puis celui de        */
/*      TMDIChild.                                                        */
/*                                                                        */
/**************************************************************************/

# define	MAX_WINDOWS		256 

struct  TTabbedWindows 
   {
		HWND		Windows [ MAX_WINDOWS ] ;
		int		Count ;
    } ;




static void	FindTabWindows ( TWindow *  win, void *  p )
   {
	register  TTabbedWindows *	tab = ( TTabbedWindows * ) p ;



	if  ( win -> GetWindowLong ( GWL_STYLE )  &  WS_TABSTOP )
		tab -> Windows [ tab -> Count ++ ] = win -> HWindow ;

// Faire de même sur les enfants des enfants, et les enfants des enfants des
// enfants, et les enfants des enfants des enfants des enfants, et comme ça
// jusqu'à la 99e génération
	win -> ForEach ( FindTabWindows, p ) ;
     }





BOOL	TCheopsMailDialog :: PreProcessMsg  ( MSG &  msg )
   {
// Gérer le passage d'un contrôle à l'autre avec tab et shift-tab
// (ça ressemble furieusement à ce que fait IsDialogMessage)
	if  ( msg. message  ==  WM_KEYDOWN  &&
			msg. wParam   ==  VK_TAB )
	   {
		TTabbedWindows 		Found ;
		register int		i, index ;
		register BOOL		backwards 	=  ( GetKeyState ( VK_SHIFT )  &  0x8000 ) ?
								TRUE : FALSE ;
		LRESULT			DlgCode	;
		HWND			hwnd ;


		Found. Count = 0 ;
		ForEach ( FindTabWindows, ( void * ) & Found ) ;

		for  ( i = 0, index = -1 ; i < Found. Count ; i ++ )
		   {
			if  ( Found. Windows [i]  ==  msg. hwnd )
			   {
				index = i ;
				break ;
			    }
		     }


		if  ( index  ==  -1 )	// Pas de fenêtre avec WS_TABSTOP
			return  ( FALSE ) ;

		if  ( backwards )
		   {
			index -- ;

			if  ( index < 0 )
				index = Found. Count - 1 ;
		     }
		else
			index = ( index + 1 ) % Found. Count ;

		hwnd = Found. Windows [index] ;
		:: SetFocus ( hwnd ) ;

		DlgCode = :: SendMessage ( hwnd, WM_GETDLGCODE, 0, 0L ) ;

	// Mettre la sélection si le contrôle ne veut pas de return
	// (cad si ce n'est pas le contenu du mail)
		if  ( ( DlgCode & DLGC_HASSETSEL )  && 
			   ! ( :: GetWindowLong ( hwnd, GWL_STYLE ) & ES_WANTRETURN ) )
			:: SendMessage ( hwnd, EM_SETSEL, 0,
				MAKELPARAM ( 0, 32767 ) ) ;

		return  ( TRUE ) ;
	     }
	else 
	     return ( TDecoratedFrame :: PreProcessMsg ( msg ) ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  TCheopsMailDialog :: CmOk -                                           */
/*      Ferme la fenêtre.                                                 */
/*                                                                        */
/**************************************************************************/

void	TCheopsMailDialog :: CmOk ( void )
   {
	CloseWindow ( 0 ) ;

	if  ( ! HWindow  &&  IsFlagSet ( wfDeleteOnClose ) )
		GetApplication ( ) -> Condemn ( this ) ;
    }







/**************************************************************************/
/*                                                                        */
/*  TCheopsMailDialog :: Activate -                                       */
/*      Donne le focus.                                                   */
/*                                                                        */
/**************************************************************************/

void	TCheopsMailDialog :: Activate  ( void ) 
   {
	Parent -> HandleMessage ( WM_MDIACTIVATE, ( WPARAM ) HWindow ) ;

	if  ( IsIconic ( ) )
		Show ( SW_RESTORE ) ;
    }





/**************************************************************************/
/**************************************************************************/
/********                                                          ********/
/********    Implémentation de la classe TCheopsReadMailDialog.    ********/
/********                                                          ********/
/**************************************************************************/
/**************************************************************************/

DEFINE_RESPONSE_TABLE1 ( TCheopsReadMailDialog, TCheopsMailDialog )
	EV_CHILD_NOTIFY_ALL_CODES ( IDC_RMAIL_MESSAGE_LIST, CmComboBox ),
	EV_COMMAND		  ( IDC_RMAIL_COPY    	  , CmCopy     ),
	EV_COMMAND		  ( IDC_RMAIL_DELETE	  , CmDelete   ),
	EV_WM_SYSCOMMAND,
END_RESPONSE_TABLE ;





/**************************************************************************/
/*                                                                        */
/*  Constructeur.                                                         */
/*                                                                        */
/**************************************************************************/

TCheopsReadMailDialog :: TCheopsReadMailDialog  ( TCheopsMenuWindow *	chopmenu,
						  TMDIClient *		client,
						  char *     		title,
						  char *     		mail_file )
		: TCheopsMailDialog ( chopmenu, client, title )
   {
	CheopsMenuWindow -> ReadMailWindow = this ;
	MailFile 	 		   = strdup ( mail_file ) ;
	Attr. AccelTable 		   = TResId ( "RMAIL_ACCEL" ) ;
	MailComboBox 	 		   = 0 ;
	Head = Tail = 0 ;

	SetIcon ( GetModule ( ), TResId ( "RMAIL_ICON" ) ) ;
     }





/**************************************************************************/
/*                                                                        */
/*  TCheopsReadMailDialog :: PreProcessMsg -                              */
/*      Referme la boîte de liste de la combobox si cette combobox        */
/*      perd le focus ou si l'utilisateur a appuyé sur escape.            */
/*                                                                        */
/**************************************************************************/

BOOL	TCheopsReadMailDialog :: PreProcessMsg  ( MSG &  msg )
   {
	if  ( MailComboBox	&&
		MailComboBox -> HWindow  ==  msg. hwnd  &&
	      ( msg. message  ==  WM_KILLFOCUS  ||
			( msg. message  ==  WM_KEYDOWN  &&  
				msg. wParam  ==  VK_ESCAPE ) ) )
		MailComboBox -> ShowList ( FALSE ) ;	
	return  ( TCheopsMailDialog :: PreProcessMsg ( msg ) ) ;
     }



/**************************************************************************/
/*                                                                        */
/*  TCheopsReadMailDialog :: Activate -                                   */
/*      Relit les courriers.                                              */
/*                                                                        */
/**************************************************************************/

void	TCheopsReadMailDialog :: Activate  ( void )
   {
	FreeMailEntries ( ) ;
	ReadMailEntries ( ) ;
	TCheopsMailDialog :: Activate ( ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TCheopsReadMailDialog :: InsertControlBarItems -                      */
/*      Insère les boutons dans la barre d'icônes.                        */
/*                                                                        */
/**************************************************************************/
	
void	TCheopsReadMailDialog :: InsertControlBarItems  ( TControlBar &  cb )
   {

// Récupérer la taille d'un caractère
	TScreenDC		DC ;
	TEXTMETRIC		tm ;

	DC. SelectObject ( cb. GetFont ( ) ) ;
	DC. GetTextMetrics ( tm ) ;


// Création de la combo box
	MailComboBox = new  TComboBox ( & cb, IDC_RMAIL_MESSAGE_LIST,
				0, 0,
				tm. tmMaxCharWidth * 20,
				200,
				CBS_HASSTRINGS  | CBS_DROPDOWNLIST |
				CBS_AUTOHSCROLL | WS_HSCROLL |
				CBS_HASSTRINGS, 0 ) ;


// Associer cette combo box à un contrôle gadget
	TControlGadget *	g = new  TControlGadget ( * MailComboBox ) ;


	g -> SetShrinkWrap ( TRUE, TRUE ) ;
	cb. Insert ( * g ) ;
	cb. Insert ( * new  TSeparatorGadget ( 18 ) ) ;


// Bouton "Copier"	
	TTextButtonGadget *	Copy = new  TTextButtonGadget ( IDC_RMAIL_COPY ) ;

	Copy -> SetShrinkWrap ( TRUE, TRUE ) ;
	Copy -> SetText ( "&Copier" ) ;
	cb. Insert ( * Copy ) ;
	cb. Insert ( * new  TSeparatorGadget ( 9 ) ) ;



// Bouton "Supprimer"	
	TTextButtonGadget *	Suppr = new  TTextButtonGadget ( IDC_RMAIL_DELETE ) ;

	Suppr -> SetShrinkWrap ( TRUE, TRUE ) ;
	Suppr -> SetText ( "&Supprimer" ) ;
	cb. Insert ( * Suppr ) ;
    }






/**************************************************************************/
/*                                                                        */
/*  TCheopsReadMailDialog :: CmCopy -                                     */
/*      Copie le texte du message dans le presse-papiers.                 */
/*                                                                        */
/**************************************************************************/

void	TCheopsReadMailDialog :: CmCopy  ( void )
   {
	uint	start, stop ;


	EditClient -> GetSelection ( start, stop ) ;


	if  ( ! start  &&  ! stop )
		EditClient -> SetSelection ( 0, 32767 ) ;

	EditClient -> Copy ( ) ;
	EditClient -> SetSelection ( 0, 0 ) ;
     }






/**************************************************************************/
/*                                                                        */
/*  TCheopsReadMailDialog :: CmDelete -                                   */
/*      Supprime le courrier en cours.                                    */
/*                                                                        */
/**************************************************************************/

void	TCheopsReadMailDialog :: CmDelete  ( void )
   {
	register int		i  =  MailComboBox -> GetSelIndex ( ) ;


	if  ( i  <  0 )
		return ;

// Construire le bloc de données à envoyer
	char		buffer [ 2 * sizeof ( INTEGER16 ) ] ;

	I16toCHAR ( 1, buffer ) ;		// Nombre de mails
	I16toCHAR ( i + 1, buffer + sizeof ( INTEGER16 ) ) ;


// Balancer la sauce
	TUnixApplication *	Application 	=  CheopsMenuWindow -> Application ;


	EventQueue -> Enqueue ( Application -> ApplicationId,
				CheopsMenuWindow -> ObjectId,
				Application -> ProcessId,
				WM_MENU_EVENT, MNF_DELETE_MAIL,
				( LPARAM ) buffer, sizeof ( buffer ) ) ;

// Fermer la fenêtre si c'était le dernier mail 
	if  ( MailComboBox -> GetCount ( )  <  2 )
		CmOk ( ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TCheopsReadMailDialog :: CmComboBox -                                 */
/*      Répond aux messages de notification de la combo box.              */
/*                                                                        */
/**************************************************************************/

void	TCheopsReadMailDialog :: CmComboBox  ( uint  notifycode ) 
   {
	register int	i ;


	if  ( notifycode  ==  CBN_SELCHANGE   &&
		( i = MailComboBox -> GetSelIndex ( ) )  >=  0 )
	   {
	// On fait un fopen ( "rb" ) car il faut des CR/LF pour marquer les
	// fins de ligne des contrôles EDIT
		register FILE *		fp = fopen ( MailFile, "rb" ) ;
		register TMailEntry *	e ;
		register char *		p ;
		register char *		Subject 	=  0 ;
		char *			Text ;
		int			Size ;


		e = ( TMailEntry * ) MailComboBox -> GetItemData ( i ) ;
		EditClient -> Clear ( ) ;

	// Courrier OK : lire dans un buffer en mémoire (pas plus de 32K!!!)
		if  ( e -> EndOffset )
		   {
			Size = ( int ) ( e -> EndOffset - e -> BeginOffset ) ;
			Text = new  char [ Size + 1 ] ;
			Text [ Size ] = 0 ;

			fseek ( fp, e -> BeginOffset, 0 ) ;
			fread ( Text, Size, 1, fp ) ;
			fclose ( fp ) ;


		// Rechercher la chaîne "Subject"
			p = Text ;

			while  ( * p )
			   {
				if  ( toupper ( * p )  ==  'S'  &&
					! strncmpi ( p, "Subject:", 8 ) )
				   {
					Subject = p ;

					strncpy ( p, "Objet:  ", 8 ) ;
					break ;
				     }

				p ++ ;
			      }

		// Ne montrer le courrier qu'à partir du sujet que si on n'est
		// pas super-utilisateur et qu'il y a bien un sujet
			if  ( ! CheopsMenuWindow -> GetUserInfo ( ). IsSuperUser  &&  
						Subject )
				EditClient -> SetText ( Subject ) ;
			else
				EditClient -> SetText ( Text ) ;

			delete []  Text ;
		    }
	     }
     }
			




/**************************************************************************/
/*                                                                        */
/*  TCheopsReadMailDialog :: FreeMailEntries -                            */
/*      Libère les structures allouées pour le courrier.                  */
/*                                                                        */
/**************************************************************************/

void	TCheopsReadMailDialog :: FreeMailEntries  ( void )
   {
	while  ( Head )
	   {
		TMailEntry *	next = Head -> Next ;


		if  ( Head -> From )
			free ( Head -> From ) ;
		if  ( Head -> Subject )
			free ( Head -> Subject ) ;

		delete  Head ;
		Head = next ;
	      }

	Head = Tail = 0 ;


	if  ( MailComboBox )
	   {
		MailComboBox -> ClearList ( ) ;
		MailComboBox -> Clear ( ) ;
	    }

	if  ( EditClient )
		EditClient -> Clear ( ) ;
      }




/**************************************************************************/
/*                                                                        */
/*  TCheopsReadMailDialog :: ReadMailEntries -                            */
/*      Lit les infos concernant les courriers reçus par l'utilisateur.   */
/*                                                                        */
/**************************************************************************/

void	TCheopsReadMailDialog :: ReadMailEntries  ( void )
  {
	register FILE *		fp  =  fopen ( MailFile, "r" ) ;
	char			Buffer [ MAX_PARAMETER_LENGTH * 2 ] ;
	register char *		p,
		      *		q ;
	register int		i ;



	if  ( fp  ==  NULL )
	   {
		:: MessageBox ( HWindow, "Erreur", MB_OK | MB_ICONEXCLAMATION,
			"Erreur de réception - Impossible de consulter votre courrier !" ) ;
		return ;
	    }


// Lire les lignes
// Un courrier démarre et finit par quatre fois le caractère 1, suivi d'une
// fin de ligne
	while  ( fgets ( Buffer, sizeof ( Buffer ), fp )  !=  NULL )
	   {
	// Ignorer tout ce qui est entre deux séquences
		if  ( strcmp ( Buffer, MAIL_SEQUENCE ) )
			continue ;

	// Allouer une nouvelle entrée
		if  ( ! Head )
			Head = Tail = new  TMailEntry ;
		else
		   {
			register TMailEntry *  t = new  TMailEntry ;
					    
			Tail -> Next = t ;
			Tail = t ;
		     }

		Tail -> From = Tail -> Subject = 0 ;
		Tail -> EndOffset = 0 ;
		Tail -> Next = 0 ;


	// Sauvegarder la position de la 1e ligne du courrier
		Tail -> BeginOffset = ftell ( fp ) ;

	// lire les lignes jusqu'à la fin du courrier de manière à récupérer le
	// nom de l'émetteur et le sujet
		while  ( fgets ( Buffer, sizeof ( Buffer ), fp )  !=  NULL )
		   {
		// Arrêter si on tombe sur la fin du courrier
			if  ( ! strcmp ( Buffer, MAIL_SEQUENCE ) )
			   {
				Tail -> EndOffset = ftell ( fp ) -
						strlen ( MAIL_SEQUENCE ) 
						- 1 ;   // Pour le carriage return
				break ;
			     }

			p = Buffer ;

		// Sauter les blancs (voire même les espaces)
			while  ( * p  &&  isspace ( * p ) )
				p ++ ;

		// Chaîne commençant par "From " : il y a le user après
			if  ( ! strncmpi ( p, "From ", 5 ) )
			   {
				p = q = p + 5 ;

				while  ( * q  &&  ! isspace ( * q ) )
					q ++ ;
				* q = 0 ;
				Tail -> From = strdup ( p ) ;
			     }
		// Chaîne commençant par "Subject:" : garder le sujet
			else if  ( ! strncmpi ( p, "Subject: ", 9 ) )
			   {
				p = p + 9 ;
				p [ strlen ( p ) - 1 ] = 0 ; 	// newline
				Tail -> Subject = strdup ( p ) ;
			    }
		     }
	     }

	fclose ( fp ) ;

	if  ( ! MailComboBox )	
		return ;


// Remettre à jour la combobox
	register TMailEntry *		e = Head ;

	MailComboBox -> Clear ( ) ;


	while  ( e )
	   {
		if  ( e -> Subject )
			strcpy ( Buffer, e -> Subject ) ;
		else
			strcpy ( Buffer, "Courrier" ) ;

		strcat ( Buffer, "    " ) ;

		if  ( e -> From )
		   {
			strcat ( Buffer, "(de l'utilisateur " ) ;
			strcat ( Buffer, e -> From ) ;
			strcat ( Buffer, ")" ) ;
		    }
		else
			strcat ( Buffer, "(origine inconnue)" ) ;

		i = MailComboBox -> InsertString ( Buffer, -1 ) ;
		MailComboBox -> SetItemData ( i, ( LPARAM ) e ) ;

		e = e -> Next ;
	      }

	MailComboBox -> SetSelIndex ( 0 ) ;
	CmComboBox ( CBN_SELCHANGE ) ;		// Provoquer l'affichage du courrier
    }






/**************************************************************************/
/**************************************************************************/
/********                                                          ********/
/********    Implémentation de la classe TCheopsSendMailDialog.    ********/
/********                                                          ********/
/**************************************************************************/
/**************************************************************************/

DEFINE_RESPONSE_TABLE1 ( TCheopsSendMailDialog, TCheopsMailDialog )
	EV_COMMAND 	  ( IDC_SMAIL_SEND     , CmSend    ),
	EV_COMMAND	  ( IDC_SMAIL_USER_LIST, CmChanged ),
	EV_EN_CHANGE	  ( IDC_SMAIL_USER_LIST, CmChanged ),
	EV_EN_CHANGE	  ( IDC_MAIL_TEXT      , CmChanged ),
	EV_WM_SYSCOMMAND,
END_RESPONSE_TABLE ;




/**************************************************************************/
/*                                                                        */
/*  Constructeur.                                                         */
/*                                                                        */
/**************************************************************************/

TCheopsSendMailDialog :: TCheopsSendMailDialog  ( TCheopsMenuWindow *	chopmenu,
						  TMDIClient *		client,
						  char *		title )
	: TCheopsMailDialog ( chopmenu, client, title )
   {
	CheopsMenuWindow -> SendMailWindow = this ;
	Attr. AccelTable 		   = TResId ( "SMAIL_ACCEL" ) ;
	UserListEdit   	 		   = 0 ;

	SetIcon ( GetModule ( ), TResId ( "SMAIL_ICON" ) ) ;
     }



/**************************************************************************/
/*                                                                        */
/*   TCheopsSendMailDialog :: InsertControlBarItems -                     */
/*      Insère le contrôle Edit pour la liste des utilisateur,            */
/*      et le bouton "Envoyer".                                           */
/*                                                                        */
/**************************************************************************/

void	TCheopsSendMailDialog :: InsertControlBarItems  ( TControlBar &  cb )
   {
// Récupérer la taille d'un caractère
	DWORD		Units 		= GetDialogBaseUnits ( ) ;
	int		CharacterWidth	= LOWORD ( Units ),
			Height		= HIWORD ( Units ) ;
	

	Height = ( 12 * Height ) / 8 ;	// Un contrôle EDIT fait 12 unités de haut


// Insérer "votre choix" 
	cb. Insert ( * new  TAutoTextGadget ( 0, TTextGadget :: None,
				TAutoTextGadget :: AutoAdjust, 10, "Liste des utilisateurs :" ) ) ;

// Puis le contrôle edit
	UserListEdit = new  TEdit ( & cb, IDC_SMAIL_USER_LIST, 0, 0, 0, 
					( CharacterWidth * 120 ) / 4,
					Height,
					0 ) ;
	UserListEdit -> Attr. Style  =  WS_CHILD | WS_VISIBLE | WS_BORDER |
					WS_TABSTOP | ES_AUTOHSCROLL  | ES_LEFT ;

	TControlGadget *	EditGadget = new  TControlGadget ( * UserListEdit ) ;

	EditGadget -> SetShrinkWrap ( TRUE, TRUE ) ;
	cb. Insert ( * EditGadget ) ;


// Insérer le bouton "Envoyer"
	SendButton = new  TTextButtonGadget ( IDC_SMAIL_SEND ) ;

	SendButton -> SetShrinkWrap ( TRUE, TRUE ) ;
	SendButton -> SetText ( "&Envoyer" ) ;
	SendButton -> SetEnabled ( FALSE ) ;
	cb. Insert ( * new  TSeparatorGadget ( 18 ) ) ;
	cb. Insert ( * SendButton ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  TCheopsSendMailDialog :: CmChanged  -                                 */
/*      Valide ou invalide l'état du bouton Envoyer.                      */
/*                                                                        */
/**************************************************************************/

void    TCheopsSendMailDialog :: CmChanged  ( void )
   {
	BOOL		old_state  =  SendButton -> GetEnabled ( ),
			new_state ;
	char		b1 [ 256 ],
			b2 [ 256 ] ;


	UserListEdit -> GetText ( b1, sizeof ( b1 ) ) ;
	EditClient   -> GetLine ( b2, sizeof ( b2 ), 0 ) ;

	if  ( ! * b1  ||  ! * b2 )
		new_state = FALSE ;
	else
		new_state = TRUE ;

	if  ( new_state  !=  old_state )
	   {
		SendButton -> SetEnabled ( new_state ) ;
		SendButton -> Invalidate ( ) ;
	    }
      }
				




/**************************************************************************/
/*                                                                        */
/*  TCheopsSendMailDialog :: Send -                                       */
/*      Envoie le courrier.                                               */
/*                                                                        */
/**************************************************************************/

void	TCheopsSendMailDialog :: CmSend  ( void ) 
   {
// Ecrire le texte du message dans un fichier
	char			Buffer [ MAX_PARAMETER_LENGTH ] ;
	char			Name   [ MAX_PARAMETER_LENGTH ] ;
	register FILE *	        fp ;
	register int		i, count ;


	tmpnam ( Name ) ;

	if  ( ( fp = fopen ( Name, "w" ) )  ==  NULL )
	   {
		:: MessageBox ( HWindow, "Erreur", MB_OK | MB_ICONEXCLAMATION,
			"Création impossible du fichier temporaire nécessaire"
			" à l'envoi du courrier." ) ;
		return ;
	     }


	count = EditClient -> GetNumLines ( ) ;
	

	for  ( i = 0 ; i < count ; i ++ )
	   {
		EditClient -> GetLine ( Buffer, sizeof ( Buffer ), i ) ;
		fprintf ( fp, "%s\n", Buffer ) ;
	    }

	fclose ( fp ) ;


// Puis construire un bloc contenant la liste des utilisateurs, suivie du
// nom du fichier
	char		ToSend [ 2 * MAX_PARAMETER_LENGTH ] ;
	int		length ;


	UserListEdit -> GetText ( ToSend, MAX_PARAMETER_LENGTH ) ;
	length = strlen ( ToSend ) + 1 ;
	strcpy ( ToSend + length, Name ) ;
	length += strlen ( Name ) + 1 ;


// Balancer la sauce
	TUnixApplication *	Application 	=  CheopsMenuWindow -> Application ;


	EventQueue -> Enqueue ( Application -> ApplicationId,
				CheopsMenuWindow -> ObjectId,
				Application -> ProcessId,
				WM_MENU_EVENT, MNF_SEND_MAIL,
				( LPARAM ) ToSend, length ) ;

// Et effacer le tout
	UserListEdit -> Clear ( ) ;
	EditClient -> Clear ( ) ;
	UserListEdit -> SetFocus ( ) ;
    }	
