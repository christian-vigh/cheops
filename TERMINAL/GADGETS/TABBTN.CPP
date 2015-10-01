/**************************************************************************/
/*                                                                        */
/*  TabBtn.cpp  -  Christian Vigh, 95/10.                                 */
/*                                                                        */
/*  Implémentation de la classe TTabButtonGadget.                         */
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


TTabButtonGadget :: TTabButtonGadget  ( int		id,
					bool  		enabled,
					bool		overlap,
					int		numchar,
					bool		shadowing )
	: Overlap ( overlap ),	// Important pour notre GetBrush
	  ShadowingEnabled ( shadowing ),
	  TShapeButtonGadget ( id, Exclusive, enabled, numchar )
   {
	ShrinkWrapWidth = TRUE ;
    }






/**************************************************************************/
/*                                                                        */
/*  TTabButtonGadget :: GetDesiredSize -                                  */
/*      Calcule la taille finale du gadget.                               */
/*                                                                        */
/**************************************************************************/

void	TTabButtonGadget :: GetDesiredSize  ( TSize &  Size )
   {
	TShapeButtonGadget :: GetDesiredSize ( Size ) ;


	cySoftCorner = Size. cy / 2 ;
	dX           = ( Size. cy - cySoftCorner ) ;	// Ca donne un angle de 45°

	if  ( ! Overlap ) 
		Size. cx += dX * 2 ;	// On devrait faire 2 * dX mais ça fait trop grand
	else
		Size. cx += dX ;
     }




/**************************************************************************/
/*                                                                        */
/*  TTabButtonGadget :: ComputePoints -                                   */
/*      Calcule les coordonnées des coins de l'onglet.                    */
/*                                                                        */
/**************************************************************************/

void	TTabButtonGadget :: ComputePoints  ( TPoint *  Points )
   {
	register int		Height	= Bounds. Height ( ),
				Width	= Bounds. Width ( ) ;



	if  ( Overlap )
		Width += dX ;


	Points [0]. x	=  0 ;
	Points [0]. y 	=  0 ;

	Points [1]. x 	=  Width ;
	Points [1]. y	=  0 ;

	Points [2]. x 	=  Width ;
	Points [2]. y	=  cySoftCorner ;

	Points [3]. x	=  Width - dX ;
	Points [3]. y	=  Height ;

	Points [4]. x	=  dX ;
	Points [4]. y 	=  Height ;

	Points [5]. x	=  0 ;
	Points [5]. y 	=  cySoftCorner ;
     }





/**************************************************************************/
/*                                                                        */
/*  TTabButtonGadget :: CheckOverlappedRegion -                           */
/*      Calcule la région visible de l'onglet s'il y a chevauchement.     */
/*                                                                        */
/**************************************************************************/

void	TTabButtonGadget :: CheckOverlappedRegion  ( TRegion &  Region )
   {
	register TGadget *		p	=  Window -> FirstGadget ( ),
			 * 		q	=  0 ;



	if  ( State  ==  Down )
		return ;


// Rechercher le gadget de gauche (y a pas de PreviousGadget( ) )
	while  ( p  &&  p  !=  this )
	   {
		q = p ;
		p = p -> NextGadget ( ) ;
	    }


// Si trouvé et que le gadget est Up, il faut retirer son intersection avec
// ce gadget de la région visible (uniquement si le gadget de gauche est un
// onglet)
	if  ( q )
	   {
		TTabButtonGadget *	tab = TYPESAFE_DOWNCAST ( q, TTabButtonGadget ) ;


		if  ( tab  &&  tab -> State   ==  Down )
		   {
			TRegion		LeftRegion ;
		


			tab -> GetRegion  ( LeftRegion, OuterRegion ) ;
			Region -= LeftRegion ;
		    }
	     }


// On regarde à droite
	p = NextGadget ( ) ;


	if  ( p )
	   {
		TTabButtonGadget *	tab = TYPESAFE_DOWNCAST ( p, TTabButtonGadget ) ;

		if  ( tab )
		   {
			TRegion		RightRegion ;


			tab -> GetRegion  ( RightRegion, OuterRegion ) ;
			Region -= RightRegion ;
		    }
	     }
     }




