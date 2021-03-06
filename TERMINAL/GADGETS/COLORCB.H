/**************************************************************************/
/*                                                                        */
/*  ColorCB.h  -  Christian Vigh, 95/11.                                  */
/*                                                                        */
/*  Objet de type TControlBar, o� la couleur des gadgets TControlGadget   */
/*  peut �tre red�finie.                                                  */
/*                                                                        */
/**************************************************************************/


# if	! defined ( __COLORCB_H__ )
# 	  define    __COLORCB_H__ 



//
// Tout �a en fait, c'est pour garder aux contr�les de type Edit la couleur
// de fond d'une fen�tre (TControlBar leur donne la couleur d'un bouton. Dans
// le cas du contr�le Edit, on ne voit pas grand-chose, sinon)
//
class	TColorControlBar 	:  public  TControlBar
   {
	private :
		COLORREF	ControlBackground,
				ControlForeground ;
		TBrush *	ControlBrush ;


	public :	
		TColorControlBar   ( TWindow *		parent 		=  0,
				      TTileDirection	direction       =  Horizontal,
				      TFont *		font		=  new  TGadgetWindowFont,
				      TModule *		module		=  0 )
			: TControlBar  ( parent, direction, font, module )
		   {
			ControlBrush = 0 ;
			SetColor ( ) ;
		    }


	       ~TColorControlBar  ( void )	
		   {
			if  ( ControlBrush )
				delete  ControlBrush ; 
		    }



	// Permet de red�finir la couleur de texte et de fond
		void	SetColor    ( COLORREF	  fore	=  GetSysColor ( COLOR_WINDOWTEXT ),
				      COLORREF	  back	=  GetSysColor ( COLOR_WINDOW ) ) ;
	

	// R�cup�ration des couleurs
		COLORREF	GetBackgroundColor ( void )
		   {
			return  ( ControlBackground ) ;
		    }


		COLORREF	GetForeGroundColor  ( void )
		   {
			return  ( ControlForeground ) ;
		    }


	protected :
	// Se synchronise avec les nouvelles couleurs syst�me
	//
		void	EvSysColorChange ( void ) ;


	// Red�finit la couleur des contr�les
	// 
		HBRUSH  EvCtlColor  ( HDC  hdc, HWND  childwnd, uint  ctltype  ) ;


	DECLARE_RESPONSE_TABLE ( TColorControlBar ) ;
    } ;
	


# endif  	/*  __COLORCB_H__  */

