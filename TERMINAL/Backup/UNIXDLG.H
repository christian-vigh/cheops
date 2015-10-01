/**************************************************************************/
/*                                                                        */
/*  UnixDlg.h  -  Christian Vigh, 94/11/16.                               */
/*                                                                        */
/*  Classe pour la gestion des boîtes de dialogue Unix.                   */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  Classe  TUnixDialogBox :                                              */
/*      Définition de la classe boîte de dialogue en provenance d'Unix.   */
/*                                                                        */
/**************************************************************************/

class	TUnixDialogBox	:  	public  TDialog
   {
	private :
		TUnixApplication *	UnixApplication ;
		TWindow *		ParentWindow ;
		int			ProcessId ;
		int			MyId ;
		HWND			DialogHandle ;
		HANDLE			MemoryHandle ;
		BOOL			IsClientWindow ;
		DWORD			UnusedStyle ;
		DIALOGBOX		DialogBoxTemplate ;
		TWindow *			ResponsableMsg;


	public :
		integer16		ObjectId ;
		TUnixIcon		Icon ;
		TControlStyle		ControlStyle ;
		HWND			LastFocusHwnd ;


	public :

	// Constructeur : initialise les données privées
		TUnixDialogBox  ( TUnixApplication *	app,
				  TWindow *		parent,
				  unsigned char *	data,
				  int			datasize,
				  int			pid,
				  BOOL			isclient,
				  TWindow *		respMsg=NULL ) ;


	// Destructeur : libère la mémoire globale allouée pour le template
	// de la boîte de dialogue :
	       ~TUnixDialogBox ( )
		  {
			GlobalFree ( MemoryHandle ) ;
			FreeDialogBoxTemplate ( DialogBoxTemplate ) ;
		   }

	// Va nous servir à intercepter les touches de fonction
		virtual BOOL  PreProcessMsg ( MSG &  msg ) ;


	// DialogFunction : détourne les messages "intéressants" pour Windows
		virtual BOOL 	DialogFunction	( UINT  	msg,
						  WPARAM	wparam,
						  LPARAM	lparam ) ;

	// ProcessCommand : Gère une commande
		BOOL		ProcessCommand  ( WPARAM	ctrlid,
						  WPARAM	hwnd,
						  WPARAM	notifycode,
						  WPARAM	wp,
						  LPARAM	lp ) ;

		BOOL		IsNotFunctionKey ( HWND ) ;

		void		CmOk 		( void ) ;
		void		CmCancel 	( void ) ;

		virtual HWND	DoCreate  	( void ) ;
		virtual int 	DoExecute       ( void ) ;


	// EvPaint pour afficher l'icône
		void	EvPaint ( void )
		   {
			if  ( ! Icon. Paint ( HWindow ) )
				TDialog :: EvPaint ( ) ;
		    }


	// SetIcon : Change l'icône de la fenêtre
		LRESULT  EvSetIcon ( WPARAM  predefined, LPARAM  lparam )
		   {
			Icon. SetIcon ( predefined, lparam ) ;
			return ( 0L ) ;
		    }


	// Retourne le style non utilisé lorsque la boîte de dialogue est
	// la partie cliente d'une TFrameWindow
		DWORD	GetUnusedStyle ( void )
		   {
			return ( UnusedStyle ) ;
		    }


	// Retourne le template de la boîte de dialogue
		void	GetDialogBoxTemplate  ( DIALOGBOX &  dlg )
		   {
			dlg = DialogBoxTemplate ;
		    }


		void	SetObjectId  ( int  id )
		   {
			ObjectId = id ;
		    }




	// Pour empêcher de changer le curseur...
		char * 	GetClassName  ( void )
		   {
			return  ( "TUnixDialog" ) ;
		    }


		void	GetWindowClass  ( WNDCLASS &  wc )
		   {
			TDialog :: GetWindowClass ( wc ) ;
			wc. hCursor = NULL ;
		     }




# pragma  warn  -inl
	DEFINE_MODALFRAME_HANDLERS ( TDialog ) ;
# pragma  warn  .inl

	DECLARE_RESPONSE_TABLE     ( TUnixDialogBox ) ;
      } ;





