/**************************************************************************/
/*                                                                        */
/*  MenuItem.h  -  Christian Vigh, 95/10.                                 */
/*                                                                        */
/*  Element de menu.                                                      */
/*                                                                        */
/**************************************************************************/



# if	! defined ( __MENUITEM_H__ )
# 	  define    __MENUITEM_H__




/**************************************************************************/
/*                                                                        */
/*  Style d'un gadget.                                                    */
/*                                                                        */
/**************************************************************************/

enum  TGadgetStyle
   {
		RaisedGadget,		// Look 3D
		NormalGadget,		// look Windows
		RecessedGadget	       // en enfoncé
    } ;




/**************************************************************************/
/*                                                                        */
/*  Classe  TGadgetMenuItem -                                             */
/*      Element de menu.                                                  */
/*      Un objet de cette classe doit être inséré dans une                */
/*      TGadgetMenuWindow pour que les messages clavier lui parviennent.  */
/*                                                                        */
/**************************************************************************/

class	TGadgetMenuWindow ;

class	TGadgetMenuItem :	public  TGadget 
   {
	public :
		enum  TType  			// Type d'un élément de menu
		   {
				Item,
				Popup,
				Separator 
		    } ;

		enum  TState			// Etat d'un élément de menu
		   {
				Disabled	=  1 << 1,
				Enabled		=  1 << 2,
				Checked		=  1 << 3,
				Unchecked	=  1 << 4,
				Grayed		=  1 << 5
		    } ;



	public :
	
	// Constructeurs et destructeur
		TGadgetMenuItem  ( int			id,
				   char *		text,
				   TType		type	=  Item,
				   TState 		state   =  Enabled ) ;
		TGadgetMenuItem  ( ) ;

	       virtual ~TGadgetMenuItem  ( ) ;



	// Paramétrage de l'élément
		void		SetText		( const char *	text ) ;
		char *		GetText		( void ) ;

		void		SetState	( TState
								state ) ;
		TState
				GetState	( void ) ;

		TType
				GetType		( void ) ;


		void		SetSelection	( BOOL		selected = FALSE ) ;
		BOOL		GetSelection	( void ) ;

		void		SetSelectionStyle ( TGadgetStyle  style ) ;
		TGadgetStyle 	GetSelectionStyle ( void ) ;


		char		GetShortcut	( void ) ;

		void		SetCheck	( TState 
								state ) ;


	// Calcul de taille
		virtual void	GetDesiredSize  ( TSize &	size ) ;




	protected :
		char *			ItemText ;	// Texte du menu ( ou 0 )
		BOOL			Selected,       // TRUE si l'élément est sélectionné
					MouseEntered ;	// True si la souris est entrée dans cet élément
		TType     Type ;		// Type de l'élément
		TState	State ;		// Etat
		int			Interval ;	// Intervalle entre les bords et le texte



	protected :
	// Gestion de la souris
		virtual void	MouseEnter	( uint  	modkeys, 
						  TPoint &  	point ) ;
		virtual void	MouseLeave	( uint  	modkeys, 
						  TPoint &  	point ) ;
		virtual void	MouseMove	( uint  	modkeys, 
						  TPoint &  	point ) ;
		virtual void	LButtonUp	( uint  	modkeys, 
						  TPoint &  	point ) ;
		virtual void	LButtonDown	( uint  	modkeys, 
						  TPoint &  	point ) ;


	// Gestion du dessin de l'élément
		void		Paint 		( TDC & 	dc ) ;
		void		PaintContents	( TDC &		dc,
						  TRect &	rect ) ;
		void		PaintBorder	( TDC & 	dc ) ;		
		void		PaintIndicator	( TDC & 	dc,
						  TRect &	rect ) ;
		void		PaintCheckMark  ( TDC &		dc,
						  TRect &	rect,
						  TState	
								state ) ;
		void		PaintSeparator  ( TDC &		dc,
						  TRect &	rect ) ;
		void		PaintSelection  ( TDC & 	dc,
						  TRect &	rect ) ;



	private :
		void		Init  		( char *		text,
						  TType 	type,
						  TState  state ) ;



		void		FillSurface 	( TDC &  	dc,
						  TPoint &	pos,
						  COLORREF	newcolor ) ;


	private :
		TGadgetStyle			SelectionStyle ;
		TSize				SelectionSize,
						TextSize,
						CheckmarkSize,
						IndicatorSize,
						BorderUnits ;
		int				BorderDelta,  	// Espace entre les élements et le bord
						ElementDelta ;	// et entre deux éléments
		int				IndicatorDelta ;// Espace avant l'indicateur de sous-menu
		int				SeparatorSize ;
		HCURSOR				SavedCursor,	
						LoadedCursor ;


	friend class	TGadgetMenuWindow ;
     } ;



# endif		/*  __MENUITEM_H__  */