/**************************************************************************/
/*                                                                        */
/*  TTabButtonGadget :: GetRegion -                                       */
/*      Crée une région correspondant à la partie spécifiée.              */
/*                                                                        */
/**************************************************************************/

BOOL	TTabButtonGadget :: GetRegion ( TRegion &  Region, TRegionType  type )
   {
	TPoint			TabPoints [6] ;
	TPoint			Points    [8] ;
	register BOOL           Status		=  TRUE ;
	register int		i,
				HiliteDelta = HiliteUnits / 2, 	// Servent à épaissir
				ShadowDelta = ShadowUnits / 2 ; // les bordures diagonales
	register int		Width	    = Bounds. Width ( ) ;



	if  ( Overlap )	
		Width += dX ;

	ComputePoints ( TabPoints ) ;


	switch  ( type )
	   {

	// Partie visible : calculer ce qui n'est pas recouvert
	// Il faut bien faire attention de calculer la coordonnée du gadget
	// DANS la fenêtre pour que la région de clipping soit correcte.
		case	VisibleRegion :
		case	OuterRegion :
			for  ( i = 0 ; i < 6 ; i ++ )
			   {
				TabPoints [i]. x += Bounds. left ;
				TabPoints [i]. y += Bounds. top ;
			    }

			Region = TRegion ( TabPoints, 6, ALTERNATE ) ;

			if  ( Overlap  &&  type  ==  VisibleRegion )
				CheckOverlappedRegion ( Region ) ;
			break ;


	// Partie englobant la totalité du gadget, sans la bordure
		case	InnerRegion :
			TabPoints [0]. x	+=  cxBorder ;
			TabPoints [0]. y 	+=  cyBorder ;

			TabPoints [1]. x 	-=  cxBorder ;
			TabPoints [1]. y	+=  cyBorder ;

			TabPoints [2]. x 	-=  cxBorder ;
			TabPoints [2]. y	-=  2 * cyBorder ;

			TabPoints [3]. x	-=  cxBorder ;
			TabPoints [3]. y	-=  2 * cyBorder ;

			TabPoints [4]. x	+=  cxBorder ;
			TabPoints [4]. y 	-=  2 * cyBorder ;

			TabPoints [5]. x	+=  cxBorder ;
			TabPoints [5]. y 	-=  2 * cyBorder ;
				       

	// et avec la bordure
		case	BorderRegion :
			Region = TRegion ( TabPoints, 6, ALTERNATE ) ;
			break ;


	// Partie dessinable
		case	DrawableRegion :
		   {
			int		delta 	=  ( State  ==  Up  ||
							( Overlap  &&  State  ==  Down ) ) ?
							0 : HiliteUnits ;
			register int	W	=  Width          -
						   HiliteUnits        -
						   ShadowUnits        -
						   ( 2 * dX )         -
						   ( 2 * cxBorder ),
					H	=  Bounds. Height ( ) -
						   HiliteUnits	      -
						   ShadowUnits 	      -
						   ( 2 * cyBorder ),
					X	=  cxBorder + HiliteUnits + delta + dX,
					Y 	=  cyBorder + HiliteUnits + delta ;


			Region = TRegion ( TRect ( X, Y, X + W, Y + H ) ) ;
			break ;
		     }


	// Partie éclairée
		case	HiliteRegion :
			Points [0]. x = cxBorder ;
			Points [0]. y = cyBorder ;

			Points [1]. x = Width - cxBorder ;
			Points [1]. y = cyBorder ;

			Points [2]. x = Width - cxBorder ;
			Points [2]. y = cyBorder + HiliteUnits ;

			Points [3]. x = cxBorder + HiliteUnits ;
			Points [3]. y = cyBorder + HiliteUnits ;

			Points [4]. x = cxBorder + HiliteUnits  ;
			Points [4]. y = cySoftCorner - cyBorder - HiliteDelta ;

			Points [5]. x = dX + cxBorder + HiliteUnits + HiliteDelta ;
			Points [5]. y = Bounds. Height ( ) - cyBorder ;

			Points [6]. x = dX + cxBorder ;
			Points [6]. y = Bounds. Height ( ) - cyBorder ;

			Points [7]. x = cxBorder ;
			Points [7]. y = cySoftCorner - cyBorder ;

			Region = TRegion ( Points, 8, ALTERNATE ) ;
			break ;


	// Partie ombrée 
		case	ShadowRegion :
			Points [0]. x = Width - cxBorder ;
			Points [0]. y = cyBorder ;

			Points [1]. x = Width - cxBorder ;
			Points [1]. y = cySoftCorner - cyBorder ;

			Points [2]. x = Width - dX - cxBorder ;
			Points [2]. y = Bounds. Height ( ) - cyBorder ;

			Points [3]. x = dX - 1 ;
			Points [3]. y = Bounds. Height ( ) - cyBorder ;

			Points [4]. x = dX + HiliteUnits - 1 ;
			Points [4]. y = Bounds. Height ( ) - cyBorder - ShadowUnits ;

			Points [5]. x = Width - dX - cxBorder - ShadowDelta ;
			Points [5]. y = Bounds. Height ( ) - cyBorder - ShadowUnits ;
	
			Points [6]. x = Width - cxBorder - ShadowUnits ;
			Points [6]. y = cySoftCorner - cyBorder - ShadowDelta ;

			Points [7]. x = Width - cxBorder - ShadowUnits ;
			Points [7]. y = cyBorder + HiliteUnits ;


			Region = TRegion ( Points, 8, ALTERNATE ) ;
			break ;


		case	OtherShadowRegion :
			Status = FALSE ;
			break ;
				
	
	// Autre : ne devrait jamais arriver
		default :
			Status = false ;
	     }



	return  ( Status ) ;
    }



	




