/**************************************************************************/
/*                                                                        */
/*  Cheops.h  -  Christian Vigh, 94/11/16.                                */
/*                                                                        */
/*  Classes pour la gestion des écrans de saisie Cheops.                  */
/*                                                                        */
/**************************************************************************/



# define	MAX_CHEOPS_FUNCTION_KEYS	8
# define	START_ID			8888
# define	KEYHANDLER(x)			void  FKEY##x ( void ) { CmCommand ( x ) ; }
# define	WM_FUNCTIONKEY			( WM_USER + 100 )

# define	ENTETE		0
# define	ONGLET		1
# define	INTITULELIG	2
# define	LIGNE		3
# define	BOUTONS		4


class	TCheopsButton ;

class	TCheopsFunctionKeys  :  public  TWindow
   {
	private :
		TCheopsButton *	Buttons [ MAX_CHEOPS_FUNCTION_KEYS ] ;
		char *		Help    [ MAX_CHEOPS_FUNCTION_KEYS ] ;
		TControlStyle	ControlStyle ;
		TWindow *    	TheClient ;
		TWindow *    	DecoratedFrame ;
		int	     	ButtonWidth,
				ButtonHeight,
				ButtonHInterval,
				ButtonVInterval ;


	public :
		TCheopsFunctionKeys ( TWindow *		frame,
				TWindow *  	parent,
				int	 	width,
				TControlStyle 	style ) ;

		virtual BOOL	PreProcessMsg 	      ( MSG &  msg ) ;
		void		SetLabel    	      ( int  keynum, char *  label,
							char *  help ) ;
		virtual void	SetupWindow 	      ( void ) ;
		void	     	ArrangeButtons        ( int  newwidth ) ;
		void	     	ComputeButtonsSize    ( int  width ) ;
		void	     	ComputeButtonPosition ( int  button,
							int& x,
							int& y ) ;

		void	CmCommand ( WPARAM  id ) ;
		KEYHANDLER ( 0 ) ;
		KEYHANDLER ( 1 ) ;
		KEYHANDLER ( 2 ) ;
		KEYHANDLER ( 3 ) ;
		KEYHANDLER ( 4 ) ;
		KEYHANDLER ( 5 ) ;
		KEYHANDLER ( 6 ) ;
		KEYHANDLER ( 7 ) ;

		void		Show ( int  cmd )
		   {
				TWindow :: Show ( cmd ) ;
				TheClient -> Show ( cmd ) ;
			 }

	DECLARE_RESPONSE_TABLE ( TCheopsFunctionKeys ) ;
    } ;


class TCheopsDialog ;
class TChoiceArray ;
class TCheopsSupport ;
class TWinlineClient ;

# define INTERLIGNE	1
# define NBLIGNEMAX	8
# define OFFSET_ID	32000

typedef struct line
	{
	char **		Tab ;
	struct line *	suiv ;
	 }  TLine ;

