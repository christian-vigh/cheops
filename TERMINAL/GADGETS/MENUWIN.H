/**************************************************************************/
/*                                                                        */
/*  MenuWin.h  -  Christian Vigh, 95/10/12.                               */
/*                                                                        */
/*  Classe "Popup menu".                                                  */
/*                                                                        */
/**************************************************************************/



# if	! defined ( __MENUWIN_H__ )
# 	  define    __MENUWIN_H__



/**************************************************************************/
/*                                                                        */
/*  Messages sp�cifiques.                                                 */
/*                                                                        */
/**************************************************************************/

//
// WM_MENUDROPPED -
//  	Ce message est envoy� par le TDropMenuGadget chaque fois qu'un menu
//	a �t� gliss�-d�plac�.
//
// WM_MENUSELECTED -
// 	Envoy� lorsque un �l�ment de menu est s�lectionn�.
//
// WM_MENUCLOSED -
//	Envoy� sur fermeture du menu.
//
# define	WM_MENUDROPPED		( WM_USER + 100 )
# define	WM_MENUSELECTED		( WM_USER + 101 )
# define	WM_MENUCLOSED		( WM_USER + 102 )











/**************************************************************************/
/*                                                                        */
/*  Classe TGadgetMenuWindow -                                            */
/*      Menu popup sous forme de gadget.                                  */
/*                                                                        */
/**************************************************************************/

class  TMenuDetachGadget ;