/**************************************************************************/
/*                                                                        */
/*   TTabButtonGadget :: SetButtonState -                                 */
/*      Si le mode overlap est ON, et que le gadget était down, il faut   */
/*      repeindre celui de droite (son coin gauche).                      */
/*                                                                        */
/**************************************************************************/

void	TTabButtonGadget :: SetButtonState  ( TState  state )
   {
	register TState		previous = State ;


	TShapeButtonGadget :: SetButtonState ( state ) ;


	if  ( Overlap   &&  previous  !=  State )
	   {
		TGadget *		g   = NextGadget ( ) ;
		TTabButtonGadget *      tab = TYPESAFE_DOWNCAST ( g, TTabButtonGadget ) ;
		
		
		if  ( previous  ==  Down  &&  State  ==  Up )
		   {
			if  ( tab  &&  tab -> State  ==  Up )
			   {
				TRegion *	Region 	=  new  TRegion ( ) ;
				

				GetRegion ( * Region, OuterRegion ) ;
				tab -> SetIntersectionRegion ( Region ) ;
				tab -> Invalidate ( ) ;
			     }
		     }
	     }
      }	


		



/**************************************************************************/
/*                                                                        */
/*  TTabButtonGadget :: GetRegionColor -                                  */
/*      En mode overlap, lorsque le bouton est Up (mais visuellement,     */
/*      down), inverse les couleurs de l'ombrage.                         */
/*                                                                        */
/**************************************************************************/

COLORREF	TTabButtonGadget :: GetRegionColor  ( TRegionType  type )
   {
	if  ( Overlap  &&  State  ==  Up )
	   {
		if  ( type  ==  HiliteRegion )
			type = ShadowRegion ;
		else if  ( type  ==  ShadowRegion )
			type = HiliteRegion ;
	    }

	return  ( TShapeButtonGadget :: GetRegionColor ( type ) ) ;
     }






/**************************************************************************/
/*                                                                        */
/*  TTabButtonGadget :: Paint -                                           */
/*      Peint l'ombre du bouton gauche et droit s'il y a chevauchement,   */
/*      et que ce bouton n'est pas Down.                                  */
/*                                                                        */
/**************************************************************************/

void	TTabButtonGadget :: Paint  ( TDC &  dc )
   {
// Dessiner d'abord le bouton
	TShapeButtonGadget :: Paint ( dc ) ;


// Puis s'arrêter là s'il n'y a pas chevauchement ou qu'il est Down
	if  ( ! Overlap  ||  State  ==  Down )
		return ;
      }	
				