class TWinLine : public TWindow
	{
	typedef struct
		{
		int		X ;
		int		Y ;
		int		CX ;
		int		CY ;
		long int	Style ;
		char		Classe ;
		TChoiceArray *	Ptr ;
		 }  InfoChamps ;


	public :
		TUnixApplication *	Application ;
		TWinlineClient * 	WinlineClient ;
		int			ObjectId ;
		int   			NbChamps ;        // nb de champs d'une ligne
		int			VisibleFields ;   // Nombre de champs visibles

	public :
		TWinLine	( TWindow *		parent,
				  unsigned char * 	data,
				  int			datasize,
				  TUnixApplication *	App,
				  TUnixDialogBox *	dial,
				  TCheopsDialog *	support);

		~TWinLine()
			{
			ApplicationTable -> RemoveObject ( Application -> ApplicationId,
				this ) ;
			::DeleteObject ( FontHandle ) ;
			DetruireData ( ) ;

			if  ( Controle )
				delete []  Controle ;
			 };

	// Efface l'objet de la table des applications
		void	EvClose  ( void )
		   {
			TWindow :: EvClose ( ) ;
		    }



		BOOL	Create  ( void ) ;
		void	DetruireData ( ) ;


		virtual bool	PreProcessMsg	( MSG &	msg);
		void 		RecevoirLignes	( int	nb);
		void 		DecodePos	( unsigned char * Data, int size);
		void 		SetObjectId	( int	Id)
			{
			ObjectId = Id ;
			 } ;
		void 		InitTab		( int	Id,char *texte ) ;
		void		EvVScroll	( UINT, UINT, HWND ) ;
		void		EvHScroll	( UINT, UINT, HWND ) ;
		LRESULT		EvCommand	( uint, HWND, uint ) ;
		uint		GetDlgItemText	( int Id, char far * text, int max) ;
		void 		InsererLigne	( int	nb, int	pos ) ;
		HWND 		DemandeFocus	( int	Id ) ;
		void 		CreerChamps	( int	debL, int	finL ) ;
		void		EnableColonne	( int	Id, int	Etat ) ;
		void		SupprimerLigne	( int	nb, int	pos ) ;
		void 		EffacerLigne	( int	nb, int	pos ) ;
		void		AfficherChoice	( int	Id ) ;
		void		HandleFocus	( int   id ) ;

		TCheopsDialog *	Support;        // ptr -> sur la fenetre support (sert à redimensionnement)
		int		HauteurLigne;    // hauteur d'une ligne en pixel
		int		LargeurLigne ;
		BOOL		ArrayVisible; // indique si un tableau est affiché

		LRESULT		SendDlgItemMessage(int childid,uint msg, WPARAM wp=0,LPARAM lp=0);

	private :
		void	DetruireLigne ( TLine * lig ) ;
		void	AllouerData ( int n ) ;
		char *	TextId ( int Id ) ;
		int   	LigAff ( int num )   // teste si une ligne est visible
		   {
			return ( num > NBLIGNEMAX ? NBLIGNEMAX : num ) ;
		    } ;

		void	MiseAjourId_M ( int ligne, int nl ) ;
		void	MiseAjourId_D ( int ligne, int nl ) ;
		void	MiseAjourId ( int Id, int NewId ) ;
		void	AfficherText ( ) ;
		TLine *	CreerLigne ( ) ;
		TLine *	GetLine ( int n, BOOL precedente = FALSE ) ;
		BOOL	ALecran ( int Id ) ;


		InfoChamps *		Controle ;       // tableau sur la position des champs
		int 			NbLigCourantes ;  // nb de lignes dans la fenetre
		HFONT 			FontHandle ;      // pointeur sur la font utilise dans les champs
		TUnixDialogBox *	Dial ;           // ne sert que pour la convertion des coorconnées
		int                     FirstLine ;	 // premiere ligne affichée
		TLine *			DataLine ;	 // Ptr -> tableau contenant le texe
							 //  de chacun des champs
		TLine *			FastPtr ;        // Ptr servant à optimiser l'accés
							 // au données ds le tableau
		int			FastInd ;         // Indice correspondant à FastPtr

	DECLARE_RESPONSE_TABLE ( TWinLine ) ;
    } ;

class TOnglet : public TWindow, public TVbxEventHandler
	{
	public :
		int 	LastControlId ;

		TOnglet	( TWindow * parent, char * pintitule ) ;

		int	actif ( )
		   {
			return ( Last ) ;
		   }
		void	Show ( int cmd )
		   {
			TWindow :: Show ( cmd ) ;
			if  ( Gadget )
				Gadget -> Show ( cmd ) ;
		    }
		void	EvClick ( VBXEVENT FAR * event ) ;
		void	VirtualClick ( int activetab, int ControlId ) ;

	private :
		TCheopsSupport *	Support ;
		TVbxControl *		Gadget ;
		int			Last ;


	DECLARE_RESPONSE_TABLE ( TOnglet ) ;
    } ;


