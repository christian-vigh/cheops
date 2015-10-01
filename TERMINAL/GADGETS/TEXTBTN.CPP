/**************************************************************************/
/*                                                                        */
/*  TextBtn.cpp  -  Christian Vigh, 95/10.                                */
/*                                                                        */
/*  Implémentation de la classe TTextButton.                              */
/*                                                                        */
/**************************************************************************/



# pragma	option -k- -N- -r- -v- -vi- -y-
# include	<owl/owlall.h>
# pragma	hdrstop
# pragma	option -k. -N. -r. -v. -vi. -y.


# include	"ChopJet.h"




/**************************************************************************/
/*                                                                        */
/*  CommandEnabler pour ce gadget.                                        */
/*                                                                        */
/**************************************************************************/

class 	TTextButtonGadgetEnabler : 	public TCommandEnabler 
   {
	public :
		TTextButtonGadgetEnabler   ( HWND 			hWndReceiver, 
					  TTextButtonGadget * 	g )
			: TCommandEnabler ( g -> GetId (), hWndReceiver ) 
		   {
		      Gadget = g ;
		    }



		void  Enable	( bool  enable )
		   {
			TCommandEnabler :: Enable ( enable ) ;
			Gadget -> SetEnabled ( enable ) ;
		     }


		void  SetText ( const char * )
		   { }


		void  SetCheck ( int   state )
		   { 
			Gadget -> SetButtonState ( TTextButtonGadget :: TState ( state ) ) ;
		     }


	protected :
		TTextButtonGadget *  Gadget ;
    } ;








/**************************************************************************/
/*                                                                        */
/*  Constructeur.                                                         */
/*                                                                        */
/**************************************************************************/

TTextButtonGadget :: TTextButtonGadget	( int    	id,
					  TType		type,
					  bool   	enabled,
					  int 		charcount )

		: TGadget ( id, Plain )
   {
	TrackMouse 	= true ;
	SetButtonState ( Up ) ;
	Pressed 	= false ;

	SetEnabled ( enabled ) ;

	CharacterCount  = charcount ;
	Text = 0 ;

	HiliteUnits	= GetSystemMetrics ( SM_CXBORDER ) * 2 ;
	ShadowUnits	= GetSystemMetrics ( SM_CXBORDER ) * 2 ;

	ShrinkWrapWidth = FALSE ;
	Type		= type ;
     }




/**************************************************************************/
/*                                                                        */
/*  Destructeur.                                                          */
/*                                                                        */
/**************************************************************************/

