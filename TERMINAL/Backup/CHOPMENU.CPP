/**************************************************************************/
/*                                                                        */
/*   ChopMenu.cpp  -  Christian Vigh, 95/10/19.                           */
/*                                                                        */
/*   Menu principal de Cheops.                                            */
/*                                                                        */
/**************************************************************************/


# include	"AllTerm.h"
# pragma	hdrstop

# if	defined ( LEBRAIN )
# include	"ddeobj.h"
# endif

# include	"ChopJet.h"
# include	"TextFile.h"
# include	"UnixApp.h"
# include	"Icon.h"
# include	"UnixDlg.h"
# include	"Event.h"
# include	"Cheops.h"
# include	"EditFile.h"
# include	"MNCommon.h"
# include	"ChopCfg.h"
# include	"ChopTree.h"
# include	"ChopMenu.h"
# include	"ChopUp.h"
# include	"ChopMail.h"



# define	INTRO_TIMER_ID		2345
# define	CHECKMAIL_TIMER_ID	2346
# define	INTRO_BITMAP		"CiLogo.bmp"
# define	MENU_BITMAP		"Prisme.bmp"



# define	ifdelete(p)		if  ( p )  delete p
# define	ifdeletea(p)		if  ( p )  delete [] p 


TCheopsMenuWindow *	TheCheopsMenuWindowObject 	=  0 ;




/**************************************************************************/
/**************************************************************************/
/********                                                          ********/
/********      Implémentation de la classe TCheopsWindowFont       ********/
/********                                                          ********/
/**************************************************************************/
/**************************************************************************/

TCheopsWindowFont :: TCheopsWindowFont  ( int  height )
	: TFont ( "MS Sans Serif", height, 0,
		  0, 0,
		  FW_NORMAL,
		  DEFAULT_PITCH | FF_DONTCARE,
		  false, false, false,
		  ANSI_CHARSET,
		  OUT_DEFAULT_PRECIS,
		  CLIP_DEFAULT_PRECIS,
		  DEFAULT_QUALITY ) 
   {  }






/**************************************************************************/
/**************************************************************************/
/********                                                          ********/
/********         Implémentation de la classe TMenuEdit            ********/
/********                                                          ********/
/**************************************************************************/
/**************************************************************************/

class	TMenuEdit :	public  TEdit
   {
	private :
		int		MyId ;
		HWND		LastFocus ;


	public :
		TMenuEdit  ( TWindow *  parent,
			     int	id,
			     char *	text,
			     int	x,
			     int	y,
			     int	w,
			     int	h,
			     uint	textlen,
			     bool	multiline = false,
			     TModule * 	module    =  0 )
			: TEdit ( parent, id, text, x, y, w, h, textlen, multiline, module ) 
		   {
			MyId      = id ;
			LastFocus = 0 ;
		    }	


	protected :
	// Mémorise la fenêtre qui a eu le focus avant celle-ci
	//
		void	EvSetFocus  ( HWND  lost )
		   {
			TEdit :: EvSetFocus ( lost ) ;
			LastFocus = lost ;
		    }


	// Envoie un message WM_COMMAND au parent de la barre de contrôle,
	// puis donne le focus à la fenêtre qui l'avait avant celle-ci
	// (on est obligé de faire ainsi, car le contrôle n'envoie pas
	// de WM_COMMAND si on appuie sur entrée)
		void	EvKeyDown ( uint  key, uint  repcnt, uint  flags )
		   {
			if  ( key  ==  VK_RETURN )
			   {
				if  ( LastFocus )
				   {
					:: SetFocus ( LastFocus ) ;
					LastFocus  = 0 ;
				    }

				Parent -> Parent -> SendMessage ( WM_COMMAND,
					MyId, 0 ) ; // 0 = msg comes from menu
			     }
			else
				TEdit :: EvKeyDown ( key, repcnt, flags ) ;
			
		      }



	//  Force le contrôle à se repeindre
	//
		void	EvWindowPosChanged  ( WINDOWPOS &  pos )
		   {
			TEdit :: EvWindowPosChanged ( pos ) ;
			Invalidate ( ) ;	
			UpdateWindow ( ) ;
		    }
			
	
	DECLARE_RESPONSE_TABLE ( TMenuEdit ) ;
     } ;
			     


DEFINE_RESPONSE_TABLE1 ( TMenuEdit, TEdit )
	EV_WM_KEYDOWN,
	EV_WM_SETFOCUS,
	EV_WM_WINDOWPOSCHANGED,
END_RESPONSE_TABLE ;





/**************************************************************************/
/**************************************************************************/
/********                                                          ********/
/********          Implémentation de la classe TIntroStatic.       ********/
/********                                                          ********/
/**************************************************************************/
/**************************************************************************/

class	TIntroStatic : 	public  TWindow
    {
	private :
		TFont *	        Font ;
		int		MaxWidth,
				MaxHeight ;
		char *		Text ;


	public :
		TIntroStatic  ( TWindow *  parent, int  w, int  h )
			: TWindow  ( parent, "" ) 
		   {

			Attr. Style = WS_CHILD | WS_VISIBLE ;
			MaxWidth  = w ;
			MaxHeight = h ;
			Text 	  = new  char [ 256 ] ;
			* Text    = 0 ;
			Font      = 0 ;
		     }


	       ~TIntroStatic ( )
		   {
			if  ( Text )	
				delete  []  Text ;
			if  ( Font )
				delete  Font ;
		     }
				



	// Change le titre de la fenêtre
		void	SetText ( char *   t )
		   {
			strcpy ( Text, t ) ;
			Invalidate  ( ) ;
		    }



	// Pour définir la couleur de fond
		char *  GetClassName  ( )
		   {
			return  ( "TCheopsIntroStatic" ) ;
		    }

		void	GetWindowClass  ( WNDCLASS &  w )
		   {
			TWindow :: GetWindowClass ( w ) ;
			w. hbrBackground = * new  TBrush ( GetSysColor ( COLOR_BTNFACE ) ) ;
		    }





		BOOL	Create  ( void ) ;
		void	Paint   ( TDC &, BOOL, TRect & ) ;
       } ;





/**************************************************************************/
/*                                                                        */
/*  TIntroStatic :: Create -                                              */
/*      Crée le contrôle avec la bonne dimension.                         */
/*                                                                        */
/**************************************************************************/

