/**************************************************************************/
/*                                                                        */
/*  Cheops.cpp  -  Christian Vigh, 94/11/16.                              */
/*                                                                        */
/*  Implémentation des classes écran de saisie Cheops.                    */
/*                                                                        */
/**************************************************************************/
/**************************************************************************

   MODIFICATIONS :

   29/04/97 (CV)
	. Prise en charge du paramètre HardCopyMode de la section [Settings]
	  du fichier .INI afin de redéfinir le cas échéant la couleur des
	  onglets (Saxtab controls).

   02/07/97 (CV)
	. Les appels à EnableWindow (sur les controles) ont été remplacés par
	  EnableTextItem, afin de passer les champs en read-only plutot qu'en
	  disabled.

   10/12/97 (CV)
	. Modification de TWinLine :: AfficherChoice pour effectuer le
	  MoveWindow avant le MovePopup (la fenetre popup ne pouvait pas
	  se cadrer correctement sur le bord de l'écran car elle n'avait pas
	  sa position définitive rendue effective par le MoveWindow).
	. Correction de la position d'ouverture d'une listbox : n'étaient pris
	  en compte ni l'espace entre le haut de la fenetre et la 1e ligne
	  ( 4 * GetSystemMetrics ( SM_CYBORDER ) ), ni l'espace entre deux
	  lignes ( INTERLIGNE ).

 **************************************************************************/

# include	"AllTerm.h"
# pragma	hdrstop


# include	"UnixApp.h"
# include	"Event.h"
# include	"Icon.h"
# include	"UnixDlg.h"
# include	"ChopJet.h"
# include	"Cheops.h"
# include	"MnCommon.h"
# include	"ChopCfg.h"
# include	"ChopTree.h"
# include	"ChopMenu.h"
# include	"ChopUp.h"


# define	WIDTH_DIVISOR		5
# define	WIDTH_DIVISOR2		4

# define	ID_ONGLET		101

# define	DATA(name,type)		 ( ( ( type * )  Message -> Data ) -> name )


DEFINE_RESPONSE_TABLE1 ( TCheopsFunctionKeys, TWindow )
	EV_COMMAND ( START_ID + 0, FKEY0 ),
	EV_COMMAND ( START_ID + 1, FKEY1 ),
	EV_COMMAND ( START_ID + 2, FKEY2 ),
	EV_COMMAND ( START_ID + 3, FKEY3 ),
	EV_COMMAND ( START_ID + 4, FKEY4 ),
	EV_COMMAND ( START_ID + 5, FKEY5 ),
	EV_COMMAND ( START_ID + 6, FKEY6 ),
	EV_COMMAND ( START_ID + 7, FKEY7 ),
END_RESPONSE_TABLE ;


DEFINE_RESPONSE_TABLE1 ( TCheopsDialog, TDecoratedFrame )
	EV_MESSAGE ( WM_FUNCTIONKEY, DoCommand ),
	EV_MESSAGE ( WM_NCACTIVATE, NcActivate ),
	EV_WM_TIMER,
	EV_WM_MENUSELECT,
	EV_WM_GETMINMAXINFO,
	EV_WM_SYSCOMMAND,
	EV_WM_NCLBUTTONDBLCLK,
	EV_WM_SIZE,
	EV_WM_MOVE,
	EV_WM_ENTERIDLE,
	EV_WM_CLOSE,
	EV_WM_SETFOCUS,
	EV_WM_KILLFOCUS,
END_RESPONSE_TABLE ;


DEFINE_RESPONSE_TABLE1 (TWinLine , TWindow )
	EV_WM_VSCROLL,
	EV_WM_HSCROLL,
	EV_WM_CLOSE,
END_RESPONSE_TABLE;

DEFINE_RESPONSE_TABLE1 (TListPat, TEnhancedListBox )
	EV_WM_KILLFOCUS,
	EV_WM_CHAR,
	EV_WM_LBUTTONDBLCLK,
	EV_WM_LBUTTONDOWN,
END_RESPONSE_TABLE ;

DEFINE_RESPONSE_TABLE2 ( TOnglet, TWindow, TVbxEventHandler )
	EV_VBXEVENTNAME ( ID_ONGLET, "Click", EvClick ),
END_RESPONSE_TABLE ;



/**************************************************************************/
/*                                                                        */
/*  Classe TCheopsButton :                                                */
/*      Se charge de conserver le focus sur le champ actuel de la boîte   */
/*      de dialogue.                                                      */
/*                                                                        */
/**************************************************************************/

class	TCheopsButton 	: public  TButton
	{
	public :
		HWND	LastFocus ;

		TCheopsButton 		( TWindow *	parent,
					  int		id,
					  char *	text,
					  int		x,
					  int		y,
					  int		cx,
					  int		cy )
			: TButton ( parent, id, text, x, y, cx, cy, FALSE ) {}


		void	EvSetFocus  ( HWND  lost )
		   {
			LastFocus = lost ;
			TButton :: EvSetFocus ( lost ) ;
		    }


	DECLARE_RESPONSE_TABLE ( TCheopsButton ) ;
    } ;



DEFINE_RESPONSE_TABLE1  ( TCheopsButton, TButton )
	EV_WM_SETFOCUS,
END_RESPONSE_TABLE ;




/**************************************************************************/
/*                                                                        */
/*   TCheopsFunctionKeys :: CmCommand, PreProcessMsg :                    */
/*      Relaye les clicks sur un bouton vers la boîte de dialogue.        */
/*                                                                        */
/**************************************************************************/

void	TCheopsFunctionKeys :: CmCommand ( WPARAM  id )
	{
	if  ( id  <=  7 )
	   {
		Buttons [id] -> HandleMessage ( BM_SETSTATE, FALSE, 0L ) ;
		Buttons [id] -> HandleMessage ( BM_SETSTYLE, BS_PUSHBUTTON, 0L ) ;
		DecoratedFrame -> HandleMessage ( WM_FUNCTIONKEY, id,
				MAKELONG ( Buttons [id] -> HWindow, BN_CLICKED ) ) ;

		if  ( Buttons [id] -> LastFocus )
			:: SetFocus ( Buttons [id] -> LastFocus ) ;
	    }
     }



BOOL  TCheopsFunctionKeys :: PreProcessMsg  ( MSG &  msg )
   {
	if  ( ( msg. message  ==  WM_KEYDOWN || msg. message  ==  WM_SYSKEYDOWN )
		 &&  msg. wParam  >=  VK_F1  &&  msg. wParam  <=  VK_F8 )
		{
		CmCommand ( msg. wParam - VK_F1 ) ;
		return ( TRUE ) ;
	    }
	return ( TWindow :: PreProcessMsg ( msg ) ) ;
    }



/**************************************************************************/
/*                                                                        */
/*   TCheopsFunctionKeys :: TCheopsFunctionKeys                           */
/*      Constructeur de la classe.                                        */
/*                                                                        */
/**************************************************************************/

TCheopsFunctionKeys :: TCheopsFunctionKeys  ( TWindow *		frame,
					      TWindow *  	parent,
					      int	 	width,
					      TControlStyle	style )
	: TWindow ( parent )
	{
	register int	i ;

	for  ( i = 0 ; i < 8 ; i ++ )
		Buttons [i] = NULL ;

	ComputeButtonsSize ( width ) ;
	DecoratedFrame  = frame ;
	Attr. W 	= width ;
	Attr. H		= ( ButtonHeight * 2 ) + ( 3 * ButtonVInterval ) ;
	Attr. Style     = WS_CHILD ;
	Attr. Style    &= ~ ( WS_VISIBLE ) ;
	ControlStyle	= style ;
    }





/**************************************************************************/
/*                                                                        */
/*   TCheopsFunctionKeys :: SetupWindow                                   */
/*      Crée les boutons touches de fonction et les affiche.              */
/*                                                                        */
/**************************************************************************/