class	TCheopsSupport	:  public  TWindow
   {
	public :
		TUnixDialogBox **	DialogBox,
			       *	IntituleLig ;
		TWinLine *		WinLine ;
		TOnglet *		Onglet ;
		TCheopsFunctionKeys *	FunctionKeys ;
		TCheopsDialog *		DecoratedFrame ;

	private :
		 int 			HDialogBox,
					HWinLine,
					HOnglet,
					HIntituleLig,
					NbDialog ;
		 TUnixApplication *	UnixApplication ;
		 char * 		pIntituleOnglet ;
		 int			ObjectId ;


	public :
		TCheopsSupport	( TCheopsDialog *	parent,
				  TUnixMessage *	message,
				  TUnixApplication *	app ) ;
		~TCheopsSupport ( ) ;

		virtual BOOL 	Create ( ) ;

		void	SetObjectId  ( int  Id )
			{
			ObjectId = Id ;
			  }


# pragma  warn  -inl
		void 		Show ( int cmd )
			{
			if  ( Onglet )
			  {
				SetWindowVisible ( Onglet ) ;
				Onglet -> Show ( cmd ) ;
				SetWindowVisible ( DialogBox [ Onglet -> actif ( ) ] ) ;
				DialogBox [ Onglet -> actif () ] -> Show ( cmd ) ;
				}
			else
				{
				SetWindowVisible ( DialogBox [0] ) ;
				DialogBox [0] -> Show ( cmd ) ;
				 }

			if  ( WinLine )
				{
				SetWindowVisible ( WinLine ) ;
				WinLine -> Show ( cmd ) ;
				 }

			if  ( IntituleLig)
				{
				SetWindowVisible ( IntituleLig ) ;
				IntituleLig -> Show ( cmd ) ;
			    }

			if  ( FunctionKeys)
			   {
				SetWindowVisible ( FunctionKeys ) ;
				FunctionKeys -> Show ( cmd ) ;
			    }
		    };
# pragma  warn  .inl

		int   		GetNbBox ( )
		   {
			return ( NbDialog ) ;
		    }
		LRESULT		SendDlgItemMessage(int childid,uint msg, WPARAM wp=0,LPARAM lp=0);
		HWND		GetDlgItem ( int ControlID ) ;
		uint		GetDlgItemText ( int Id, char far * text, int max) ;

		TWinLine *	AddWinLignes	( TUnixMessage * message ) ;
		int		GetPosition	( int	 Id ) ;
    } ;








/**************************************************************************/
/*                                                                        */
/*  TCheopsControlBar -                                                   */
/*      Gère la barre d'icônes.                                           */
/*                                                                        */
/**************************************************************************/

# define	Control_Bar_Base 	START_ID


class	TCheopsButtonGadget ;
class	TCheopsDialog ;
class	TDummyButtonGadget ;


class	TCheopsControlBar 	: public  TControlBar
   {
	private :
		TCheopsButtonGadget *	ButtonGadgets [ MAX_CHEOPS_FUNCTION_KEYS ] ;
		TCheopsDialog *		CheopsDialog ;
		BOOL			InUpdate ;
		TDummyButtonGadget * 	DummyGadget ;


	public :
		TCheopsControlBar   ( TCheopsDialog *	parent,
				      TTileDirection	direction	=  Horizontal,
				      TFont *		font		=  new  TGadgetWindowFont ) ;

	       ~TCheopsControlBar  ( ) ;

		void	BeginUpdate  ( void )
		   {
			InUpdate = TRUE ;
		    }

		void	EndUpdate  ( void ) ;

		void	SetFunctionKey	( int		keynum,
					  char *	text,
					  char *	help_text ) ;

	} ;




