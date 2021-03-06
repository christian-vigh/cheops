/**************************************************************************/
/*                                                                        */
/*  UnixDlg.h  -  Christian Vigh, 94/11/16.                               */
/*                                                                        */
/*  Classe pour la gestion des bo�tes de dialogue Unix.                   */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/*                                                                        */
/*  Classe  TUnixDialogBox :                                              */
/*      D�finition de la classe bo�te de dialogue en provenance d'Unix.   */
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

	// Constructeur : initialise les donn�es priv�es
		TUnixDialogBox  ( TUnixApplication *	app,
				  TWindow *		parent,
				  unsigned char *	data,
				  int			datasize,
				  int			pid,
				  BOOL			isclient,
				  TWindow *		respMsg=NULL ) ;


	// Destructeur : lib�re la m�moire globale allou�e pour le template
	// de la bo�te de dialogue :
	       ~TUnixDialogBox ( )
		  {
			GlobalFree ( MemoryHandle ) ;
			FreeDialogBoxTemplate ( DialogBoxTemplate ) ;
		   }

	// Va nous servir � intercepter les touches de fonction
		virtual BOOL  PreProcessMsg ( MSG &  msg ) ;


	// DialogFunction : d�tourne les messages "int�ressants" pour Windows
		virtual BOOL 	DialogFunction	( UINT  	msg,
						  WPARAM	wparam,
						  LPARAM	lparam ) ;

	// ProcessCommand : G�re une commande
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


	// EvPaint pour afficher l'ic�ne
		void	EvPaint ( void )
		   {
			if  ( ! Icon. Paint ( HWindow ) )
				TDialog :: EvPaint ( ) ;
		    }


	// SetIcon : Change l'ic�ne de la fen�tre
		LRESULT  EvSetIcon ( WPARAM  predefined, LPARAM  lparam )
		   {
			Icon. SetIcon ( predefined, lparam ) ;
			return ( 0L ) ;
		    }


	// Retourne le style non utilis� lorsque la bo�te de dialogue est
	// la partie cliente d'une TFrameWindow
		DWORD	GetUnusedStyle ( void )
		   {
			return ( UnusedStyle ) ;
		    }


	// Retourne le template de la bo�te de dialogue
		void	GetDialogBoxTemplate  ( DIALOGBOX &  dlg )
		   {
			dlg = DialogBoxTemplate ;
		    }


		void	SetObjectId  ( int  id )
		   {
			ObjectId = id ;
		    }




	// Pour emp�cher de changer le curseur...
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