void	TCheopsFunctionKeys :: SetupWindow  ( void )
   {
	int			i ;
	HWND			client ;
	char			buffer [ 32 ] ;


	TWindow :: SetupWindow ( ) ;

	switch ( ControlStyle )
	   {
		case	Borland :
			client = :: CreateWindow ( "BorShade", "",
					BSS_RGROUP | BSS_LEFT | WS_CHILD,
					0, 0, Attr. W, Attr. H, HWindow,
					NULL,
					* GetApplicationObject ( ), NULL ) ;
			break ;

		case	Microsoft3D :
		case	Microsoft :
		default :
			client = :: CreateWindow ( "BUTTON", "",
					WS_CHILD | BS_GROUPBOX,
					0, 0, Attr. W, Attr. H, HWindow,
					NULL,
					* GetApplicationObject ( ), NULL ) ;
	     }

	TheClient = new  TWindow ( client ) ;

	for  ( i = 0 ; i < MAX_CHEOPS_FUNCTION_KEYS ; i ++ )
	   {
		sprintf ( buffer, "F%d", i + 1 ) ;
		Buttons [i] = new  TCheopsButton ( ( TWindow * ) this,
				START_ID + i, buffer,
					0, 0, ButtonWidth, ButtonHeight ) ;
		Buttons [i] -> Create ( ) ;
	     }

	ArrangeButtons ( Attr. W ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TCheopsFunctionKeys :: ComputeButtonsSize                             */
/*      Calcule la taille des 8 touches de fonction (ainsi que de leur    */
/*      espacement horizontal et vertical) en respectant les règles       */
/*      suivantes :                                                       */
/*                                                                        */
/*      . 4 boutons par ligne                                             */
/*      . La hauteur est égale à la largeur / WIDTH_DIVISOR.              */
/*                                                                        */
/**************************************************************************/

void	TCheopsFunctionKeys :: ComputeButtonsSize ( int  width )
	{
	int		w, h, vi, hi ;

	w  = ( width * WIDTH_DIVISOR2 ) / ( ( 4 * WIDTH_DIVISOR2 ) + 5 ) ;
	hi = ( width - ( w * 4 ) ) / 5 ;
	h  = w / ( WIDTH_DIVISOR - 1 ) ;
	vi = h / ( WIDTH_DIVISOR - 1 ) ;

	ButtonWidth  	= w ;
	ButtonHeight 	= h ;
	ButtonVInterval	= vi ;
	ButtonHInterval = hi ;
     }




/**************************************************************************/
/*                                                                        */
/*  TCheopsFunctionKeys :: ComputeButtonPosition                          */
/*      Calcule l'origine du bouton numéro i (de 0 à 7).                  */
/*                                                                        */
/**************************************************************************/

void	TCheopsFunctionKeys :: ComputeButtonPosition  ( int	i,
							int &   x,
							int &   y )
   {
	register int	count 	=  ( i / 4 ),
			j	=  ( i % 4 ) ;

	x  =  ( ( j + 1 ) * ButtonHInterval ) + ( j * ButtonWidth ) ;
	y  =  ( ( count + 1 ) * ButtonVInterval ) + ( count * ButtonHeight ) ;
    }


/**************************************************************************/
/*                                                                        */
/*  TCheopsFunctionKeys :: SetLabel                                       */
/*      Change le label d'une touche de fonction.                         */
/*                                                                        */
/**************************************************************************/

void    TCheopsFunctionKeys :: SetLabel ( int  keynum, char *  label, char *  help )
   {
	if  ( keynum >= 1  &&  keynum <= 8 )
	   {
		Buttons [ keynum - 1 ] -> SetWindowText ( label ) ;
		Help [ keynum - 1 ] =  help ;
	    }
    }




/**************************************************************************/
/*                                                                        */
/*  TCheopsFunctionKeys :: ArrangeButtons                                 */
/*      Réarrange les touches de fonction afin de tenir compte de la      */
/*      largeur spécifiée.                                                */
/*                                                                        */
/**************************************************************************/

void	TCheopsFunctionKeys :: ArrangeButtons  ( int  newwidth )
   {
	int		i, x, y ;
	TRect		FrameRect ;


	ComputeButtonPosition ( MAX_CHEOPS_FUNCTION_KEYS, x, y ) ;
	ComputeButtonsSize    ( newwidth ) ;


	for  ( i = 0 ; i < MAX_CHEOPS_FUNCTION_KEYS ; i ++ )
		{
		if  ( Buttons [i]  ==  NULL )
			continue ;

		ComputeButtonPosition ( i, x, y ) ;
		Buttons [i] -> SetWindowPos ( 0,
			x, y, ButtonWidth, ButtonHeight,
			   SWP_NOZORDER ) ;
		Buttons [i] -> Invalidate ( ) ;
		Buttons [i] -> UpdateWindow ( ) ;
	     }

	y += ButtonVInterval + ButtonHeight ;
	Attr. H = y ;
    }





/**************************************************************************/
/*                                                                        */
/*  TCheopsDialog :: TCheopsDialog                                        */
/*      Constructeur de la classe.                                        */
/*                                                                        */
/**************************************************************************/

TCheopsDialog :: TCheopsDialog  ( TUnixApplication *    app,
				  TWindow *		parent,
				  TUnixMessage *	message,
				  BOOL			is_modal )
		: ControlBar ( 0 ), StatusBar ( 0 ),
		  TDecoratedFrame ( parent, "",
			Support = new  TCheopsSupport ( this, message, app ),
				TRUE )
   {
	DWORD		UnusedStyle ;

// Récupération des paramètres
	Application  = app ;
	Message	     = message ;
	menu	     = NULL ;

	UnusedStyle   = Support->DialogBox[0] -> GetUnusedStyle ( ) ;
	UnusedStyle  &= ~( DS_MODALFRAME ) ;

	Attr. Style  |= UnusedStyle | WS_CLIPCHILDREN ;
	Attr. Style  &= ~( WS_VISIBLE ) ;

	HasBeenDisplayed = FALSE ;

	OldFunctionKeys = Configuration -> Terminal ( ). OldFunctionKeys ;
	HelpId = -1 ;
	IsModal = is_modal ;
	UserShutdown = FALSE ;

// Enlever les possibilités d'iconifier la fenetre si elle est modale ; en effet,
// lorsqu'une telle fenetre est iconifiée, elle est dévalidée mais Windows ne
// valide apparemment aucune autre fenetre -> la seule solution est alors de
// terminer l'application
	if  ( IsModal )
	   	Attr. Style  &=  ~ ( WS_MINIMIZEBOX | WS_MAXIMIZEBOX ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  Destructeur de la classe TCheopsDialog.                               */
/*                                                                        */
/**************************************************************************/

TCheopsDialog :: ~TCheopsDialog ( )
   {
	KillTimer (1) ;

	ApplicationTable -> RemoveObject ( Application -> ApplicationId,
		this ) ;

	if  ( menu )
		delete menu ;

	if  ( HelpId  !=  -1 )
		Help -> RemoveHelp ( HelpId ) ;

	if  ( StatusBar )
		delete  StatusBar ;

	if  ( ControlBar )
		delete  ControlBar ;

	if  ( FilletBar )
		delete  FilletBar ;

	RestoreFocusToChild ( ( TheCheopsMenuWindowObject ) ?
		TheCheopsMenuWindowObject -> HWindow : TerminalWindow -> HWindow,
			HWindow ) ;
    }



/**************************************************************************/
/*                                                                        */
/*   TCheopsDialog :: ProcessModalState                                   */
/*      Invalide les fenetres parentes de celle-ci afin de simuler le     */
/*      caractère modal d'une fenetre.                                    */
/*                                                                        */
/**************************************************************************/

void	TCheopsDialog :: ProcessModalState ( bool   newstate )
   {
	register TWindow *	ParentWindow	=  Parent ;


	while  ( ParentWindow  &&
			ParentWindow -> GetWindowTask ( )  ==  GetCurrentTask ( ) )
	   {
		ParentWindow -> EnableWindow ( newstate ) ;
		ParentWindow = ParentWindow -> Parent ;
	     }
     }



/**************************************************************************/
/*                                                                        */
/*   TCheopsDialog :: SetupWindow                                         */
/*      Crée l'écran de saisie Cheops, les touches de fonctions, et       */
/*      ajuste la fenêtre à une taille convenable.                        */
/*                                                                        */
/**************************************************************************/

void	TCheopsDialog :: SetupWindow ( void )
   {
	TDecoratedFrame :: SetupWindow ( ) ;
    }



BOOL	TCheopsDialog :: Create ( void )
   {
	BOOL		Status ;
	DIALOGBOX	Template ;


// Création de la barre d'état
	long int	HintColor ;
	CheopsConfiguration -> GetClassItem ( this, "HintColor", HintColor,
					GetSysColor ( COLOR_BTNFACE ) ) ;


	StatusBar = new TColoredStatusBar ( this, HintColor, TGadget :: Recessed ) ;
	Insert ( * StatusBar, TDecoratedFrame :: Bottom ) ;
	StatusBar -> SetHintMode ( TStatusBar :: NoHints ) ;


// Création éventuelle de la barre d'icônes
// (on insère une bitmap vide de manière à ce que la barre ait la bonne 
// hauteur avant son affichage)
	if  ( ! OldFunctionKeys )
	   {
		ControlBar = new TCheopsControlBar ( this ) ;
		Insert ( * ControlBar, TDecoratedFrame :: Top ) ;
	     }


// Insertion du liséré
	COLORREF	Color = ( TheCheopsMenuWindowObject ) ?
					TheCheopsMenuWindowObject -> GetLastModuleColor ( ) :
					GetSysColor ( COLOR_BTNFACE ) ;


	FilletBar = new  TFilletBar ( this, TFilletBar :: Horizontal, Color ) ;
	Insert ( * FilletBar, TDecoratedFrame :: Top ) ;


// Créer la fenêtre
	SetIcon ( GetModule ( ), TResId ( "ICON_SCREEN" ) ) ;
	Status    = TDecoratedFrame :: Create ( ) ;

// Créer la fenêtre cliente
	Support->DialogBox[0] -> GetDialogBoxTemplate ( Template ) ;
	Beautify ( Template. DialogTemplate. dtCaptionText ) ;
	SetCaption ( Template. DialogTemplate. dtCaptionText ) ;
	SetClientSize ( MaxWidth, MaxHeight ) ;


// Gestion des fenetres modales : remonter la chaine des parents pour
// invalider leurs fenetres
	if  ( IsModal )
		ProcessModalState ( false ) ;

	return ( Status ) ;
	 }




/**************************************************************************/
/*                                                                        */
/*  TCheopsDialog :: EvSetFocus, EvKillFocus, EvActivate, HandleFocus     */
/*       Gestion du focus sur les controles.                              */
/*                                                                        */
/**************************************************************************/

void	TCheopsDialog :: EvKillFocus  ( HWND  gained )
   {
	TDecoratedFrame :: EvKillFocus ( gained ) ;
     }



void	TCheopsDialog :: EvSetFocus  ( HWND  lost )
   {
	TDecoratedFrame :: EvSetFocus  ( lost ) ;

// Récupérer la boite de dialogue active
	TUnixDialogBox *	Box 	=  0 ;


	if  ( Support -> Onglet )
		Box = Support -> DialogBox [ Support -> Onglet -> actif ( ) ] ;
	else
		Box = Support -> DialogBox [0] ;


// Redonner le focus au dernier champ qui l'avait
// (la partie lignes se charge aussi de conserver dans le champ LastFocusHwnd
// le HWND du dernier champ lignes ayant eu le focus)
	if  ( Box  &&  Box -> LastFocusHwnd  && 
			( :: GetWindowLong ( Box -> LastFocusHwnd, GWL_STYLE ) & WS_DISABLED )  ==  0 )
		:: SetFocus ( Box -> LastFocusHwnd ) ;
     }



/**************************************************************************/
/*                                                                        */
/*  TCheopsDialog :: CanClose -                                           */
/*      Revalide les fenetres parentes si la boite était modale.          */
/*      Redonne le focus à une fenetre d'edition ou un écran de saisie    */
/*      s'il en reste.                                                    */
/*                                                                        */
/**************************************************************************/

bool	TCheopsDialog :: CanClose ( void )
   {
	if  ( IsModal )
		ProcessModalState ( true ) ;
	return ( TRUE ) ;
     }




/**************************************************************************/
/*                                                                        */
/*   TCheopsDialog :: SetClientSize -                                     */
/*      Modifie la taille de la fenêtre cliente et ajuste la decorated    */
/*      frame en fonction.                                                */
/*                                                                        */
/**************************************************************************/

void	TCheopsDialog :: GetDecorationSize  ( int &  width, int &  height )
   {
	TRect		Zero ( 0, 0, 0, 0 ) ;
	TRect		StatusBarRect 	=  ( StatusBar  &&  StatusBar -> HWindow ) ?
						StatusBar  -> GetWindowRect ( ) :
						Zero,
			ControlBarRect  =  ( ControlBar &&  ControlBar -> HWindow ) ?
						ControlBar -> GetWindowRect ( ) :
						Zero,
			FilletRect   	=  ( FilletBar  &&  FilletBar -> HWindow ) ? 
						FilletBar  -> GetWindowRect ( ) :
						Zero ;


// Calculer la largeur de la fenêtre
	width	=  ( 2 * GetSystemMetrics ( SM_CXFRAME ) ) ;


// Puis la hauteur des décorations
	height =  StatusBarRect . Height ( ) - 1  +
		  ControlBarRect. Height ( ) - 1  +
		  FilletRect    . Height ( ) - 1 ;

// ... et des bordures ...
	height += ( 2 * GetSystemMetrics ( SM_CYFRAME ) ) +
		  GetSystemMetrics ( SM_CYCAPTION ) ;

	height -- ;  // je ne sais pas pourquoi il faut ajuster d'un pixel

// Et éventuellement du menu
	if  ( GetMenu ( )  !=  NULL )
		height += GetSystemMetrics ( SM_CYMENU ) ;
    }

		  
		   
		   


void	TCheopsDialog :: SetClientSize  ( int  width, int  height ) 
   {
	int	w, h ;


	GetDecorationSize ( w, h ) ;
	SetWindowPos ( NULL, 0, 0, w + width, h + height, 
				SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  TCheopsDialog :: DoSelectMenu, DoSelect                               */
/*      Gestion de la selection dans un menu.                             */
/*                                                                        */
/**************************************************************************/

void		TCheopsDialog :: EvMenuSelect  ( uint menuItemId, uint flags, HMENU /*hMenu*/ )
	{
		int	i ;

		if  ( menu  &&  menuItemId  &&  ( flags & MF_POPUP ) )
		   {
			i = menu -> GetMenuItemCount ( ) ;
			for  ( ; i  >=  0 ; i -- )
				if  ( menuItemId  ==  ( uint ) menu -> GetSubMenu ( i ) )
					break ;
			menu_id = i ;
			 }
	 }


LRESULT		TCheopsDialog :: EvCommand  ( uint id, HWND hWndCtl, uint notifyCode )
	{
		if  ( hWndCtl  ==  0  &&  notifyCode  ==  0 )
		   {
			EventQueue -> Enqueue ( Application -> ApplicationId,
					ObjectId,
					Application -> ProcessId,
					WM_MENUSELECT,
					0,
					menu_id,
					UWC_LPARAM_NOT_POINTER ) ;
			EventQueue -> Enqueue ( Application -> ApplicationId,
					ObjectId,
					Application -> ProcessId,
					WM_MENUSELECT,
					1,
					- id,
					UWC_LPARAM_NOT_POINTER ) ;
		    }

		return ( 0L ) ;
	 }


/**************************************************************************/
/*                                                                        */
/*  TCheopsDialog :: EvKeydown, PreProcessMsg                             */
/*      Gestion de l'appui sur une touche de fonction.                    */
/*                                                                        */
/**************************************************************************/

LRESULT		TCheopsDialog :: DoCommand  ( WPARAM  key, LPARAM  /* param */ )
   {
	WORD		KeyState 	=  ( WORD ) :: GetKeyState ( VK_MENU ) ;


	if  ( ( KeyState  &  0x8000 )  ==  0 )
	   {
		EventQueue -> Enqueue ( Application -> ApplicationId,
					ObjectId,
					Application -> ProcessId,
					WM_CHAR,
					VK_F1 + key,
					0,
					UWC_LPARAM_NOT_POINTER ) ;

		return ( 1L ) ;
	    }
	else
		return ( 0L ) ;
    }




BOOL  TCheopsDialog :: PreProcessMsg  ( MSG &  msg )
   {
	switch  ( msg. message )
	  {
		case	WM_KEYDOWN :
		case	WM_SYSKEYDOWN :
			if   (  msg. wParam  >=  VK_F1  &&  msg. wParam  <=  VK_F12 )
			  {
				if  ( DoCommand ( msg. wParam - VK_F1, 0xFFFFFFFFL ) )
					return ( TRUE ) ;
			   }
			break ;
	    }

// Ca, c'est pour que les messages de la statusbar soient mis à jour 
	IdleAction ( 0 ) ;

	return ( TDecoratedFrame :: PreProcessMsg ( msg ) ) ;   
    }


/**************************************************************************/
/*                                                                        */
/*  TCheopsDialog :: NcActivate			                          */
/*      Gestion du focus dans une fenetre fille		                  */
/*                                                                        */
/**************************************************************************/

LRESULT		TCheopsDialog :: NcActivate  ( WPARAM  key, LPARAM  param )
   {

/*** 
	Le code suivant permet de conserver une barre de titre active meme
	si la fenetre perd le focus, mais finalement, ça n'est pas une très
	bonne idée car on ne sait plus qui de l'écran de saisie ou de gmenu
	conserve le focus. On ne conservera cette barre de titre active que si
	la fenetre ouverte est une fille de l'écran de saisie (comme les listes
	déroulantes dans la partie ligne, par exemple.

	if  ( :: GetWindowTask ( ( HWND ) LOWORD ( param ) )  ==  :: GetCurrentTask ( ) )
		return ( DefWindowProc ( WM_NCACTIVATE, 1, ( LPARAM ) HWindow ) ) ;
	else
		return ( DefWindowProc ( WM_NCACTIVATE, key, param ) ) ;
 ***/
	TWindow *	Window 	=  GetWindowPtr ( ( HWND ) LOWORD ( param ) ) ;


	if  ( Window  &&  typeid ( * Window )  ==  typeid ( TChoiceArray ) )
		return ( DefWindowProc ( WM_NCACTIVATE, 1, ( LPARAM ) HWindow ) ) ;
	else
		return ( DefWindowProc ( WM_NCACTIVATE, key, param ) ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TCheopsDialog :: EvGetMinMaxInfo / EvSysCommand, etc...               */
/*      Empêche une fenêtre Cheops de dépasser la taille limite.          */
/*                                                                        */
/**************************************************************************/

void	TCheopsDialog :: EvGetMinMaxInfo  ( MINMAXINFO &  Info )
   {
	TRect		WindowRect ;
	int		W, H ;


	GetWindowRect ( WindowRect ) ;
	GetDecorationSize ( W, H ) ;

// On détermine la taille minimale de la fenêtre
	Info. ptMinTrackSize. x	 =  10 ;
	Info. ptMinTrackSize. y  =  10 ; 	// au pif

	if  ( Support->FunctionKeys  !=  NULL  &&
			Support->FunctionKeys -> HWindow  !=  NULL )
		Info. ptMinTrackSize. y += Support->FunctionKeys -> Attr. H + 10 ; // Idem (pour le 10)

// Sa taille maximale
	Info. ptMaxTrackSize. x  =  W + MaxWidth ;
	Info. ptMaxTrackSize. y  =  H + MaxHeight ;

// Sa taille maximale lorsque la fenêtre est maximisée
	Info. ptMaxSize		 =  Info. ptMaxTrackSize ;

// et sa position dans ce dernier cas (on garde la position actuelle de la
// fenêtre)
	Info. ptMaxPosition. x   =  WindowRect. left ;
	Info. ptMaxPosition. y   =  WindowRect. top ;
    }




void	TCheopsDialog :: EvSysCommand  ( UINT  SysCmd, TPoint &  Point )
   {
	if  ( SysCmd  ==  SC_MAXIMIZE )
		{
		SetClientSize ( MaxWidth, MaxHeight ) ;
	      }
	else if  ( SysCmd  ==  SC_MINIMIZE  &&	TheCheopsMenuWindowObject )
		TheCheopsMenuWindowObject -> IconifyMe ( this, TResId ( "ICON_SCREEN" ) ) ;
	else
		TDecoratedFrame :: EvSysCommand ( SysCmd, Point ) ;
     }



void	TCheopsDialog :: EvNCLButtonDblClk  ( UINT	HitTest,
					      TPoint &  Point )
	{
	if  ( HitTest  ==  HTCAPTION )
		EvSysCommand ( SC_MAXIMIZE, Point ) ;
	else
		TDecoratedFrame :: EvNCLButtonDblClk ( HitTest, Point ) ;
     }


void	TCheopsDialog :: EvSize  ( UINT  SizeType, TSize &  Size )
   {
	TDecoratedFrame :: EvSize ( SizeType, Size ) ;

	if  ( Support->FunctionKeys  !=  NULL )
	   {
		switch ( SizeType )
			{
			case	SIZE_MAXIMIZED :
				EvSysCommand ( SC_MAXIMIZE, TPoint ( 0, 0 ) ) ;
				break ;
		    }
	    }
    }


void	TCheopsDialog :: EvTimer ( uint Id )
	{
	Support -> DialogBox [0] -> DialogFunction ( WM_TIMER, Id, 0L ) ;
	 }


/************************************************************************/
/*									*/
/*		Implémentation de la classe TWinLine			*/
/*									*/
/************************************************************************/

class	TWinlineClient :	public  TWindow 
   {
	public :
		TWinlineClient ( TWinLine *  parent,
				 char *	     title )
			: TWindow ( parent, title ) 
		   {
		    }


		LRESULT  DefWindowProc  ( uint  msg, WPARAM  wp, LPARAM  lp )
		   {
			if  ( msg  ==  WM_COMMAND )
				return ( Parent -> HandleMessage ( WM_COMMAND, wp, lp ) ) ;
			else
				return ( TWindow :: DefWindowProc ( msg, wp, lp ) ) ;
		    }


		char *	GetClassName  ( void )
		   {
			return  ( "TWinlineClient" ) ;
		    }


		void	GetWindowClass  ( WNDCLASS &  wc )
		   {
			TWindow :: GetWindowClass ( wc ) ;
			wc. hbrBackground = ( HBRUSH ) GetStockObject ( NULL_BRUSH ) ;
		    }
    } ;





TWinLine :: TWinLine ( TWindow *	parent,
			     unsigned char *	Data,
			     int		DataSize,
			     TUnixApplication *	App,
			     TUnixDialogBox *	dial,
			     TCheopsDialog *	support )
		: TWindow(parent,"",0),Application(App),Dial(dial) ,Support(support)

   {
	Attr. Style 	= WS_CHILD | WS_VSCROLL | WS_HSCROLL | DS_LOCALEDIT ;
	Attr. Style    &= ~ ( WS_VISIBLE ) ;

	DecodePos ( Data, DataSize ) ;
	NbLigCourantes 	= 0 ;
	DataLine 	= NULL ;
	FirstLine 	= 0 ;
	ArrayVisible	= FALSE ;
    }



BOOL	TWinLine :: Create  ( void )
   {
	register BOOL	Status = TWindow :: Create ( ) ;


	WinlineClient = new  TWinlineClient ( this, "" ) ;
	WinlineClient -> Attr. Style = WS_CHILD | WS_VISIBLE ;
	WinlineClient -> Create ( ) ;

	return ( Status ) ;
     }
	



/*******************************************************************************
	Decode le bloc de données afin de connaître la position de
	chaque champ.
	Creer la police qui sera uilisée dans ces champs

*******************************************************************************/
void	TWinLine :: DecodePos  ( unsigned char * Data, int size )
   {
	DIALOGBOX	tmp ;
	RECT 		rect ;
	register int	offset = 4 * GetSystemMetrics ( SM_CYBORDER ) ;


	CreateDialogBoxTemplate ( Data , size ,tmp ) ;
	HANDLE hd = CreateDialogBoxHandle ( tmp ) ;
	GlobalFree ( hd ) ;


// on récupère la font et on crée une nouvelle police pour les controles
	DWORD		Zob = GetDialogBaseUnits ( ) ;
	int		height = HIWORD ( Zob ) ;

	height = ( height * ( int ) tmp. FontInfo. PointSize ) / 8 ;

	FontHandle = CreateFont  ( height - 2,
			   0,
			   0, 0,
			   FW_BOLD,
			   0,
			   0,
			   0,
			   DEFAULT_CHARSET,
			   OUT_DEFAULT_PRECIS,
			   CLIP_DEFAULT_PRECIS,
			   PROOF_QUALITY,
			   DEFAULT_PITCH | FF_DONTCARE,
			   tmp.FontInfo.szTypeFace) ;

// on calule la hauteur d'une ligne
	rect. left = rect. top = 0 ;
	rect. right = tmp. DialogTemplate. dtCX ;
	rect. bottom = tmp. DialogTemplate. dtCY ;
	MapDialogRect ( Dial -> HWindow, & rect ) ;
	HauteurLigne = rect. bottom - rect. top ;
	LargeurLigne = rect. right  - rect. left ;

// on récupere le nombre de champs
	NbChamps = VisibleFields = tmp. DialogTemplate. dtItemCount ;
	Controle = new InfoChamps[NbChamps] ;

// on recupere les infos sur chaque champs et on convertit les coordonnées
	for  ( int i = 0; i < NbChamps; i ++ )
	   {
		rect. left = tmp. DialogItemTemplates [i]. dtilX ;
		rect. top = 0 ;
		rect. right = tmp. DialogItemTemplates [i]. dtilCX ;
		rect. bottom = tmp. DialogItemTemplates [i]. dtilCY ;
		MapDialogRect ( Dial -> HWindow, & rect ) ;
		Controle [i]. Style = tmp. DialogItemTemplates [i]. dtilStyle ;
		Controle [i]. Classe = tmp. DialogItemTemplates [i]. dtilClass[0] ;
		Controle [i]. X = rect. left ;
		Controle [i]. Y = offset ;
		Controle [i]. CX = rect. right ;
		Controle [i]. CY = rect. bottom + ( 2 * offset ) ;
	    }

	FreeDialogBoxTemplate ( tmp ) ;
    }


/*
 * 	Teste si un champ est visible à l'écran
 */

BOOL	TWinLine :: ALecran  ( int Id )
   {
	return  ( (Id > FirstLine * 100)  &&
			( Id < ( FirstLine + LigAff ( NbLigCourantes ) ) * 100 ) ) ;
    }

/*
 *   Fonction servant à désactiver une colonne entière de la partie ligne.
 *	Elle redonne le focus à la fenêtre TWinLine pour être sûr qu'un
 *	champ désactivé ne le garde pas
 */
void	TWinLine :: EnableColonne ( int Id, int Etat )
    {
	int 	x = Id % 100 ;
	HWND	hId ;
//	DWORD	lg ;

	if  ( x > NbChamps) return ;
	if  ( !ArrayVisible )
		SetFocus ( ) ;
	for  ( int i = FirstLine; i < FirstLine + NBLIGNEMAX; i ++ )
	   {
		hId = WinlineClient -> GetDlgItem ( i * 100 + x ) ;

/***
		lg = ::GetWindowLong ( hId, GWL_STYLE ) ;
		if  ( Etat )
			lg &= ~WS_DISABLED;
		else
			lg |= WS_DISABLED;

		::SetWindowLong(hId, GWL_STYLE,lg) ;
 ***/
		EnableTextItem ( hId, Etat ) ;
	    }
}

/******************************************************************************
		Affiche la ListBox au bon emplacement

		La fenêtre étant une POPUP il faut convertir les
		coordonnées client en coordonnées écran
******************************************************************************/
void	TWinLine :: AfficherChoice ( int Id )
	{
	int		index = ( Id % 100 ) - 1 ;
	TChoiceArray 	*Choice = Controle[index].Ptr;
	int		id=GetWindowPtr(GetFocus())->Attr.Id;
	int             Lig=id /100 ;
	int		offset = 4 * GetSystemMetrics ( SM_CYBORDER ) ;

	Choice -> LB -> Showdropdown ( ) ;
	Choice -> LB -> Invalidate ( ) ;

	if  ( ( Lig > NbLigCourantes - 1 )  ||  ( ( Id % 100 ) > NbChamps ) )
		return ;

	Choice -> SetOldFocusId ( id ) ;
	TPoint pt ( Controle [ ( Id % 100 ) - 1 ]. X,
		   offset + ( ( Lig - FirstLine + 1 ) * HauteurLigne )
			+ ( ( Lig - FirstLine ) * INTERLIGNE ) ) ;
	WinlineClient -> ClientToScreen ( pt ) ;

	Choice -> MoveWindow ( pt. x, pt. y, 
		max ( Controle [ ( Id % 100 ) - 1 ]. CX, 
		      Choice -> LB -> x1 + Choice -> LB -> x2 ), 
		Choice -> LB -> Attr. H, TRUE ) ;
	Choice -> LB -> MovePopup ( Choice -> HWindow, max ( Controle [ ( Id % 100 ) - 1 ]. CX ,
				Choice -> LB -> x1 + Choice -> LB -> x2 ) ) ;
	Choice -> LB -> boolpopup = FALSE ;

	Choice -> Show ( SW_SHOW ) ;

	ArrayVisible = TRUE ;
	Choice -> LB -> SetFocus ( ) ;
	}

/*
 *	Fonction qui s'occupe de gérer les cas où le focus est demander sur
 *	un champ qui n'est pas à l'ecran.
 */
HWND	TWinLine :: DemandeFocus ( int ControlId )
   {
	int	NewLine ;

	if  ( ! ALecran ( ControlId ) )
	   {
		int	ligne = ControlId / 100 ;

		if  ( ligne > NbLigCourantes )
			MessageBox ( "TWinLine :: DemandeFocus : Identificateur incorrect.", "Erreur interne" ) ;
		NewLine = ligne + NBLIGNEMAX ;

		if  ( NewLine > NbLigCourantes )
			NewLine = NbLigCourantes ;
		NewLine -= NBLIGNEMAX ;

		SetScrollPos ( SB_VERT, NewLine ) ;

		if  ( NewLine > FirstLine )
			MiseAjourId_D ( FirstLine, NewLine ) ;
		else
			MiseAjourId_M ( FirstLine, NewLine ) ;
		AfficherText ( ) ;
	    }

	return  ( WinlineClient -> GetDlgItem ( ControlId ) ) ;
    }

/*
 *  Cette fonction initialise le tableau pour stocker l'ensemble des données
 *  qui seront affichées dans les TEdit
 *  Si le tableau contient déja des données, elle les détruit
 *  Elle repositionne le Biniou sur la premiere ligne et met à jour les
 *  identificateurs des champs.
 */
void	TWinLine :: RecevoirLignes ( int nb )
   {
//  on commence par détruire les anciennes données
	if  ( NbLigCourantes )
		DetruireData ( ) ;

// on alloue la memoire pour le tableau de données
	AllouerData ( nb ) ;

	NbLigCourantes = nb ;

	MiseAjourId_M ( FirstLine, 0 ) ;

	if  ( NbLigCourantes > NBLIGNEMAX )
	   {
		SetScrollRange ( SB_VERT, 0, NbLigCourantes - NBLIGNEMAX, false ) ;
		SetScrollPos ( SB_VERT, 0 ) ;
	    }
	else
	   {
		SetScrollRange ( SB_VERT, 0, NbLigCourantes ) ;
		SetScrollPos ( SB_VERT, 0 ) ;
	    }
    }


/*
 *  Change l'identificateur d'un champ ( de Id en NewId)
 */
void	TWinLine :: MiseAjourId ( int Id, int NewId )
   {
	TWindow *	W = WinlineClient -> ChildWithId ( Id ) ;
	TEdit *		We ;

	if  ( W )
	   {
		We = ( TEdit * ) dynamic_cast <TEdit *> (W) ;
		if  ( We == NULL )
			throw Bad_cast() ;
		We -> SetWindowWord( GWW_ID, NewId ) ;
		We -> Attr. Id = NewId ;
		We -> SetText ("") ;
		 }
	else
		MessageBox ("TWinLine::MiseAjourId : identificateur de contrôle inconnu.", "Erreur interne" ) ;
    }

/*
 * 	Remet à jour tous les identificateurs lorsque l'ascenceur remonte
 *	On commence par la première ligne afin d'éviter d'avoir plusieurs
 *	champs avec le même Id
 */
void	TWinLine :: MiseAjourId_M ( int ligne, int nl )
    {
	int	NewLine = nl;

	for  ( int i = ligne; i < ligne + NBLIGNEMAX; i ++, NewLine ++ )
	   {
		for  ( int j = 0; j < NbChamps; j ++ )
			MiseAjourId ( ( i * 100 ) + j + 1,
						( NewLine * 100 ) + j + 1 ) ;
	     }

	FirstLine = nl ;
    }

/*
 *	Remet à jour les Id des champs lorsque l'ascenceur descend
 *	Il faut commencer par la ligne du bas
 */
void	TWinLine :: MiseAjourId_D ( int ligne, int nl )
	 {
	int	NewLine = nl + NBLIGNEMAX - 1 ;

	for  ( int i = ligne + NBLIGNEMAX - 1; i  >=  ligne; i --, NewLine -- )
	   {
		for  ( int j = 0; j < NbChamps; j ++ )
			MiseAjourId ( ( i * 100 ) + j + 1, ( NewLine * 100 ) + j + 1 ) ;
	    }

	FirstLine = nl ;
    }

/*
 * 	Affiche le texte associé aux TEdit présent à l'écran
 */
void	TWinLine :: AfficherText ( )
   {
	int 		Id;
	TWindow *	W;
	TEdit *		We;
	char *		txt;
	int 		fin = FirstLine + NBLIGNEMAX ;

	if  ( fin > NbLigCourantes ) fin = NbLigCourantes ;

	for  ( int i = FirstLine; i < FirstLine + NBLIGNEMAX; i ++ )
	   {
		for  ( int j = 0; j < NbChamps; j ++ )
		   {
			Id = ( i * 100 ) + j + 1 ;
			W = WinlineClient -> ChildWithId ( Id ) ;
			if  ( W )
			   {
				We = ( TEdit * ) dynamic_cast <TEdit *> (W) ;
				if  ( We  ==  NULL )
					throw Bad_cast () ;
				if  ( i < fin )
				   {
					if  ( ( txt = TextId ( Id ) )  !=  NULL )
						We -> SetText ( txt ) ;
				    }
				else
					We -> SetText ("") ;
			    }
			else
				MessageBox ("TWinLine::AfficherTexte : Identificateur de contrôle inconnu", "Erreur interne" ) ;
		    }
	      }
    }

/*
 * 	Fonction de création des champs
 *	Elle ne crée que des TEdit même lorsqu'un COMBOBOX est prévue
 *	Pour chaque COMBOBOX prévue, elle crée une fenêtre suppléméentaire
 *	contenant une listBox
 */

 void	TWinLine :: CreerChamps ( int debL, int finL )
   {
	int		i, j ;
	TEdit *		tmpE ;
	TChoiceArray *	tmpC ;
	int 		Id ;
	int		offset = 4 * GetSystemMetrics ( SM_CYBORDER ) ;

	for  ( i = debL; i < finL; i ++ )
	   {
		for  ( j = 0; j < NbChamps; j ++ )
		   {
			Id = ( i * 100 ) + j + 1 ;
			tmpE = new TEdit ( WinlineClient,Id,"",
				Controle [j]. X,
				offset + ( i * ( HauteurLigne + INTERLIGNE ) ),
				Controle [j]. CX,
				HauteurLigne ) ;
			Controle [j]. Ptr = NULL ;
			tmpE -> Attr. Style = Controle [j]. Style|WS_CHILD|ES_READONLY ;
			try
			  {
				tmpE -> Create ( ) ;
			   }
			catch ( ... )
			   {
				:: MessageBox ( NULL, "Arrghh !", "Erreur !", MB_OK ) ;
			    }


		// On rajoute le bit WS_DISABLED après création pour que les
		// champs ne changent pas d'apparence (texte grisé) ET que la
		// procédure boite de dialogue croie quand meme qu'ils sont
		// disabled.
			DWORD	Style = tmpE -> GetWindowLong ( GWL_STYLE ) ;
			
			tmpE -> SetWindowLong ( GWL_STYLE, Style | WS_DISABLED ) ;
			tmpE -> HandleMessage ( WM_SETFONT, ( WPARAM ) FontHandle, 1 ) ;
			tmpE -> Show ( 1 ) ;
		    }
	     }

	for  ( j = 0; j < NbChamps; j ++ )
	    {
		if  ( Controle [j]. Classe  ==  'C' )
		   {
			int	height = offset + ( j * ( HauteurLigne + INTERLIGNE ) ) ;


			tmpC = new TChoiceArray ( WinlineClient, OFFSET_ID + j + 1,
				Controle [j]. X, 6 * ( HauteurLigne ),
				Controle [j]. CX + GetSystemMetrics ( SM_CXVSCROLL ),
				Controle [j]. CY - HauteurLigne, this ) ;
			tmpC -> Create ( ) ;
			tmpC -> LB -> HandleMessage ( WM_SETFONT, ( WPARAM ) FontHandle, 1 ) ;
			tmpC -> Show ( SW_HIDE ) ;
			tmpC -> LB -> EditRect =  TRect ( 
					Controle [j]. X,
					height,
					Controle [j]. X + Controle [j]. CX,
					height + HauteurLigne ) ;
			Controle [j]. Ptr = tmpC ;
		    }
	    }
    }

/*
 * Fonction qui libère la mémoire allouée pour une ligne du tableau de données
 */
void	TWinLine :: DetruireLigne ( TLine * lig )
   {
	for  ( int i = 0; i < NbChamps; i ++ )
		if  ( lig -> Tab [i] )
			delete [] lig -> Tab [i] ;
	delete [] lig -> Tab ;
	delete lig ;
    }

/*
 *  Fonction qui libère la mémoire allouée pour l'ensemble du tableau
 */
void	TWinLine :: DetruireData ( )
   {
	TLine *	lc = DataLine,
	      *	tmp ;

	while  ( lc )
	   {
		tmp = lc ;
		lc = lc -> suiv ;
		DetruireLigne ( tmp ) ;
		 }
	DataLine = FastPtr = NULL ;
        FastInd = 0 ;
    }

/*
 *  Fonction qui alloue la mémoire pour l'ensemble du tableau  en
 *  fonction du nombre nb de ligne
 */
void	TWinLine :: AllouerData ( int nb )
   {
	int	i ;
	TLine *	l = NULL,
	      *	l2 = NULL ;

	for  ( i = 0; i < nb; i ++ )
	   {
		l = CreerLigne ( ) ;
		l -> suiv = l2 ;
		l2 = l ;
	    }
	DataLine = FastPtr = l ;
	FastInd = 0 ;
    }

/*
 *  Alloue la mémoire pour une ligne
 */
TLine *	TWinLine :: CreerLigne ( )
   {
	TLine *	l = new TLine ;

	l -> Tab = new char * [NbChamps] ;

	for  ( int j = 0; j < NbChamps; j ++ )
		l -> Tab [j] = NULL ;
	return ( l ) ;
    }

/*
 *      Supprime une ligne
 *	Il faut penser à réajuster l'ascenceur avec le nouveau nb de lignes
 */
void	TWinLine :: SupprimerLigne ( int nb, int pos )
   {
	TLine *	LigAsup,
	      *	LigPrec ;

	if  ( nb  !=  NbLigCourantes - 1 )
	   {
		MessageBox ("TWinLine::SupprimerLigne : Numéro de ligne incorrect", "Erreur interne" ) ;
		return ;
	    }

	if  ( ! NbLigCourantes )
	   {
		MessageBox ("TWinLine::SupprimerLigne : plus de lignes à supprimer", "Erreur interne") ;
		return ;
	    }

	if  ( ! pos )
	   {
		LigAsup = DataLine ;
		DataLine = DataLine -> suiv ;
	    }
	else
		{
		LigPrec = GetLine ( pos, TRUE ) ;
		if  ( LigPrec )
		   {
			LigAsup = LigPrec -> suiv ;
			LigPrec -> suiv = LigAsup -> suiv ;
		    }
		else
			LigAsup = NULL ;
	    }
	FastPtr = DataLine ;
	FastInd = 0 ;

	if  ( LigAsup )
	   {
		NbLigCourantes = nb ;
		if  ( nb > 8 )
			SetScrollRange ( SB_VERT, 0, nb - NBLIGNEMAX ) ;
		DetruireLigne ( LigAsup ) ;
		AfficherText ( ) ;
	    }
   }


/*
 * 	Insertion d'une ligne
 *	Il faut penser à réajuster l'ascenceur avec le nouveau nb de lignes
 */
void	TWinLine :: InsererLigne ( int nb, int pos )
   {
	TLine *	louI,
	      *	laI ;

	if  ( nb  !=  NbLigCourantes + 1 )
	   {
		MessageBox ("TWinLine::InsererLigne : numéro de ligne incorrect.", "Erreur interne") ;
		return ;
	    }
	if  ( ! pos )
	   {
		if  ( nb  ==  1 )
			{
			laI = DataLine = CreerLigne ( ) ;
			DataLine -> suiv = NULL ;
		    }
		else
		   {
			laI = CreerLigne ( ) ;
			laI -> suiv = DataLine ;
			DataLine = laI ;
		    }
	    }
	else
	   {
		if  ( ( louI = GetLine ( pos, TRUE ) )  ==  NULL )
		   {
			MessageBox ("TWinLine::InsereLigne : impossible d'inserer une ligne.", "Erreur interne" ) ;
			return ;
		    }
		laI = CreerLigne () ;
		laI -> suiv = louI -> suiv ;
		louI -> suiv = laI ;
	    }
	FastPtr = DataLine ;
	FastInd = 0 ;

	if  ( nb > 8 )
		SetScrollRange ( SB_VERT, 0, nb - NBLIGNEMAX ) ;
	else
		SetScrollRange ( SB_VERT, 0, nb ) ;

	NbLigCourantes = nb ;

	AfficherText ( ) ;
    }


/*
 * 	Permet d'effacer une ligne
 */
void	TWinLine :: EffacerLigne  ( int nb, int pos )
   {
	TLine *	lig ;

	if  ( nb  !=  NbLigCourantes )
	   {
		MessageBox ("TWinLine::EffacerLigne : impossible d'effacer une ligne.", "Erreur interne") ;
		return ;
	    }

	if  ( ( lig = GetLine ( pos ) )  !=  NULL )
	   {
		for  ( int i = 0; i < NbChamps; i ++ )
			if  ( lig -> Tab [i] )
			   {
				delete lig -> Tab [i] ;
				lig -> Tab [i] = NULL ;
			    }
	    }

	AfficherText ( ) ;
    }


/*
 * 	Cette fonction renvoie un pointeur sur la ligne y du tableau
 *		ou sur la ligne y-1 si précedente vaut VRAI
 */
TLine *	TWinLine :: GetLine  ( int y, BOOL precedente )
   {
	TLine *	pos = DataLine ;
	int     ind = 0 ;

	if  ( precedente )
		y -- ;
	if  ( y  ==  0 )
		return ( DataLine ) ;
	if  ( y  >=  FastInd )
	   {
		pos = FastPtr ;
		ind = FastInd ;
		 }

	while  ( ( ind  !=  y )  &&  pos )
	   {
		ind ++ ;
		pos = pos -> suiv ;
	    }
	if  ( pos )
	   {
		FastPtr = pos ;
		FastInd = ind ;
	    }

	return ( pos ) ;
    }


/*
 * 	Cette fonction met à jour le tableau contenant le texte de
 *	chaque identificateur
 */

void	TWinLine :: InitTab ( int Id, char * texte )
   {
	int 	x = ( Id % 100 ) - 1 ;
	int 	y = Id / 100 ;
	TLine *	pos =GetLine ( y ) ;
	char   ach [ 200 ] ;

	wsprintf ( ach, "on est sorti du tableau Id=%d  texte='%s'", Id, texte ) ;

	if  ( x  >=  NbChamps )
	   {
		MessageBox( "TWinLine::InitTab : Identificateur de contrôle incorrect.", "Erreur interne" ) ;
		return ;
	    }

	if  ( ! pos )
	   {
		return ;
	    }
	pos -> Tab [x] = new char [ 256 ] ;
	strcpy ( pos -> Tab [x], texte ) ;
    }


/*
 * 	Renvoie le texte normalement contenu dans le champ Id
 */

char *	TWinLine :: TextId  ( int Id )
	{
	int 	x = ( Id % 100 ) - 1 ;
	int 	y = Id / 100 ;
	TLine *	pos =GetLine ( y ) ;

	if  ( ! pos )
	   {
		return ( NULL ) ;
	    }
	return ( pos -> Tab [x] ) ;
    }


/*
 * 	Fonction qui gére les déplacements de l'ascenceur
 *		- elle redonne à chaque champs son nouvel Id
 *		- elle réaffiche le texte à l'interieur des nouveaux champs
 */

void	TWinLine :: EvVScroll  ( UINT code, UINT b, HWND  c )
   {
	int	NewLine ;               // position courante du Scroller


	NewLine = GetScrollPos ( SB_VERT ) ;

// Gestion grafique de la ScrollBar.
	switch  ( code )
	   {				// Contre l'envoi d'un code indesirable

		case    SB_PAGEDOWN :
			if  ( NewLine + 7  <  NbLigCourantes )
				NewLine += 7 ;
			else
				NewLine = NbLigCourantes - 1 ;
			SetScrollPos ( SB_VERT, NewLine ) ;
			break ;


		case	SB_PAGEUP :
			if  ( NewLine - 7  >=  0 )
				NewLine -= 7 ;
			else
				NewLine = 0 ;
			SetScrollPos ( SB_VERT, NewLine ) ;
			break ;

		case	SB_LINEUP :
			if  ( NewLine )
				NewLine -- ;
			SetScrollPos ( SB_VERT, NewLine ) ;
			break ;


		case	SB_LINEDOWN :
			if  ( NewLine + 1  <  NbLigCourantes )
				NewLine ++ ;
			SetScrollPos ( SB_VERT, NewLine ) ;
			break ;

		case	SB_THUMBPOSITION :
			if  ( b  <  NbLigCourantes )
				SetScrollPos ( SB_VERT, b ) ;
			break ;


		default :
			TWindow :: EvVScroll ( code, b, c ) ;
	    }



// Remise a jour des textes contenus dans les TEdits.
	NewLine = GetScrollPos ( SB_VERT ) ;

	if  ( NewLine  !=  FirstLine )
	   {
		if  ( NewLine > FirstLine )
			MiseAjourId_D ( FirstLine, NewLine ) ;
		else
			MiseAjourId_M ( FirstLine, NewLine ) ;
		AfficherText ( ) ;
	    }
    }





/**************************************************************************/
/*                                                                        */
/*  TWinLine :: EvHScroll -                                               */
/*      Gère le déplacement horizontal pour les écrans de saisie dont la  */
/*      partie ligne excède la largeur de la boîte de dialogue.           */
/*                                                                        */
/**************************************************************************/

void	TWinLine :: EvHScroll  ( UINT code, UINT b, HWND  c )
   {
	int	NewField,               // position courante du Scroller
		OldField ;
	int	ScrollMin	=  VisibleFields,
		ScrollMax 	=  NbChamps ;


	OldField = NewField = GetScrollPos ( SB_HORZ ) ;

// Gestion grafique de la ScrollBar.
	switch  ( code )
	   {				// Contre l'envoi d'un code indesirable

		case    SB_PAGEDOWN :
			if  ( NewField  <  ScrollMax )
				NewField ++ ;
			break ;


		case	SB_PAGEUP :
			if  ( NewField  >  ScrollMin )
				NewField -- ;
			break ;

		case	SB_LINEUP :
			if  ( NewField  >  ScrollMin )
				NewField -- ;
			break ;


		case	SB_LINEDOWN :
			if  ( NewField  <  ScrollMax )
				NewField ++ ;
			break ;

		case	SB_THUMBPOSITION :
			if  ( b  <  ScrollMin )
				NewField = ScrollMin ;
			else if  ( b  >  ScrollMax )
				NewField = ScrollMax ;
			else
				NewField = b ;
			break ;


		default :
			TWindow :: EvHScroll ( code, b, c ) ;
			return ;
	    }


	if  ( OldField  ==  NewField )
		return ;


// Et la position de la fenêtre
	HWND			NewControl	=  WinlineClient -> GetDlgItem ( ( FirstLine * 100 ) + NewField ),
				OldControl 	=  WinlineClient -> GetDlgItem ( ( FirstLine * 100 ) + OldField ) ;

	if  ( ! OldControl  ||  ! NewControl ) 
		return ;

// Récupérer le TWindow des intitulés
	TUnixDialogBox *	LineHeader = Support -> Support -> IntituleLig ;

//  Position actuelle de la fenetre WinlineClient
	TRect			WinlineClientRect  	=  WinlineClient -> GetWindowRect ( ) ;

// Largeur de la partie visible
	int			Width	=  GetClientRect ( ). Width ( ) ;

// Position (en coordonnées écran) du controle vers lequel on doit se déplacer
	TRect			NewControlRect ;

	:: GetWindowRect ( NewControl, & NewControlRect ) ;

// Coordonnée X à convertir en coordonnées client par rapport à l'objet TWinLine
	TPoint			NewControlPos ( NewControlRect. left, 0 ) ;

	ScreenToClient ( NewControlPos ) ;

// Largeur du controle vers lequel on doit se déplacer
	int			NewControlWidth = NewControlRect. Width ( ) ;

// Delta à déplacer
	int			Delta ;

	Delta = Width - ( NewControlPos. x + NewControlWidth ) - 1 ;

// Coordonnées réelles de la fenetre TWinLineClient et de l'intitulé
	TRect			ClientRect  =  WinlineClient -> GetWindowRect ( ),
				HeaderRect  =  LineHeader -> GetWindowRect ( ) ;
					  

// Deplacement
	TPoint 		P  ( ClientRect. left + Delta, ClientRect. top ),
			PH ( HeaderRect. left + Delta, HeaderRect. top ) ;

	ScreenToClient ( P ) ;
	Support -> Support -> ScreenToClient ( PH ) ;


// Si on revient sur le champ 1, on recadre tout
// (si on ne fait pas ça, on va essayer de se cadrer sur le bord droit du
// dernier champ visible. Dans ce cas, la partie lignes, au lieu de commencer
// en 0, va commencer quelques pixels trop à droite)
// En outre, on interdit au bord gauche de la partie ligne d'avoir une
// abscisse strictement positive (ce qui ferait apparaitre un trou noir sur la
// gauche). Cette situation arrive notamment avec fcdi, où le dernier champ 
// est l'étoile indiquant si l'article est en sortie auto ou pas. Ce champ est
// le dernier dans l'ordre des controles, mais le 2e dans l'ordre de présentation.
	if  ( NewField  ==  VisibleFields  ||  P. x  >  0 )
	   {
		P. x  = PH. x = 0 ; 
		NewField = 1 ;		// Pour mettre correctement à jour le scroller
	    }

	:: LockWindowUpdate ( Support -> HWindow ) ;

	WinlineClient -> SetWindowPos ( NULL, P. x, P. y, 0, 0,
		SWP_NOSIZE | SWP_NOZORDER ) ;
	WinlineClient -> UpdateWindow ( ) ;

	LineHeader -> SetWindowPos ( NULL, PH. x, PH. y, 0, 0,
		SWP_NOSIZE | SWP_NOZORDER ) ;
	LineHeader -> UpdateWindow ( ) ;

	:: LockWindowUpdate ( NULL ) ;


// Ajuster la position du scroller
	SetScrollPos ( SB_HORZ, NewField ) ;

    }




/**************************************************************************/
/*                                                                        */
/*  TWinLine :: HandleFocus -                                             */
/*      Déplace la fenêtre si le focus est donné à un champ qui est en    */
/*      dehors.                                                           */
/*                                                                        */
/**************************************************************************/

void	TWinLine :: HandleFocus  ( int  id )
   {
	register int 	NewControl	=  id % 100 ;


	EvHScroll ( SB_THUMBPOSITION, NewControl, 0 ) ;
     }



/*
 * 	On surcharge la fonction de base de la classe TWindow  pour effectuer
 *	des traitements supplémentaires avant d'envoyer les messages au différents
 *	contrôle.
 *	Elle permet notamment d'intercepter les données qui seront afficher dans
 *	les champs et de mettre à jour le tableau correspondant.
 *
 *	ATTENTION : cette fonction n'est pas virtuelle, il faut absolument avoir
 *		    un pointeur de type TWinLIne pour que la fonction soit appelée
 */
LRESULT	TWinLine :: SendDlgItemMessage ( int ControlId, uint msg, WPARAM wp, LPARAM lp )
	{
	int 	Id = ControlId ;
	uint 	Msg = msg ;
	LRESULT Status = 0L ;
	BOOL	Envoyer = TRUE ;

	switch  ( msg )
		{
		case  WM_SETTEXT :
			InitTab ( Id, ( char * ) lp ) ;
			break ;

		case  WM_SETFOCUS :
			HandleFocus ( ControlId ) ;
			break ;

		case  CB_ADDSTRING :
			Id = ControlId % 100 ;
			Status = Controle [ Id - 1 ]. Ptr -> LB -> AddString ( ( char * ) lp) ;
			Envoyer = FALSE ;
			break ;
		case  CB_INSERTSTRING :
			Id = ControlId % 100 ;
			Status = Controle [ Id - 1 ]. Ptr -> LB -> InsertString ( ( char * ) lp, wp ) ;
			Envoyer = FALSE ;
			break ;
		case  CB_DELETESTRING :
			Id = ControlId % 100 ;
			Status = Controle [ Id - 1 ]. Ptr -> LB -> DeleteString ( wp ) ;
			Envoyer = FALSE ;
			break ;


		case  CB_SHOWDROPDOWN :
			if  ( wp )
				AfficherChoice ( ControlId ) ;
			else
			   {
				TChoiceArray 	*Choice = Controle[(ControlId % 100) -1].Ptr;
				Choice -> Show ( SW_HIDE ) ;
				ArrayVisible = FALSE ;
			    }
			Envoyer = FALSE ;
			break ;

		case  CB_RESETCONTENT :
			Id = ControlId % 100 ;
			Controle [ Id - 1 ]. Ptr -> LB -> ClearList ( ) ;
			Envoyer = FALSE ;
			break ;

		case  CB_GETCOUNT :
			Id = ControlId % 100 ;
			Status = Controle [ Id - 1 ]. Ptr -> LB -> GetCount ( ) ;
			Envoyer = FALSE ;
			break ;

		case  CB_SETCURSEL :
			Id = ControlId % 100 ;
			Status = Controle [ Id - 1 ]. Ptr -> LB -> SetSelIndex ( wp ) ;
			Envoyer = FALSE ;
			break ;

		case  CB_GETCURSEL :
			Id = ControlId % 100 ;
			Status = Controle [ Id - 1 ]. Ptr -> LB -> GetSelIndex ( ) ;
			Envoyer = FALSE ;
			break ;

		case  WM_ENABLE :
			EnableTextItem ( WinlineClient -> GetDlgItem ( Id ), wp ) ;
			Envoyer = FALSE ;
			break ;

		default :
			break ;
		}
	if  ( Envoyer )
		Status = WinlineClient -> SendDlgItemMessage ( Id, Msg, wp, lp ) ;

	return ( Status ) ;
	}

/***************************************************************************

	Fonction qui indique à UNIX qu'une fenêtre de TWinLine
	a recu le FOCUS

***************************************************************************/

LRESULT	TWinLine :: EvCommand  ( uint id, HWND  hwnd, uint  notify )
   {
	LRESULT 	status ;
	TWindow *	W ;
	TChoiceArray *	tab ;
	bool		SaveFocus	=  false ;


	switch  ( notify )
	   {
		case  EN_SETFOCUS :
		case  CBN_SETFOCUS :
			SaveFocus = true ;
			HandleFocus ( id ) ;
			if  ( id > OFFSET_ID )
			   {
				W = WinlineClient -> ChildWithId ( id ) ;
				tab = ( TChoiceArray * ) dynamic_cast <TListBox *> ( W );
				if  ( tab  ==  NULL )
					throw Bad_cast ( ) ;
				id = tab -> GetOldFocusId ( ) ;
			    }

			if  ( EventQueue -> SetFocus ( Application -> ProcessId,
						       id, MAKELPARAM ( ( WORD ) hwnd, notify ) ) )
				break ;
			EventQueue -> Enqueue (
					Application -> ApplicationId,
					ObjectId,
					Application -> ProcessId,
					WM_COMMAND,
					id,
					MAKELPARAM ( ( WORD ) hwnd, notify ),
					UWC_LPARAM_NOT_POINTER ) ;
			status = TWindow :: EvCommand ( id, hwnd, notify )  ;
			break;

		default :
			status = TWindow :: EvCommand ( id, hwnd, notify ) ;
		}


// Un des champs a émis une notification pour perte ou gain de focus. Conserver
// son hwnd afin que TCheopsDialog puisse le cas échéant lui redonner le focus
	if  ( SaveFocus )
	   {
		TUnixDialogBox *	Box 	=  0 ;


		if  ( Support -> Support -> Onglet )
			Box = Support -> Support -> DialogBox [
				Support -> Support -> Onglet -> actif ( ) ] ;
		else
			Box = Support -> Support -> DialogBox [0] ;

		if  ( Box )
			Box -> LastFocusHwnd = hwnd ;
	     }



	return ( status ) ;
   }

/*
/*	Recuperation d'un texte dans la partie ligne ( visible et invisible )
 */

uint	TWinLine :: GetDlgItemText ( int ControlId, char far * text, int max)
   {
	char *	Buffer ;
	uint	Status ;

	if  ( text  ==  NULL )
		return ( 0 ) ;

	if  ( ALecran ( ControlId ) )
		Status = WinlineClient -> GetDlgItemText ( ControlId, text, max ) ;
	else
	   {				// Simule un GetDlgItemText Hors champ
		Buffer = TextId ( ControlId ) ;
		if  ( Buffer ==  NULL )
			return ( 0 ) ;
		strncpy ( text, TextId ( ControlId ), max ) ;
		text [ max -1 ] = 0 ;
		Status = strlen ( text ) ;
	    }

	return ( Status ) ;
    }

bool	TWinLine :: PreProcessMsg ( MSG & msg )
   {
	BOOL		Back = GetKeyState ( VK_SHIFT ) & 0x8000 ;
	BOOL		ReturnValue = FALSE ;



	switch  ( msg. message )
	   {
		case  WM_KEYDOWN :
			switch  ( msg. wParam )
			   {
				case  VK_RETURN :
					ReturnValue = TRUE ;

				case  VK_UP :
				case  VK_DOWN :
				case  VK_PRIOR :
				case  VK_NEXT :
				case  VK_ESCAPE :
					EventQueue -> Enqueue (
						Application -> ApplicationId,
						ObjectId,
						Application -> ProcessId,
						WM_KEYDOWN,
						msg.wParam,
						msg.lParam,
						UWC_LPARAM_NOT_POINTER ) ;

					break ;


				case  VK_TAB :
				   {
					int		Id    = :: GetDlgCtrlID ( :: GetFocus ( ) ) ;
					char		Buffer [ MAX_PARAMETER_LENGTH ] ;


					if  ( ! Id )	// Paranoïa
						break ;

ShootAgain :
				// Backtab : ne pas aller au-delà du 1er champ de la ligne
					if  ( Back )
					   {
						if  ( ( Id % 100 )  ==  1 )
							Id = ( ( Id / 100 ) * 100 ) + NbChamps ;
						else
							Id -- ;
					    }
				// Tab : ne pas aller au-delà du dernier champ de la ligne
					else
					   {
						int	i = Id % 100 ;

						if  ( i + 1  >  NbChamps )
							Id = ( ( Id / 100 ) * 100 ) + 1 ;
						else
							Id ++ ;
					     }

				// C'est bon, on file le focus
					HWND	EditHwnd = WinlineClient -> GetDlgItem ( Id ) ;


					if  ( ! EditHwnd )
						break ;

					if  ( ! :: IsWindowEnabled ( EditHwnd )  ||
					      ! :: IsWindowVisible ( EditHwnd ) ) 
						goto  ShootAgain ;

				// Se positionner au champ suivant s'il s'agit d'une listbox
					:: GetClassName ( EditHwnd, Buffer, sizeof ( Buffer ) ) ;

					if  ( stricmp ( Buffer, "EDIT" ) )
						goto  ShootAgain ;

				// C'est bon, donner le focus au contrôle
					:: SetFocus ( EditHwnd ) ;
				    }        
				ReturnValue = TRUE ;
				}
			break;
	       }

	if  ( ReturnValue )
		return ( TRUE ) ;
	else
		return ( TWindow :: PreProcessMsg ( msg ) ) ;
    }


/****************************************************************************

	Après la coupure de courant je recommence tout.
		Fait chier .....

*****************************************************************************/

TChoiceArray :: TChoiceArray ( TWindow * parent, int Id, int x, int y, int cx, int cy, TWinLine * dady )
		: TWindow ( parent, 0, 0 )
	{
	Attr. X = x ;
	Attr. Y = y ;
	Attr. W = cx ;
	Attr. H = cy ;
	Attr. Style = WS_POPUP | WS_BORDER ;
	Attr. Style    &= ~ ( WS_VISIBLE ) ;

	LB = new TListPat ( this, Id, 0, 0, cx - 2 * GetSystemMetrics ( SM_CXBORDER ),
						cy, dady, this ) ;
	OldFocusId = 0 ;
	 }

BOOL	TChoiceArray :: Create ( )
   {
	BOOL	Status = TWindow :: Create ( ) ;

	if  ( LB )
		LB -> Create ( ) ;

	return ( Status ) ;
    }


int	TChoiceArray :: GetOldFocusId ( )
   {
	return ( OldFocusId ) ;
    }


void	TChoiceArray :: SetOldFocusId ( int Id )
   {
	OldFocusId = Id ;
    }

void	TListPat :: EvKillFocus ( HWND hw )
   {
	Papa -> Show ( SW_HIDE ) ;
	WinL -> ArrayVisible = FALSE ;

	TWindow :: EvKillFocus ( hw ) ;
    }

void	TListPat :: EvLButtonDblClk ( UINT  modkeys, TPoint &  pt )
   {
	EvLButtonDown ( modkeys, pt ) ;
    }



void	TListPat :: EvLButtonDown  ( UINT  modkeys, TPoint &  pt ) 
   {
	TEnhancedListBox :: EvLButtonDown ( modkeys, pt ) ;


// Envoyer l'évènement à Unix
	EventQueue -> Enqueue (
		WinL -> Application -> ApplicationId,
			WinL -> ObjectId,
			WinL -> Application -> ProcessId,
			WM_KEYDOWN,
			VK_RETURN,
			0L,
			UWC_LPARAM_NOT_POINTER ) ;
    }


void	TListPat :: EvChar ( UINT key, UINT repeat, UINT flags )
   {
/*aze	int		Id = Papa -> GetOldFocusId ( ) ;
	char		texte [256] ;
	TWindow *	Wedit = WinL -> WinlineClient -> ChildWithId ( Id ) ;
 revoire si utile */

	TEnhancedListBox :: EvChar ( key, repeat, flags ) ;
    }



BOOL	TListPat :: PreProcessMsg ( MSG & msg )
   {
	if  ( ( msg. message  ==  WM_KEYDOWN  /*||  msg. message  ==  WM_SYSKEYDOWN*/ ) )
		switch  ( msg. wParam )
		   {
			case  VK_F1 :
			case  VK_F2 :
			case  VK_F3 :
			case  VK_F4 :
			case  VK_F5 :
			case  VK_F6 :
			case  VK_F7 :
			case  VK_F8 :
			case  VK_RETURN :
			case  VK_ESCAPE :
				WinL -> PreProcessMsg ( msg ) ;
				break ;

			case  VK_PRIOR :
			case  VK_UP :
				if  ( ! TListBox :: DefWindowProc ( LB_GETCURSEL, 0L, 0L )  &&  ! base_display )
					WinL -> PreProcessMsg ( msg ) ;
				break ;
			case  VK_NEXT :
			case  VK_DOWN :
				if  ( TListBox :: DefWindowProc ( LB_GETCURSEL, 0L, 0L ) +1  >=  GetCount ( )
					&&  base_display + Nb_display > indexmax )
					WinL -> PreProcessMsg ( msg ) ;
				break ;
	    }
	return ( TEnhancedListBox :: PreProcessMsg ( msg ) ) ;
    }


/*****************************************************************************

		Implémentation de la classe TCheopsSupport

*****************************************************************************/

TCheopsSupport :: TCheopsSupport ( TCheopsDialog * parent, TUnixMessage * message,
				TUnixApplication * app )
			: TWindow ( 0, "", 0 )
   {
	UTCheopsDialog *	Msg = ( UTCheopsDialog * ) message -> Data ;
	UTCheopsGroupe *	InfoBloc = ( UTCheopsGroupe * ) ( message -> Data +
						 sizeof ( UTCheopsDialog ) ) ;
	unsigned char *	Data ;
	int		DataSize ;
	int		offset ;
	char *		p ;
	char		IntituleOnglet [ 200 ] ;

	UnixApplication = app ;
	DecoratedFrame	= parent ;
	HOnglet 	= 0 ;
	HDialogBox	= 0 ;
	HWinLine	= 0 ;
	HIntituleLig	= 0 ;
	Attr. Style     = WS_CHILD ;
	Attr. H		= 1 ;
	Attr. W		= 1 ;


	CHARtoI16 ( NbDialog, Msg -> NbBox ) ;

	DialogBox	= new TUnixDialogBox * [ max ( NbDialog, 1) ] ;
	memset ( IntituleOnglet, 0, sizeof ( IntituleOnglet ) ) ;

// on décortique le message
	if  ( ! NbDialog )
		DialogBox [0] = new TUnixDialogBox ( app, this, (unsigned char *)IntituleOnglet, 0,
				message -> ProcessId, TRUE, DecoratedFrame ) ;

	for  ( int i = 0; i < NbDialog; i ++ )
	   {
		CHARtoI16 ( offset, InfoBloc -> offset ) ;
		Data = ( unsigned char * ) ( message -> Data + offset ) ;
		CHARtoI16 ( DataSize, InfoBloc -> size ) ;
		if  ( i )
			strcat ( IntituleOnglet, "|" ) ;
		strcat ( IntituleOnglet, InfoBloc -> intitule ) ;
		DialogBox [i] = new TUnixDialogBox ( app, this, Data, DataSize,
				message -> ProcessId, TRUE, DecoratedFrame ) ;
		p = ( char * ) InfoBloc ;
		p += sizeof ( UTCheopsGroupe ) ;
		p += strlen ( InfoBloc -> intitule ) ;
		InfoBloc = ( UTCheopsGroupe * ) p ;
	    }
	pIntituleOnglet = new char [ strlen ( IntituleOnglet ) +1 ] ;
	strcpy ( pIntituleOnglet, IntituleOnglet ) ;
	Device -> HostToLocalCharSet ( pIntituleOnglet ) ;



	Onglet 		= NULL ;
	WinLine 	= NULL ;
	FunctionKeys 	= NULL ;
	IntituleLig	= NULL ;
    }


TCheopsSupport :: ~TCheopsSupport ( )
   {
	for  ( int  i = 0 ; i < NbDialog ; i ++ )
	   {
		if  ( DialogBox [i] )
			delete  DialogBox [i] ;
	    }

	if  ( pIntituleOnglet )
		delete []  pIntituleOnglet ;

	if  ( IntituleLig )
		delete  IntituleLig ;

	delete [] DialogBox ;
    }




/**************************************************************************/
/*                                                                        */
/*  TCheopsSupport :: Create                                              */
/*      Effectue la création de la fenêtre cliente de TCheopsDialog,      */
/*      puis crée la ou les boîtes de dialogue, ainsi que les touches     */
/*      de fonction, la barre d'icônes, et la barre de messages.          */
/*      Ajuste la taille de la fenêtre conformément.                      */
/*                                                                        */
/*  Remarque : lorsqu'il y a une barre de messages ou d'icônes, celle-ci  */
/*      deborde de la fenêtre de 1 pixel. En haut, donc, la barre         */
/*      démarre à la coordonnée -1,-1. En bas, elle déborde sur la        */
/*      bordure. Pour éviter de prendre en compte ce pixel caché qui      */
/*      fausse le calcul de la hauteur totale, on fait -1 sur une         */
/*      coordonnée.                                                       */
/*                                                                        */
/**************************************************************************/

BOOL	TCheopsSupport :: Create ()
   {
	BOOL			Status ;
	TRect			DialogBoxRect   ( 0, 0, 0, 0 ),
				FunctionKeyRect ( 0, 0, 0, 0 ),
				OngletRect 	( 0, 0, 0, 0 ) ;
	int			Delta  =  0 ;
	char			HideOnglet	=  FALSE ;


// Création de la fenêtre cliente et des boîtes de dialogue
	Status = TWindow :: Create ( ) ;


	for  ( int  i = 0 ; i < NbDialog ; i ++ )
		DialogBox [i] -> Create ( ) ;

	DialogBox [0] -> GetClientRect ( DialogBoxRect ) ;
	HDialogBox = DialogBoxRect. Height ( ) ;


// Mettre les boîtes à la bonne origine
	for  ( i = 0 ; i < NbDialog ; i ++ )
	   {
		TRect		Rect ;


		Rect. top    = GetPosition ( ENTETE ) ;
		Rect. left   = 0 ;
		Rect. right  = DialogBoxRect. right - DialogBoxRect. left ;
		Rect. bottom = DialogBoxRect. bottom - DialogBoxRect. top ;

		DialogBox [i] -> MoveWindow ( Rect, FALSE ) ;
	     }


// Création de l'onglet si nécessaire
	if  ( NbDialog > 1 )
	   {
		Onglet =  new TOnglet ( this, pIntituleOnglet ) ;

		if  ( strlen ( pIntituleOnglet )  ==  NbDialog -1 )
			HideOnglet = TRUE ;

		if  ( pIntituleOnglet )
		   {
			delete []  pIntituleOnglet ;
			pIntituleOnglet = 0 ;
		    }

		Onglet -> GetWindowRect ( OngletRect ) ;

		if  ( ! HideOnglet ) 
			HOnglet = OngletRect. Height ( ) ;
	    }


// Création des touches de fonction si nécessaire
	if  ( DecoratedFrame -> OldFunctionKeys )
	   {
		FunctionKeys = new TCheopsFunctionKeys ( DecoratedFrame, this,
					DialogBoxRect. right - DialogBoxRect. left,
						DialogBox [0] -> ControlStyle ) ;
		FunctionKeys -> Attr. X = 0 ;
		FunctionKeys -> Attr. Y = GetPosition ( BOUTONS ) ;
		FunctionKeys -> Create ( ) ;
		FunctionKeys -> GetWindowRect ( FunctionKeyRect ) ;
	     }



// Calcul de la taille finale de la fenêtre
	int		Width		=  0,
			Height		=  0 ;


// Calcul de la largeur
	Width	=  DialogBoxRect. Width ( ) + Delta ;


// Calcul de la hauteur.
	Height  =  DialogBoxRect.   Height ( ) +
		   FunctionKeyRect. Height ( ) ;

	if  ( ! HideOnglet )
		Height += OngletRect. Height ( ) ;

// Paramètres pour le minmax info de la TCheopsDialog
	DecoratedFrame -> MaxWidth	=  Width ;
	DecoratedFrame -> MaxHeight  	=  Height ;

// Et voilou !
	return ( Status ) ;
    }





/*
 * 	On surcharge eds fonction de base de la classe TWindow  pour effectuer
 *	des traitements supplémentaires avant d'envoyer les messages au différents
 *	contrôle.
 *	Elle permet notamment de rediriger les messages celon leurs destinataires
 *	SendDlgItemMessage ( )
 *	GetDlgItem ( )
 *	GetDlgItemText ( )
 */
LRESULT	TCheopsSupport :: SendDlgItemMessage ( int ControlId, uint msg, WPARAM wp, LPARAM lp )
   {
	int 	Id = ( ControlId - ControlId % 100 ) /100,
		i = Id ;
	LRESULT Status = NULL ;

	switch  ( msg )
	   {
		case  WM_ENABLE :
			if  ( ! Id )
				for  ( i = 0 ; i < NbDialog ; i ++ )
					EnableTextItem ( DialogBox [i] -> GetDlgItem ( ControlId ), wp ) ;
			else
					EnableTextItem ( DialogBox [ i - 1 ] -> GetDlgItem ( ControlId ), wp ) ;
			break ;


		case	CB_SHOWDROPDOWN :
		case	CB_GETCURSEL :
			if  ( ! Id  &&  Onglet )
			   {
				i = Onglet -> actif ( ) ;
				Status = DialogBox [i] -> SendDlgItemMessage ( ControlId, msg, wp, lp ) ;
				break ;
			    }

		default :
			if  ( ! Id )
				for  ( i = 0 ; i < NbDialog ; i ++ )
					Status = DialogBox [i] -> SendDlgItemMessage ( ControlId, msg, wp, lp ) ;
			else
				Status = DialogBox [ i - 1 ] -> SendDlgItemMessage ( ControlId, msg, wp, lp ) ;
	    }

	return ( Status ) ;
   }

HWND	TCheopsSupport :: GetDlgItem ( int ControlId )
   {
	int 	Id = ( ControlId - ControlId % 100 ) /100 ;

	Onglet -> VirtualClick ( Id -1, ControlId ) ;
	if  ( ! Id )
		return ( DialogBox [ Onglet -> actif () ] -> GetDlgItem ( ControlId ) ) ;
	/* else */
	return ( DialogBox [ Id -1 ] -> GetDlgItem ( ControlId ) ) ;
    }

uint	TCheopsSupport :: GetDlgItemText ( int ControlId, char far * text, int max)
   {
	int 	Id = ( ControlId - ControlId % 100 ) /100,
		i ;
	uint	Status ;


	if  ( ! Id )
	   {
		Id = Onglet -> actif ( ) ;
		Status = DialogBox [ Id ] -> GetDlgItemText ( ControlId, text, max ) ;
		for  ( i = 0 ; i < NbDialog ; i ++ )
			DialogBox [i] -> SetDlgItemText ( ControlId, text ) ;
	    }
	else
		Status = DialogBox [ Id -1 ] -> GetDlgItemText ( ControlId, text, max )  ;

	return ( Status ) ;
    }







/**************************************************************************/
/*							  		  */
/*     TCheopsDialog :: AddWinLIgnes                                      */
/*         Ajoute une partie ligne à la boite de dialogue                 */
/*                                                                        */
/**************************************************************************/

TWinLine *	TCheopsSupport :: AddWinLignes ( TUnixMessage * Message )
   {
	UTCheopsGroupe *	InfoBloc = ( UTCheopsGroupe * ) ( Message -> Data +
						 sizeof ( UTCheopsDialog ) ) ;
	unsigned char *		Data,
		      *		p ;
	int			DataSize,
				offset,
				Nb ;
	register BOOL		FirstTime 	=  ( WinLine  ==  0 ) ;
	register int		ScrollWidth 	= GetSystemMetrics ( SM_CYHSCROLL ) ;



	CHARtoI16 ( Nb, DATA ( NbBox, UTCheopsDialog ) ) ;
	if  ( Nb  !=  2 )
		return ( NULL ) ;

	for  ( int i = 0; i < Nb; i ++ )
	   {
		CHARtoI16 ( offset, InfoBloc -> offset ) ;
		Data = ( unsigned char * ) ( Message -> Data + offset ) ;
		CHARtoI16 ( DataSize, InfoBloc -> size ) ;
		if  ( i  ==  0 )
		   {
			IntituleLig = new TUnixDialogBox ( UnixApplication, this,
				Data,DataSize,Message->ProcessId, TRUE,DecoratedFrame );
			IntituleLig -> SetObjectId ( ObjectId ) ;
		     }
		else
			WinLine = new TWinLine ( this, Data, DataSize,
				UnixApplication, DialogBox [0], DecoratedFrame ) ;

		p = ( unsigned char * ) InfoBloc ;
		p += sizeof ( UTCheopsGroupe ) ;
		p += strlen ( InfoBloc -> intitule ) ;
		InfoBloc = ( UTCheopsGroupe * ) p ;
	    }

	TRect rect, WindowRect, IntituleRect ;

	IntituleLig -> Create() ;
	IntituleLig -> Show (1) ;
	IntituleLig -> GetWindowRect (IntituleRect ) ;
	HIntituleLig = IntituleRect. bottom - IntituleRect. top ;


	WinLine -> Create ( ) ;
	WinLine -> CreerChamps ( 0, NBLIGNEMAX ) ;


// on ajuste la fenetre à sa nouvelle taille (définitive !!!!!!!)
	int Htotale = WinLine -> HauteurLigne + INTERLIGNE ;
	HWinLine = ( NBLIGNEMAX * Htotale ) +
			( 8 * GetSystemMetrics ( SM_CYBORDER ) ) ;

// Créer un borshade
	:: CreateWindow ( "BorShade", "",
			BSS_GROUP | BSS_CAPTION | BSS_LEFT |	
			WS_CHILD | WS_VISIBLE,
			0, 0, WinLine -> LargeurLigne, HWinLine,
			WinLine -> WinlineClient -> HWindow,
			NULL, * GetModule ( ), NULL ) ;
				

// Taille de la boîte de dialogue
	TRect		CheopsRect ;

	
	DecoratedFrame -> GetClientRect ( CheopsRect ) ;

	if  ( CheopsRect. Width ( )  >=  WinLine -> LargeurLigne )
		ScrollWidth = 0 ;


// on recalcule la taille maximale pour la fenêtre principale (TCheopsDialog)
	WinLine -> Show ( 1 ) ;

	if  ( FirstTime )
	   {	
		DecoratedFrame -> MaxHeight = Attr. H + HWinLine + HIntituleLig + ScrollWidth ;
		DecoratedFrame -> SetClientSize ( Attr. W,
			Attr. H + HWinLine + HIntituleLig + ScrollWidth ) ;
	    }
	
	DecoratedFrame -> MaxWidth  = Attr. W ;

// on positionne la partie ligne
// d'abord l'intitulé
	int		width = DecoratedFrame -> MaxWidth ;

	IntituleLig -> MoveWindow ( 0, GetPosition ( INTITULELIG ),
			 IntituleRect. Width ( ), HIntituleLig ) ;
	IntituleLig -> Invalidate ( ) ;

// puis les lignes
	WinLine -> MoveWindow ( 0 , GetPosition ( LIGNE ), width, HWinLine ) ;
	WinLine -> WinlineClient -> MoveWindow ( 0, 0, WinLine -> LargeurLigne, HWinLine ) ;
	WinLine -> Invalidate ( ) ;
	WinLine -> SetScrollRange ( SB_VERT, 0, 0 ) ;

// Mettre une scrollbar horizontale si la largeur de la partie lignes excède
// celle de la boître de dialogue
	if  ( CheopsRect. Width ( )  <  WinLine -> LargeurLigne )
	   {
		TPoint		ScreenPoint ( CheopsRect. right, 0 ) ;

	// Comme on utilise des coordonnées écran pour déterminer la position
	// d'un champ, il faut convertir le point le plus à droite de 
	// CheopsRect (exprimé en coordonnées client) en coordonnées écran
		DecoratedFrame -> ClientToScreen ( ScreenPoint ) ;

		for  ( int  field = 1 ; field <=  WinLine -> NbChamps ; field ++ )
		   {
			TRect		Rect ;
			HWND		hwnd  =  WinLine -> WinlineClient -> GetDlgItem ( field ) ;


			:: GetWindowRect ( hwnd, & Rect ) ;

			if  ( Rect. right  >  ScreenPoint. x )
			   {
				if  ( field  >  1 )
					field -- ; // ce champ-ci est à moitié visible...
				WinLine -> VisibleFields = field ;
				break ;
			     }
		     }
	      }

/***
	Initialement, le code suivant définissait x positions pour le curseur
	de la scrollbar si une ligne possédait x champs sur une largeur totale
	dépassant celle de la fenetre. Cela impliquait qu'on pouvait scroller
	horizontalement jusqu'à ne plus apercevoir q'un seul champ.

	if  ( CheopsRect. Width ( )  <  WinLine -> LargeurLigne )
	   {
		WinLine -> SetScrollRange ( SB_HORZ, 1, WinLine -> NbChamps ) ;
		WinLine -> MoveWindow ( 0, GetPosition ( LIGNE ), width,
			HWinLine + ScrollWidth ) ;
	     }
	else
		WinLine -> SetScrollRange ( SB_HORZ, 0, 0 ) ;

	Ce code a été remplacé par le suivant, qui ne définit comme positions 
	possibles que le nombre de champs non visibles entièrement.
 ***/


	if  ( WinLine -> VisibleFields  ==  WinLine -> NbChamps )
		WinLine -> SetScrollRange ( SB_HORZ, 0, 0 ) ;
	else
	   {
		WinLine -> SetScrollRange ( SB_HORZ, WinLine -> VisibleFields, 
			WinLine -> NbChamps ) ;
		WinLine -> MoveWindow ( 0, GetPosition ( LIGNE ), width,
			HWinLine + ScrollWidth ) ;
	     }
	    
// on repositionne correctement les touches
	if  ( DecoratedFrame -> OldFunctionKeys )
		FunctionKeys -> MoveWindow ( 0, GetPosition ( BOUTONS ),
					FunctionKeys -> Attr. W, FunctionKeys -> Attr. H ) ;

	return ( WinLine ) ;
    }

int	TCheopsSupport :: GetPosition ( int Id )
   {
	switch  ( Id )
	   {
		case  ENTETE :
			return 0;
		case  ONGLET :
			return ( HDialogBox ) ;
		case  INTITULELIG :
			return ( HDialogBox + HOnglet ) ;
		case  LIGNE :
			return ( HDialogBox + HOnglet + HIntituleLig ) ;
		case  BOUTONS :
			return ( HDialogBox + HOnglet + HIntituleLig + HWinLine ) ;

		default :
			return -1;
	    }
    }


/**************************************************************************/
/*                                                                        */
/*  Classe TOnglet : 	                                                  */
/*	Gere le changement de boite de dialogue dans la partie entete.	  */
/*                                                                        */
/**************************************************************************/

TOnglet :: TOnglet ( TWindow * parent, char * pIntitule ) : TWindow ( parent,"Onglet" )
   {
	TRect	DialogRect ;
	BOOL	HardCopyMode ;

	Support = ( TCheopsSupport * ) parent ;
	LastControlId = Last = 0 ;

	Attr. Style = WS_CHILD ;
	Attr. Style    &= ~ ( WS_VISIBLE ) ;
	Attr. X = 0 ;
	Attr. Y = Support -> GetPosition ( ONGLET ) ;

	Support -> DialogBox [0] -> GetWindowRect ( DialogRect ) ;
	Attr. W = DialogRect. right - DialogRect. left ;

	Attr. H = 21 ;

	Create ( ) ;
	Gadget = new TVbxControl ( this, ID_ONGLET, "SAXTABS.VBX", "SaxTab", pIntitule,
			0, 0, Attr. W, Attr. H ) ;
	Gadget -> Create ( ) ;

// Changer les couleurs si on veut faire des hardcopies
	HardCopyMode = Configuration -> GetBoolean ( "Settings", "HardCopyMode", FALSE ) ;

	Gadget -> SetProp ( "ActiveTextColor", 0x000000L ) ;

	if  ( HardCopyMode )
	   {
		Gadget -> SetProp ( "ActiveTabColor", 0xFFFFFFL ) ;
		Gadget -> SetProp ( "BackColor", 0xFFFFFFL ) ;
		Gadget -> SetProp ( "InactiveTabColor", 0xC0C0C0L ) ;
	    }
	else
	   {
		Gadget -> SetProp ( "ActiveTabColor", 0xc0c0c0L ) ;
		Gadget -> SetProp ( "BackColor", 0x808080L ) ;
		Gadget -> SetProp ( "InactiveTabColor", 0xa0a0a0L ) ;
	     }

	Gadget -> SetProp ( "InactiveTextColor", 0 ) ;
    }

/**************************************************************************/
/*                                                                        */
/*  TOnglet :: EvClick 	                                                  */
/*	L'Onglet à été activé pour changer de boite de dialogue.	  */
/*  TOnglet :: VirtualClick						  */
/*	Changement de la boite de dialogue par forcage interne.		  */
/*                                                                        */
/**************************************************************************/

void	TOnglet :: EvClick  ( VBXEVENT FAR * /* event */ )
   {
	int	activetab ;

	Gadget -> GetProp ( "ActiveTab", activetab ) ;
	if  ( activetab  !=  Last )
	   {
		Support -> LockWindowUpdate ( ) ;
		Support -> DialogBox [ Last ] -> Show ( 0 ) ;
		Support -> DialogBox [ activetab ] -> Show ( 1 ) ;

		if  ( LastControlId  &&  LastControlId  < 100 )
		   {
			char	text [ 100 ] ;

			Support -> GetDlgItemText ( LastControlId, text, sizeof ( text ) ) ;
			Last = activetab ;
			Support -> SetDlgItemText ( LastControlId, text ) ;
			::SetFocus ( Support -> DialogBox [ activetab ] -> GetDlgItem ( LastControlId ) ) ;
		    }

		Support -> DialogBox [ activetab ] -> UpdateWindow ( ) ;
		::LockWindowUpdate ( NULL ) ;

		Last = activetab ;
	    }
    }

void	TOnglet :: VirtualClick ( int activetab, int ControlId )
   {
	if  ( ControlId )
		LastControlId = ControlId ;

	if  ( activetab  <=  0 )       // Calcul si necessaire
		if  ( ControlId >= 100 )
			activetab = ( ControlId - ControlId % 100 ) / 100 - 1 ;
		else
			return ;

	if  (activetab >= 0  &&  activetab != Last  &&  activetab < Support -> GetNbBox () )
	   {
		Support -> LockWindowUpdate ( ) ;
		Support -> DialogBox [ Last ] -> Show ( 0 ) ;
		Support -> DialogBox [ activetab ] -> Show ( 1 ) ;
		Gadget -> SetProp ( "ActiveTab", activetab ) ;
		::LockWindowUpdate ( NULL ) ;

		Last = activetab ;
	    }
    }





/**************************************************************************/
/*                                                                        */
/*  Constructeur du TCheopsControlBar                                     */
/*                                                                        */
/**************************************************************************/

// Crée un bouton mais empêche qu'il soit déssiné
class	TDummyButtonGadget	:  public  TButtonGadget 
  {
	public :
		TDummyButtonGadget ( )
			: TButtonGadget ( TResId ( "Dummy" ), Control_Bar_Base,
						TButtonGadget :: Command, TRUE )
		   { }



		void	Paint ( TDC & )
		   { }
    } ;





TCheopsControlBar :: TCheopsControlBar   ( TCheopsDialog *	parent,
					   TTileDirection	direction,
		      			   TFont *		font )
	:  TControlBar ( parent, direction, font )
   {
	CheopsDialog	=  parent ;
	memset ( ButtonGadgets, 0, sizeof ( ButtonGadgets ) ) ;

	Margins. Left = Margins. Right = 6 ;
	Margins. Top = Margins. Bottom = 4 ;


// Créer un gadget de la même taille qu'un bouton de manière à ce que le
// premier affichage de la barre soit avec la bonne hauteur
	DummyGadget  = new  TDummyButtonGadget ( ) ;

	DummyGadget -> SetEnabled ( TRUE ) ;
	DummyGadget -> SetBorderStyle ( TButtonGadget :: Embossed ) ;
	Insert ( * DummyGadget ) ;
   }


/**************************************************************************/
/*                                                                        */
/*  Destructeur du TCheopsControlBar                                      */
/*                                                                        */
/**************************************************************************/

TCheopsControlBar :: ~TCheopsControlBar  ( )
   {
	for  ( int  i = 0 ; i < MAX_CHEOPS_FUNCTION_KEYS ; i ++ )
	   {
		if  ( ButtonGadgets [i] )
			delete  ButtonGadgets [i] ;
	    }

	delete  DummyGadget ;
     }



/**************************************************************************/
/*                                                                        */
/*  TCheopsControlBar :: SetFunctionKey -                                 */
/*      Définit une touche de fonction (un bouton gadget) dans la barre   */
/*      d'icônes.                                                         */
/*                                                                        */
/**************************************************************************/

void	TCheopsControlBar :: SetFunctionKey  ( int		id,
					       char *		text,
					       char *		help_text )
   {
	char			ResourceName [ 128 ] ;
	register char *		p,
		      *		q ;



// Contrôle
	if  ( id  <  1  ||  id  >  MAX_CHEOPS_FUNCTION_KEYS )
		return ;
	id -- ;



// Trouver le nom de la resource bitmap du bouton après élimination des 
// blancs
	p = text, q = ResourceName ;

	while  ( * p )
	   {
		if  ( strchr ( "ÀÁÂÃÄÅÆàáâãäåæ", tolower ( * p ) )  !=  NULL )
			* q ++ = 'a' ;
		else if  ( strchr ( "ÈÉÊËèéêë", tolower ( * p ) )  !=  NULL )
			* q ++ = 'e' ;
		else if  ( strchr ( "ÌÍÎÏìíîï", tolower ( * p ) )  !=  NULL )
			* q ++ = 'i' ;
		else if  ( strchr ( "ÒÓÔÕÖØòóôõöø", tolower ( * p ) )  !=  NULL )
			* q ++ = 'o' ;
		else if  ( strchr ( "ÙÚÛÜùúûü", tolower ( * p ) )  !=  NULL )
			* q ++ = 'u' ;
		else if  ( strchr ( "Ýýÿ", tolower ( * p ) )  !=  NULL )
			* q ++ = 'y' ;
		else if  ( strchr ( "Ýýÿ", tolower ( * p ) )  !=  NULL )
			* q ++ = 'y' ;
		else if  ( strchr ( "Çç", tolower ( * p ) )  !=  NULL )
			* q ++ = 'c' ;
		else if  ( isalpha ( * p )  ||
				( p != text  &&  isdigit ( * p ) ) )
			* q ++ = * p ;

		p ++ ;
	    }
	* q = 0 ;


// Si le nom est vide, il faut supprimer le machin
	if  ( q  ==  ResourceName )
	   {
		if  ( ! ButtonGadgets [id]  )
			return ;		// Suppression d'un supprimé inutile

		Remove ( * ButtonGadgets [id] ) ;
		delete  ButtonGadgets [id] ;		
		ButtonGadgets [id] = 0 ;
	     }
	else
	   {
	// Supprimer l'éventuel ancien gadget
		if  ( ButtonGadgets [id] )
		   {
			Remove ( * ButtonGadgets [id] ) ;
			delete  ButtonGadgets [id] ;
		    }

	// Créer le bouton
		ButtonGadgets [id] = new  TCheopsButtonGadget ( 
						CheopsDialog,
						TResId ( ResourceName ),
						id + Control_Bar_Base, text, help_text ) ;
		ButtonGadgets [id] -> SetEnabled ( TRUE ) ;
		ButtonGadgets [id] -> SetBorderStyle ( TButtonGadget :: Embossed ) ;
	    }
							

// Si la fonction a été appelée sans BeginUpdate, on appelle EndUpdate pour
// tout mettre à jour
	if  ( ! InUpdate )
		EndUpdate ( ) ;
    }




/**************************************************************************/
/*                                                                        */
/*   TCheopsControlBar :: EndUpdate -                                     */
/*      Remet à jour la barre d'icônes en :                               */
/*                                                                        */
/*      . Supprimant tous les gadgets                                     */
/*      . En remettant les nouveaux                                       */
/*      . En appelant LayoutSession pour la redessiner.                   */
/*                                                                        */
/*                                                                        */
/**************************************************************************/

void	TCheopsControlBar :: EndUpdate  ( void )
   {

// Supprimer tous les gadgets de la fenêtre
	TGadget *		Gadget 	=  FirstGadget ( ) ;
	TGadget *		Temp ;



// Supprimer les gadgets alloués
	while  ( Gadget )
	   {
		Temp = NextGadget ( * Gadget ) ;
		Remove  ( * Gadget ) ;
		if ( typeid ( * Gadget )  ==  typeid ( TSeparatorGadget ) )
			delete  Gadget ;
		Gadget = Temp ;
	    }



// Reconstruire la babarre
	TSeparatorGadget *	Separator ;
	int			Width	    	=  6 ;
	int			ButtonWidth ;
	TSize			ButtonSize ;
	register int		i ;


	ButtonSize = DummyGadget -> GetBounds ( ). Width ( ) ;

	
	for  ( i = 0 ; i < MAX_CHEOPS_FUNCTION_KEYS ; i ++ )
	   {
		if  ( ButtonGadgets [i] )
		   {
			Insert ( * ButtonGadgets [i] ) ;
			ButtonWidth = 0 ;
		    }
		else
			ButtonWidth = ButtonSize. cx ;

		Separator = new  TSeparatorGadget ( Width + ButtonWidth ) ;
		Separator -> SetBorderStyle ( TGadget :: Raised ) ;
		Insert ( * Separator ) ;
	     }


// Redessiner
	InUpdate = FALSE ;
	Invalidate ( ) ;
	LayoutSession ( ) ;
    }                     





/**************************************************************************/
/*                                                                        */
/*  Constructeur de TCheopsButtonGadget.                                  */
/*  Détermine si la bitmap est interne ou externe.                        */
/*                                                                        */
/**************************************************************************/

TCheopsButtonGadget :: TCheopsButtonGadget  ( 
		TCheopsDialog *		frame,
		TResId			resource_id,	
		int			id,
		char *			text,
		char *			help_text )
	: TButtonGadget ( resource_id, id, TButtonGadget :: Command, TRUE ) 
  {
	char			buffer [ MAX_PARAMETER_LENGTH ] ;
	register char *	        p,
		      *		q ;


	CommandId 	 =  id ;
	CheopsDialog	 =  frame ;
	CheopsControlBar =  frame -> ControlBar ;
	StatusBar        =  frame -> StatusBar ;
	Text		 =  strdup ( text ) ;
	HelpText	 =  ( char * ) malloc ( strlen ( help_text ) + 32 ) ;

	wsprintf ( HelpText, "(F%d)    %s", id - Control_Bar_Base + 1,
			help_text ) ;
	Beautify ( Text ) ;
	Beautify ( HelpText ) ;
	TrackMouse = FALSE ;

// Déterminer si la bitmap est externe ou pas
	Configuration -> GetApplicationDirectory ( buffer ) ;
	strcat ( buffer, ( char * ) resource_id ) ;
	
	p = buffer + strlen ( buffer ) - 1 ;

// Chercher s'il faut ajouter un suffixe .BMP
	q = NULL ;

	while  ( p  >=  buffer  &&  strchr ( "/\\:", * p )  !=  NULL )
	   {
		if  ( * p  ==  '.' )
		   {
			q = p ;
			break ;
		     }

		p -- ;
	     }

// Tronquer le nom de fichier à 8 caractères
	if  ( q )
		* q = 0 ;
	p ++ ;
	p [8] = 0 ;

	if  ( q )
		* q = '.' ;	// remettre le point
	
	if  ( q  ==  NULL )
		strcat ( buffer, ".BMP" ) ;
	

// Le fichier existe-t-il ?
	if  ( access  ( buffer, 0 )  <  0 )
	   {
		BitmapFile = 0 ;

	// Non. Vérifier que la ressource existe dans l'exécutable.
	// Si la ressource n'existe pas, on en fournit une par défaut
		HRSRC	hr	=  :: FindResource ( * :: Module,
					resource_id, RT_BITMAP ) ;


		if  ( hr  ==  NULL )
		   {
		// OWL duplique l'identificateur au moyen de strnewdup.
		// Il faut libérer l'ancienne id et y assigner une nouvelle de
		// la meme manière 
			if  ( ResId. IsString ( ) )
				delete ( char far * ) ResId ;
			ResId = strnewdup ( "Unknown" ) ;
		     }
           }
	else
		BitmapFile = strdup ( buffer ) ;
   }





/**************************************************************************/
/*                                                                        */
/*  TCheopsButtonGagdet :: GetGlyphDib -                                  */
/*      Surcharge la fonction de TButtonGadget afin de permettre la       */
/*      lecture d'une bitmap extern au programme.                         */
/*                                                                        */
/**************************************************************************/

TDib *	TCheopsButtonGadget :: GetGlyphDib  ( )
   {
	if  ( BitmapFile ) 	// Bitmap externe
	   {
		TDib *	glyph = new  TDib ( BitmapFile ) ;

	// Laisser faire OWL si le DIB ne peut etre créé
		if  ( glyph )
		   {
			glyph -> MapUIColors ( TDib :: MapFace		||
					       TDib :: MapText		||
					       TDib :: MapShadow	||
					       TDib :: MapHighlight ) ;

			return  ( glyph ) ;
		      }		
	    }


	return  ( TButtonGadget :: GetGlyphDib ( ) ) ;
    }