class  TGadgetMenuWindow 	: public  TGadgetWindow 
   {
	public :
		enum  TPlacement 	// Signification pour SelectItem
		   {
			Next, 		// S�lectionner le suivant 
			Previous,	// le pr�c�dent
			First, 		// Le premier
			Last,		// Le dernier
			Current		// Le gadget actuellement s�lectionn�
		    } ;


		enum  TPopupDirection	// Direction d'ouverture des menus
		   {
			TopRight,
			TopLeft,
			BottomLeft,
			BottomRight,
			TotalMess
		    } ;
			

			
	public :

	// Constructeur pour cr�er un menu initial
		TGadgetMenuWindow  ( TPoint &		desired_bottom,
				     TWindow *		receiver,
				     char *		title    =  0,
				     TFont *		font 	 =  new  TGadgetWindowFont ( 8 ) ) ;

	// Constructeur pour cr�er un sous-menu
		TGadgetMenuWindow  ( TGadgetMenuWindow * parent_menu,
				     int		gadget_id,
				     char *		title    =  0,
				     TFont *		font 	 =  new  TGadgetWindowFont ( 8 ) ) ;

	// Constructeur pour une copie de menu
		TGadgetMenuWindow  ( TGadgetMenuWindow &  copy,
				     TFont *		  font	 =  new  TGadgetWindowFont ( 8 ) ) ;


	// Destructeur
		virtual ~TGadgetMenuWindow  ( void ) ;

		virtual BOOL	CanClose    ( void ) ;


	// Fonctions relatives � la s�lection d'�l�ments
		void		SelectItem	( TPlacement  	placement,
						  TGadgetMenuItem *
								relative_to 	=  0 ) ;
		void		SelectItem	( int		index ) ;
		void		SelectItem	( TGadgetMenuItem *
								g ) ;

		TGadgetMenuItem *
				GetItem		( TPlacement    placement 	=  Current,
						  TGadgetMenuItem *
								relative_to	=  0 ) ;
		TGadgetMenuItem *
				GetItem		( int		index 		=  -1 ) ;

		int		GetItemCount	( void ) ;



	// Param�trage du menu
		void		SetStyle	( TGadgetStyle  style ) ;
		TGadgetStyle	GetStyle 	( void ) ;
		
		void		SetSelectionStyle ( TGadgetStyle  style ) ;
		TGadgetStyle    GetSelectionStyle ( void ) ;


		void		SetTitle	( char *   	title ) ;
		char *		GetTitle	( void ) ;


	// Le gadget TDrawmachin appelle cette fonction en cas de glisser-
	// lacher		
		virtual void   	EvDrop		( TPoint & 	pos ) ;


	// Fonctions surcharg�es de TGadgetWindow, afin de changer la mani�re
	// de calculer la taille de la fen�tre
		virtual BOOL	Create		( void ) ;
		virtual void	GetDesiredSize	( TSize &	size ) ;
		virtual void	GetInnerRect 	( TRect & 	rect ) ;


	// Ins�re le gadget et change son style de s�lection conform�ment �
	// celui qui a �t� choisi	
		virtual void	Insert		( TGadget &	gadget,
						  TGadgetWindow :: TPlacement	
								placement = After,
						  TGadget *	sibling   = 0 ) ;

	// Retourne un pointeur sur le gadget ayant l'id sp�cifi�
		TGadgetMenuItem *	GadgetWithId  ( int  id )
		   {
			TGadget *	g = TGadgetWindow :: GadgetWithId ( id ) ;


			return  ( TYPESAFE_DOWNCAST ( g, TGadgetMenuItem ) ) ;
		    }



	// Renvoie la position o� le menu a �t� l�ch�
		TPoint &	GetDroppedPosition  ( void )
		   {
			return  ( DroppedPosition ) ;
		    }


		void		SetDroppedPosition  ( TPoint &  pos )
		   {
			DroppedPosition = pos ;
		    }


	// Ferme proprement la fen�tre
		static void	ShutDownWindow  ( TWindow *  w, int  retval = 0 ) ;
		virtual void	ShutDownWindow  ( int  retval  =  0 ) ;


	// Gestion du timer
		void		SetTimer  	( void ) ;
		void		KillTimer	( void ) ;

		void		SetPopupTimeout ( uint  timeout )
		   {
			TimeoutValue = timeout ;
		    }

		virtual void	Timeout  ( void )
		   { }


	// Pour savoir � quel objet on a affaire
		BOOL	IsMainMenu  ( void )
		   {
			return  ( MenuType  ==  MainMenu ) ;
		    }


		BOOL	IsSubMenu  ( void )
		   {
			return  ( MenuType  ==  SubMenu ) ;
		    }


		BOOL	IsClone ( void )
		   {
			return  ( MenuType  ==  Clone ) ;
		    }


	// Redessine la fen�tre
		void	LayoutSession		( void ) ;		

	// Empeche la fermeture de la fenetre par appui sur alt-F4
		void	EvSysCommand  ( uint  cmdtype, TPoint &  point )
		   {
			if  ( cmdtype  !=  SC_CLOSE )
				TGadgetWindow :: EvSysCommand ( cmdtype, point ) ;
		    }


	protected :
		virtual void	EvChar		( uint		key,
						  uint		repcnt,	
						  uint		flags ) ;

		virtual void	EvKeyDown	( uint		key,
						  uint		repcnt,	
						  uint		flags ) ;

		virtual void	Paint		( TDC &		dc,
						  BOOL		erase,
						  TRect &	rect ) ;

		virtual void	EvMouseMove	( uint		modflags,
						  TPoint & 	point ) ;
		virtual void	EvLButtonDown	( uint		modflags,
						  TPoint & 	point ) ;
		virtual void	EvLButtonUp 	( uint		modflags,
						  TPoint & 	point ) ;


	// Emp�che deux fois le lancement du m�me programme
		virtual void	EvLButtonDblClk ( uint, TPoint & )
		  { }


		virtual TRect	TileGadgets	( void ) ;


	// Gestion de la classe
		char *		GetClassName  	( void ) ;
		void		GetWindowClass	( WNDCLASS &	cw ) ;
		


		TGadgetMenuItem * 	GetCurrentGadget  	( void ) ;
		TGadgetMenuItem * 	GetNextGadget  		( TGadgetMenuItem *  g ) ;
		TGadgetMenuItem * 	GetPreviousGadget  	( TGadgetMenuItem *  g ) ;
		TGadgetMenuItem * 	GetFirstGadget  	( void ) ;
		TGadgetMenuItem * 	GetLastGadget  		( void ) ;
		TGadgetMenuItem * 	GetGadget  		( int	index ) ;

		virtual void		MenuSelect		( TGadgetMenuItem * gadget ) ;

		virtual void		ComputeChildPosition	( TSize	&	size ) ;
		


	// Gestion du focus
		virtual void	EvKillFocus  	( HWND  gained ) ;
		virtual void	EvSetFocus  	( HWND  lost )   ;

		virtual void	ChildDied	( TGadgetWindow * ) 
		   { }
				

		virtual void	EvTimer		( uint  id ) ;


	protected :
		enum  TGadgetMenuType    	// Type de cet objet :
		   {
				MainMenu,       // Menu principal
				SubMenu,	// Sous-menu
				Clone		// Copie d'un menu ou sous-menu
		    } ;
				
		TGadgetMenuType		MenuType ;


	// Duplication du menu sp�cifi�
		void			CloneWindow		( TGadgetMenuWindow & ) ;

		
	protected :
		BOOL 			ShutDownStarted ;	// TRUE si un shutdown est en cours
		BOOL			CriticalSection ;	// Pour prot�ger
		BOOL			MenuDropped ;		// TRUE si le menu doit devenir flottant
		TGadgetStyle 		SelectionStyle ;	// Type de la s�lection
		TGadgetStyle		MenuStyle ;		// Style du menu
		TGadgetMenuItem *	CurrentGadget ;		// Gadget actuellement s�lectionn�
		TGadgetMenuItem *	ButtonDownGadget ;	// Gadget sur lequel on a cliqu�
		TPoint			DesiredBottom ;		// Position d�sir�e pour le bas
		TWindow *		Receiver ;		// Fen�tre recevant les messages menu
		TPoint 			DroppedPosition ;	// Position o� la fen�tre a �t� lach�e
		char *			Title ;			// Titre 
		BOOL			ButtonPressed ;		// TRUE si un �l�ment a �t� s�lectionn�
		long int		AtMouseTicks ;
		BOOL			GotKillFocus ;		// TRUE si le message KILLFOCUS a �t� re�u


	// Gestion de la fermeture des sous-menus : deux ESCapes 
	// appuy�s dans un temps inf�rieur � celui du double-clic ferment
	// toute la hi�rarchie de menus
		BOOL			DiedFromEscape ;
		int			DoubleClickTime ;
		DWORD			DiedFromEscapeTime ;


	// Gestion des sous-menus et des cl�nes
		TGadgetMenuWindow *	ParentMenu ;		// Menu parent (dans le cas d'un sous-menu)
		int			ParentMenuId ;		// Id du gadget parent
		TGadgetMenuWindow *	ChildMenu ;		// Menu enfant 	
		TGadgetMenuWindow *	ClonedWindow ;
		BOOL			IgnoreNextButtonUp ;
		BOOL			IgnoreNextTimer ;
		BOOL			GotSelection ;
		TPopupDirection		CurrentDirection ;


	// Gestion du timer et de l'ouverture automatique des menus
		TGadget *		EnteredGadget ;
		uint			TimeoutValue ;
		int			TimerId ;
		

	// Autres fonctions
		TGadgetMenuItem *       GetValidItem		( TGadget *	gadget ) ;
		TGadgetMenuItem *	GetValidItemPointer	( TGadget *     gadget ) ;
		LRESULT			WmMenuDropped		( WPARAM, LPARAM ) ;					
		void			Init			( char *        title ) ;

		
	friend class	TGadgetMenuItem ;
	friend class    TMenuDetachGadget ;
		
	DECLARE_RESPONSE_TABLE ( TGadgetMenuWindow ) ;
    } ;
	
	
		
# endif  	/*  __MENUWIN_H__  */	
