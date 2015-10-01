/**************************************************************************/
/*                                                                        */
/*  MenuItem.cpp -  Christian Vigh, 95/10.                                */
/*                                                                        */
/*  Implémentation de la classe TGadgetMenuItem.                          */
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

// Constructeur pour les gadgets de type séparateur
TGadgetMenuItem :: TGadgetMenuItem  ( )
	: TGadget ( -1 )
   {
	Init ( 0, Separator, Enabled ) ;
    }



// Et pour les autres
TGadgetMenuItem :: TGadgetMenuItem  ( int			id,
				      char *			name,
				      TType	type,
				      TState	state )
	: TGadget ( id )
   {
	Init ( name, type, state ) ;
    }



// Initialisation de l'objet
void	TGadgetMenuItem :: Init ( char *		name,
				  TType	type,
				  TState  state ) 
   {
	SavedCursor	=  0 ;
	LoadedCursor	=  0 ;
	SelectionStyle	=  NormalGadget ;
	Type		=  type ;
	State		=  state ;
	Interval	=  0 ;
	Selected	=  FALSE ;
	MouseEntered	=  FALSE ;
	ItemText   	=  0 ;

	SetText ( name ) ;

	Margins. Left	= 
	Margins. Right	=
	Margins. Top	=
	Margins. Bottom = 0 ;


// Calculer la taille des checkmarks et des indicateurs
	CheckmarkSize. cx = 12 ;
	CheckmarkSize. cy = 12 ;

	IndicatorSize. cx = 12 ;
	IndicatorSize. cy = 12 ;


// Calculer les unités de bordure 
	BorderUnits. cx  =  GetSystemMetrics ( SM_CXBORDER ) ;
	BorderUnits. cy  =  GetSystemMetrics ( SM_CYBORDER ) ;
	

// Puis les épaisseurs de sélection, exprimées en pixels
	SelectionSize. cx  =  2 * BorderUnits. cx ;
	SelectionSize. cy  =  2 * BorderUnits. cy ;

	SeparatorSize = 8 * BorderUnits. cy ;

// Et les séparations entre éléments
	BorderDelta    = 3 * BorderUnits. cx ;
	ElementDelta   = 6 * BorderUnits. cy ;
	IndicatorDelta = 2 * ElementDelta ;
     }





/**************************************************************************/
/*                                                                        */
/*  Destructeur.                                                          */
/*                                                                        */
/**************************************************************************/