BOOL	TIntroStatic :: Create  ( void )
   {
	register int	cyCaption	=  GetSystemMetrics ( SM_CYCAPTION ),
			cyBorder	=  GetSystemMetrics ( SM_CYBORDER ) ;
	register int	height ;
	


// Créer la police et calculer sa taille réelle
	TScreenDC	DC ;
	TEXTMETRIC	tm ;


	Font = new  TFont ( "Times New Roman", -14, 0, 0, 0, FW_ULTRABOLD,
			VARIABLE_PITCH | FF_DONTCARE,
			false, false, false,
			ANSI_CHARSET,
			OUT_CHARACTER_PRECIS,
			CLIP_CHARACTER_PRECIS,
			PROOF_QUALITY ) ;

	DC. SelectObject ( * Font ) ;
	DC. GetTextMetrics ( tm ) ;
	DC. RestoreFont ( ) ;
	height = tm. tmHeight + tm. tmExternalLeading +
			( 4 * cyBorder ) ;	// un peu de rab pour aérer


	Attr. H = height + ( 4 * cyBorder ) ;	// Les effets de relief font 2
	Attr. X = cyCaption ;
	Attr. Y = MaxHeight - cyCaption - Attr. H ;
	Attr. W = MaxWidth  - ( 2 * cyCaption ) ;


	return  ( TWindow :: Create ( ) ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TIntroStatic :: Paint -                                               */
/*      Dessine le contrôle, avec texte centré.                           */
/*                                                                        */
/**************************************************************************/

void	TIntroStatic :: Paint ( TDC &  dc, BOOL, TRect & )
   {
	TBrush		HiliteBrush ( GetSysColor ( COLOR_BTNHIGHLIGHT ) ),
			ShadowBrush ( GetSysColor ( COLOR_BTNSHADOW ) ) ;
	register int	cxBorder	=  GetSystemMetrics ( SM_CXBORDER ),
			cyBorder	=  GetSystemMetrics ( SM_CYBORDER ) ;
	TRect		DrawRect 	= GetClientRect ( ) ;
	int		x		= 2 * cxBorder ;



	dc. SelectObject ( ShadowBrush ) ;

	while  ( x )
	   {
	// Ligne verticale
		dc. PatBlt (  DrawRect. left  + x, DrawRect. top    + x,
				cxBorder, DrawRect. Height ( ) - ( 2 * x ), PATCOPY ) ;

	// Et horizontale
		dc. PatBlt ( DrawRect. left   +  x, DrawRect. top + x,
				DrawRect. Width ( ) - ( 2 * x ), cyBorder, PATCOPY ) ;

		x -- ;
	     }

	dc. RestoreBrush ( ) ;


// Peindre l'ombre du bouton si son état est haut
	dc. SelectObject ( HiliteBrush ) ;
	x = 2 * cxBorder ;

	while  ( x )
	   {
	// Ligne verticale
		dc. PatBlt ( DrawRect. right - x - 1, DrawRect. top    + x,
				cxBorder, DrawRect. Height ( ) - ( 2 * x ), PATCOPY ) ;

	// Et horizontale
		dc. PatBlt ( DrawRect. left   +  x, DrawRect. bottom - x - 1,
				DrawRect. Width ( ) - x - 1, cyBorder, PATCOPY ) ;

		x -- ;
	     }

	dc. RestoreBrush ( ) ;


// Remplir l'intérieur 
	DrawRect. left 		+=  2 * cxBorder ;
	DrawRect. right	        -=  2 * cxBorder ;
	DrawRect. top		+=  2 * cyBorder ;
	DrawRect. bottom	-=  2 * cyBorder ;


// Dessiner le texte
	if  ( Text  &&  * Text ) 
	   {
		int		OldMode   = dc. SetBkMode ( TRANSPARENT ) ;
		COLORREF	OldColor  = dc. SetTextColor ( GetSysColor ( COLOR_BTNTEXT ) ) ;


		dc. SelectObject ( * Font ) ;
		dc. DrawText ( Text, strlen ( Text ), DrawRect, 
					DT_SINGLELINE | DT_CENTER | DT_VCENTER ) ;


		dc. RestoreFont ( ) ;
		dc. SetBkMode ( OldMode ) ;
		dc. SetTextColor ( OldColor ) ;
	     }
    }



		


/**************************************************************************/
/**************************************************************************/
/********                                                          ********/
/********          Implémentation de la classe TIntroWindow.       ********/
/********                                                          ********/
/**************************************************************************/
/**************************************************************************/


class	TIntroWindow 	:  public  TWindow 
   {
	private :
		TIntroStatic *	InfoText ;
		char *		BitmapFile ;
		TDib *		Bitmap ;



	public :
		TIntroWindow  ( TWindow *  parent, char *  bitmapname )
			: TWindow ( parent, 0 )
		   {
			Attr. Style    = WS_POPUP | WS_BORDER | WS_VISIBLE ;
			Attr. ExStyle  = WS_EX_TOPMOST ;
			InfoText = 0 ;
			Bitmap   = 0 ;

			if  ( bitmapname  &&  * bitmapname ) 
				BitmapFile = strdup ( bitmapname ) ;
			else
				BitmapFile = strdup ( INTRO_BITMAP ) ;
		    }



	       ~TIntroWindow  ( ) 
		   {
			if  ( InfoText )
				delete  InfoText ;
			if  ( Bitmap )
				delete  Bitmap ;
			if  ( BitmapFile )
				free (  BitmapFile ) ;
		     }
	


	// SetText : change le texte affiché dans la fenêtre d'introduction
		void	SetText  ( char *  t1 )
		   {
			if  ( InfoText )
			   {
				if  ( t1 ) 
					InfoText -> SetText ( t1 ) ;
				else
				   {
					delete  InfoText ;
					InfoText = 0 ;
				    }
			    }
		    }




	// Syscommand : empêche la fermeture avec Alt-F4
		void	EvSysCommand ( uint  cmdtype, TPoint &  pos )
		   {
			//if  ( cmdtype  !=  SC_CLOSE )
			TWindow :: EvSysCommand ( cmdtype, pos ) ;
		    }


	// Ca, c'est pour qu'on change pas le curseur n'importe comment
		char *	GetClassName   ( void ) 
		   {
			return  ( "TCheopsIntroWindow" ) ;
		    }

		void	GetWindowClass ( WNDCLASS &  w )
		   {
			TWindow :: GetWindowClass ( w ) ;
			w. hCursor = 0 ;
		    }



		BOOL	Create  ( void ) ;
		void	Paint   ( TDC &  dc, bool  erase, TRect &  rect ) ;


	DECLARE_RESPONSE_TABLE ( TIntroWindow ) ;
     } ;





DEFINE_RESPONSE_TABLE1 ( TIntroWindow, TWindow ) 
	EV_WM_SYSCOMMAND,
END_RESPONSE_TABLE ;





/**************************************************************************/
/*                                                                        */
/*  TIntroWindow :: Create -                                              */
/*      Crée la fenêtre d'introduction avec une dimension conforme à      */
/*      la bitmap, puis place le contrôle static.                         */
/*                                                                        */
/**************************************************************************/

BOOL	TIntroWindow :: Create  ( void )
   {
	register int		cxScreen	=  GetSystemMetrics ( SM_CXSCREEN ),
				cyScreen	=  GetSystemMetrics ( SM_CYSCREEN ),
				cxBorder	=  GetSystemMetrics ( SM_CXBORDER ),
				cyBorder	=  GetSystemMetrics ( SM_CYBORDER ) ;


// Récupérer la dimension de la bitmap
	BITMAPINFOHEADER *	Info ;
	char			Path [ MAX_PARAMETER_LENGTH ] ;
	register char *		p ;
	int			Width	=  600,
				Height  =  400 ;


	strcpy ( Path, BitmapFile ) ;

	if  ( access ( Path, 0 )  <  0 )
	   {
		TerminalApplication -> GetModuleFileName ( Path, sizeof ( Path ) ) ;
		p = Path + strlen ( Path ) - 1 ;

		while  ( p >= Path  &&  strchr ( "/:\\", * p )  ==  NULL )
			p -- ;
		p ++ ;
		strcpy ( p, BitmapFile ) ;
	     }

	if  ( access ( Path, 0 )  >=  0 )
	   {
		Bitmap = new  TDib ( Path ) ;
		Info   = Bitmap -> GetInfoHeader ( ) ;
		Width  = ( int ) Info -> biWidth ;
		Height = ( int ) Info -> biHeight ;
	     }
		

// Centrer la fenêtre à l'écran
	Attr. X = ( cxScreen - Width  ) / 2 ;
	Attr. Y = ( cyScreen - Height ) / 2 ;
	Attr. W = Width  + ( 2 * cxBorder ) ;
	Attr. H = Height + ( 2 * cyBorder ) ;

// Créer le contrôle statique
	InfoText = new TIntroStatic ( this, Attr. W, Attr.H ) ;

	return  ( TWindow :: Create ( ) ) ;
    }

	





/**************************************************************************/
/*                                                                        */
/*   TIntroWindow :: Paint                                                */
/*      Dessine le bitmap.                                                */
/*                                                                        */
/**************************************************************************/

void	TIntroWindow :: Paint  ( TDC  &  dc, BOOL, TRect &  rect )
   {
	if  ( Bitmap )
	   {
		BITMAPINFOHEADER *	bm ;


		bm = Bitmap -> GetInfoHeader ( ) ;

		dc. SetDIBitsToDevice ( 
			TRect ( 0, 0, ( int ) bm -> biWidth,
				      ( int ) bm -> biHeight ),
			TPoint ( 0, 0 ),
			* Bitmap ) ;	
	     }

	TWindow :: Paint ( dc, FALSE, rect ) ;
     }



			
/**************************************************************************/
/**************************************************************************/
/********                                                          ********/
/********          Implémentation de la classe TArrowWindow.       ********/
/********                                                          ********/
/**************************************************************************/
/**************************************************************************/

class	TArrowWindow : 	public  TWindow
   {
	private :
		HICON		ArrowIcon ;



	public :
	// constructeur : charge l'icône
		TArrowWindow  ( TWindow *  parent, TResId  iconid )
			: TWindow ( parent )
		   {
			Attr. Style   =  WS_CHILD ;
			Attr. H       =  GetSystemMetrics ( SM_CYICON ) ;
			Attr. W       =  GetSystemMetrics ( SM_CXICON ) ;
			
			ArrowIcon     =  :: LoadIcon ( * GetModule ( ), iconid ) ;
		     }


	// Destructeur : libérer l'icône
		~TArrowWindow  ( )
		   {
			if  ( ArrowIcon )
				:: DestroyIcon ( ArrowIcon ) ;
		    }


	// Paint : dessiner l'icône
		void	Paint  ( TDC &  dc, BOOL, TRect & )
		   {
			dc. DrawIcon ( 0, 0, ArrowIcon ) ;
		    }

	// Surcharger ces fonctions pour avoir une brosse nulle (et ne pas 
	// effacer les fenêtres en dessous)
		char *	GetClassName  ( void )
		   {
			return  ( "ArrowWindow" ) ;
		    }


		void	GetWindowClass ( WNDCLASS &  cls )
		   {
			TWindow :: GetWindowClass ( cls ) ;
			cls. hbrBackground = ( HBRUSH ) GetStockObject ( NULL_BRUSH ) ;
		    }

    } ;








/**************************************************************************/
/**************************************************************************/
/********                                                          ********/
/********     Implémentation de la classe TCheopsIconicWindow      ********/
/********                                                          ********/
/**************************************************************************/
/**************************************************************************/

class	TCheopsIconicWindow 	:  public  TMDIChild
   {
	private :
		TWindow *	CheopsWindow ;
		BOOL		ClosedBySysCommand ;


	public :
		TCheopsIconicWindow   ( TMDIClient * 	parent,
					TWindow *	chopwin,
					TResId &	icon )
			: TMDIChild ( * parent, chopwin -> Title )
		   {
			CheopsWindow = chopwin ;
			SetIcon ( GetModule ( ), icon ) ;
			Attr. Style  &=  ~ ( WS_VISIBLE | WS_MAXIMIZEBOX ) ;
			ClosedBySysCommand = FALSE ;
		     }




	// Create : cache la fenêtre Cheops et montre celle-ci sous forme
	// d'icône
		BOOL	Create ( void )
		   {
			register bool	Status ;


			Status = TMDIChild :: Create ( ) ;
			Show ( SW_MINIMIZE ) ;
			CheopsWindow -> Show ( SW_HIDE ) ;
			return  ( Status ) ;
		      }


	// EvClose ; Remontre la fenêtre Cheops avant de se détruire
		void	EvClose  ( void )
		   {
			if  ( ClosedBySysCommand )
				CheopsWindow -> Show ( SW_SHOW ) ;
			else
				CheopsWindow -> PostMessage ( WM_CLOSE ) ;
			TMDIChild :: EvClose ( ) ;
		     }



	// EvSysCommand : rattrape la commande RESTORE
		void	EvSysCommand ( uint  cmdtype, TPoint &  pos )
		   {
			register uint 	type = ( cmdtype & 0xFFF0 ) ;


			if  ( type  ==  SC_RESTORE  ||  type  ==  SC_MAXIMIZE )
			   {
				ClosedBySysCommand = TRUE ;
				SendMessage ( WM_CLOSE ) ;
			    }
			else
				TMDIChild :: EvSysCommand ( cmdtype, pos ) ;
		      }
		
			
	DECLARE_RESPONSE_TABLE ( TCheopsIconicWindow ) ;
    } ;



DEFINE_RESPONSE_TABLE1 ( TCheopsIconicWindow, TMDIChild )
	EV_WM_CLOSE,
	EV_WM_SYSCOMMAND,
END_RESPONSE_TABLE ;







/**************************************************************************/
/*                                                                        */
/*  newstring -                                                           */
/*      Duplique une chaîne de caractères.                                */
/*                                                                        */
/**************************************************************************/

static char *	newstring ( char *  str )
   {
	if  ( ! str )
		return  ( 0 ) ;


	register char *		p = new  char [ strlen ( str ) + 1 ] ;
	strcpy ( p, str ) ;
	return ( p ) ;
     }





/**************************************************************************/
/**************************************************************************/
/********                                                          ********/
/********          Implémentation de la classe TCheopsMenu         ********/
/********                                                          ********/
/**************************************************************************/
/**************************************************************************/


DEFINE_RESPONSE_TABLE1 ( TCheopsMenuWindow, TDecoratedMDIFrame )
	EV_COMMAND_AND_ID  ( Cheops_Module_Base +  0	, EvModuleCommand ),
	EV_COMMAND_AND_ID  ( Cheops_Module_Base +  1	, EvModuleCommand ),
	EV_COMMAND_AND_ID  ( Cheops_Module_Base +  2	, EvModuleCommand ),
	EV_COMMAND_AND_ID  ( Cheops_Module_Base +  3	, EvModuleCommand ),
	EV_COMMAND_AND_ID  ( Cheops_Module_Base +  4	, EvModuleCommand ),
	EV_COMMAND_AND_ID  ( Cheops_Module_Base +  5	, EvModuleCommand ),
	EV_COMMAND_AND_ID  ( Cheops_Module_Base +  6	, EvModuleCommand ),
	EV_COMMAND_AND_ID  ( Cheops_Module_Base +  7	, EvModuleCommand ),
	EV_COMMAND_AND_ID  ( Cheops_Module_Base +  8	, EvModuleCommand ),
	EV_COMMAND_AND_ID  ( Cheops_Module_Base +  9	, EvModuleCommand ),
	EV_COMMAND_AND_ID  ( Cheops_Module_Base + 10	, EvModuleCommand ),
	EV_COMMAND_AND_ID  ( Cheops_Module_Base + 11	, EvModuleCommand ),
	EV_COMMAND_AND_ID  ( Cheops_Module_Base + 12	, EvModuleCommand ),
	EV_COMMAND_AND_ID  ( Cheops_Module_Base + 13	, EvModuleCommand ),
	EV_COMMAND_AND_ID  ( Cheops_Module_Base + 14	, EvModuleCommand ),
	EV_COMMAND_AND_ID  ( Cheops_Module_Base + 15	, EvModuleCommand ),
	EV_COMMAND_AND_ID  ( Cheops_Module_Base + 16	, EvModuleCommand ),
	EV_MESSAGE  	   ( WM_NCACTIVATE		, EvMyNCActivate  ),
	EV_MESSAGE	   ( WM_MENUDROPPED             , WmMenuDropped   ),
	EV_MESSAGE	   ( WM_MENUSELECTED		, WmMenuSelected  ),
	EV_MESSAGE	   ( WM_MENUCLOSED		, WmMenuClosed    ),
# if  ! STANDALONE
	EV_MESSAGE	   ( WM_ENDAPPLICATION		, WmEndApplication),
# endif
	EV_WM_INITMENUPOPUP,
	EV_WM_TIMER,
	EV_WM_CLOSE,
	EV_WM_KILLFOCUS,
	EV_WM_SETFOCUS,

	EV_COMMAND_ENABLE  ( CM_FILE_READMAIL		, CommandEnable   ),
	EV_COMMAND_ENABLE  ( CM_WINDOW_ORDER_MENUS	, CommandEnable   ),
	EV_COMMAND_ENABLE  ( CM_WINDOW_ICONIFY_SCREENS	, CommandEnable   ),
	EV_COMMAND_ENABLE  ( CM_WINDOW_ICONIFY_LISTS	, CommandEnable   ),
	EV_COMMAND_ENABLE  ( CM_WINDOW_ICONIFY_ALL	, CommandEnable   ),
	EV_COMMAND	   ( CM_ENTER_COMMAND	      	, CmEnterCommand  ),
	EV_COMMAND	   ( CM_YOUR_CHOICE		, CmYourChoice    ),
	EV_COMMAND	   ( CM_FILE_QUIT		, CmFileQuit	  ),
	EV_COMMAND  	   ( CM_FILE_CONFIGURE		, CmConfig        ),
	EV_COMMAND	   ( CM_CONFIG_YEAR		, CmConfigYear    ),
	EV_COMMAND         ( CM_CONFIG_MONTH		, CmConfigMonth   ),
	EV_COMMAND	   ( CM_CONFIG_PRINTER		, CmConfigPrinter ),
	EV_COMMAND	   ( CM_FILE_LOG		, CmFileLog       ),
	EV_COMMAND	   ( CM_FILE_READMAIL		, CmFileReadMail  ),
	EV_COMMAND	   ( CM_FILE_SENDMAIL		, CmFileSendMail  ),
	EV_COMMAND 	   ( CM_WINDOW_ORDER_MENUS	, CmWindowOrderMenus ),
	EV_COMMAND	   ( CM_WINDOW_ORDER_ICONS	, CmWindowOrderIcons ),
	EV_COMMAND	   ( CM_WINDOW_ICONIFY_SCREENS  , CmWindowIconifyScreens ),
	EV_COMMAND	   ( CM_WINDOW_ICONIFY_LISTS    , CmWindowIconifyLists   ),
	EV_COMMAND	   ( CM_WINDOW_ICONIFY_ALL	, CmWindowIconifyAll	 ),
	EV_COMMAND	   ( CM_UNHIDE_COMMAND		, CmUnhideCommand	),
END_RESPONSE_TABLE ;



/**************************************************************************/
/*                                                                        */
/*  Constructeur -                                                        */
/*                                                                        */
/**************************************************************************/

TCheopsMenuWindow :: TCheopsMenuWindow  ( TWindow *  parent, char *  title )
	: TDecoratedMDIFrame ( title, 	 TResId ( "CHEOPS_MENU" ),
		* ( CheopsMenuWindowClient = new  TCheopsMenuWindowClient ( this ) ), TRUE ) 
   {
// Sauvegarder le parent : on va le cacher, mais il faudra le restaurer	
	MenuParent = parent ;

// Initialisation des membres
	memset ( & CheopsUserInfo, 0, sizeof ( CheopsUserInfo ) ) ;
	memset ( & CheopsUserConfig, 0, sizeof ( CheopsUserConfig ) ) ;

	StatusBar		=  0 ;
	MainMenuControlBar      =  0 ;
	CheopsModules		=  0 ;
	CheopsModuleCount	=  0 ;
	CheopsMenuGadgets	=  0 ;
	MenuTree		=  0 ;
	FloatingMenus		=  0 ;
	IntroWindow		=  0 ;
	LastOpenedModule	=  -1 ;
	ShutDownRequested	=  FALSE ;
	MailAvailable		=  FALSE ;
	ReadMailWindow		=  0 ;
	SendMailWindow 		=  0 ;

	Attr. Style  	       |=  WS_MAXIMIZE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS ;
	Attr. Style  	       &=  ~WS_VISIBLE ;
	Attr. AccelTable        =  TResId ( "CHEOPS_ACCEL" ) ;

// Création de la fenêtre dessinant la flèche
	ArrowWindow   =  new  TArrowWindow ( this, TResId ( "ARROW_ICON" ) ) ;


	TheCheopsMenuWindowObject = this ;
    }




/**************************************************************************/
/*                                                                        */
/*   Destructeur -                                                        */
/*                                                                        */
/**************************************************************************/

TCheopsMenuWindow :: ~TCheopsMenuWindow  ( )
  {
	register int	i ;


# if	! STANDALONE
	Application -> DeleteObject ( ObjectId, FALSE ) ;
# endif

	TheCheopsMenuWindowObject = 0 ;


// Si la fenêtre avait un parent, il a été caché; il faut donc le montrer
	if  ( MenuParent )
	   {
		MenuParent -> BringWindowToTop ( ) ;
		MenuParent -> Show ( SW_SHOW ) ; 
	    }
	else
		MessageBox ( "MenuParent = 0 !!!!", "Erreur dans ~TCheopsMenuWindow()", MB_OK ) ;


// Effacer les barres
	ifdelete ( MainMenuControlBar ) ;
	ifdelete ( ControlBar ) ;
	ifdelete ( StatusBar ) ;
	ifdelete ( IntroWindow ) ;
	

// et les objets alloués
	ifdeletea ( CheopsUserInfo. TTYName ) ;
	ifdeletea ( CheopsUserInfo. UserName ) ;

	for  ( i = 0 ; i < CheopsUserInfo. UnixPrinterCount ; i ++ )
		ifdeletea  ( CheopsUserInfo. UnixPrinters [i] ) ;
	ifdeletea  ( CheopsUserInfo. UnixPrinters ) ;

	ifdeletea ( CheopsUserConfig. SiteName ) ;
	ifdeletea ( CheopsUserConfig. PrinterName ) ;
	ifdeletea ( CheopsUserConfig. DescriptPath ) ;
	ifdeletea ( CheopsUserConfig. Protection ) ;
	ifdeletea ( CheopsUserConfig. ProgramPath ) ;
	ifdeletea ( CheopsUserConfig. DataPath ) ;


// Effacement des modules : ne pas faire de delete sur les gadgets, car cela
// a déjà été fait par le destructeur de la MainMenuControlBar
	for  ( i = 0 ; i < CheopsModuleCount ; i ++ )
	   {
		ifdeletea  ( CheopsModules [i]. ModuleName ) ;
		ifdeletea  ( CheopsModules [i]. ModuleText ) ;
	    }
	ifdeletea ( CheopsModules ) ;

	ifdelete  ( ArrowWindow ) ;
	ifdelete  ( MenuTree ) ;
   }





/**************************************************************************/
/*                                                                        */
/*  TCheopsMenuWindow :: Create -                                         */
/*      Crée la fenêtre et y insère les décorations.                      */
/*                                                                        */
/**************************************************************************/

BOOL	TCheopsMenuWindow :: Create  ( void )
   {
	register int 	Status ;



// Créer les barres
	MainMenuControlBar = new  TControlBar ( this ) ;
	MainMenuControlBar -> SetHintMode ( TGadgetWindow :: NoHints ) ;

	for  ( int  i = 0 ; i < CheopsModuleCount ; i ++ )
		MainMenuControlBar -> Insert ( * CheopsMenuGadgets [i] ) ;	


	StatusBar	   = new  TStatusBar  ( this ) ;
	MonthTextGadget    = new  TMultiFontTextGadget ( CM_CONFIG_MONTH,
					TMultiFontTextGadget :: Recessed, 
					0, 0, 8, TRUE ) ;
	YearTextGadget	   = new  TMultiFontTextGadget ( CM_CONFIG_YEAR,
					TMultiFontTextGadget :: Recessed, 
					0, 0, 8, TRUE ) ;
	PrinterTextGadget  = new  TMultiFontTextGadget ( CM_CONFIG_PRINTER,
					TMultiFontTextGadget :: Recessed, 
					0, 0, 8, TRUE ) ;
	UserTextGadget	   = new  TMultiFontTextGadget ( 0,
					TMultiFontTextGadget :: Recessed, 
					0, 0, 8, FALSE ) ;

	StatusBar -> Insert ( * MonthTextGadget ) ;
	StatusBar -> Insert ( * YearTextGadget ) ;
	StatusBar -> Insert ( * PrinterTextGadget ) ;
	StatusBar -> Insert ( * UserTextGadget ) ;


	ControlBar = new TColorControlBar ( this ) ;
	ControlBar -> Attr. Style  |=  WS_CLIPCHILDREN ;
	ControlBar -> SetHintMode ( TGadgetWindow :: EnterHints ) ;

	ControlBar -> Insert ( * new  TButtonGadget ( CM_FILE_CONFIGURE,
			CM_FILE_CONFIGURE, TButtonGadget :: Command ) ) ;
	ControlBar -> Insert ( * new  TButtonGadget ( CM_FILE_LOG,       
			CM_FILE_LOG, TButtonGadget :: Command ) ) ;
	ControlBar -> Insert ( * new  TSeparatorGadget ( 32 ) ) ;

	ControlBar -> Insert ( * new  TButtonGadget ( CM_FILE_READMAIL,
			CM_FILE_READMAIL, TButtonGadget :: Command ) ) ;
	ControlBar -> Insert ( * new  TButtonGadget ( CM_FILE_SENDMAIL,
			CM_FILE_SENDMAIL, TButtonGadget :: Command ) ) ;

// Insérer un séparateur qui va cadrer le reste des gadgets à droite
	ControlBar -> Insert ( * new  TShrinkSeparatorGadget ( 6 ) ) ;

// Récupérer la taille d'un caractère
	TEXTMETRIC 		tm ;
	TScreenDC		DC ;

	DC. SelectObject ( ControlBar -> GetFont ( ) ) ;
	DC. GetTextMetrics ( tm ) ;


// Insérer "votre choix" 
	ControlBar -> Insert ( * new  TAutoTextGadget ( 0, TTextGadget :: None,
				TAutoTextGadget :: AutoAdjust, 10, "Votre choix :" ) ) ;

// Puis le contrôle edit
	EditControlGadget = new  TMenuEdit ( StatusBar, CM_YOUR_CHOICE, 0, 0, 0, 
					tm. tmMaxCharWidth * 15, 
					tm. tmHeight + ( 8 * GetSystemMetrics ( SM_CYBORDER ) ),
					0 ) ;
	EditControlGadget -> Attr. Style  =  WS_CHILD | WS_VISIBLE | WS_BORDER |
					     ES_AUTOHSCROLL  | ES_LEFT ;

	TControlGadget *	EditGadget = new  TControlGadget ( * EditControlGadget ) ;

	EditGadget -> SetShrinkWrap ( TRUE, TRUE ) ;
	
	ControlBar -> Insert ( * EditGadget ) ;


// Encore un petit séparateur, sinon le contrôle EDIT est trop près du bord
	ControlBar -> Insert ( * new  TSeparatorGadget ) ;
									 

// Insérer les barres
	Insert ( * ControlBar        , TDecoratedFrame :: Top ) ;
	Insert ( * StatusBar         , TDecoratedFrame :: Bottom ) ;
	Insert ( * MainMenuControlBar, TDecoratedFrame :: Bottom ) ;


// Icône de la fenêtre
	SetIcon ( GetModule ( ), TResId ( "MENU_ICON" ) ) ;


// Créer la fenêtre
	UpdateInformation ( ) ;
	Status = TDecoratedFrame :: Create ( ) ;

// Redéfinir la police du contrôle EDIT
	EditControlGadget -> SendMessage ( WM_SETFONT, 
			( WPARAM ) ( HFONT ) ControlBar -> GetFont ( ), TRUE ) ;


// Créer la flèche
	ArrowWindow -> Create ( ) ;		


// Créer les menus flottants
	FloatingMenusAutoCreate ( ) ;
		

// Cacher le parent
	if  ( MenuParent )
		MenuParent -> Show ( SW_HIDE ) ;

// Donner un nom à l'application (changer le texte du menu)
	CheopsConfiguration -> Get ( "Settings", "ApplicationName", 
		ApplicationName, "Cheops" ) ;

// Récupérer le menu de la fenetre
	HMENU		hMenu 	=  GetMenu ( ) ;

        ModifyMenu ( hMenu, 1, MF_BYPOSITION | MF_STRING, 0, ApplicationName ) ;
	

	return  ( Status ) ;
     }	





/**************************************************************************/
/*                                                                        */
/*  TCheopsMenuWindow :: CmFileQuit -                                     */
/*      Quitte le menu.                                                   */
/*                                                                        */
/**************************************************************************/

void	TCheopsMenuWindow :: CmFileQuit ( void )
   {
	EvClose ( ) ;
    }



/**************************************************************************/
/*                                                                        */
/*  TCheopsMenuWindow :: CmUnhideCommand -                                */
/*      Réaffiche ou recache la fenetre de l'émulateur.                   */
/*                                                                        */
/**************************************************************************/

void	TCheopsMenuWindow :: CmUnhideCommand ( void )
   {
	if  ( TerminalWindow -> IsWindowVisible ( ) )  
		TerminalWindow -> Show ( SW_HIDE ) ;
	else
	   {
		TerminalWindow -> BringWindowToTop ( ) ;
		TerminalWindow -> Show ( SW_SHOW ) ;
	     }
     }


/**************************************************************************/
/*                                                                        */
/*  TCheopsMenuWindow :: CanClose -                                       */
/*      Retourne TRUE si on peut quitter le menu.                         */
/*                                                                        */
/**************************************************************************/

BOOL	TCheopsMenuWindow :: CanClose  ( void )
   {
	char			Message [ 256 ] ;
	register BOOL		Status = TRUE ;


	if  ( ! ShutDownRequested )
	   {
		wsprintf ( Message, "Voulez-vous quitter %s ?", ApplicationName ) ;
		Status = ( MessageBox  ( Message, "Confirmation",
						MB_YESNOCANCEL )  ==  IDYES ) ;
	     }


	if  ( Status )
	   {
		register TCheopsFloatingMenu *	fm = FloatingMenus,
					     *	fmtemp ;


	// Effacer d'abord la section entière
		CheopsConfiguration -> Set ( "Floating Menus", 0, "" ) ;

		while  ( fm )
		   {
			char		Buffer [ MAX_PARAMETER_LENGTH ] ;
			TRect		Rect = fm -> GetWindowRect ( ) ;


			wsprintf ( Buffer, "%s,%d,%d,%d", fm -> MenuWindow -> ModuleName,
					Rect. left, Rect. top,
					fm -> IsMinimized ( ) ? 1 : 0 ) ;
			CheopsConfiguration -> Set ( "Floating Menus", 
				fm -> MenuWindow -> GetMenu ( ). MenuName, Buffer ) ;

			fmtemp = fm ;
			fm = fm -> Next ;
			delete  fmtemp ;
		     }


		EventQueue -> Enqueue ( Application -> ApplicationId,
					ObjectId,
					Application -> ProcessId,
					WM_CLOSE,
					( GetKeyState ( VK_SHIFT ) & 0x8000 ) ?  1 : 0,
					0, UWC_LPARAM_NOT_POINTER ) ;

		ApplicationTable -> RemoveObject ( Application -> ApplicationId, this ) ;
	      }

	return  ( Status ) ;
    }
					       




/**************************************************************************/
/*                                                                        */
/*  TCheopsMenuWindow :: CmFileConfigure -                                */
/*      Config Cheops.                                                    */
/*                                                                        */
/**************************************************************************/

void	TCheopsMenuWindow :: CmConfig  ( void )
  {
	CmFileConfigure ( ) ;
   }


void	TCheopsMenuWindow :: CmConfigYear ( void )
   {
	CmFileConfigure ( IDC_YEAR ) ;
    }


void	TCheopsMenuWindow :: CmConfigMonth ( void )
   {
	CmFileConfigure ( IDC_MONTH ) ;
    }


void	TCheopsMenuWindow :: CmConfigPrinter ( void )
   {
	CmFileConfigure ( IDC_PRINTER ) ;
    }




void	TCheopsMenuWindow :: CmFileConfigure   ( int  focus )
   {
	if  ( TCheopsConfigurationDialog ( this, 
		CheopsUserConfig, CheopsUserInfo, focus ). 
			Execute ( )  ==  IDOK )
	   {
	// Envoyer l'évènement à Unix
		char			SendBuffer [ 1024 ] ;
		register char *	        p	=  SendBuffer ;


		copystr ( p, CheopsUserConfig. SiteName ) ;
		copyi16 ( p, CheopsUserConfig. Year ) ;
		copystr ( p, CheopsUserConfig. PrinterName ) ;
		copych  ( p, CheopsUserConfig. Month ) ;
		copystr ( p, CheopsUserConfig. Protection ) ;
		copystr ( p, CheopsUserConfig. ProgramPath ) ;
		copystr ( p, CheopsUserConfig. DataPath ) ;
		copystr ( p, CheopsUserConfig. DescriptPath ) ;

# if	! STANDALONE
		EventQueue -> Enqueue ( Application -> ApplicationId,
				ObjectId,
				Application -> ProcessId,
				WM_MENU_EVENT,
				MNF_USER_CONFIG,
				( LPARAM ) SendBuffer, ( int ) ( p - SendBuffer ) ) ;
# endif
	


		UpdateInformation ( ) ;
	     }
    }




/**************************************************************************/
/*                                                                        */
/*  TCheopsMenuWindow :: CmFileLog -                                      */
/*      Affiche la fenêtre Historique.                                    */
/*                                                                        */
/**************************************************************************/

void	TCheopsMenuWindow :: CmFileLog  ( void )
   {
    }





/**************************************************************************/
/*                                                                        */
/*  TCheopsMenuWindow :: CmFileReadMail -                                 */
/*      Lit le courrier.                                                  */
/*                                                                        */
/**************************************************************************/

void	TCheopsMenuWindow :: CmFileReadMail  ( void )
   {
# if	! STANDALONE
	EventQueue -> Enqueue ( Application -> ApplicationId,
			ObjectId,
			Application -> ProcessId,
			WM_MENU_EVENT,
			MNF_READ_MAIL,
			0, 0 ) ;
# else
	char			Buffer [ MAX_PARAMETER_LENGTH ] ;
	TCheopsMailDialog *   	mail ;


	wsprintf ( Buffer, "Courrier de l'utilisateur %s",
			CheopsUserInfo. UserName ) ;
	mail = new  TCheopsMailDialog ( GetClientWindow ( ),
				Buffer, "/terminal/bin/dev/mail.usr" ) ;
	mail -> Create ( ) ;
# endif
    }                




/**************************************************************************/
/*                                                                        */
/*  TCheopsMenuWindow :: CmFileSendMail -                                 */
/*      Envoie du courrier.                                               */
/*                                                                        */
/**************************************************************************/

void	TCheopsMenuWindow :: CmFileSendMail  ( void )
   {
	if  ( SendMailWindow )	
		SendMailWindow -> Activate ( ) ;
	else
	   {
		SendMailWindow = new  TCheopsSendMailDialog ( this,
					CheopsMenuWindowClient,
					"Envoi d'un courrier" ) ;
		SendMailWindow -> Create ( ) ;
	     }
    }




/**************************************************************************/
/*                                                                        */
/*  TCheopsMenuWindow :: CmWindowOrderMenus -                             */
/*      Range les menus.                                                  */
/*                                                                        */
/**************************************************************************/

void	TCheopsMenuWindow :: CmWindowOrderMenus ( void )
  {
	TCheopsFloatingMenu *	fm	 = FloatingMenus ;
	TRect			Rect	 = GetClientWindow ( ) -> GetClientRect ( ) ;
	register int		cyDelta	 = GetSystemMetrics ( SM_CYBORDER ) * 2,
				cxDelta	 = GetSystemMetrics ( SM_CYCAPTION ) ;
	int			currentx = cxDelta,
				currenty = cyDelta ;



	while  ( fm )
	  { 
		if  ( ! fm -> IsMinimized ( ) )
			fm -> HandleMessage ( WM_SYSCOMMAND, SC_MINIMIZE ) ;


		TRect 	MenuRect  =  fm -> GetWindowRect ( ) ;


	// Cas où la hauteur est totalement remplie par les menus
		if  ( currenty + MenuRect. Height ( )  >  Rect. Height ( ) )
		   {
			currenty  = cyDelta ;
			currentx += cxDelta * 2 ;
		    }


		fm -> SetWindowPos ( 0, currentx, currenty,
			0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE ) ;

		currenty += MenuRect. Height ( ) + cyDelta ;

		fm = fm -> Next ;
	     }
   }




/**************************************************************************/
/*                                                                        */
/*  TCheopsMenuWindow :: CmWindowOrderIcons -                             */
/*      Range les icônes.                                                 */
/*                                                                        */
/**************************************************************************/

void	TCheopsMenuWindow :: CmWindowOrderIcons  ( void )
   {
	CheopsMenuWindowClient -> SendMessage ( WM_MDIICONARRANGE ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TCheopsMenuWindow :: CmWindowIconifyScreens -                         */
/*      Met tous les écrans de saisie de Cheops sous forme d'icônes.      */
/*                                                                        */
/**************************************************************************/

static BOOL FAR PASCAL	Iconify ( HWND  hwnd, LPARAM  screens )
   {
	TWindow *	win  	=  GetWindowPtr ( hwnd ) ;


	if (  win )
	   {
		if  ( screens  &&  
			typeid ( * win )  ==  typeid ( TCheopsDialog ) )
			win -> SendMessage ( WM_SYSCOMMAND, SC_MINIMIZE ) ;
		else if  ( ! screens  &&  typeid ( * win )  ==  typeid ( TEditWindow ) )
			win -> SendMessage ( WM_SYSCOMMAND, SC_MINIMIZE ) ;
	     }

	return  ( TRUE ) ;
    }                                                                    


void	TCheopsMenuWindow :: CmWindowIconifyScreens  ( void )
   {
	EnumTaskWindows ( GetCurrentTask ( ), Iconify, 1 ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  TCheopsMenuWindow :: CmWindowIconifyLists -                           */
/*      Met toutes les fenêtres d'édition sous forme d'icône.             */
/*                                                                        */
/**************************************************************************/

void	TCheopsMenuWindow :: CmWindowIconifyLists  ( void )
   {
	EnumTaskWindows ( GetCurrentTask ( ), Iconify, 0 ) ;
    }





/**************************************************************************/
/*                                                                        */
/*   TCheopsMenuWindow :: CmYourChoice -                                  */
/*      Commande Unix à exécuter.                                         */
/*                                                                        */
/**************************************************************************/

void	TCheopsMenuWindow :: CmYourChoice  ( void )
   {
	char	Command [ MAX_PARAMETER_LENGTH ] ;

# if	defined ( LEBRAIN )
	char	Appel_Brain [ 100 ] ;
# endif


	* Command = 0 ;	// Nom du module
	EditControlGadget -> GetText ( Command + 1, sizeof ( Command ) - 1 ) ;
	LastOpenedModule = -1 ;
	EditControlGadget -> Clear ( ) ;


	if (  ! stricmp ( Command + 1, "arreter" )  ||
	      ! stricmp ( Command + 1, "arrêter" ) )
	   {
		EvClose ( ) ;
		return ;
	     }

//  Signalisation de l'appel au cerveau pour memoire
# if	defined ( LEBRAIN )
	DdeGlobal. Appel ( DDELINK_LEBRAIN ) -> Connect ( ) ;
	sprintf ( Appel_Brain, "Utilise %s", Command +1 ) ;
	DdeGlobal. Appel ( DDELINK_LEBRAIN ) -> Execute ( Appel_Brain ) ;
	DdeGlobal. Appel ( DDELINK_LEBRAIN ) -> Disconnect ( ) ;
# endif


// Envoyer l'évènement
	EventQueue -> Enqueue ( Application -> ApplicationId,
				ObjectId,
				Application -> ProcessId,
				WM_MENU_EVENT, MNF_EXECUTE,
				( LPARAM ) Command,
				strlen ( Command + 1 ) + 2 ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TCheopsMenuWindow :: CmEnterCommand -                                 */
/*      Positionne sur la fenêtre "commandes unix".                       */
/*                                                                        */
/**************************************************************************/

void	TCheopsMenuWindow :: CmEnterCommand  ( void )
   {
	EditControlGadget -> SetFocus ( ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  TCheopsMenuWindow :: CommandEnable -                                  */
/*      Gère l'état de la commande :                                      */
/*                                                                        */
/*      . "Lire le courrier"                                              */
/*                                                                        */
/**************************************************************************/

# define	FOUND		1234


static BOOL FAR PASCAL    AnyWindow  ( HWND  hwnd, LPARAM  p )
   {
	TWindow *	win    =  GetWindowPtr ( hwnd ) ;
	int *		param  =  ( int * ) p ;


	if  ( win )
	   {
		if  ( * param  &&
			typeid ( * win )  ==  typeid ( TCheopsDialog ) ) 
		   {
			* param = FOUND ;
			return ( FALSE ) ;
		     }
		else if  ( ! * param  &&
			typeid ( * win )  ==  typeid ( TEditWindow ) )
		    {
			* param = FOUND ;
			return  ( FALSE ) ;
		     }
	      }

	return  ( TRUE ) ;
     }

		    

void	TCheopsMenuWindow :: CommandEnable  ( TCommandEnabler &  ce )
   {
	int		anyscreens	=  1, 
			anylists 	=  0 ;


	switch  ( ce. Id )
	   {
		case	CM_FILE_READMAIL :
			ce. Enable ( MailAvailable ) ;
			break ;

		case	CM_WINDOW_ORDER_MENUS :
			ce. Enable ( FloatingMenus  !=  NULL ) ;
			break ;

		case	CM_WINDOW_ICONIFY_SCREENS :
			EnumTaskWindows ( GetCurrentTask ( ), AnyWindow,
						( LPARAM ) & anyscreens ) ;
			ce. Enable ( anyscreens  ==  FOUND ) ;
			break ;

		case	CM_WINDOW_ICONIFY_LISTS :
			EnumTaskWindows ( GetCurrentTask ( ), AnyWindow,
						( LPARAM ) & anylists ) ;
			ce. Enable ( anylists  ==  FOUND ) ;
			break ;

		case	CM_WINDOW_ICONIFY_ALL :
			EnumTaskWindows ( GetCurrentTask ( ), AnyWindow,
						( LPARAM ) & anyscreens ) ;
			EnumTaskWindows ( GetCurrentTask ( ), AnyWindow,
						( LPARAM ) & anylists ) ;
			ce. Enable ( anyscreens  ==  FOUND  ||  
				     anylists    ==  FOUND ) ;
			break ;
	    }
     }







/**************************************************************************/
/*                                                                        */
/*  TCheopsMenuWindow :: EvModuleCommand                                  */
/*      Gère l'appui sur les onglets.                                     */
/*                                                                        */
/**************************************************************************/


void	TCheopsMenuWindow :: EvModuleCommand ( uint  id  )
   {
	TCheopsMenu *		Menu  =  MenuTree -> GetMenu ( CheopsModules [id - Cheops_Module_Base]. ModuleName ) ;
	TGadget *		g     = MainMenuControlBar -> GadgetWithId ( id ) ;
	TRect			rect  = g -> GetBounds ( ) ;
	TPoint 			pos  ( rect. left, rect. top ) ;
	

	SetFocus ( ) ;
	LastOpenedModule = id - Cheops_Module_Base ;
	MainMenuControlBar -> ClientToScreen ( pos ) ;

// Affichage de la flèche
	TPoint		ArrowPos ( pos ) ;


	ScreenToClient ( ArrowPos ) ;
	ArrowWindow -> SetWindowPos ( 0, ArrowPos. x, 
				ArrowPos. y - ArrowWindow -> Attr. H, 0, 0,
					SWP_NOSIZE | SWP_NOZORDER ) ;
	ArrowWindow -> Show ( SW_SHOW ) ;

// Cadrer le menu sur la flèche
	pos. x += ArrowWindow -> Attr. W ;
	pos. y -= ArrowWindow -> Attr. H / 2 ;


	TCheopsGadgetMenuWindow *  GadgetMenuWindow = new  TCheopsGadgetMenuWindow 
								( pos, this, Menu,
									CheopsModules [id - Cheops_Module_Base]. ModuleName  ) ;
	GadgetMenuWindow -> SetStyle ( InitialMenuStyle ) ;
	GadgetMenuWindow -> SetSelectionStyle  ( InitialSelectionStyle ) ;
	GadgetMenuWindow -> SetPopupTimeout ( Configuration -> Terminal ( ). MenuAutoOpenDelay ) ;
	GadgetMenuWindow -> Create ( ) ;
    }



/**************************************************************************/
/*                                                                        */
/*  TCheopsMenuWindow :: SetUserInfo -                                    */
/*      Modifie les informations concernant l'utilisateur.                */
/*                                                                        */
/**************************************************************************/

TCheopsUserInfo &  TCheopsMenuWindow :: GetUserInfo ( void )
   {
	return  ( CheopsUserInfo ) ;
    }


TCheopsUserConfig &  TCheopsMenuWindow :: GetConfiguration ( void ) 
   {
	return ( CheopsUserConfig ) ;
    }


void	TCheopsMenuWindow :: SetUserInfo  ( TCheopsUserInfo &  		userinfo,
					    TCheopsUserConfig & 	config )
   {
	ifdeletea  ( CheopsUserInfo. TTYName ) ;
	CheopsUserInfo. TTYName = newstring ( userinfo. TTYName ) ;

	ifdeletea  ( CheopsUserInfo. UserName ) ;
	CheopsUserInfo. UserName = newstring ( userinfo. UserName ) ;

	ifdeletea ( CheopsUserConfig. SiteName ) ;
	CheopsUserConfig. SiteName = newstring ( config. SiteName ) ;

	ifdeletea ( CheopsUserConfig. PrinterName ) ;
	CheopsUserConfig. PrinterName = newstring ( config. PrinterName ) ;

	ifdeletea ( CheopsUserConfig. DescriptPath ) ;
	CheopsUserConfig. DescriptPath = newstring ( config. DescriptPath ) ;

	ifdeletea ( CheopsUserConfig. ProgramPath ) ;
	CheopsUserConfig. ProgramPath = newstring ( config. ProgramPath ) ;

	ifdeletea ( CheopsUserConfig. Protection ) ;
	CheopsUserConfig. Protection = newstring ( config. Protection ) ;

	ifdeletea ( CheopsUserConfig. DataPath ) ;
	CheopsUserConfig. DataPath = newstring ( config. DataPath ) ;

	CheopsUserConfig. Year  = config. Year ;
	CheopsUserConfig. Month = config. Month ;


// Tableau des noms d'imprimantes Unix
	for  ( int  i = 0 ; i < CheopsUserInfo. UnixPrinterCount ; i ++ )
		ifdeletea  ( CheopsUserInfo. UnixPrinters [i] ) ;
	ifdeletea  ( CheopsUserInfo. UnixPrinters ) ;

	CheopsUserInfo. UnixPrinters = new  char * [ userinfo. UnixPrinterCount ] ;

	for  ( i = 0 ; i < userinfo. UnixPrinterCount ; i ++ )
		CheopsUserInfo. UnixPrinters [i] = newstring ( userinfo. UnixPrinters [i] ) ;

	CheopsUserInfo. UnixPrinterCount = userinfo. UnixPrinterCount ;

	CheopsUserInfo. IsSuperUser = userinfo. IsSuperUser ;


	UpdateInformation ( ) ;
     }





/**************************************************************************/
/*                                                                        */
/*  TCheopsMenuWindow :: SetModules -                                     */
/*      Définit les modules de base de Cheops.                            */
/*                                                                        */
/**************************************************************************/

TCheopsModule *	  TCheopsMenuWindow :: GetModules ( int &   count )
   {
	count = CheopsModuleCount ;
	return (  CheopsModules ) ;
    }



void	TCheopsMenuWindow :: SetModules ( TCheopsModule *	mod,
					  int			count )
   {
	register int		i ;


	for  ( i = 0 ; i < CheopsModuleCount ; i ++ )
	   {
		ifdelete ( CheopsModules [i]. ModuleName ) ;
		ifdelete ( CheopsModules [i]. ModuleText ) ;
	    
		if  ( CheopsMenuGadgets [i] )
		   {
			MainMenuControlBar -> Remove ( * CheopsMenuGadgets [i] ) ;
			delete  CheopsMenuGadgets [i] ;
		    }
	    }
	ifdelete ( CheopsModules ) ;
	ifdelete ( CheopsMenuGadgets ) ;

	CheopsModuleCount =  count ;
	CheopsModules     =  new  TCheopsModule [count] ;
	CheopsMenuGadgets =  new  TCheopsTabButtonGadget * [count] ;

	for  ( i = 0 ; i < count ; i ++ )
	   {
		CheopsModules [i]. ModuleName  = newstring ( mod [i]. ModuleName ) ;
		CheopsModules [i]. ModuleText  = newstring ( mod [i]. ModuleText ) ;
		CheopsModules [i]. ModuleColor = mod [i]. ModuleColor ;
		CheopsMenuGadgets [i] = new  TCheopsTabButtonGadget (
			Cheops_Module_Base + i, 
				CheopsModules [i]. ModuleColor, true ) ;
		CheopsMenuGadgets [i] -> SetText ( mod [i]. ModuleText ) ;

		if  ( MainMenuControlBar ) 
			MainMenuControlBar -> Insert ( * CheopsMenuGadgets [i] ) ;	
	    }

	if  ( MainMenuControlBar )
		MainMenuControlBar -> LayoutSession ( ) ;
	UpdateInformation ( ) ;
      }




/**************************************************************************/
/*                                                                        */
/*   TCheopsMenuWindow :: EvKillFocus, EvSetFocus -                       */
/*      Gestion du focus pour la dernière sous-fenetre active.            */
/*                                                                        */
/**************************************************************************/

void	TCheopsMenuWindow :: EvKillFocus  ( HWND  gained )
   {
	TDecoratedMDIFrame :: EvKillFocus ( gained ) ;
     }



void	TCheopsMenuWindow :: EvSetFocus  ( HWND  lost )
   {
	TDecoratedMDIFrame :: EvSetFocus ( lost ) ;
     }



	
/**************************************************************************/
/*                                                                        */
/*  TCheopsMenuWindow :: EvInitMenuPopup -                                */
/*      Modifie le menu Modules afin de refléter les modules actuellement */
/*      affichés.                                                         */
/*                                                                        */
/**************************************************************************/

void	TCheopsMenuWindow :: EvInitMenuPopup  ( HMENU	menu,
						uint	index,
						BOOL	is_sys )
   {
	register int	i, count ;




// Si ça n'est pas le menu Cheops, ne rien faire
	if  ( is_sys  ||  GetMenuItemID ( menu, 0 )  !=  Cheops_Module_Base )
	   {
		TDecoratedFrame :: EvInitMenuPopup ( menu, index, is_sys ) ;
		return ;
	     }


// C'est le menu Cheops : Supprimer tous ses éléments
	count = GetMenuItemCount ( menu ) ;
	for  ( i = 0 ; i < count ; i ++ )
		DeleteMenu ( menu, 0, MF_BYPOSITION ) ;


// Puis ajouter les modules actuellement définis
	for  ( i = 0 ; i < CheopsModuleCount ; i ++ )
		AppendMenu ( menu, MF_STRING | MF_ENABLED,
			Cheops_Module_Base + i, CheopsModules [i]. ModuleText ) ;

	TDecoratedFrame :: EvInitMenuPopup ( menu, index, is_sys ) ;
      }



/**************************************************************************/
/*                                                                        */
/*   TCheopsMenuWindow :: UpdateInformation -                             */
/*      Remet à jour les informations utilisateur visibles à l'écran.     */
/*                                                                        */
/**************************************************************************/

void	TCheopsMenuWindow :: UpdateInformation  ( void )
   {
// Barre d'état
	if  ( StatusBar )
	   {
		char		Buffer [ MAX_PARAMETER_LENGTH ] ;


		wsprintf ( Buffer, "[Height:8]Mois :   [Height,Weight:900]%.2d", CheopsUserConfig. Month ) ;
		MonthTextGadget -> SetText ( Buffer ) ;

		wsprintf ( Buffer, "[Height:8]Exercice :   [Height,Weight:900]%.4d", CheopsUserConfig. Year ) ;
		YearTextGadget -> SetText ( Buffer ) ;

		wsprintf ( Buffer, "[Height:8]Imprimante :   [Height,Weight:900]%s", CheopsUserConfig. PrinterName ) ;
		PrinterTextGadget -> SetText ( Buffer ) ;

		wsprintf ( Buffer, "[Height:8]Utilisateur %s sur %s",
			CheopsUserInfo. UserName,
				CheopsUserInfo. TTYName ) ;
		UserTextGadget -> SetText ( Buffer ) ;

		if  ( ! IntroWindow ) 
			StatusBar -> Invalidate ( ) ;		
	     }
    }





/**************************************************************************/
/*                                                                        */
/*   TCheopsMenuWindow :: EvMenuDropped -                                 */
/*      Crée une TFloatingMenuWindow à partir du menu qui a été lâché.    */
/*      Attention : il est important de ne pas créer de fenêtre avant de  */
/*      clôner l'objet TGadgetMenuWindow, car dans ce cas, cette dernière */
/*      recevrait un kill focus, et elle se détruirait.                   */
/*      Clôner un objet en cours de destruction, ensuite, on peut         */
/*      facilement imaginer ce que ça va donner...                        */
/*                                                                        */
/**************************************************************************/

void	TCheopsMenuWindow :: EvMenuDropped ( TCheopsGadgetMenuWindow *  menu )
   {
	register TCheopsFloatingMenu *	fm    =  FloatingMenus ;
	register bool			found =  false ;
	register char *			name  =  menu -> GetMenu ( ). MenuName ;


	ArrowWindow -> Show ( SW_HIDE ) ;
	ArrowWindow -> UpdateWindow ( ) ;

	while  ( fm )
	   {
		if  ( ! strcmp ( name, fm -> MenuWindow -> GetMenu ( ). MenuName ) )
		   {
			found = true ;
			break ;
		    }


		fm = fm -> Next ;
	     }


	if  ( found )
		fm -> SetFocus ( ) ;
	else
		CreateFloatingMenu ( menu, menu -> GetDroppedPosition ( ) ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TCheopsMenuWindow :: EvMenuSelected -                                 */
/*      Réagit à la sélection d'un menu.                                  */
/*      Attention !                                                       */
/**************************************************************************/

void	TCheopsMenuWindow :: EvMenuSelected  ( int  		    	  gadgetid,
					       TCheopsGadgetMenuWindow *  menu )
   {
	char 			Buffer  [ MAX_PARAMETER_LENGTH * 2 ] ;

# if	defined ( LEBRAIN )
	char			Appel_Brain [ 100 ] ;		/* dedie a LeBrain */
	char *			pchar, Buffer2 [ 100 ] ;	/* dedie a LeBrain */
# endif

	TCheopsMenu &		SelectionMenu = menu -> GetMenu ( ) ;
	register int		Size ;


// Construire le bloc à envoyer pour exécution
	strcpy ( Buffer, menu -> GetModuleName ( ) ) ;
	Size = strlen ( Buffer ) + 1 ;

// Aide en ligne : mettre un '!' devant la commande
	if  ( GetKeyState ( VK_SHIFT )  &  0x8000 )
		Buffer [ Size ++ ] = '!' ;

	strcpy ( Buffer + Size, SelectionMenu. MenuItems [gadgetid]. ItemName ) ;
	Size += strlen ( Buffer + Size ) + 1 ;



// L'élément sélectionné est un menu : il faut afficher le sous-menu
	if  ( SelectionMenu. MenuItems [gadgetid]. ItemType  ==  MenuItem )
	   {
		TCheopsMenu *	SubMenu = MenuTree -> GetMenu ( SelectionMenu. MenuItems [gadgetid]. ItemName ) ;


	// Vérifier quand même que le sous-menu est défini
		if  ( SubMenu )
		   {
			TCheopsFloatingMenu *	fm = menu -> GetFloatingParent ( ) ;


		// si le menu est dans une floating frame, il faut en créer une autre
			if  ( fm )
			   {
				TRect				Rect  =  fm -> GetWindowRect ( ) ;
				TPoint				Pos ( Rect. left, Rect. top ) ;
				int				delta =  GetSystemMetrics ( SM_CYCAPTION ) ;
				TCheopsGadgetMenuWindow *       mw ;



				Pos. x += delta ;
				Pos. y += delta ;


				mw = new  TCheopsGadgetMenuWindow ( TPoint ( 0, 0 ),
						this, SubMenu, menu -> ModuleName ) ;

				mw -> SetStyle ( menu -> GetStyle ( ) ) ;
				mw -> SetSelectionStyle ( menu -> GetSelectionStyle ( ) ) ;
				CreateFloatingMenu ( mw, Pos ) ;
				delete  mw ;
			     }
		// Sinon, on se contente de créer un nouveau sous-menu
			else
			   {
				TCheopsGadgetMenuWindow *  GadgetMenuWindow = new  TCheopsGadgetMenuWindow
								( menu, gadgetid, SubMenu ) ;
				menu -> SetChildState ( FALSE ) ;
				menu -> Invalidate ( ) ;
				GadgetMenuWindow -> Create ( ) ;
			     }
		     }
		else
			:: MessageBox ( HWindow, "Erreur de permission", MB_OK,
				"Vous n'avez pas accès au sous-menu '%s'",
				SelectionMenu. MenuItems [gadgetid]. ItemText ) ;
	      }
// Cas d'un programme
	else if  ( SelectionMenu. MenuItems [gadgetid]. ItemType  ==  ProgramItem )
	   {
		register char *		p ;

	// Supprimer l'éventuelle checkmark d'un élément du menu, et en mettre
	// une sur l'élément exécuté (pour les floating menus seulement)
		CheopsConfiguration -> Set ( "Last Execution",
			SelectionMenu. MenuName,
				SelectionMenu. MenuItems [gadgetid]. ItemName ) ;

		register TCheopsFloatingMenu *	fm = FloatingMenus ;

		while  ( fm )
		   {
			p = fm -> MenuWindow -> GetMenu ( ). MenuName ;

			if  ( ! strcmp ( p, SelectionMenu. MenuName ) )
				fm -> UpdateCheckmark ( gadgetid ) ;

			fm = fm -> Next ;
		      }

	//  Signalisation de l'appel au cerveau pour memoire
# if	defined ( LEBRAIN )
		DdeGlobal. Appel ( DDELINK_LEBRAIN ) -> Connect ( TRUE ) ;
		sprintf ( Appel_Brain, "Utilise %s_exe", Buffer +strlen ( Buffer ) +1) ;
		DdeGlobal. Appel ( DDELINK_LEBRAIN ) -> Execute ( Appel_Brain ) ;
		strcpy ( Buffer2, menu -> GetMenuPath ( ) ) ;
		for ( pchar = Buffer2 + strlen ( Buffer2 ); pchar != Buffer2; pchar -- )
			if  ( * pchar  ==  ':' )
			   {
				sprintf ( Appel_Brain, "Utilise %s_menu", pchar +1 ) ;
				DdeGlobal. Appel ( DDELINK_LEBRAIN ) -> Execute ( Appel_Brain ) ;
				* pchar = 0 ;
			    }
		if  ( * pchar )
		   {
			sprintf ( Appel_Brain, "Utilise %s_menu", pchar ) ;
			DdeGlobal. Appel ( DDELINK_LEBRAIN ) -> Execute ( Appel_Brain ) ;
		    }

		DdeGlobal. Appel ( DDELINK_LEBRAIN ) -> Disconnect ( ) ;
# endif


	// Envoyer le machin
		EventQueue -> Enqueue ( Application -> ApplicationId,
					ObjectId,
					Application -> ProcessId,
					WM_MENU_EVENT, MNF_EXECUTE,
					( LPARAM ) Buffer, Size ) ;


	// Modifier la couleur du dernier module utilisé
		register int	i ;


		for  ( LastOpenedModule = -1, i = 0 ;
				i < CheopsModuleCount ; i ++ )
		   {
			if  ( ! stricmp ( CheopsModules [i]. ModuleName,
					menu -> ModuleName ) )
			   {
				LastOpenedModule = i ;
				break ;
			    }
		     }
	      }
    }





/**************************************************************************/
/*                                                                        */
/*  TCheopsMenuWindow :: EvMenuClosed -                                   */
/*      Réagit à la fermeture d'un menu en détruisant l'objet (qui n'est  */
/*      le fils de personne et ne sera plus utilisé par personne).        */
/*                                                                        */
/**************************************************************************/

void	TCheopsMenuWindow :: EvMenuClosed ( TCheopsGadgetMenuWindow *  menu )
   {
	if  ( menu -> IsMainMenu ( ) )
	   {
		ArrowWindow -> Show ( SW_HIDE ) ;
		ArrowWindow -> UpdateWindow ( ) ;
	     }


	register TCheopsFloatingMenu *	fm = menu -> GetFloatingParent ( ),
			       	     *	p  = FloatingMenus ;


	if  ( fm  ==  p )
		FloatingMenus = 0 ;
	else
	   {
		while  ( p  &&  p  !=  fm )
			p = p -> Next ;

		if  ( p )
		   {
			p -> Previous -> Next = p -> Next ;

			if  ( p -> Next )
				p -> Next -> Previous = p -> Previous ;
		    }
	      }	
    }




/**************************************************************************/
/*                                                                        */
/*  TCheopsMenuWindow :: EvTimer -                                        */
/*      Supprime la fenêtre d'introduction.                               */
/*                                                                        */
/**************************************************************************/

void	TCheopsMenuWindow :: EvTimer  ( uint  id )
   {
	switch  ( id )
	   {
		case	INTRO_TIMER_ID :
			KillTimer ( id ) ;

			if  ( IntroWindow )
			   {
				IntroWindow -> Show ( SW_HIDE ) ;
				delete  IntroWindow ;
				IntroWindow = 0 ;
			     }
			break ;


		case	CHECKMAIL_TIMER_ID :
# if 	! STANDALONE
			EventQueue -> Enqueue ( Application -> ApplicationId,
				ObjectId,
				Application -> ProcessId,
				WM_MENU_EVENT,
				MNF_CHECK_MAIL,
				0, 0 ) ;
# endif
			break ;
	     }
	

    }




/**************************************************************************/
/*                                                                        */
/*  TCheopsMenuWindow :: ProcessMenuFunction -                            */
/*      interprète les codes fonctions du menu Cheops et les exécute.     */
/*                                                                        */
/**************************************************************************/

char *	TCheopsMenuWindow :: ProcessMenuFunction  ( int		subfunction,
						    char *	data,
						    int &	Status,
						    int & 	Size )
   {
	static HCURSOR		SavedCursor = 0 ;
	char			Buffer     [ MAX_PARAMETER_LENGTH ] ;
	register FILE *		fp ;
	register char *		p ;
	register int		i, count ;
	UINT			pc ;
	TCheopsModule *		modules ;




	switch  ( subfunction )
	   {    

	// Début de l'introduction
		case	MNF_INTRO_BEGIN :
			if  ( ! ( GetKeyState ( VK_SHIFT )  &  0x8000 ) )
			   {
				SavedCursor = :: SetCursor ( :: LoadCursor ( NULL, IDC_WAIT ) ) ;
				Device -> HostToLocalCharSet ( data ) ;
				IntroWindow = new  TIntroWindow ( 0, data ) ;
				IntroWindow -> Create ( ) ;
			     }

			Status = UWS_OK ;
			Size   = 0 ;
			return ( 0 ) ;
				


	// Fin de l'introduction : tout est normalement récecptionné, on peut
	// donc créer l'objet
		case	MNF_INTRO_END :
			if  ( SavedCursor )
				:: SetCursor ( SavedCursor ) ;
			Create ( ) ;

			if  ( IntroWindow ) 
			   {
				IntroWindow -> SetText ( "Initialisation terminée." ) ;
				SetTimer ( INTRO_TIMER_ID, 1800 /* ms */ ) ;
				SetTimer ( CHECKMAIL_TIMER_ID, 30000 /* 30s */ ) ;
			     }

			Status = UWS_OK ;
			Size = 0 ;
			return ( 0 ) ;


	// Changer le texte de l'intro
		case	MNF_INTRO_SETTEXT :
			if  ( IntroWindow )
			   {
				Device -> HostToLocalCharSet ( data ) ;
				IntroWindow -> SetText ( data ) ;
			     }
			Status = UWS_OK ;
			Size = 0 ;
			return  ( 0 ) ;



	// Retourner le chemin de l'application
		case	MNF_GET_APPLICATION_PATH :
			GetModuleFileName ( * GetModule ( ), Buffer, sizeof ( Buffer ) ) ;
			i = strlen ( Buffer ) ;
			p = Buffer + i - 1 ;

			while  ( p  >=  Buffer  && * p  !=  '\\'  &&  * p  !=  '/' )
				p -- ;
			p ++ ;
			* p = 0 ;

			Size = strlen ( Buffer ) + 1 ;
			Status = UWS_OK ;
			return  ( newstring ( Buffer ) ) ;

							
	// Définir la configuration de l'utilisateur
		case	MNF_USER_CONFIG :
			getstr ( CheopsUserConfig. SiteName    , data ) ;
			geti16 ( CheopsUserConfig. Year        , data ) ;	
			getstr ( CheopsUserConfig. PrinterName , data ) ;
			getch  ( CheopsUserConfig. Month       , data ) ;
			getstr ( CheopsUserConfig. Protection  , data ) ;
			getstr ( CheopsUserConfig. ProgramPath , data ) ;
			getstr ( CheopsUserConfig. DataPath    , data ) ;
			getstr ( CheopsUserConfig. DescriptPath, data ) ;

			Size = 0 ;
			Status = UWS_OK ;
			return ( 0 ) ;


	// Les informations utilisateur
		case	MNF_USER_INFO :
			for  ( i = 0 ; i < CheopsUserInfo. UnixPrinterCount ; i ++ )
				ifdelete  ( CheopsUserInfo. UnixPrinters [i] ) ;
			ifdelete  ( CheopsUserInfo. UnixPrinters ) ;

			getstr ( CheopsUserInfo. UserName   , data ) ;
			getstr ( CheopsUserInfo. TTYName    , data ) ;
			getch  ( CheopsUserInfo. IsSuperUser, data ) ;
			geti16 ( CheopsUserInfo. UnixPrinterCount, data ) ;

			CheopsUserInfo. UnixPrinters = new char * [ CheopsUserInfo. UnixPrinterCount ] ;

			for  ( i = 0 ; i < CheopsUserInfo. UnixPrinterCount ; i ++ )
			   {
				CheopsUserInfo. UnixPrinters [i] = 0 ;
				getstr ( CheopsUserInfo. UnixPrinters [i], data ) ;
			    }
		
			Size = 0 ;
			Status = UWS_OK ;
			return ( 0 ) ;


	// Les modules Cheops 
		case	MNF_CHEOPS_MODULES :
			count   = * data ++ ;
			modules = new  TCheopsModule [count] ;

			for  ( i = 0 ; i < count ; i ++ )
			   {
				modules [i]. ModuleText = 
				modules [i]. ModuleName = 0 ;

				getstr ( modules [i]. ModuleName, data ) ;
				getstr ( modules [i]. ModuleText, data ) ;
				geti32 ( modules [i]. ModuleColor, data ) ;
			     }

			SetModules ( modules, count ) ;

			for  ( i = 0 ; i < count ; i ++ )
			   {
				ifdeletea ( modules [i]. ModuleName ) ;
				ifdeletea ( modules [i]. ModuleText ) ;
			     }
			delete []  modules ;

			Size = 0 ;
			Status = UWS_OK ;
			return ( 0 ) ;


	// L'arborescence des menus
		case	MNF_MENU_FILE :
			if  ( ( fp = fopen ( data, "r" ) )  ==  NULL )
				Status = UWS_EBADF ;
			else
			   {
				Status = UWS_OK ;
				BuildMenuTree ( fp ) ;
			     }

			Size = 0 ;
			return ( 0 ) ;


	// Le fichier .INI a été transféré
		case	MNF_INI_FILE :
			CheopsConfiguration -> SetFileName ( data ) ;
			Status = UWS_OK ;
			Size = 0 ;
			return ( 0 ) ;


	// Du courrier est disponible ou non
		case	MNF_MAIL_EVENT :
			i = MailAvailable ;
			MailAvailable = * data ;

			if  ( i  !=  MailAvailable )
				Invalidate ( ) ;
			Status = UWS_OK ;
			Size = 0 ;
			return  ( 0 ) ;

			
	// Du courrier
		case	MNF_MAIL_FILE :
			geti16 ( i, data ) ;

		// La valeur du premier mot détermine s'il s'agit d'un courrier
		// réceptionné (dans ce cas, la suite donne le nom du fichier
		// courrier sous msdos) ou envoyé (dans ce cas, la suite donne 
		// le nom du fichier temporaire qui avait été créé par 
		// SendMailDialog, et qu'on peut maintenant supprimer)
			if  ( i  ==  MNF_SEND_MAIL )
				unlink ( data ) ;
			else 
			   {
				if  ( ReadMailWindow )
					ReadMailWindow -> Activate ( ) ;	
				else
				   {
					wsprintf ( Buffer, "Courrier pour l'utilisateur %s",
						CheopsUserInfo. UserName ) ;
					ReadMailWindow = new  TCheopsReadMailDialog ( 
						this,
						CheopsMenuWindowClient,
						Buffer, data ) ;
					ReadMailWindow -> Create ( ) ;
				     }
			      }
			Status = UWS_OK ;
			Size = 0 ;
			return ( 0 ) ;


	// Ouvrir un menu
		case	MNF_OPEN_MENU :
			p = strchr ( data, ':' ) ;

			if  ( p )
			   {
				strncpy ( Buffer, data, ( int ) ( p - data ) ) ;
				Buffer [ ( int ) ( p - data ) ] = 0 ;

				for  ( i = 0 ; i < CheopsModuleCount ; i ++ )
				   {
					if  ( ! stricmp ( CheopsModules [i]. ModuleName,
							Buffer ) )
					   {
						CheopsConfiguration -> Set ( "Opened Menus",
							Buffer, data ) ;
						EvModuleCommand ( i + Cheops_Module_Base ) ;
						Status = UWS_OK ;
						Size   = 0 ;
						return ( 0 ) ;
					     }
				      }
			       }

			Status = UWS_EINVAL ;
			Size = 0 ;
			return ( 0 ) ;
				

	// Mode shell ou pas
		case	MNF_SET_SHELL_MODE :
			if  ( * data )
			   {
				TerminalWindow -> BringWindowToTop ( ) ;
				TerminalWindow -> Show ( SW_SHOW ) ;
			    }
			else
				TerminalWindow -> Show ( SW_HIDE ) ;
			Status =  UWS_OK ;
			Size = 0 ;
			return ( 0 ) ;
		


	// Resources système libres
		case	MNF_GET_FREE_RESOURCES :
			p  = new char [ sizeof ( INTEGER16 ) * 3 ] ;

			pc = GetFreeSystemResources ( GFSR_SYSTEMRESOURCES ) ;
			I16toCHAR ( pc, p ) ;

			pc = GetFreeSystemResources ( GFSR_GDIRESOURCES ) ;
			I16toCHAR ( pc, p + sizeof ( INTEGER16 ) ) ;

			pc = GetFreeSystemResources ( GFSR_USERRESOURCES ) ;
			I16toCHAR ( pc, p + ( 2 * sizeof ( INTEGER16 ) ) ) ;

			Status = UWS_OK ;
			Size = 3 * sizeof ( INTEGER16 ) ;
			return ( p ) ;


		default :
			Status = UWS_OK ;
			Size = 0 ;
			return ( 0 ) ;
	    }

     }






/**************************************************************************/
/*                                                                        */
/*   TCheopsMenuWindow :: CreateFloatingMenu -                            */
/*      Crée un menu flottant à la position spécifiée.                    */
/*                                                                        */
/**************************************************************************/

void	TCheopsMenuWindow :: CreateFloatingMenu  ( TCheopsGadgetMenuWindow *	menu,
						   TPoint &		        pos,
						   bool				minimized ) 
   {
	register TCheopsFloatingMenu *		fm,
				     *		p  =  FloatingMenus ;



	GetClientWindow ( ) -> ScreenToClient ( pos ) ;
	fm = new  TCheopsFloatingMenu ( CheopsMenuWindowClient, 
			new  TCheopsGadgetMenuWindow ( * menu ),
				pos ) ;

// Insérer le menu floattant dans la liste
	if  ( p )
	   {
		while  ( p -> Next )
			p = p -> Next ;
		p -> Next = fm ;
	     }
	else
		FloatingMenus = fm ;

	fm -> Next = 0 ;
	fm -> Previous = p ;

// Changer la police
	if  ( fm -> CaptionFont )
		delete  fm -> CaptionFont ;
	fm -> CaptionFont = new  TCheopsWindowFont ( 8 ) ;


// Créer la fenêtre
	fm -> Create ( ) ;

	if  ( minimized )
		fm -> HandleMessage ( WM_SYSCOMMAND, SC_MINIMIZE ) ;
	fm -> Show ( SW_SHOW ) ;
     }




/**************************************************************************/
/*                                                                        */
/*  TCheopsMenuWindow :: FloatingMenusAutoCreate -                        */
/*      Crée les menus flottants tels qu'ils avaient été sauvegardés.     */
/*                                                                        */
/**************************************************************************/

void	TCheopsMenuWindow :: FloatingMenusAutoCreate ( void )
   {
	char				SectionItems [ 1024 ],
					Buffer       [ MAX_PARAMETER_LENGTH ] ;
	TPoint				Pos ( 0, 0 ) ;
	register char *	        	p ;
	register char *			xp, 
		      *			yp 	=  0,
		      *			statep	=  0 ;
	TCheopsGadgetMenuWindow *	mw ;
	TCheopsMenu *			menu ;
	BOOL				minimized = FALSE ;



	CheopsConfiguration -> Get ( "Floating Menus", 0, SectionItems ) ;

// Styles des menus
	if  ( CheopsConfiguration )
	   {
		InitialMenuStyle	=  GetProfileStyle ( "Defaults", "MenuStyle" ) ;
		InitialSelectionStyle	=  GetProfileStyle ( "Defaults", "MenuSelectionStyle" ) ;	
	    }
	else
	   {
		InitialMenuStyle 	=  RecessedGadget ;
		InitialSelectionStyle 	=  RaisedGadget ;
	    }

 
	for  ( p = SectionItems ; * p ; p += strlen ( p ) + 1 )
	   {
		CheopsConfiguration -> Get ( "Floating Menus", p, Buffer ) ;


		xp = strchr ( Buffer, ',' ) ;


		if  ( xp )
		   {
			* xp = 0 ;
			yp   = strchr ( xp + 1, ',' ) ;

			if  ( yp )
			   {			  
				* yp = 0 ;

				statep = strchr ( yp + 1, ',' ) ;

				if  ( statep )
				   {
					* statep = 0 ;
					minimized = atoi ( statep + 1 ) ;
				    }

				Pos. y = atoi ( yp + 1 ) ;

			    }

			Pos. x = atoi ( xp + 1 ) ;
		     }

		menu = MenuTree -> GetMenu ( p ) ;

		if  ( menu )
		   {
			mw = new  TCheopsGadgetMenuWindow ( TPoint ( 0, 0 ),
					this, menu, Buffer ) ;

			mw -> SetStyle ( InitialMenuStyle ) ;
			mw -> SetSelectionStyle ( InitialSelectionStyle ) ;
			CreateFloatingMenu ( mw, Pos, minimized ) ;
			delete  mw ;
		     }
	      }
      }





/**************************************************************************/
/*                                                                        */
/*   TCheopsMenuWindow :: RemoveFloatingMenu -                            */
/*      Appelé par un menu flottant lorsqu'il doit être retiré de la      */
/*      liste.                                                            */
/*                                                                        */
/**************************************************************************/

void	TCheopsMenuWindow :: RemoveFloatingMenu  ( TCheopsFloatingMenu *  m )
   {
	if  ( ! m )
		return ;

	if  ( m  ==  FloatingMenus )
	   {
		FloatingMenus = FloatingMenus -> Next ;

		if  ( FloatingMenus )
			FloatingMenus -> Previous = 0 ;
	    }
	else
	   {
		if  ( m -> Next )
			m -> Next -> Previous = m -> Previous ;
		if  ( m -> Previous )
			m -> Previous -> Next = m -> Next ;
	     }
      }



/**************************************************************************/
/*                                                                        */
/*  TCheopsMenuWindow :: EvClose -                                        */
/*      Ferme proprement la fenêtre et envoie un évènement vers Unix.     */
/*                                                                        */
/**************************************************************************/

void 	TCheopsMenuWindow :: EvClose  ( void )
   {
	TDecoratedFrame :: EvClose ( ) ;
    }
	





/**************************************************************************/
/*                                                                        */
/*  TCheopsMenuwindow :: PreProcessMsg -                                  */
/*      Appelle le Idleaction de la barre d'état, car cette fonction      */
/*      n'est appelée que pour la fenêtre principale de l'application.    */
/*      Traite aussi l'icônification des fenêtres Cheops.                 */
/*                                                                        */
/**************************************************************************/

BOOL	TCheopsMenuWindow :: PreProcessMsg ( MSG &  msg )
   {
// Mise à jour volontaire des gadgets
	IdleAction ( 0 ) ;
	return  ( TDecoratedFrame :: PreProcessMsg ( msg ) ) ;
     }





/**************************************************************************/
/*                                                                        */
/*  TCheopsMenuWindow :: IconifyMe -                                      */
/*      Lorsqu'une fenêtre Cheops ou Editfile reçoit un WM_SYSCOMMAND     */
/*      avec SC_CLOSE, elle appelle cette fonction pour créer une         */
/*      fenêtre MDI sous forme d'icône, de manière à ce que ces icônes    */
/*      soient rangées dans la partie cliente du menu.                    */
/*                                                                        */
/**************************************************************************/

void	TCheopsMenuWindow :: IconifyMe ( TWindow *   w,
				         TResId &    icon )
   {
	if  ( ( w -> GetWindowLong ( GWL_STYLE )  &  WS_VISIBLE )  &&
		! w -> IsIconic ( ) )
	   {
		TCheopsIconicWindow *	IW ;

	
		IW = new  TCheopsIconicWindow ( GetClientWindow ( ),
				w, icon ) ;
		IW -> Create ( ) ;
	     }
    }






/**************************************************************************/
/**************************************************************************/
/********                                                          ********/
/********      Implémentation de la classe TCheopsMenuWindowClient ********/
/********                                                          ********/
/**************************************************************************/
/**************************************************************************/


DEFINE_RESPONSE_TABLE1 ( TCheopsMenuWindowClient, TMDIClient )
	EV_WM_VSCROLL,
	EV_WM_HSCROLL,
END_RESPONSE_TABLE ;



/**************************************************************************/
/*                                                                        */
/*  Constructeur -                                                        */
/*                                                                        */
/**************************************************************************/

TCheopsMenuWindowClient :: TCheopsMenuWindowClient  ( TCheopsMenuWindow *  parent )
		: TMDIClient ( ) 
   {
	CheopsMenuWindow = parent ;


// Créer la bitmap
	char			Path [ MAX_PARAMETER_LENGTH ] ;
	register char *		p ;


	MemDC		 = 0 ;
	MemDCBitmap	 = 0 ;


	TerminalApplication -> GetModuleFileName ( Path, sizeof ( Path ) ) ;
	p = Path + strlen ( Path ) - 1 ;

	while  ( p >= Path  &&  strchr ( "/:\\", * p )  ==  NULL )
		p -- ;
	p ++ ;
	strcpy ( p, MENU_BITMAP ) ;

	if  ( access ( Path, 0 )  >=  0 )
	   {
	// Note : le SetBkgndColor est TREEEEEEEEEEEEEEEEEEEEEEEEEEEEEEEES important !
	// 	. Il empêche l'effacement du fond de la fenêtre (donc de la bitmap)
	//	  avec WM_PAINT
	//	. Lorsqu'on déplace une fenêtre enfant, on ne voit pas, pendant une
	//	  fraction de seconde, de fond noir à sa place d'origine (fond noir dû
	//	  à un WM_ERASEBKGND suivi d'un WM_PAINT).
		SetBkgndColor ( NoErase ) ;


		BITMAPINFOHEADER *	bm ;
		register int		Width,
					Height ;
		TRect			ScreenRect ( 0, 0, 
						BitmapWidth  = GetSystemMetrics ( SM_CXSCREEN ),
						BitmapHeight = GetSystemMetrics ( SM_CYSCREEN ) ) ;
		TDib			Bitmap ( Path ) ;


		MemDC  = new  TMemoryDC ;


	// Calculer la taille du dessin
		bm     = Bitmap. GetInfoHeader ( ) ;
		Width  = ( int ) bm -> biWidth ;
		Height = ( int ) bm -> biHeight ;

	// Afficher le dessin dans un contexte mémoire
		MemDCBitmap = new  TBitmap ( TScreenDC ( ), BitmapWidth, BitmapHeight ) ;
		MemDC -> SelectObject ( * MemDCBitmap ) ;
		MemDC -> StretchDIBits ( ScreenRect, 
			TRect ( 0, 0, Width, Height ), Bitmap ) ;
	     }
	else
		SetBkgndColor ( GetSysColor ( COLOR_WINDOW ) ) ;
    }



/**************************************************************************/
/*                                                                        */
/*   Destructeur -                                                        */
/*                                                                        */
/**************************************************************************/

TCheopsMenuWindowClient :: ~TCheopsMenuWindowClient  ( )
  {
	if  ( MemDC )
		delete  MemDC ;

	if  ( MemDCBitmap )
		delete  MemDCBitmap ;
   }




/**************************************************************************/
/*                                                                        */
/*  TCheopsMenuWindowClient :: EvHScroll, EvVScroll -                     */
/*      Empêche la mise à jour de la fenêtre bitmap durant le scroll.     */
/*                                                                        */
/**************************************************************************/

void	TCheopsMenuWindowClient :: EvHScroll  ( uint  type, uint  thumbpos, HWND  ctl )
   {
	register BOOL	PreventScroll  =  ( type  !=  SB_ENDSCROLL     &&
					    type  !=  SB_THUMBTRACK ) ;


	if  ( PreventScroll )
		:: LockWindowUpdate ( HWindow ) ;

	TMDIClient :: EvHScroll ( type, thumbpos, ctl ) ;

	if  ( PreventScroll )
	   {
		Validate ( ) ;		// Pour empêcher de repeindre la partie scrollée
		:: LockWindowUpdate ( 0 ) ;
	    }
     }




void	TCheopsMenuWindowClient :: EvVScroll  ( uint  type, uint  thumbpos, HWND  ctl )
   {
	register BOOL	PreventScroll  =  ( type  !=  SB_ENDSCROLL     &&
					    type  !=  SB_THUMBTRACK ) ;


	if  ( PreventScroll )
		:: LockWindowUpdate ( HWindow ) ;

	TMDIClient :: EvVScroll ( type, thumbpos, ctl ) ;

	if  ( PreventScroll )
	   {
		Validate ( ) ;
		:: LockWindowUpdate ( 0 ) ;
	     }
     }






/**************************************************************************/
/*                                                                        */
/*  TCheopsMenuWindowClient :: Create -                                   */
/*      Crée la fenêtre bitmap.                                           */
/*                                                                        */
/**************************************************************************/

BOOL	TCheopsMenuWindowClient :: Create ( void )
   {
	register BOOL	Status = TMDIClient :: Create ( ) ;

	return ( Status ) ;
    }




/**************************************************************************/
/*                                                                        */
/*   TCheopsMenuWIndowClient :: Paint -                                   */
/*      Dessine le bitmap.                                                */
/*                                                                        */
/**************************************************************************/

void	TCheopsMenuWindowClient :: Paint  ( TDC  &  dc, BOOL, TRect &  rect )
   {
	TMDIClient :: Paint ( dc, FALSE, rect ) ;


	if  ( MemDC )
	   {
		TRect		UpdateRect ;


		if  ( ! GetUpdateRect ( UpdateRect, FALSE ) )
			UpdateRect = GetClientRect ( ) ;

		dc. BitBlt ( UpdateRect, * MemDC, 
			TPoint ( UpdateRect. left, UpdateRect. top ) ) ;
	      }
     }




	



/**************************************************************************/
/**************************************************************************/
/********                                                          ********/
/********    Implémentation de la classe TCheopsTabButtonGadget    ********/
/********                                                          ********/
/**************************************************************************/
/**************************************************************************/



/**************************************************************************/
/*                                                                        */
/*  Constructeur.                                                         */
/*                                                                        */
/**************************************************************************/

TCheopsTabButtonGadget :: TCheopsTabButtonGadget   ( int	id,
						     COLORREF   color,
						     bool	enabled,
						     int       	numchar ) 
	: SquareSize ( 0, 0 ),
	  Color ( color ),
	  TTabButtonGadget ( id, enabled, TRUE, numchar )
    { }





/**************************************************************************/
/*                                                                        */
/*  TCheopsTabButtonGadget :: GetDesiredSize -                            */
/*      Retourne la taille du gadget avec le petit grigri.                */
/*                                                                        */
/**************************************************************************/

void	TCheopsTabButtonGadget :: GetDesiredSize  ( TSize &  size )
   {
	register int		cxBorder	=  GetSystemMetrics ( SM_CXBORDER ),
				cyBorder	=  GetSystemMetrics ( SM_CYBORDER ) ;
	register int		x		=  GetSystemMetrics ( SM_CYCAPTION ) / 2,
				sz		=  x + cxBorder + cyBorder ;


	
	TTabButtonGadget :: GetDesiredSize  ( size ) ;
	SquareSize. cx = sz ;
	SquareSize. cy = sz ;
	size. cx += SquareSize. cx * 2 ; 

	if  ( size. cy  <  SquareSize. cy )
		size. cy = SquareSize. cy ;
     }





/**************************************************************************/
/*                                                                        */
/*  TCheopsTabButtonGadget :: PaintContents -                             */
/*      Dessine le contenu du bouton.                                     */
/*                                                                        */
/**************************************************************************/

void	TCheopsTabButtonGadget :: PaintContents  ( TDC &  dc, TRect &  rect )
   {
	int		cxBorder	=  GetSystemMetrics ( SM_CXBORDER ),
			cyBorder	=  GetSystemMetrics ( SM_CYBORDER ) ;


	dc. SaveDC ( ) ;


// Paramètrage du texte
	dc. SetBkMode ( TRANSPARENT ) ;
	dc. SetTextColor ( GetSysColor ( COLOR_BTNTEXT ) ) ;
	dc. SetBkColor   ( GetSysColor ( COLOR_BTNFACE ) ) ;

// Calcul de la position du texte
	int		sqx, sqy ;


	if  ( Color  ==  0xFFFFFFFFL )
		sqx = sqy = 0 ;
	else
	   {
		sqx = SquareSize. cx ;
		sqy = SquareSize. cy ;
	    }


	TSize		Extent 	=  dc. GetTextExtent ( Text, strlen ( Text ) ) ;
	int		deltax  =  rect. left + ( ( rect. Width ( ) - ( 2 * sqx ) - Extent. cx ) / 2 ),
			deltay  =  rect. top  + ( ( rect. Height ( ) - max ( sqy, Extent. cy ) ) / 2 ) ;


// Dessin du grigri
// Attention : les restorebrush, c'est pas du pipeau ! si on ne le fait pas,
// on perd des ressources !
	if  ( sqx )
	   {
		TBrush		BorderBrush ( GetSysColor ( COLOR_WINDOWFRAME ) ) ;
		TBrush		RectBrush   ( Color ) ;
		TRect		SquareRect ( deltax, 
					rect. top + ( ( rect. Height ( ) - SquareSize. cy ) / 2 ),
					0, 0 ) ;


		SquareRect. right  = SquareRect. left + SquareSize. cx ;
		SquareRect. bottom = SquareRect. top  + SquareSize. cy ;



	// Contour
		dc. FillRect ( SquareRect, BorderBrush ) ;
		SquareRect. left 	+= cxBorder ;
		SquareRect. top 	+= cyBorder ;
		SquareRect. right 	-= cxBorder ;
		SquareRect. bottom 	-= cyBorder ;

	// Le rectangle va maintenant décrire la partie colorée
		dc. FillRect ( SquareRect, RectBrush ) ;
	     }
  


// Ecrire le texte
	dc. TextOut ( deltax + ( 2 * sqx ), deltay,
			Text, strlen ( Text ) ) ;


	dc. RestoreDC ( ) ;
    }









/**************************************************************************/
/**************************************************************************/
/********                                                          ********/
/********  Implémentation de la classe TCheopsConfigurationDialog  ********/
/********                                                          ********/
/**************************************************************************/
/**************************************************************************/


DEFINE_RESPONSE_TABLE1 ( TCheopsConfigurationDialog, TDialog )
	EV_COMMAND ( IDOK, CmOk ),
END_RESPONSE_TABLE ;



/**************************************************************************/
/*                                                                        */
/*  Classe validateur pour le mois et l'exercice.                         */
/*                                                                        */
/**************************************************************************/

class 	TJunkRangeValidator 	:  public  TRangeValidator
   {
	private :
		TWindow *  	Parent ;
		char *		Message ;


	public :
		TJunkRangeValidator  ( TWindow *  parent, 
				       char *	  message,
				       long int   min,
				       long int   max )
			: TRangeValidator ( min, max ),
			  Parent ( parent ),
			  Message  ( message )
		   { }


		void	Error  ( void )
		   {
			:: MessageBox ( Parent -> GetLastActivePopup ( ), Message,
				"Erreur", MB_OK | MB_ICONEXCLAMATION ) ;
		    }
      } ;




/**************************************************************************/
/*                                                                        */
/*  Constructeur.                                                         */
/*                                                                        */
/**************************************************************************/

TCheopsConfigurationDialog :: TCheopsConfigurationDialog  ( TWindow *		parent,
							    TCheopsUserConfig & user_config,
							    TCheopsUserInfo &	user_info,
							    int			give_focus_to )
	: TDialog ( parent, ( user_info. IsSuperUser ) ?
			IDD_CHEOPS_SUCONFIG : IDD_CHEOPS_CONFIG ) 
   {
	UserInfo    = & user_info ;
	UserConfig  = & user_config ;
	GiveFocusTo = give_focus_to ;


// Création des contrôles
	YearControl     = new  TEdit ( this, IDC_YEAR ) ;
	MonthControl    = new  TEdit ( this, IDC_MONTH ) ;
	SiteNameControl = new  TEdit ( this, IDC_SITE_NAME ) ;
	PrinterControl  = new  TComboBox ( this, IDC_PRINTER ) ;

	if  ( user_info. IsSuperUser )
	   {
		ProtectionControl =  new  TEdit ( this, IDC_PROT ) ;
		ProgsControl      =  new  TEdit ( this, IDC_PROGS ) ;
		DescripControl    =  new  TEdit ( this, IDC_DESCRIP ) ;
		DataControl       =  new  TEdit ( this, IDC_DATA ) ;
	    }
    }




/**************************************************************************/
/*                                                                        */
/*  Destructeur.                                                          */
/*                                                                        */
/**************************************************************************/

TCheopsConfigurationDialog :: ~TCheopsConfigurationDialog  ( )
   {
	delete  YearControl ;
	delete  MonthControl ;
	delete  SiteNameControl ;
	delete  PrinterControl ;

	if  ( UserInfo -> IsSuperUser ) 
	   {
		delete  ProtectionControl ;
		delete  ProgsControl ;
		delete  DescripControl ;
		delete  DataControl ;
	    }
     }





/**************************************************************************/
/*                                                                        */
/*  TCheopsConfigurationDialog :: EvInitDialog -                          */
/*      Initialise les contrôles.                                         */
/*                                                                        */
/**************************************************************************/

BOOL	TCheopsConfigurationDialog :: EvInitDialog  ( HWND  focus )
   {
	char		Buffer [64] ;
	register BOOL	Status = TDialog :: EvInitDialog ( focus ) ;


// Exercice
	YearControl -> SetValidator  ( new  TJunkRangeValidator ( this,
			"Exercice de configuration incorrect", 0, 9999 ) ) ;
	wsprintf ( Buffer, "%d", UserConfig -> Year ) ;
	YearControl -> SetText ( Buffer ) ;

// Mois de config
	MonthControl -> SetValidator ( new  TJunkRangeValidator ( this,
			"Mois de configuration incorrect", 1, 99 ) ) ;
	wsprintf ( Buffer, "%d", UserConfig -> Month ) ;
	MonthControl -> SetText ( Buffer ) ;

// NOMSITE
	SiteNameControl -> SetText ( UserConfig -> SiteName ) ;

// Répertoires
	if  ( UserInfo -> IsSuperUser )
	   {
		ProtectionControl -> SetText ( UserConfig -> Protection ) ;
		ProgsControl      -> SetText ( UserConfig -> ProgramPath ) ;
		DataControl       -> SetText ( UserConfig -> DataPath ) ;
		DescripControl    -> SetText ( UserConfig -> DescriptPath ) ;
	    }


// Imprimante
	for  ( int  i = 0 ; i < UserInfo -> UnixPrinterCount ; i ++ )
		PrinterControl -> AddString ( UserInfo -> UnixPrinters [i] ) ;
	PrinterControl -> SetText ( UserConfig -> PrinterName ) ;


// Faut-il donner le focus à un contrôle spécifique ?
	if  ( GiveFocusTo  !=  -1 )
	   {
		:: SetFocus  ( GetDlgItem ( GiveFocusTo ) ) ;

		if  ( GiveFocusTo  !=  IDC_PRINTER )
			SendDlgItemMessage ( GiveFocusTo, EM_SETSEL, 0,
				MAKELPARAM ( 0, 0xFFFF ) ) ;
		Status = FALSE ;
	    }

	return  ( Status ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  TCheopsConfigurationDialog :: CmOk -                                  */
/*      Récupère les infos.                                               */
/*                                                                        */
/**************************************************************************/

void	TCheopsConfigurationDialog :: CmOk  ( void ) 
   {
	char		Buffer [ MAX_PARAMETER_LENGTH ] ;
	register int	i, found = 0 ;


// Imprimante
	PrinterControl -> GetText ( Buffer, sizeof ( Buffer ) ) ;

	for  ( i = 0 ; i < UserInfo -> UnixPrinterCount ; i ++ )
	   {
		if  ( ! strcmp ( Buffer, UserInfo -> UnixPrinters [i] ) )
		   {
			found = 1 ;
			break ;
		    }
	     }


	if  ( ! found ) 
	   {
		
		if  ( :: MessageBox ( HWindow, 
			"L'imprimante que vous avez donnée n'est pas définie "
			"sur votre système Cheops. "
			"Désirez-vous quand même conserver votre choix ?",
			"Erreur", MB_YESNOCANCEL | MB_ICONEXCLAMATION )  !=  IDYES )
			return ;
	     }


	ifdelete  ( UserConfig -> PrinterName ) ;
	UserConfig -> PrinterName = newstring ( Buffer ) ;


// Exercice
	YearControl -> GetText ( Buffer, sizeof ( Buffer ) ) ;
	i = atoi ( Buffer ) ;

	if  ( i < 100 )
	   {
		if  ( i < 50 )
			i += 2000 ;
		else
			i += 1900 ;
	    }

	UserConfig -> Year = i ;

	
// Mois
	MonthControl -> GetText ( Buffer, atoi ( Buffer ) ) ;
	UserConfig -> Month = atoi ( Buffer ) ;


// NOMSITE
	SiteNameControl -> GetText ( Buffer, sizeof ( Buffer ) ) ;
	ifdelete ( UserConfig -> SiteName ) ;
	UserConfig -> SiteName = newstring ( Buffer ) ;

// Répertoires
	if  ( UserInfo -> IsSuperUser )
	   {
		ProtectionControl -> GetText ( Buffer, sizeof ( Buffer ) ) ;
		ifdelete  ( UserConfig -> Protection ) ;
		UserConfig -> Protection = newstring ( Buffer ) ;

		ProgsControl -> GetText ( Buffer, sizeof ( Buffer ) ) ;
		ifdelete  ( UserConfig -> ProgramPath ) ;
		UserConfig -> ProgramPath = newstring ( Buffer ) ;

		DataControl -> GetText ( Buffer, sizeof ( Buffer ) ) ;
		ifdelete  ( UserConfig -> DataPath ) ;
		UserConfig -> DataPath = newstring ( Buffer ) ;

		DescripControl -> GetText ( Buffer, sizeof ( Buffer ) ) ;
		ifdelete  ( UserConfig -> DescriptPath ) ;
		UserConfig -> DescriptPath = newstring ( Buffer ) ;
	      }
	TDialog :: CmOk ( ) ;
    }






