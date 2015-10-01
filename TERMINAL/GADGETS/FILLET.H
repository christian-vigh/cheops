/**************************************************************************/
/*                                                                        */
/*  Fillet.h  -  Christian Vigh, 95/10.                                   */
/*                                                                        */
/*  Fenêtre gadget à insérer dans une decorated frame, permettant de      */
/*  représenter un liséré de couleur paramétrable.                        */
/*                                                                        */
/**************************************************************************/




# if	! defined ( __FILLET_H__ )
# 	  define    __FILLET_H__



class	TFilletBar	:  public  TGadgetWindow 
   {
	private :
		COLORREF		Color ;
		int			WidthOrHeight ;


	public :
		TFilletBar  ( TWindow * 	parent,
			      TTileDirection    direction =  Horizontal,
			      COLORREF		color	=  GetSysColor ( COLOR_BTNFACE ),
			      int		worh    =  0 ) ;


	// Permet de changer la couleur de fond
		void		SetColor  	( COLORREF   color )
		   {
			Color = color ;
			Invalidate ( ) ;
		    }


	// Retourne la couleur de fond
		COLORREF        GetColor  	( void )
		   {
			return  ( Color ) ;
		    }



	protected :
		BOOL		PreProcessMsg 	( MSG & ) ;
		void		Paint         	( TDC &, BOOL, TRect & ) ;
		void		GetDesiredSize  ( TSize & ) ;
		
     } ;





# endif		/*  __FILLET_H__  */