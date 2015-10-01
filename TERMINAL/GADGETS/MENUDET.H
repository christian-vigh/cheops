/**************************************************************************/
/*                                                                        */
/*  MenuDet.h  -  Christian Vigh, 95/10.                                  */
/*                                                                        */
/*  Gadget à insérer dans une TGadgetMenuWindow, permettant d'effectuer   */
/*  un glissé sur le menu.                                                */
/*                                                                        */
/**************************************************************************/



# if	! defined ( __MENUDET_H__ )
# 	  define    __MENUDET_H__


class 	TMenuDetachGadget 	:  public  TGadget 
   {
	private :
		TGadgetStyle	SelectionStyle ;
		BOOL		Pressed ;
		TPoint		MousePosition ;
		HCURSOR		SavedCursor ;
		TBitmap *	Bitmap ;
		BITMAP		BitmapInfo ;
		int		Height ;
		int		DeltaX,		// Différence entre le point de clic
				DeltaY ;	// et l'origine du gadget


	public :
		TMenuDetachGadget ( ) ;
		virtual ~TMenuDetachGadget ( ) ;	


		virtual void	GetDesiredSize  ( TSize &  sz ) ;



	protected :		
		virtual void	LButtonDown	( uint  	modkeys, 
						  TPoint & 	pos ) ;

		virtual void	MouseMove	( uint  	modkeys, 
						  TPoint & 	pos ) ;

		virtual void	LButtonUp	( uint  	modkeys, 
						  TPoint & 	pos ) ;

		virtual void	Paint 		( TDC &		dc ) ;      

	
	private :	
		void		DrawSelection	( TPoint &	pos ) ;
    } ;



# endif		/*  __MENUDET_H__  */