TTextButtonGadget :: ~TTextButtonGadget ( )
   {
	if  ( Text )
		free ( Text ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  TTextButtonGadget :: CommandEnable -                                  */
/*      Envoie un message Command enable à la fenêtre gadget.             */
/*                                                                        */
/**************************************************************************/


void	TTextButtonGadget :: CommandEnable ( void )
   {
	Window -> Parent -> HandleMessage ( WM_COMMAND_ENABLE, 0,
		( LPARAM ) & TTextButtonGadgetEnabler ( * Window -> Parent, this ) ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  TTextButtonGadget :: SysColorChange -                                 */
/*      Synchronize les couleurs du bouton avec les nouvelles couleurs    */
/*      du système.                                                       */
/*                                                                        */
/**************************************************************************/


void	TTextButtonGadget :: SysColorChange ( )
   {
	Invalidate ( ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  TTextButtonGagdet :: CheckExclusively -                               */
/*      Vérifie qu'il n'y a qu'un onglet d'actif.                         */
/*                                                                        */
/**************************************************************************/

void	TTextButtonGadget :: CheckExclusively ( )
   {
	if  ( GetButtonState ( )  !=  Down )
	   {
		if  ( Window ) 
		   {
			:: LockWindowUpdate ( Window -> HWindow ) ;

			TGadget *        	g     = Window -> FirstGadget ( ) ;


		// 
		// Mettre tous les autres onglets à "non sélectionné"
		// Note : même si l'état du bouton n'a pas changé, on appelle
		// SetButtonState (charge à la classe dérivée de vérifier 
		// le nouvel état du bouton afin de ne pas se repeindre
		// inutilement)
			while  ( g ) 
			   {
				TTextButtonGadget *  tab = TYPESAFE_DOWNCAST ( g, TTextButtonGadget ) ;


			// Ne modifier que les boutons exclusifs
				if  ( tab -> Type  !=  Exclusive )
					continue ;


				if  ( tab  &&  tab  !=  this )
					tab -> SetButtonState ( Up ) ;
				else if  ( tab  ==  this )
					tab -> SetButtonState ( Down ) ;
					
				g = g -> NextGadget ( ) ;
			      }

			:: LockWindowUpdate ( 0 ) ;
		    }
	   }
    }






/**************************************************************************/
/*                                                                        */
/*  TTextButtonGadget :: SetButtonState -                                 */
/*      Change l'état du bouton.                                          */
/*                                                                        */
/**************************************************************************/


void	TTextButtonGadget :: SetButtonState ( TState  state )
   {
	if  ( state  !=  GetButtonState ( ) ) 
	   {
		State = state ;
		Invalidate ( ) ;
		Update ( ) ;
	    }
     }




/**************************************************************************/
/*                                                                        */
/*  TTextButtonGadget :: GetDesiredSize -                                 */
/*      Retourne la taille du bouton avec son texte.                      */
/*                                                                        */
/**************************************************************************/


void      TTextButtonGadget :: GetDesiredSize ( TSize &  size )
   {
	TScreenDC		DC ;
	TEXTMETRIC		tm ;


     

	size. cx = size. cy = 0 ;
	TGadget :: GetDesiredSize ( size ) ;
	DC. SelectObject   ( Window -> GetFont ( ) ) ;
	DC. GetTextMetrics ( tm ) ;


	if  ( ShrinkWrapWidth )
	   {                                     
		if  ( Text )
			size. cx += DC. GetTextExtent ( Text, strlen ( Text ) ). cx ;
	    }
	else
	   {
		size. cx += ( ( tm. tmMaxCharWidth + tm. tmAveCharWidth ) / 2 ) *
				CharacterCount ;
	     }


	size. cy  += Window -> GetFontHeight ( ) ;

	size. cx  +=  ( HiliteUnits + ShadowUnits ) * 2 ;
	size. cy  +=  ( ShadowUnits + HiliteUnits ) * 2 ;
    }







/**************************************************************************/
/*                                                                        */
/*  TTextButtonGadget :: Paint -                                          */
/*      Repeint le bouton.                                                */
/*                                                                        */
/**************************************************************************/


void	TTextButtonGadget :: Paint ( TDC &  dc )
   {
	int		cxBorder	=  GetSystemMetrics ( SM_CXBORDER ),
			cyBorder	=  GetSystemMetrics ( SM_CYBORDER ) ;
	TRect   	InnerRect ( 0, 0,
				    Bounds. Width  ( ),
				    Bounds. Height ( ) ) ;
	TBrush  	FaceBrush   ( GetSysColor ( COLOR_BTNFACE ) ),
			HiliteBrush ( GetSysColor ( COLOR_BTNHIGHLIGHT ) ),
			ShadowBrush ( GetSysColor ( COLOR_BTNSHADOW ) ) ;
	TRect		ContentRect 	=  InnerRect ;
	int		HiliteSize, ShadowSize ;



// Peindre le bord
	dc. FillRect ( InnerRect, FaceBrush ) ;
	PaintBorder ( dc ) ;


// Déterminer la hauteur et la largeur des "effets" de relief
	if  ( GetButtonState ( )  ==  Up  &&  ! Pressed )
	   {
		dc. SelectObject ( HiliteBrush ) ;
		HiliteSize = HiliteUnits ;
		ShadowSize = ShadowUnits ;

	// Calculer la taille du contenu affichable du bouton
		ContentRect. left	+=  HiliteSize ;
		ContentRect. right	-=  HiliteSize ;
		ContentRect. top	+=  ShadowSize ;
		ContentRect. bottom	-=  ShadowSize ;
	     }
	else
	   {
		dc. SelectObject ( ShadowBrush ) ;
		HiliteSize = ShadowUnits ;
		ShadowSize = 0 ;

		ContentRect. left += HiliteUnits + ShadowUnits ;
		ContentRect. top  += HiliteUnits + ShadowUnits ;
	    }





// Afficher le côté "lumineux" (gauche et haut) ou "ombré", selon que le bouton
// est relâché ou enfoncé
	TRect		DrawRect	=  InnerRect ;
	int		x		=  HiliteSize ;


	while  ( x )
	   {
		int		x2 = ( GetButtonState ( )  ==  Up ) ?  x : 0 ;
	// Ligne verticale
		dc. PatBlt (  DrawRect. left  + x, DrawRect. top    + x,
				cxBorder, DrawRect. Height ( ) - ( 2 * x2 ), PATCOPY ) ;

	// Et horizontale
		dc. PatBlt ( DrawRect. left   +  x2, DrawRect. top + x,
				DrawRect. Width ( ) - ( 2 * x2 ), cyBorder, PATCOPY ) ;

		x -- ;
	     }

	dc. RestoreBrush ( ) ;


// Peindre l'ombre du bouton si son état est haut
	dc. SelectObject ( ShadowBrush ) ;
	x = ShadowSize ;
	DrawRect = InnerRect ;

	while  ( x )
	   {
	// Ligne verticale
		dc. PatBlt ( DrawRect. right - x - 1, DrawRect. top    + x,
				cxBorder, DrawRect. Height ( ) - ( 2 * x ), PATCOPY ) ;

	// Et horizontale
		dc. PatBlt ( DrawRect. left   +  x, DrawRect. bottom - x - 1,
				DrawRect. Width ( ) - x - 1, cyBorder, PATCOPY ) ;

		x -- ;
	     }

	dc. RestoreBrush ( ) ;


// Dessine le contenu du bigntz	
	PaintContents ( dc, ContentRect ) ;
   }





/**************************************************************************/
/*                                                                        */
/*  TTextButtonGadget :: Invalidate -                                     */
/*      Invalide l'onglet (mais pas sa bordure).                          */
/*                                                                        */
/**************************************************************************/


void  TTextButtonGadget :: Invalidate ( void )
   {
	TRect  	r ( 0, 0, Bounds. Width ( ), Bounds. Height ( ) ) ;


	r. Inflate ( - GetSystemMetrics ( SM_CXBORDER ), 
			- GetSystemMetrics ( SM_CYBORDER ) ) ;
	InvalidateRect ( r, false ) ;
     }





/**************************************************************************/
/*                                                                        */
/*  TTextButtonGagdet :: BeginPressed                                     */
/*      Met le bouton à l'état enfoncé, le repeint et rentre dans l'état  */
/*      Menu select.                                                      */
/*                                                                        */
/**************************************************************************/

void  TTextButtonGadget :: BeginPressed ( TPoint & )
   {
	Pressed = true ;

	if  ( Type  ==  Command )
		SetButtonState ( Down ) ;

	if  ( Window -> GetHintMode ( )  ==  TGadgetWindow :: PressHints )
		Window -> SetHintCommand ( GetId ( ) ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TTextButtonGadget :: CancelPressed -                                  */
/*      Anunle l'état enfoncé, repeint le bouton et sort de l'état        */
/*      Menu select.                                                      */
/*                                                                        */
/**************************************************************************/

void   TTextButtonGadget :: CancelPressed ( TPoint & )
   {
	Pressed = false ;

	if  ( Type  ==  Command )
		SetButtonState ( Up ) ;


	if  ( Window -> GetHintMode ( )  ==  TGadgetWindow :: PressHints )
		Window -> SetHintCommand ( -1 ) ;
     }




/**************************************************************************/
/*                                                                        */
/*  TTextButtonGagdet :: Activate -                                       */
/*      Désactive les autres onglets.                                     */
/*                                                                        */
/**************************************************************************/

void	TTextButtonGadget :: Activate ( TPoint &  pt )
   {
	switch  ( Type )
	   {
		case	Exclusive :
			if  ( GetButtonState ( )  !=  Down )
				CheckExclusively ( ) ;
			break ;


		case	NonExclusive :
			SetButtonState ( ( GetButtonState ( )  ==  Up ) ?  Down : Up ) ;
			break ;
	      }


	CancelPressed ( pt ) ;

	if  ( ! ( Exclusive  &&  GetButtonState ( ) !=  Down ) &&  GetEnabled ( ) )
		Window -> Parent -> PostMessage ( WM_COMMAND, GetId ( ) ) ;
    }





/**************************************************************************/
/*                                                                        */
/*   TTextButtonGadget -                                                  */
/*      Messages souris.                                                  */
/*                                                                        */
/**************************************************************************/

void	TTextButtonGadget :: LButtonDown ( uint  modKeys, TPoint &  pt )
   {
	TGadget :: LButtonDown ( modKeys, pt ) ;
	BeginPressed ( pt ) ;
    }



void	TTextButtonGadget :: MouseMove ( uint  modKeys, TPoint &  pt )
   {
	TGadget :: MouseMove ( modKeys, pt ) ;

	bool  hit = PtIn ( pt ) ;
	
	if ( Pressed ) 
	   {
		if  ( ! hit )
			CancelPressed ( pt ) ;
	    }
	else if ( hit ) 
		BeginPressed ( pt ) ;
     }




void	TTextButtonGadget :: MouseEnter ( uint  modKeys, TPoint &  pt )
   {
	TGadget :: MouseEnter ( modKeys, pt ) ;

	if  ( Window -> GetHintMode ( )  ==  TGadgetWindow :: EnterHints )
		Window -> SetHintCommand ( GetId ( ) ) ;
    }





void	TTextButtonGadget :: MouseLeave ( uint  modKeys, TPoint &  pt )
   {
	TGadget :: MouseLeave ( modKeys, pt ) ;

	if  ( Window -> GetHintMode ( )  ==  TGadgetWindow :: EnterHints )
		Window -> SetHintCommand ( - 1 ) ;
    }




void	TTextButtonGadget :: LButtonUp ( uint  modKeys, TPoint &  pt )
   {
	TGadget :: LButtonUp ( modKeys, pt ) ;

	if  ( Pressed )
		Activate ( pt ) ;
     }





/**************************************************************************/
/*                                                                        */
/*  TTextButtonGadget :: SetText -                                        */
/*      Change le texte du bouton.                                        */
/*                                                                        */
/**************************************************************************/

void	TTextButtonGadget :: SetText  ( char *  text )
   {
	if  ( Text )
		free ( Text ) ;
	Text = strdup ( text ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TTextButtonGadget :: PaintContents -                                  */
/*      Dessine l'intérieur du bouton.                                    */
/*                                                                        */
/**************************************************************************/

void	TTextButtonGadget :: PaintContents ( TDC &  dc, TRect &  rect )
   {
	if  ( Text )
	   {
		dc. SaveDC ( ) ;
		dc. SetBkMode ( TRANSPARENT ) ;
		dc. SelectObject ( Window -> GetFont ( ) ) ;
		dc. SetTextColor ( GetSysColor ( 
			GetEnabled ( ) ?  COLOR_BTNTEXT : COLOR_GRAYTEXT ) ) ;
		dc. SetBkColor   ( GetSysColor ( COLOR_BTNFACE ) ) ;
		dc. DrawText ( Text, strlen ( Text ), rect, DT_CENTER |
							    DT_VCENTER |
							    DT_SINGLELINE ) ;
		dc. RestoreDC ( ) ;
	     }
     }
