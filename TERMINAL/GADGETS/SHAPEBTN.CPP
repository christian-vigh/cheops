/**************************************************************************/
/*                                                                        */
/*  ShapeBtn.cpp - Christian Vigh, 95/10.                                 */
/*                                                                        */
/*  Implémentation de la classe TShapeButtonGadget.                       */
/*                                                                        */
/**************************************************************************/



# pragma	option -k- -N- -r- -v- -vi- -y-
# include	<owl/owlall.h>
# pragma	hdrstop
# pragma	option -k. -N. -r. -v. -vi. -y.


# include	"ChopJet.h"




/**************************************************************************/
/*                                                                        */
/*  Constructeur.                                                         */
/*                                                                        */
/**************************************************************************/


TShapeButtonGadget :: TShapeButtonGadget  ( int		id,
					    TType	type,
					    bool  	enabled,
					    int		numchar )
	: TTextButtonGadget ( id, type, enabled, numchar )
   {
	cxBorder      = GetSystemMetrics ( SM_CXBORDER ) ;
	cyBorder      = GetSystemMetrics ( SM_CYBORDER ) ;
	MustIntersect = FALSE ;
    }




/**************************************************************************/
/*                                                                        */
/*  Destructeur.                                                          */
/*                                                                        */
/**************************************************************************/

TShapeButtonGadget :: ~TShapeButtonGadget  ( void )
   {
     }




/**************************************************************************/
/*                                                                        */
/*  TShapeButtonGadget :: PtIn -                                          */
/*      Détermine si le point spécifié est situé dans l'onglet.           */
/*                                                                        */
/**************************************************************************/

BOOL	TShapeButtonGadget :: PtIn  ( TPoint &  p )
   {
	TRegion		Region ;


	GetRegion ( Region, VisibleRegion ) ;
	return  ( Region. Contains ( TPoint ( p. x + Bounds. left, 
					      p. y + Bounds. top ) ) ) ;
    }






/**************************************************************************/
/*                                                                        */
/*  TShapeButtonGadget :: Invalidate -                                    */
/*      Invalide la partie visible de l'onglet.                           */
/*                                                                        */
/**************************************************************************/


void  TShapeButtonGadget :: Invalidate ( void )
   {
	TRegion 	Region ;


	GetRegion ( Region, VisibleRegion ) ;

	if  ( MustIntersect )
	   {
		Region  &=  * IntersectionRegion ;
		MustIntersect = FALSE ;
		delete  IntersectionRegion ;
	    }

	Window -> InvalidateRgn ( Region ) ;
     }




/**************************************************************************/
/*                                                                        */
/*  TShapeButtonGadget :: GetRegionColor -                                */
/*      Retourne la couleur correspondant à la région spécifiée.          */
/*                                                                        */
/**************************************************************************/

COLORREF 	TShapeButtonGadget :: GetRegionColor   ( TRegionType  type )
   {
	COLORREF		Color = 0 ;


	switch (  type ) 
	   {
		case	BorderRegion :
			Color = GetSysColor ( COLOR_WINDOWFRAME ) ;
			break ;

		case	InnerRegion :
			Color = GetSysColor ( COLOR_BTNFACE ) ;
			break ;

		case	HiliteRegion :
			Color = GetSysColor ( COLOR_BTNHIGHLIGHT ) ;
			break ;

		case	ShadowRegion :
			Color = GetSysColor ( COLOR_BTNSHADOW ) ;
			break ;

		case	OtherShadowRegion :
			Color = RGB ( 255, 255, 0 )  ;
			break ;			
	      }


	return  ( Color ) ;
     }




/**************************************************************************/
/*                                                                        */
/*  TShapeButtonGadget :: Paint -                                         */
/*      Dessine le bouton.                                                */
/*                                                                        */
/**************************************************************************/

void	TShapeButtonGadget :: Paint  ( TDC &  dc )
   {
	BOOL		Clipped		=  FALSE ;
	TRegion		Region ;
	



	dc. SaveDC ( ) ;


// Clipper sur la partie visible	
	if  ( GetRegion ( Region, VisibleRegion ) )
	   {
		Clipped = TRUE ;
		dc. SelectClipRgn ( Region ) ;
	    }


// Dessiner l'intérieur (en premier, ça évite des calculs de coordonnées
// pas faciles)
	if  ( GetRegion ( Region, InnerRegion ) )
	   {
		TBrush		FaceBrush ( GetRegionColor ( InnerRegion ) ) ;

		dc. FillRgn ( Region, FaceBrush ) ;
	    }


// le bord
	if  ( GetRegion ( Region, BorderRegion ) )
	   {
		TBrush		BorderBrush ( GetRegionColor ( BorderRegion ) ) ;

		dc. FrameRgn ( Region, BorderBrush, TPoint ( cxBorder, cyBorder ) ) ;
	    }


// Puis la partie éclairée
	if ( GetRegion ( Region, HiliteRegion ) )
	   {
		TBrush 		HiliteBrush ( GetRegionColor ( HiliteRegion ) ) ;


		dc. FillRgn ( Region, HiliteBrush ) ;
	     }


// et la partie ombrée
	if  ( GetRegion ( Region, ShadowRegion ) )
	    {
		TBrush		ShadowBrush ( GetRegionColor ( ShadowRegion ) ) ;

		dc. FillRgn ( Region, ShadowBrush ) ;
	     }
  

// Restaurer le contexte
	dc. RestoreDC ( ) ;

	if  ( Clipped )
		:: SelectClipRgn ( dc, NULL ) ;


// Calculer le rectangle correspondant à la partie dessinable
	if  ( GetRegion ( Region, DrawableRegion ) )
	   {
		TRect		Rect ;

		Region. GetRgnBox ( Rect ) ;
		PaintContents ( dc, Rect ) ;
	     }

// La partie "autres ombres" est dessinée en dernier
	if  ( GetRegion ( Region, OtherShadowRegion ) )
	   {
		TBrush		OtherShadowBrush ( GetRegionColor ( OtherShadowRegion ) ) ;

		dc. FillRgn ( Region, OtherShadowBrush ) ;
	    }
     }
