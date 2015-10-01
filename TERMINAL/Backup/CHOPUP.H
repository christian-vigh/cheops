/**************************************************************************/
/*                                                                        */
/*  ChopUp.h  -  Christian Vigh, 95/10.                                   */
/*                                                                        */
/*  Implémente une classe dérivée de TGadgetMenuWindow, qui permet        */
/*  de gérer un menu à partir des structures de définition de menus       */
/*  Cheops.                                                               */
/*                                                                        */
/**************************************************************************/


# if	! defined ( __CHOPUP_H__ )
#	  define    __CHOPUP_H__ )



class	TCheopsFloatingMenu ;
class	TCheopsMenuWindow ;


class	TCheopsGadgetMenuWindow :  public  TGadgetMenuWindow
   {
	private :
		TCheopsMenu *		Menu ;
		int			AdditionalItemCount ;
		char *			ModuleName,
		     *			ThisMenuPath ;
		BOOL			LastChild ;
		TCheopsFloatingMenu *	FloatingParent ;


	public :
	// Constructeur pour créer un menu initial
		TCheopsGadgetMenuWindow  ( TPoint &		desired_bottom,
					   TWindow *		receiver,
					   TCheopsMenu *	menu,
					   char *		module_name,
					   TFont *		font 	 =  new  TCheopsWindowFont ( 8 ) )
			: TGadgetMenuWindow ( desired_bottom, receiver, menu -> MenuText,
					font )
		   {
			Menu         = menu ;
			ThisMenuPath = new char [ MAX_PARAMETER_LENGTH ] ;
			ModuleName   = strdup ( module_name ) ;
			strcpy ( ThisMenuPath, module_name ) ;
			AdditionalItemCount = 2 ;
			LastChild = TRUE ;
			FloatingParent = 0 ;
		    }


	// Constructeur pour créer un sous-menu
		TCheopsGadgetMenuWindow  ( TCheopsGadgetMenuWindow *  parent_menu,
					   int			gadget_id,
					   TCheopsMenu *	menu,
					   TFont *		font 	 =  new  TCheopsWindowFont ( 8 ) )
			: TGadgetMenuWindow ( parent_menu, gadget_id, menu -> MenuText,
					font )
		   {
			Menu 	      = menu ;
			ThisMenuPath  = new char [ MAX_PARAMETER_LENGTH ] ;
			ModuleName    = strdup ( parent_menu -> ModuleName ) ;
			strcpy ( ThisMenuPath, parent_menu -> ThisMenuPath ) ;
			strcat ( ThisMenuPath, ":" ) ;
			strcat ( ThisMenuPath, Menu -> MenuName ) ;
			AdditionalItemCount = 2 ;
			LastChild = TRUE ;
			FloatingParent = 0 ;
		    }


	// Constructeur pour une copie de menu
		TCheopsGadgetMenuWindow  ( TCheopsGadgetMenuWindow &  copy,
					   TFont *		      font   =  new  TCheopsWindowFont ( 8 ) ) ;


	// Destructeur : libère le menu
	       virtual ~TCheopsGadgetMenuWindow ( ) ;


	// Création de la fenêtre : insertion automatique des gadgets
		virtual BOOL	Create  ( void ) ;


	// Récupération du menu
		TCheopsMenu &  GetMenu  ( void )
		   {
			return  ( * Menu ) ;
		    }


	// Et du module
		char *		GetModuleName ( void )
		   {
			return  ( ModuleName ) ;
		    }


	// A appeler si on est le dernier sous-menu
		void	SetChildState ( BOOL  last )
		   {
			LastChild = last ;
		    }


		BOOL	GetChildState ( void )
		   {
			return ( LastChild ) ;
		    }



	// Gestion des menus dans une TFloatingFrame
		void	SetFloatingParent ( TCheopsFloatingMenu *  fm )
		   {
			FloatingParent = fm ;
		    }


		TCheopsFloatingMenu *  GetFloatingParent ( void )
		   {
			return  ( FloatingParent ) ;
		    }

		char *	GetMenuPath ( void )
		   {
			return  ( ThisMenuPath ) ;
		    }

	// Prend en compte la taille du tiny caption
		void	GetDesiredSize  	( TSize &  	size ) ;
		bool       PreProcessMsg		( MSG & 	msg ) ;

	protected :
	// Insertion des éléments du menu
		void		InsertItems ( void ) ;


	// Cette fonction est appelée lorsqu'un sous-menu perd le focus au
	// profit d'un parent
		virtual void 	ChildDied ( TGadgetWindow * )
		   {
			LastChild = TRUE ;
		    }


	friend class  TCheopsMenuWindow ;
    } ;         





/**************************************************************************/
/*                                                                        */
/*  Classe  TCheopsFloatingMenu -                                         */
/*      Gestion d'un menu flottant.                                       */
/*                                                                        */
/**************************************************************************/

class  TCheopsFloatingMenu 	:  public TFloatingGadgetWindow
   {
	public :
		TCheopsFloatingMenu   (  TCheopsMenuWindowClient *  	parent,
					 TCheopsGadgetMenuWindow *      client,
					 TPoint &			pos ) ;

	        virtual ~TCheopsFloatingMenu   ( ) ;	


		TCheopsFloatingMenu *	NextMenu ( void )
		   {
			return ( Next ) ;
		    }


		TCheopsFloatingMenu *   PreviousMenu  ( void )
		   {
			return  ( Previous ) ;
		    }



		void	UpdateCheckmark 		( int  gadgetid ) ;


		TFont *		GetCaptionFont ( void )
		   {
			return ( CaptionFont ) ;
		    }



	// Fonctions pour faire comme TMDIChild
		bool       PreProcessMsg		( MSG & 	msg ) ;
		bool       ShowWindow			( int 		cmdShow ) ;
		bool       EnableWindow			( bool 		enable ) ;
		void       Destroy			( int 		retVal = 0 ) ;


	protected:
		void       PerformCreate		( int menuOrId);
		LRESULT    DefWindowProc		( uint 		msg, 
							  WPARAM 	wParam, 
							  LPARAM 	lParam ) ;
		void       EvMDIActivate		( HWND 		hWndActivated,
							  HWND 		hWndDeactivated) ;
		bool       EvNCActivate			( bool active) ;

		void	   EvSetFocus  ( HWND  lost )
		   {
			TFloatingGadgetWindow :: EvSetFocus ( lost ) ;
			BringWindowToTop ( ) ;
		    }




	private :
		TCheopsGadgetMenuWindow *	MenuWindow ;
		TCheopsFloatingMenu *		Next,
				    *		Previous ;

	friend class  TCheopsMenuWindow ;


	DECLARE_RESPONSE_TABLE ( TCheopsFloatingMenu ) ;
     } ;




# endif  	/*  __CHOPUP_H__  */
		
		  
		