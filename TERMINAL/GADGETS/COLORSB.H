/**************************************************************************/
/*                                                                        */
/*  ColorSB.h  -  Christian Vigh, 10/95.                                  */
/*                                                                        */
/*  Classe StatusBar avec couleur de 'hint' paramétrable.                 */
/*                                                                        */
/**************************************************************************/




# if	! defined ( __COLORSB_H__ )
#	  define    __COLORSB_H__



class		TColoredStatusBar :  public  TStatusBar 
   {
	public :
	// Constructeur
		TColoredStatusBar ( TWindow *			parent,
				    COLORREF			newcolor        = GetSysColor ( COLOR_BTNFACE ),
				    TGadget :: TBorderStyle     style 	        = TGadget :: Recessed,
				    uint			mode_indicators = 0,
				    TFont *			font		= new  TGadgetWindowFont,
				    TModule *			module	 	=  0 ) 
			: TStatusBar ( parent, style, mode_indicators, font, module )
		   { 
			SetColor ( newcolor ) ;
			HintCommand = 0 ;
		    }




	// Définit la couleur du hint
		void	SetColor  ( COLORREF	value ) 
		   {
			HintColor = value ;
		    }

	
	// Change les couleurs en fonctions de l'état de la barre
		void	SeHintCommand  ( int  id )
		   {
			HintCommand  =  id ;

			if  ( id > 0 )
				ChangeColors ( HintColor ) ;
			else
				ChangeColors ( GetSysColor ( COLOR_BTNFACE ) ) ;

			TStatusBar :: SetHintCommand ( id ) ;
		     }

		
	protected :
	// Change effectivement les couleurs
		void	ChangeColors ( COLORREF  newcolor )
		   {
			SetBkgndColor ( newcolor ) ;
			delete  BkgndBrush ;
			BkgndBrush = new  TBrush ( newcolor ) ;
		     }




	// Change les couleurs si la status bar affiche un texte d'aide
	// Il s'agit ici du PaintGadgets d'OWL purement repompé afin d'utiliser
	// NOTRE couleur de fond pour le texte d'aide.
	// On profite du fait qu'il n'y a pas de TStatusBar::PaintGadgets,
	// mais  qu'il y a un TMessageBar :: PaintGadgets.
		void    PaintGadgets  (TDC &  dc, bool  erase, TRect &  rect )
		  {
			if ( HighlightLine  &&  rect. top  == 0 )
				dc. TextRect ( 0, 0, rect.right, 
					GetSystemMetrics ( SM_CYBORDER ),
					GetSysColor ( COLOR_BTNHIGHLIGHT ) ) ;

			if ( HintText ) 
			   {
				TRect  clientRect = GetClientRect ( ) ;
				int    y          = ( clientRect.bottom - 
							GetFontHeight ( ) ) /  2 ;

				if  ( HighlightLine )
					clientRect. top += GetSystemMetrics ( SM_CYBORDER ) ;

					dc. SelectObject ( GetFont ( ) ) ;
				dc. SetBkColor   ( HintColor ) ;
				dc. ExtTextOut   (5, y, ETO_OPAQUE, & clientRect, 
					HintText, strlen ( HintText ) ) ;
			     }
			else 
				TStatusBar :: PaintGadgets ( dc, erase, rect ) ;
		    }



	private :
		COLORREF	HintColor ;	// Couleur du hint
		int		HintCommand ;
     } ;



# endif		/*  __COLORSB_H__  */