class   TCheopsDialog	:  public  TDecoratedFrame
	{
	public :
		int			menu_id ;
		TMenu * 		menu ;
		TCheopsSupport *	Support ;
		int			MaxWidth,
					MaxHeight ;
		int			HelpId ;
		TStatusBar *		StatusBar ;
		TCheopsControlBar *   	ControlBar ;
		TFilletBar *		FilletBar ;
		BOOL			OldFunctionKeys ;
		BOOL			HasBeenDisplayed ;
		BOOL			IsModal ;
		BOOL			UserShutdown ;


	private :
		TUnixApplication *	Application ;
		TUnixMessage *		Message ;
		int			ObjectId ;

	public :
		TCheopsDialog  ( TUnixApplication *	app,
				 TWindow *		parent,
				 TUnixMessage *		message,
			         BOOL			is_modal 	=  FALSE ) ;

		~TCheopsDialog ( ) ;


		virtual BOOL	PreProcessMsg   ( MSG & ) ;
		virtual void	SetupWindow	( void ) ;

		virtual BOOL	Create 		( void ) ;

		void		SetClientSize	( int  w, int  h ) ;
		void		GetDecorationSize   ( int &  w, int &  h ) ;

		void		ProcessModalState  ( bool   newstate ) ;

		LRESULT	DoCommand    		( WPARAM, LPARAM ) ;
		LRESULT	NcActivate    		( WPARAM, LPARAM ) ;
		void	EvMenuSelect		( uint menuItemId, uint flags, HMENU hMenu) ;
		LRESULT EvCommand		( uint id, HWND hWndCtl, uint notifyCode) ;
		void	EvTimer			( uint Id ) ;


		TUnixDialogBox *	GetDialogBox  (int nb = 0 )
		   {
			int n = Support -> GetNbBox ( ) ;

			if  ( nb  >  n - 1 )
				nb = 0 ;
			return  ( Support -> DialogBox [nb] ) ;
			 }


		void	SetFunctionKey ( int  num, char *  label, char *  help )
		   {
			if  ( OldFunctionKeys )
				Support -> FunctionKeys -> SetLabel ( num, label, help ) ;
			 }


# pragma  warn  -inl
		void	SetObjectId  ( int  id )
		   {
			int n = Support -> GetNbBox();

			Support -> SetObjectId ( id ) ;

			for  ( int i = 0; i < n; i ++ )
				Support -> DialogBox [i] -> ObjectId = ObjectId = id ;
		    }
# pragma  warn  .inl


		void	Show ( int  cmd )
		   {
			TDecoratedFrame :: Show ( cmd ) ;

			SetWindowVisible ( Support ) ;
			Support -> Show ( cmd ) ;
		    }


		void   	EvGetMinMaxInfo  ( MINMAXINFO &  Info ) ;
		void	EvSysCommand	 ( UINT		 SysCmd,
					   TPoint &  	 Point ) ;
		void	EvNCLButtonDblClk( UINT		 HitTest,
					   TPoint &	 Point ) ;
		void	EvSize		 ( UINT		 SizeType,
					   TSize &	 Size ) ;


	// Gestion du focus
		void	EvSetFocus	  ( HWND  	lost ) ;
		void	EvKillFocus	  ( HWND	gained ) ;


	// Efface l'objet de la table des applications
		void	EvClose  ( void )
		   {
			TDecoratedFrame :: EvClose ( ) ;
		    }


	// Revalide les fenetres parentes si la boite était modale
		bool	CanClose ( void ) ;


		int	SetHelp  ( int   id )
		   {
			HelpId = Help -> AddHelp ( this, Configuration -> CheopsHelpFileName,
							TResId ( id ) ) ;

			return ( HelpId ) ;
		     }



	// Redéfinit les boutons de la barre d'icônes et la réinsère avec le
	// liséré
		void	EndFunctionKeyUpdate ( void )
		   {
			:: LockWindowUpdate ( HWindow ) ;
			ControlBar -> EndUpdate ( ) ;
			Insert  ( * ControlBar, TDecoratedFrame :: Top ) ;
			Insert  ( * FilletBar , TDecoratedFrame :: Top ) ;
			Layout ( ) ;
			:: LockWindowUpdate ( NULL ) ;
		    }


	// Pour empêcher de changer le curseur...
		char * 	GetClassName  ( void )
		   {
			return  ( "TCheopsDialog" ) ;
		    }


		void	GetWindowClass  ( WNDCLASS &  wc )
		   {
			TDecoratedFrame :: GetWindowClass ( wc ) ;
			wc. hCursor = NULL ;
		     }



	DECLARE_RESPONSE_TABLE ( TCheopsDialog ) ;
      } ;