TGadgetMenuItem :: ~TGadgetMenuItem  ( )
   {
	if  ( ItemText )
		delete  [] ItemText ;
	if  ( SavedCursor )
		SetCursor ( SavedCursor ) ;
	if  ( LoadedCursor )
		DestroyCursor ( LoadedCursor ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  Fonctions de paramètrage et de récupération d'informations.           */
/*                                                                        */
/**************************************************************************/

// Redéfinir le texte de l'élément de menu
// 
void	TGadgetMenuItem :: SetText  ( const char *	text )
   {
	if  ( ItemText )
		delete  []  ItemText ;

	if  ( text )
	   {
		ItemText = new  char [ strlen ( text ) + 1 ] ;
		strcpy ( ItemText, text ) ;
	    }
	else
		ItemText = 0 ;

	Invalidate ( TRUE ) ;		// Redessiner le gadget
     }



// Récupérer le texte de l'élément d'un menu
//
char *	TGadgetMenuItem :: GetText ( void )
   { 
	return  ( ItemText ) ;
    }




// Redéfinir l'état de l'élément
//
void	TGadgetMenuItem :: SetState  ( TState  st )
   {
	if  ( st  !=  State )
	   {
		State = st ;
		Invalidate ( TRUE ) ;
	    }
    }




// Récupérer l'état de l'élément
//
TGadgetMenuItem :: TState  TGadgetMenuItem :: GetState ( void )
   {
	return  ( State ) ;
    }

				      


// Récupérer le type de l'élément
//
TGadgetMenuItem :: TType 	TGadgetMenuItem :: GetType ( void )
   {
	return  ( Type ) ;
    }




// Change l'état de sélection de l'élément
//
void	TGadgetMenuItem :: SetSelection ( BOOL  sel )
   {
	if  ( Type  !=  Separator  &&  Selected  !=  sel )
	   {
		Selected = sel ;
		Invalidate ( FALSE ) ;
	    }
     }




// Retourne TRUE si la sélection se trouve sur l'élément
//
BOOL	TGadgetMenuItem :: GetSelection  ( void )
   {
	return ( Selected ) ;
    }





// Change l'état de la checkmark
//
void	TGadgetMenuItem :: SetCheck  ( TState  st )
   {
	TState 	Previous 	=  State & ( Checked | Unchecked ) ;
	register BOOL		ChangedSize 	=  FALSE ;



	if  ( st & Checked )
	   {
		State  &=  ~Unchecked ;
		State  |=  Checked ;
		ChangedSize = ! ( Previous & ( Checked | Unchecked ) ) ;
	    }
	else if  ( st & Unchecked )
	   {
		State  &=  ~Checked ;
		State  |=  Unchecked ;
		ChangedSize = ! ( Previous & ( Checked | Unchecked ) ) ;
	    }                                                           
	else
	   {
		State  &=  ~( Checked | Unchecked ) ;
		ChangedSize = ( Previous & ( Checked | Unchecked ) ) ;
	    }


	if  ( Window )
	   {
		if  ( ChangedSize )
			Window -> GadgetChangedSize ( * this ) ;
		else
			Invalidate ( FALSE ) ;
	    }
     }	   





// Changer le style de la sélection
//
void	TGadgetMenuItem :: SetSelectionStyle  ( TGadgetStyle  style )
   {
	SelectionStyle = style ;
    }



TGadgetStyle	TGadgetMenuItem :: GetSelectionStyle  ( void )
   {
	return  ( SelectionStyle ) ;
    }



	     



/**************************************************************************/
/*                                                                        */
/*  TGadgetMenuItem :: GetDesiredSize -                                   */
/*      Retourne la taille désirée.                                       */
/*                                                                        */
/**************************************************************************/

void	TGadgetMenuItem :: GetDesiredSize  ( TSize &  size )
   {
// Si l'élément est un séparateur, calculer sa hauteur
	if  ( Type  ==  Separator )
	   {
		size. cx = 0 ;
		size. cy = SeparatorSize ;
		return ;
	    }


// Autre : D'abord, la taille prise pour dessiner la sélection
	size. cx = 2 * SelectionSize. cx ;
	size. cy = 2 * SelectionSize. cy ;


// Ajouter ensuite :
// 	. L'espace entre le bord et la checkmark
// 	. La taille de la checkmark
// 	. L'espace entre la checkmark et le texte
// MaxHeight retient la hauteur maximale entre la checkmark, le texte et 
// l'indicateur de sous-menu. Il sera ensuite ajoute à la hauteur finale
	register int		MaxHeight = max ( CheckmarkSize. cy, IndicatorSize. cy ) ;


	size. cx += BorderDelta + CheckmarkSize. cx + ElementDelta ; 


// Récupérer ensuite la hauteur du texte
	if  ( ItemText  &&  Window )
	   {
		TScreenDC	DC ;
		TEXTMETRIC 	tm ;
		TSize		TextSize ;


		DC. SelectObject ( Window -> GetFont ( ) ) ;
		TextSize = DC. GetTextExtent ( ItemText, strlen ( ItemText ) ) ;
		DC. GetTextMetrics ( tm ) ;

	// Ajouter la largeur supplémentaire
		size. cx += TextSize. cx ;

	// Et calculer la taille
		TextSize. cy += tm. tmExternalLeading + 
				( 2 * SelectionSize. cy ) ;

		if  ( TextSize. cy  >  MaxHeight )
			MaxHeight = TextSize. cy ;
	     }


// Ajouter l'espace entre le texte et l'indicateur, puis la taille de 
// l'indicateur s'il s'agit d'un menu popup
	if  ( Type  ==  Popup )
	   {
		size. cx += IndicatorDelta + IndicatorSize. cx ;

		if  ( IndicatorSize. cy  >  MaxHeight )
			MaxHeight = IndicatorSize. cy ;
	     }


// Puis le dernier espace final
	size. cx += BorderDelta ;


// Ajouter la hauteur maximale calculée
	size. cy += MaxHeight ;

    }





/**************************************************************************/
/*                                                                        */
/*   TGadgetMenuWindow :: GetShortCut -                                   */
/*      Retourne le caractère souligné de l'élément.                      */
/*                                                                        */
/**************************************************************************/

char	TGadgetMenuItem :: GetShortcut  ( void )
   {
	if  ( Type  !=  Separator  ||  ItemText )
	   {
		register char *		p 	= ItemText ;



		while  ( * p )
		   {
			if  ( * p  ==  '&' )
			   {
				p ++ ;

				if  ( * p  ==  '&' )
					p ++ ;
				else if  ( * p )
					return  ( * p ) ;
			    }
			p ++ ;
		    }
	     } 

	return ( 0 ) ;
     }




/**************************************************************************/
/*                                                                        */
/*  TGadgetMenuItem :: LButtonDown -                                      */
/*      Appui sur le bouton gauche.                                       */
/*                                                                        */
/**************************************************************************/

void	TGadgetMenuItem :: LButtonDown  ( uint  modkeys, TPoint &  pos )
   {
	TGadget :: LButtonDown ( modkeys, pos ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  TGadgetMenuItem :: LButtonUp -                                        */
/*      Gère le relâchement du bouton de la souris et signale une         */
/*      sélection à la fenêtre gadget.                                    */
/*                                                                        */
/**************************************************************************/

void	TGadgetMenuItem :: LButtonUp    ( uint  modkeys, TPoint &  pos )
   {
	TGadget :: LButtonUp ( modkeys, pos ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  TGadgetMenuItem :: MouseEnter -                                       */
/*      Gère l'entrée de la souris dans le rectangle correspondant à      */
/*      l'élément de menu.                                                */
/*                                                                        */
/**************************************************************************/

void	TGadgetMenuItem :: MouseEnter    ( uint  modkeys, TPoint &  pos )
   {
	TGadget :: MouseEnter ( modkeys, pos ) ;
	MouseEntered = TRUE ;

	if  ( Type  ==  Popup )
	   {
		if  ( ! LoadedCursor )
			LoadedCursor = LoadCursor ( * :: Module, "POPUP_CURSOR" ) ;

		if  ( ! SavedCursor )
			SavedCursor = SetCursor ( LoadedCursor ) ;
	     }
    }




/**************************************************************************/
/*                                                                        */
/*  TGadgetMenuItem :: MouseLeave -                                       */
/*      Gère la sortie de la souris dans le rectangle correspondant à     */
/*      l'élément de menu.                                                */
/*                                                                        */
/**************************************************************************/

void	TGadgetMenuItem :: MouseLeave    ( uint  modkeys, TPoint &  pos )
   {
	TGadget :: MouseLeave ( modkeys, pos ) ;
	MouseEntered = FALSE ;

	if  ( Type  ==  Popup )
	   {
		if  ( SavedCursor )
		   {
			SetCursor ( SavedCursor ) ;
			SavedCursor = 0 ;
		    }

		if  ( LoadedCursor )
		   {
			DestroyCursor ( LoadedCursor ) ;
			LoadedCursor = 0 ;
		    }
	     }
    }





/**************************************************************************/
/*                                                                        */
/*  TGadgetMenuItem :: MouseMove  -                                       */
/*      Gère le déplacement de la souris.                                 */
/*                                                                        */
/*                                                                        */
/**************************************************************************/

void	TGadgetMenuItem :: MouseMove    ( uint  modkeys, TPoint &  pos )
   {
	TGadget :: MouseMove ( modkeys, pos ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TGadgetMenuItem :: PaintBorder -                                      */
/*      Ne fait rien, puisqu'il n'y a pas de bordure à dessiner.          */
/*                                                                        */
/**************************************************************************/

void	TGadgetMenuItem :: PaintBorder  ( TDC & )
   { }





/**************************************************************************/
/*                                                                        */
/*  TGadgetMenuItem :: Paint -                                            */
/*      Peint le menu. Appelle PaintCheckMark s'il y en a une,            */
/*      puis PaintContents, et enfin PaintIndicator.                      */
/*                                                                        */
/**************************************************************************/

void	TGadgetMenuItem :: Paint  ( TDC &  dc )
   {
	TRect       	Rect ( 0, 0, Bounds. Width ( ), Bounds. Height ( ) ) ;



// Cas du séparateur : ne dessiner que lui
	if  ( Type  ==  Separator )
	   {
		PaintSeparator ( dc, Rect ) ;
		return  ;
	    }


// Tout effacer
	TBrush		Background ( GetSysColor ( COLOR_BTNFACE ) ) ;

	
	dc. FillRect ( Rect, Background ) ;


// Peindre la sélection
	PaintSelection ( dc, Rect ) ;

	Rect. top    = SelectionSize. cy ;
	Rect. bottom = Bounds. Height ( ) -  SelectionSize. cy ;
	Rect. left   = SelectionSize. cx + BorderDelta ;
	Rect. right  = SelectionSize. cx + CheckmarkSize. cx ;


// Peindre éventuellement la checkmark
	TState	st = State & ( Checked | Unchecked ) ;

	if  ( st )
		PaintCheckMark ( dc, Rect, st ) ;


// Dessiner le texte
	Rect. left += CheckmarkSize. cx + ElementDelta ;
	PaintContents ( dc, Rect ) ;


// Dessiner l'indicateur s'il s'agit d'un menu popup
	if  ( Type  ==  Popup )
	   {
		Rect. right   = Bounds. Width ( ) - 
					SelectionSize. cx -
					BorderDelta ;	// Cadrer sur la droite
		Rect. left    = Rect. right - IndicatorSize. cx ;
		PaintIndicator ( dc, Rect ) ;
	    }
     }		





/**************************************************************************/
/*                                                                        */
/*  TGadgetMenuItem :: PaintSelection -                                   */
/*      Peint la sélection.                                               */
/*                                                                        */
/**************************************************************************/

void	TGadgetMenuItem :: PaintSelection ( TDC &  dc, TRect &  rect )
   {
	if  ( SelectionStyle  !=  NormalGadget )
	   {
		COLORREF		C1 ;
		COLORREF		C2 ;


	// Choix des couleurs selon la sélection et le style
		if  ( Selected )
		   {
			if  ( SelectionStyle  ==  RaisedGadget )
			   {
				C1 = GetSysColor ( COLOR_BTNHIGHLIGHT ) ;
				C2 = GetSysColor ( COLOR_BTNSHADOW ) ;
			    }
			else
			   {
				C1 = GetSysColor ( COLOR_BTNSHADOW ) ;
				C2 = GetSysColor ( COLOR_BTNHIGHLIGHT ) ;
			    }
		    }
		else        
			C1 = C2 = GetSysColor ( COLOR_BTNFACE ) ;
		

	// Création des pinceaux
		TBrush			HiliteBrush ( C1 ) ;
		TBrush			ShadowBrush ( C2 ) ;
		register int		i ;	// Déclaration de la varibale i


	// Dessiner la partie éclairée
		dc. SelectObject ( HiliteBrush ) ;

		for  ( i = 0 ; i < SelectionSize. cy ; i ++ )	// Ligne horizontale
			dc. PatBlt ( rect. left + i,
				     rect. top + i,
				     rect. Width ( ) - ( i * 2 ),
				     BorderUnits. cy,
				     PATCOPY ) ;
				 
		for  ( i = 0 ; i < SelectionSize. cx ; i ++ )	// Ligne verticale
			dc. PatBlt ( rect. left + i,
				     rect. top  + i,
				     BorderUnits. cx,
				     rect. Height ( ) - ( i * 2 ),
				     PATCOPY ) ;

		dc. RestoreBrush ( ) ;

		
	// Puis la partie sombre
		dc. SelectObject ( ShadowBrush ) ;

		for  ( i = 0 ; i < SelectionSize. cx ; i ++ )	// Ligne verticale
			dc. PatBlt ( rect. right - 1 - i,
				     rect. top + i,
				     BorderUnits. cx,
				     rect. Height ( ) - ( 2 * i ),
				     PATCOPY ) ;


		for  ( i = 0 ; i < SelectionSize. cy ; i ++ )	// Ligne verticale
			dc. PatBlt ( rect. left + i,
				     rect. bottom - 1 - i,
				     rect. Width ( ) - ( 2 * i ),
				     BorderUnits. cy,
				     PATCOPY ) ;

		dc. RestoreBrush ( ) ;
	    }
	else if  ( SelectionStyle  ==  NormalGadget  &&  Selected )
	   {
		TBrush		Brush ( GetSysColor ( COLOR_HIGHLIGHT ) ) ;
		TRect		newRect ( rect. left  + BorderUnits. cx,
					  rect. top,
					  rect. right - BorderUnits. cx,
					  rect. bottom ) ;
					  


		dc. SelectObject ( Brush ) ;
		dc. PatBlt ( newRect, PATCOPY ) ;
		dc. RestoreBrush ( ) ;
	    }
    }


	




/**************************************************************************/
/*                                                                        */
/*  TGadgetMenuItem :: FillSurface -                                      */
/*      Remplit une surface avec la couleur spécifiée.                    */
/*                                                                        */
/**************************************************************************/

void	TGadgetMenuItem :: FillSurface  ( TDC &  dc, TPoint &  pos, COLORREF  color )
   {
	TBrush		Brush ( color ) ;
	COLORREF	old_color	=  dc. GetPixel ( pos ) ;


	dc. SelectObject ( Brush ) ;
	dc. ExtFloodFill ( pos, old_color, FLOODFILLSURFACE ) ;
	dc. RestoreBrush ( ) ;
    }

	


/**************************************************************************/
/*                                                                        */
/*  TGadgetMenuItem :: PaintCheckMark -                                   */
/*      Dessine une checkmark standard.                                   */
/*                                                                        */
/**************************************************************************/

void	TGadgetMenuItem :: PaintCheckMark  ( TDC &  			dc, 
					     TRect & 			rect, 
					     TState	st )
   {
	TBrush 		BorderBrush ( GetSysColor ( COLOR_BTNTEXT ) ) ;
	TPen		ShadowPen   ( GetSysColor ( COLOR_BTNSHADOW ) ),
			HilitePen   ( GetSysColor ( COLOR_BTNHIGHLIGHT ) ),
			BlackPen    ( GetSysColor ( COLOR_BTNTEXT ) ) ;


	if  ( st  & ( Checked | Unchecked ) )
	   {
		register int	y = rect. top + ( ( rect. Height ( ) - CheckmarkSize. cy ) / 2 ) ;


	// Rectangle
		dc. FrameRect ( rect. left, y, 
				rect. left + CheckmarkSize. cx,
				y + CheckmarkSize. cy, BorderBrush ) ;

	// Partie sombre
		dc. SelectObject ( ShadowPen ) ;
		dc. MoveTo ( rect. left + 1, y + 1 ) ;
		dc. LineTo ( rect. left + CheckmarkSize. cx - 1, y + 1 ) ;
		dc. MoveTo ( rect. left + 1, y + 2 ) ;
		dc. LineTo ( rect. left + 1, y + CheckmarkSize. cy - 1 ) ;
		dc. RestorePen ( ) ;


	// Partie éclairée
		dc. SelectObject ( HilitePen ) ;
		dc. MoveTo ( rect. left + CheckmarkSize. cx - 2, y + 1 ) ;
		dc. LineTo ( rect. left + CheckmarkSize. cx - 2, 
				y + CheckmarkSize. cy - 2 ) ;
		dc. MoveTo ( rect. left + 1, y + CheckmarkSize. cy - 2 ) ;
		dc. LineTo ( rect. left + CheckmarkSize. cx - 2,
				y + CheckmarkSize. cy - 2 ) ;
		dc. RestorePen ( ) ;


	// La croix
		if  ( State  &  Checked )
		   {
			dc. SelectObject ( BlackPen ) ;
			dc. MoveTo ( rect. left + 3, y + 3 ) ;
			dc. LineTo ( rect. left + CheckmarkSize. cx - 3,
					y + CheckmarkSize. cy - 3 ) ;
			dc. MoveTo ( rect. left + CheckmarkSize. cx - 4,
					y + 3 ) ;
			dc. LineTo ( rect. left + 2, y + CheckmarkSize. cy - 3 ) ;
			dc. RestorePen ( ) ;
		     }

	// Remplir la surface
		FillSurface ( dc, TPoint ( rect. left + 3,
				   rect. top  + ( IndicatorSize. cy / 2 ) ),
				GetSysColor ( COLOR_BTNFACE ) ) ;

	   }
    }   







/**************************************************************************/
/*                                                                        */
/*  TGadgetMenuItem :: PaintIndicator -                                   */
/*      dessine l'indicateur de sous-menu.                                */
/*                                                                        */
/**************************************************************************/

void	TGadgetMenuItem :: PaintIndicator  ( TDC &  DC, TRect &  rect )
   {
	TPen			BlackPen   ( TColor ( 0 ) ),
				RedPen1    ( RGB ( 128, 0, 0 ) ),
				RedPen2	   ( RGB ( 255, 0, 0 ) ) ;
	TBrush			GrayBrush  ( GetSysColor ( COLOR_BTNHIGHLIGHT ) ),
				WhiteBrush ( RGB ( 255, 255, 255 ) ) ;
	TPoint			Org 	=  DC. GetViewportOrg ( ) ;
	


	DC. SetViewportOrg ( TPoint ( Org. x + rect. left, Org. y + rect. top + 
			( ( rect. Height ( ) - IndicatorSize. cy ) / 2 ) ) ) ;

//  L'indicateur est d'abord peint dans une bitmap d'un contexte mémoire
// D'abord, le rectangle du dessous
	DC. SelectObject ( BlackPen ) ;
	DC. SelectObject ( GrayBrush ) ;
	
// Tracé du 1er rectangle (celui du dessous)
	DC. Rectangle ( TRect ( 0, 0, IndicatorSize. cx - 3, 
				IndicatorSize. cy - 3 ) ) ;
	DC. RestoreBrush ( ) ;

// Puis du second
	DC. SelectObject ( WhiteBrush ) ;
	DC. Rectangle ( TRect ( 3, 3, IndicatorSize. cx, IndicatorSize. cy ) ) ;
	DC. RestoreBrush ( ) ;

	DC. RestorePen ( ) ;

// Tracé de la 1e sélection
	DC. SelectObject ( RedPen1 ) ;
	DC. MoveTo ( 1, 3 ) ;
	DC. LineTo ( 3, 3 ) ;
	DC. RestorePen ( ) ;

// Puis de la seconde
	DC. SelectObject ( RedPen2 ) ;
	DC. MoveTo ( 4, 6 ) ;
	DC. LineTo ( IndicatorSize. cx - 1, 6 ) ;
	DC. RestorePen ( ) ;

	DC. SetViewportOrg ( Org ) ;
    }                            








/**************************************************************************/
/*                                                                        */
/*  TGadgetMenuItem :: PaintContents -                                    */
/*      Dessine le contenu du menu.                                       */
/*                                                                        */
/**************************************************************************/

void	TGadgetMenuItem :: PaintContents  ( TDC &  dc, TRect &  rect )
   {
	if  ( ItemText )
	   {
		TGadgetMenuWindow *	w 	= TYPESAFE_DOWNCAST ( Window, TGadgetMenuWindow ) ;
		register BOOL		IsClone	= ( w  &&  w -> IsClone ( ) ) ?  TRUE : FALSE ;


		dc. SaveDC ( ) ;
		dc. SetBkMode ( TRANSPARENT ) ;

		dc. SelectObject ( Window -> GetFont ( ) ) ;

		if  ( State  &  ( Grayed | Disabled )  ||
			( ! IsClone  &&  GetFocus ( )  !=  Window -> HWindow ) )
			dc. SetTextColor ( GetSysColor ( COLOR_BTNSHADOW ) ) ;

		dc. DrawText ( ItemText, strlen ( ItemText ), 
			rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOCLIP ) ;

		dc. RestoreDC ( ) ;
	    }
    }





/**************************************************************************/
/*                                                                        */
/*  TGadgetMenuItem :: PaintSeparator -                                   */
/*      Dessine un séparateur de menu.                                    */
/*                                                                        */
/**************************************************************************/

void	TGadgetMenuItem :: PaintSeparator ( TDC &  dc, TRect &  rect )
   {
	TBrush			HiliteBrush ( GetSysColor ( COLOR_BTNHIGHLIGHT ) ) ;
	TBrush			ShadowBrush ( GetSysColor ( COLOR_BTNSHADOW ) ) ;
	int			count  = ( SeparatorSize - 4 ) / 2 ;
	register int		i ;


// Dessiner la partie ombrée
	i = count ;
	dc. SelectObject ( ShadowBrush ) ;

	while  ( i -- )
		dc. PatBlt  ( rect. left + i,
			      rect. top + 2 + ( count - i ),
			      rect. Width ( ) - ( 2 * i ),
			      BorderUnits. cy, PATCOPY ) ;			
	
	dc. RestoreBrush ( ) ;

// Puis la partie éclairée
	i = 0 ;
	dc. SelectObject ( HiliteBrush ) ;


	while  ( i ++ < count )
		dc. PatBlt ( rect. left + i,
			     rect. top + 2 + count + i,
			     rect. Width ( ) - ( 2 * i ),
			     BorderUnits. cy, PATCOPY ) ;

	dc. RestoreBrush ( ) ;

    }



