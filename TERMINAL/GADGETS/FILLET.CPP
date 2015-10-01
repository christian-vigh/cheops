/**************************************************************************/
/*                                                                        */
/*  Fillet.cpp  -  Christian Vigh, 95/10.                                 */
/*                                                                        */
/*  Implémentation de la classe TFilletBar.                               */
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

TFilletBar :: TFilletBar  ( TWindow * 		parent,
			    TTileDirection    	direction,
			    COLORREF		color,
			    int			worh )
		: TGadgetWindow ( parent, direction )
    {
	Margins. Units = TMargins :: BorderUnits ;
	Margins. Left  = Margins. Right  = 0 ;
	Margins. Top   = Margins. Bottom = 0 ;
	Color = color ;

	if  ( worh  < 1 )
		worh = GetSystemMetrics ( SM_CYMENU ) / 2 ;

	if  ( Direction  ==  Horizontal )
		Attr. H = worh ;
	else
		Attr. W = worh ;

	WidthOrHeight = worh ;
     }




/**************************************************************************/
/*                                                                        */
/*  TFilletBar :: PreProcessMsg -                                         */
/*      Indique à OWL qu'on ne souhaite traiter aucun message avant les   */
/*      parents.                                                          */
/*                                                                        */
/**************************************************************************/

BOOL  TFilletBar :: PreProcessMsg ( MSG & )
   {
	return ( FALSE ) ;
    }





/**************************************************************************/
/*                                                                        */
/*   TFilletBar :: GetDesiredSize -                                       */
/*      Retourne la taille désirée.                                       */
/*                                                                        */
/**************************************************************************/

void	TFilletBar :: GetDesiredSize  ( TSize &  size )
   {
	if  ( Direction  ==  Horizontal )
	   {
		size. cx = Attr. W ;
		size. cy = WidthOrHeight ;
	    }
	else
	   {
		size. cx = WidthOrHeight ;
		size. cy = Attr. H ;
	    }
     }





/**************************************************************************/
/*                                                                        */
/*  TFilletBar :: Paint -                                                 */
/*      Dessine le liséré.                                                */
/*                                                                        */
/**************************************************************************/

void	TFilletBar :: Paint  ( TDC &  DC, BOOL, TRect & )
   {
	TBrush		Brush  		( Color ),
			HighlightBrush  ( GetSysColor ( COLOR_BTNHIGHLIGHT ) ),
			ShadowBrush     ( GetSysColor ( COLOR_BTNSHADOW ) ) ;
	TRect		Rect  	= GetClientRect ( ) ;
	int		xB 	= GetSystemMetrics ( SM_CXBORDER ),
			yB 	= GetSystemMetrics ( SM_CYBORDER ) ;


// Dessin de l'ombrage du machin
	DC. SelectObject ( HighlightBrush ) ;
	DC. PatBlt ( 0, 0, Rect. Width ( ) - xB, yB, PATCOPY ) ;
	DC. PatBlt ( 0, 0, xB, Rect. Height ( ) - yB, PATCOPY ) ;

	DC. SelectObject ( ShadowBrush ) ;
	DC. PatBlt ( 1, Rect. Height ( ) -xB, Rect. Width ( ) - xB, yB, PATCOPY ) ;
	DC. PatBlt ( Rect. Width ( ) - xB, yB, xB, Rect. Height ( ) - yB, PATCOPY ) ;

// Dessin du fond
	Rect. left 	+= xB ;
	Rect. right	-= xB ,
	Rect. top	+= yB ;
	Rect. bottom	-= yB ;
	DC. FillRect ( Rect, Brush ) ;
    }