/**************************************************************************/
/*                                                                        */
/*   TCheopsButtonGadget -                                                */
/*      Gère un bouton de la barre d'icônes.                              */
/*                                                                        */
/**************************************************************************/

class	TCheopsButtonGadget 	:  public  TButtonGadget
	 {
	public :
		char *			Text,
		     *			HelpText ;
		int			CommandId ;
		char *			BitmapFile ;


	private :
		TStatusBar *		StatusBar ;
		TCheopsControlBar *	CheopsControlBar ;
		TCheopsDialog *		CheopsDialog ;
		

	public :
		TCheopsButtonGadget  ( TCheopsDialog *		frame,
				       TResId			resource_id,	
				       int			id,
				       char *			text,
				       char *			help_text ) ;

	       ~TCheopsButtonGadget ( )
		   {
			free ( Text ) ;
			free ( HelpText ) ;	

			if  ( BitmapFile )
				free  ( BitmapFile ) ;
		    }


	// Permettre l'utilisation de bitmaps externes
		TDib *	GetGlyphDib  ( ) ;


	// Gestion  des "hints"
		void	Activate ( TPoint &  p )
		   {
			CheopsControlBar -> SetFocus ( ) ;
			TButtonGadget :: Activate ( p ) ;
			CheopsDialog -> HandleMessage ( WM_FUNCTIONKEY,
						CommandId - Control_Bar_Base,
						MAKELONG ( Window -> HWindow,
								BN_CLICKED ) ) ;
		     }


		void	MouseEnter  ( uint, TPoint & )
		   {
			if  ( HelpText  &&  * HelpText )
				StatusBar -> SetHintText ( HelpText ) ;
		     }


		void 	MouseLeave  ( uint, TPoint & )
		   {
			if  ( HelpText  &&  * HelpText )
				StatusBar -> SetHintText ( 0 ) ;
			 }


		void	CommandEnable ( void )
			{ }
		 } ;





/***************************************************************************

	Sert à simuler les combobox.


****************************************************************************/
class TChoiceArray;
class TEnhancedListBox;

class TListPat : public TEnhancedListBox
	{
	private :
		TWinLine *	WinL ;
		TChoiceArray *	Papa ;
	public :
		TListPat  ( TWindow * parent, int Id, int x, int y,
				int w, int h, TWinLine * wl, TChoiceArray * dady )
			: TEnhancedListBox  ( parent, Id, x, y, w, h ),
			  WinL  (wl), Papa  ( dady ) { } ;

		void EvKillFocus  	( HWND hw ) ;
		void EvLButtonDblClk	( UINT modkeys, TPoint & pt ) ;
       		void EvLButtonDown  	( UINT modkeys, TPoint & pt ) ;
		void EvChar		( UINT key, UINT repeat, UINT flags ) ;
		BOOL PreProcessMsg	( MSG & msg ) ;

	DECLARE_RESPONSE_TABLE ( TListPat ) ;
	 } ;

/****************************************************************************

	fenêtre servant de support pour la ListBox

*****************************************************************************/

class TChoiceArray : public TWindow
	{
	public :
		TChoiceArray  ( TWindow * parent, int Id ,int x,int y,int cx,int cy,
					TWinLine *dady);

		virtual BOOL 	Create ( ) ;
		int		GetOldFocusId	( ) ;
		void    	SetOldFocusId	( int Id ) ;
		void		EvKillFocus	( HWND hwndGetFocus ) ;

		TListPat *	LB ;

	private :
		int	OldFocusId ;

    } ;
