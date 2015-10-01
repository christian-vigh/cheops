/**************************************************************************/
/*                                                                        */
/*  MenuWin.cpp  -  Christian Vigh, 95/10.                                */
/*                                                                        */
/*  Implémentation de la classe TGadgetMenuWindow.                        */
/*                                                                        */
/**************************************************************************/

/**************************************************************************
  MODIFICATIONS :

  28/04/1997 (CV)
	Initialisation de MenuDropped à FALSE dans EvLButtonUp pour éviter
	qu'un menu se "croie" décroché dans le cas de figure suivant :
	. L'utilisateur ouvre un sous-menu
	. Il clique dans l'objet TMenuDetachGadget afin de refermer le sous-
	  menu. Comme il y a de fortes chances qu'un mouvement de la souris
	  survienne à ce moment-là, l'objet TMenuDetachGadget envoie un
	  message WM_MENUDROPPED au parent.

 **************************************************************************/


# pragma	option -k- -N- -r- -v- -vi- -y-
# include	<owl/owlall.h>
# pragma	hdrstop
# pragma	option -k. -N. -r. -v. -vi. -y.


# include	"ChopJet.h"




// Id du timer utilisé pour l'ouverture automatique des menus popup
//
# define	TIMER_ID		123



/**************************************************************************/
/*                                                                        */
/*  Table de réponses.                                                    */
/*                                                                        */
/**************************************************************************/

DEFINE_RESPONSE_TABLE1 ( TGadgetMenuWindow, TGadgetWindow )
	EV_WM_CHAR,
	EV_WM_KEYDOWN,
	EV_WM_KILLFOCUS,
	EV_WM_SETFOCUS,
	EV_WM_LBUTTONDOWN,
	EV_WM_LBUTTONUP,
	EV_WM_LBUTTONDBLCLK,
	EV_WM_MOUSEMOVE,
	EV_WM_TIMER,
	EV_MESSAGE  ( WM_MENUDROPPED, WmMenuDropped ),
	EV_WM_SYSCOMMAND,
END_RESPONSE_TABLE ;




/**************************************************************************/
/*                                                                        */
/*  Constructeur pour un menu principal.                                  */
/*                                                                        */
/**************************************************************************/

TGadgetMenuWindow :: TGadgetMenuWindow  ( TPoint &	desired_bottom,
					  TWindow *	receiver,
					  char *	title,
					  TFont *	font )
	: TGadgetWindow ( 0, Vertical, font ) 
   { 
	DesiredBottom = desired_bottom ;
	Receiver      = receiver ;

	Attr. Style   = WS_VISIBLE  |  WS_POPUP | WS_CLIPSIBLINGS ;
	ParentMenu    = 0 ;
	ChildMenu     = 0 ;

	MenuType = MainMenu ;
	SelectionStyle 	= RaisedGadget ;
	MenuStyle       = RaisedGadget ;
	SetShrinkWrap ( TRUE, TRUE ) ;

	EnteredGadget 		= 0 ;
	TimeoutValue		= 0 ;
	TimerId			= 0 ;

	CurrentDirection	= TopRight ;

	Init ( title ) ;

    }




/**************************************************************************/
/*                                                                        */
/*   Constructeur pour un sous-menu.                                      */
/*                                                                        */
/**************************************************************************/

TGadgetMenuWindow :: TGadgetMenuWindow  ( TGadgetMenuWindow *	parent,
					  int			id,
					  char *		title,
					  TFont *		font )
	: TGadgetWindow ( 0, Vertical, font ) 
   { 
	Receiver      = parent -> Receiver ;

	Attr. Style   = WS_VISIBLE | WS_POPUP | WS_CLIPSIBLINGS ;

	ParentMenu    = parent ;
	ParentMenuId  = id ;
	ParentMenu -> ChildMenu = this ;
	ChildMenu     = 0 ;
	SelectionStyle 	= ParentMenu -> SelectionStyle ;
	MenuStyle       = ParentMenu -> MenuStyle ;

	SetShrinkWrap ( ParentMenu -> ShrinkWrapWidth,
				ParentMenu -> ShrinkWrapHeight ) ;

	EnteredGadget 		= ParentMenu -> EnteredGadget ;
	TimeoutValue		= ParentMenu -> TimeoutValue ;
	TimerId			= ParentMenu -> TimerId ;

	CurrentDirection	= ParentMenu -> CurrentDirection ;	
	
	MenuType = SubMenu ;
	Init ( title ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  Constructeur pour créer un menu à partir d'une copie.                 */
/*                                                                        */
/**************************************************************************/

TGadgetMenuWindow :: TGadgetMenuWindow  ( TGadgetMenuWindow &  other, 
					  TFont *	       font )
	: TGadgetWindow ( 0, Vertical, font ) 
   {
	MenuType = Clone ;
	Attr. Style = WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS ;	
	Title = 0 ;
	CloneWindow ( other ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  TGadgetMenuWindow :: CloneWindow                                      */
/*      Fait en sorte que cet objet soit la copie conforme du menu        */
/*      spécifié.                                                         */
/*      Note : l'autre menu doit être ensuite détruit. En effet, comme    */
/*      la classe TGadget ne comporte pas ce genre de fonction Clone,     */
/*      le seul moyen de dupliquer celle-ci est de faire un remove de     */
/*      tous les gadgets sur l'original puis de  les insérer ici.         */
/*                                                                        */
/**************************************************************************/

void	TGadgetMenuWindow :: CloneWindow ( TGadgetMenuWindow &  w )
   {
// Données à ne pas recopier
	ShutDownStarted 	=  FALSE ;
	CurrentGadget		=  0 ;
	DroppedPosition		=  TPoint ( 0, 0 ) ;
	ButtonPressed		=  FALSE ;
	AtMouseTicks		=  0 ;
	DiedFromEscape		=  FALSE ;
	DiedFromEscapeTime	=  0 ;
	ChildMenu		=  0 ;
	IgnoreNextButtonUp	=  FALSE ;
	IgnoreNextTimer		=  FALSE ;
	GotSelection		=  FALSE ;
	EnteredGadget		=  0 ;
	TimerId 		=  0 ;
	TimeoutValue		=  0 ;
	CriticalSection		=  FALSE ;
	MenuDropped		=  FALSE ;
	GotKillFocus		=  FALSE ;


// Données à recopier
	ClonedWindow		=  & w ;
	ShrinkWrapWidth		=  w. ShrinkWrapWidth ;
	ShrinkWrapHeight	=  w. ShrinkWrapHeight ;
	SelectionStyle		=  w. SelectionStyle ;
	MenuStyle		=  w. MenuStyle ;
	ButtonDownGadget	=  w. ButtonDownGadget ;
	DesiredBottom		=  w. DesiredBottom ;
	Receiver		=  w. Receiver ;
	DoubleClickTime		=  w. DoubleClickTime ;
	ParentMenu		=  w. ParentMenu ;
	ParentMenuId		=  w. ParentMenuId ;
	CurrentDirection	=  w. CurrentDirection ;
	SetTitle ( w. Title ) ;
    }







/**************************************************************************/
/*                                                                        */
/*  TGadgetMenuWindow :: Init                                             */
/*      Initialise les variables indépendantes du type de menu.           */
/*                                                                        */
/**************************************************************************/

void	TGadgetMenuWindow :: Init  ( char *  title )
   {
	ClonedWindow	= 0 ;
	CurrentGadget 	= 0 ;
	Title	      	= 0 ;
	ButtonPressed 	= FALSE ;
	DroppedPosition = TPoint ( 0, 0 ) ;

	SetTitle  ( title ) ;

	IgnoreNextButtonUp = FALSE ;
	IgnoreNextTimer	   = FALSE ;
	GotSelection	   = FALSE ;
	ButtonDownGadget   = 0 ;
	DiedFromEscape	   = FALSE ;
	DoubleClickTime	   = GetProfileInt ( "Windows", "DoubleClickSpeed", 550 ) ;
	CriticalSection    = FALSE ;
	MenuDropped	   = FALSE ;
	GotKillFocus	   = FALSE ;
     }                                    




/**************************************************************************/
/*                                                                        */
/*  Destructeur.                                                          */
/*                                                                        */
/**************************************************************************/

TGadgetMenuWindow :: ~TGadgetMenuWindow ( )
  {
	if  ( Title )
		delete []  Title ; 
   }



BOOL	TGadgetMenuWindow :: CanClose ( void )
   {
// Prévenir le receveur
	if  ( Receiver )
	   {
		CriticalSection = TRUE ;
		Receiver -> HandleMessage ( WM_MENUCLOSED, 0,
			( LPARAM ) this ) ;
		CriticalSection = FALSE ;
	     }

	return ( TRUE ) ;
     }





/**************************************************************************/
/*                                                                        */
/*  TGadgetMenuWindow :: SetStyle, GetStyle -                             */
/*      Changement / récupération du style du menu.                       */
/*                                                                        */
/**************************************************************************/

void	TGadgetMenuWindow :: SetStyle  ( TGadgetStyle  style )
   {
	MenuStyle = style ;
    }



TGadgetStyle	TGadgetMenuWindow :: GetStyle  ( void )
   {
	return  ( MenuStyle ) ;
    }



/**************************************************************************/
/*                                                                        */
/*  TGadgetMenuWindow :: GetSelectionStyle, SetSelectionStyle -           */
/*      Récupère/Change le style de la sélection.                         */
/*                                                                        */
/**************************************************************************/

TGadgetStyle  TGadgetMenuWindow :: GetSelectionStyle  ( void )
   {
	return  ( SelectionStyle ) ;
    }



void  TGadgetMenuWindow :: SetSelectionStyle  ( TGadgetStyle  style )
   {
	if  ( style  !=  SelectionStyle )
	   {
		register TGadget *	gadget  =  FirstGadget ( ) ;


		SelectionStyle = style ;


		while  ( gadget )
		   {
			register TGadgetMenuItem *  item = TYPESAFE_DOWNCAST ( gadget, TGadgetMenuItem ) ;


			if  ( item  &&  item -> GetSelectionStyle ( )  !=  style )
			   {
				item -> SetSelectionStyle ( style ) ;

				if  ( item -> GetSelection ( ) )
					InvalidateRect ( item -> GetBounds ( ) ) ;
			    }


			gadget = gadget -> NextGadget ( ) ;
		     }
	      }
      }




/**************************************************************************/
/*                                                                        */
/*  TGadgetMenuWindow :: Insert -                                         */
/*      Insère le gadget et redéfinit son style de sélection.             */
/*                                                                        */
/**************************************************************************/

void	TGadgetMenuWindow :: Insert ( TGadget &  g, 
					TGadgetWindow :: TPlacement p, TGadget *  sibling )
   {
	TGadgetWindow :: Insert ( g, p, sibling ) ;


	register TGadgetMenuItem * 	item = TYPESAFE_DOWNCAST ( & g, TGadgetMenuItem ) ;


	if  ( item )
		item -> SetSelectionStyle ( SelectionStyle ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TGadgetMenuWindow :: GetTitle, SeTitle -                              */
/*      Récupération/changement du titre du menu.                         */
/*                                                                        */
/**************************************************************************/

void	TGadgetMenuWindow :: SetTitle  ( char *  title )
   {
	if  ( Title )
		delete  Title ;

	if  ( title )
	   {
		Title = new  char [ strlen ( title ) + 1 ] ;
		strcpy ( Title, title ) ;
	    }
	else
		Title = 0 ;
     }



char *	TGadgetMenuWindow :: GetTitle ( void )
   {
	return  ( Title ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  TGadgetMenuWindow :: GetItem -                                        */
/*      Récupère le gadget dont le numéro est spécifié.                   */
/*      Si ce numéro est négatif, retourne le gadget courant.             */
/*                                                                        */
/**************************************************************************/

TGadgetMenuItem *   TGadgetMenuWindow :: GetItem  ( int  index )
   {
	return  ( ( index < 0 ) ? 
			GetCurrentGadget ( ) : GetGadget ( index ) ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  TGadgetMenuWindow :: GetItem -                                        */
/*      Retourne le gadget dont l'emplacement est spécifié.               */
/*                                                                        */
/**************************************************************************/

TGadgetMenuItem *  TGadgetMenuWindow :: GetItem ( TPlacement 		placement,
						  TGadgetMenuItem *	relative_to )
   {
	switch  ( placement )
	   {
	// Premier gadget
		case	First :
			return  ( GetFirstGadget ( ) ) ;

	// Dernier gadget
		case	Last :
			return  ( GetLastGadget ( ) ) ;

	// Gadget suivant - retourner le suivant du courant si 'relative_to'
	// est nul
		case	Next :
			return  ( GetNextGadget ( ( relative_to ) ?
					relative_to : GetCurrentGadget ( ) ) ) ;

	// Gadget précédent - même topo
		case 	Previous :
			return  ( GetPreviousGadget ( ( relative_to ) ?
					relative_to : GetCurrentGadget ( ) ) ) ;
			 
	// Gadget courant
		case	Current :
		default :
			return  ( GetCurrentGadget ( ) ) ;
	   }
      }




/**************************************************************************/
/*                                                                        */
/*  TGadgetMenuWindow :: GetItemCount -                                   */
/*      Retourne le nombre de gadgets éléments de menu.                   */
/*                                                                        */
/**************************************************************************/

int	TGadgetMenuWindow :: GetItemCount  ( void ) 
   {
	register TGadget *		p 	=  FirstGadget ( ) ;
	register TGadgetMenuItem *	item ;
	register int			count 	=  0 ;



	while  ( p )
	   {
		item = GetValidItem ( p ) ;

		if  ( item )
			count ++ ;

		p = p -> NextGadget ( ) ;
	     }


	return  ( count ) ;
     }





/**************************************************************************/
/*                                                                        */
/*  Fonctions de récupération d'items.                                    */
/*                                                                        */
/**************************************************************************/


// Récupérer le gadget courant (celui sélectionné)
//
TGadgetMenuItem *	TGadgetMenuWindow :: GetCurrentGadget  ( void )
   {
	if  ( CurrentGadget )
		return  ( CurrentGadget ) ;


	register TGadget *		p	=  FirstGadget ( ) ;
	register TGadgetMenuItem *	item ;


	while  ( p )
	   {
		item = GetValidItem ( p ) ;

		if  ( item  &&  item -> GetSelection ( ) )
		   {
			CurrentGadget = item ;
			return  ( item ) ;
		    }

		p = p -> NextGadget ( ) ;
	    }

	return  ( 0 ) ;
     }




// Récupérer le premier gadget de type menu item
//
TGadgetMenuItem *  TGadgetMenuWindow :: GetFirstGadget ( void )
   {
	register TGadget *		p 	=  FirstGadget ( ) ;
	register TGadgetMenuItem *	item ;


	while  ( p )
	   {
		item = GetValidItem ( p ) ;

		if  ( item ) 
			return  ( item ) ;

		p = p -> NextGadget ( ) ;
	    }

	return  ( 0 ) ;
     }




// Récupérer le dernier gadget de type menu item
//
TGadgetMenuItem *  TGadgetMenuWindow :: GetLastGadget ( void )
   {
	register TGadget *		p 	=  FirstGadget ( ) ;
	register TGadgetMenuItem *	item,
				 *	prev    =  0 ;


	while  ( p )
	   {
		item = GetValidItem ( p ) ;

		if  ( item ) 
			prev = item ;			

		p = p -> NextGadget ( ) ;
	    }

	return  ( prev ) ;
     }





// Récupérer le gadget suivant celui spécifié
//
TGadgetMenuItem *  TGadgetMenuWindow :: GetNextGadget ( TGadgetMenuItem *  ref )
   {
	register TGadget *		p  =  ( ref ) ?  ref -> NextGadget ( ) : 0 ;
	register TGadgetMenuItem *	item ;



	while  ( p )
	   {
		item = GetValidItem ( p ) ;

		if  ( item )
			return  ( item ) ;

		p = p -> NextGadget ( ) ;
	    }

	return ( 0 ) ;
    }



// Récupérer le gadget précedent celui spécifié
//
TGadgetMenuItem *  TGadgetMenuWindow :: GetPreviousGadget ( TGadgetMenuItem *  ref )
   {
	register TGadget *		p     =  FirstGadget ( ) ;
	register TGadgetMenuItem *	item,
				 *	prev  =  0 ;



	if  ( p  ==  ref )
		return ( 0 ) ;	// Pas de précédent 


	while  ( p  )
	   {
		item = GetValidItem ( p ) ;
		
		if  ( item  &&  item  !=  ref )
			prev = item ;

		p = p -> NextGadget ( ) ;

		if  ( p  ==  ref )
			break ;
	    }


	return  ( prev ) ;
    }




// Retourne le gadget dont l'index est spécifié
//
TGadgetMenuItem *  TGadgetMenuWindow :: GetGadget  ( int  index )
   {
	register TGadget *		p 	= FirstGadget ( ) ;
	register TGadgetMenuItem *	item,
				 *	prev 	= 0 ;



	while  ( p  &&  index >=  0 )
	   {
		item = GetValidItem ( p ) ;

		if  ( item )
		   {
			index -- ;
			prev = item ;
		    }

		p = p -> NextGadget ( ) ;
	    }

	return ( prev ) ;
    }






/**************************************************************************/
/*                                                                        */
/*  TGadgetMenuWindow :: Create -                                         */
/*      Définit le gadget courant comme étant le premier puis crée la     */
/*      fenêtre.                                                          */
/*                                                                        */
/**************************************************************************/

BOOL	TGadgetMenuWindow :: Create ( void )
   {
	if  ( HWindow )
		return ( FALSE ) ;


	register BOOL	Status ;


	CurrentGadget = GetFirstGadget ( ) ;

	Status = TGadgetWindow :: Create ( ) ;

	if  ( CurrentGadget )
		CurrentGadget -> SetSelection ( TRUE ) ;

	return ( Status ) ;
     }





/**************************************************************************/
/*                                                                        */
/*  TGadgetMenuWindow :: SelectItem -                                     */
/*      Sélectionne un élément de menu.                                   */
/*                                                                        */
/*                                                                        */
/**************************************************************************/
		
void	TGadgetMenuWindow :: SelectItem  ( TPlacement 		placement,
					   TGadgetMenuItem *    relto )
   {
	SelectItem ( GetItem ( placement, relto ) ) ;
    }



void	TGadgetMenuWindow :: SelectItem ( int	index )
   {
	SelectItem ( GetItem ( index ) ) ;
    }



void	TGadgetMenuWindow :: SelectItem  ( TGadgetMenuItem *	gadget )
   {
	if  ( gadget  &&  gadget  !=  CurrentGadget  &&
		! ( gadget -> GetState ( )  &  ( TGadgetMenuItem :: Disabled |
						 TGadgetMenuItem :: Grayed ) ) )
	   {
		:: LockWindowUpdate ( HWindow ) ;

	// Enlever la sélection du précédent élément
		if  ( CurrentGadget )
		   {	
			CurrentGadget -> SetSelection ( FALSE ) ;
			CurrentGadget = 0 ;
		    }

	// Et la mettre sur le nouveau
		if  ( GetValidItemPointer ( gadget ) )
		   {
			gadget -> SetSelection ( TRUE ) ;
			CurrentGadget = gadget ;
		    }

		:: LockWindowUpdate ( NULL ) ;
	    }
    }






/**************************************************************************/
/*                                                                        */
/*  TGadgetMenuWindow :: Paint -                                          */
/*      Redessine le menu, avec des effets de relief.                     */
/*                                                                        */
/**************************************************************************/

void	TGadgetMenuWindow :: Paint ( TDC &  dc, BOOL  erase, TRect &  rect )
   {
// Déterminer la couleur des bordures ombrées
	COLORREF	C1	= 0, 
			C2 	= 0 ;

	switch  ( MenuStyle )
	   {
		case	NormalGadget :
			C1 	=  GetSysColor ( COLOR_BTNFACE ) ;
			C2	=  C1 ;
			break ;

		case	RaisedGadget :
			C1 	=  GetSysColor ( COLOR_BTNHIGHLIGHT ) ;
			C2 	=  GetSysColor ( COLOR_BTNSHADOW ) ;
			break ;

		case	RecessedGadget :
			C1 	=  GetSysColor ( COLOR_BTNSHADOW ) ;
			C2	=  GetSysColor ( COLOR_BTNHIGHLIGHT ) ;
			break ;
	     }	



// Créer les pinceaux
	TBrush		ShadowBrush ( C2 ),
			HiliteBrush ( C1 ),
			BorderBrush ( GetSysColor ( COLOR_WINDOWFRAME ) ),
			InnerBrush  ( GetSysColor ( COLOR_BTNFACE ) ) ;
	TRect		Rect		=  GetClientRect ( ) ;
	register int	cxBorder        =  GetSystemMetrics ( SM_CXBORDER ),
			cyBorder	=  GetSystemMetrics ( SM_CYBORDER ) ;


// Tracer le contour
	dc. FrameRect ( Rect, BorderBrush ) ;
	Rect. top 	+= cyBorder ;
	Rect. left	+= cxBorder ;
	Rect. bottom	-= cyBorder ;
	Rect. right	-= cxBorder ;


// Tracer la bordure éclairée
	dc. SelectObject ( HiliteBrush ) ;

	dc. PatBlt ( Rect. left, Rect. top, 
			Rect. Width ( ), cyBorder,
				PATCOPY ) ;
	dc. PatBlt ( Rect. left, Rect. top + cyBorder,
			Rect. Width ( ) - cxBorder, cyBorder,	
				PATCOPY ) ;

	dc. PatBlt ( Rect. left, Rect. top,
			cxBorder, Rect. Height ( ),
				PATCOPY ) ;
	dc. PatBlt ( Rect. left + cxBorder, Rect. top,
			cxBorder, Rect. Height ( ) - cyBorder,
				PATCOPY ) ;

	dc. RestoreBrush ( ) ;



// Puis la bordure sombre
	dc. SelectObject ( ShadowBrush ) ;
	
	dc. PatBlt ( Rect. right - cxBorder, Rect. top,
			cxBorder, Rect. Height ( ),
				PATCOPY ) ;
	dc. PatBlt ( Rect. right - ( 2 * cxBorder ), Rect. top + cyBorder,
			cxBorder, Rect. Height ( ) - ( 2 * cyBorder ),
				PATCOPY ) ;

	dc. PatBlt ( Rect. left + cxBorder, Rect. bottom - cyBorder,
			Rect. Width ( ) - ( 2 * cxBorder ), cyBorder,
				PATCOPY ) ;
	dc. PatBlt ( Rect. left + ( 2 * cxBorder ), Rect. bottom - ( 2 * cyBorder ),
			Rect. Width ( ) - ( 4 * cxBorder ), cyBorder,
				PATCOPY ) ;

	dc. RestoreBrush ( ) ;


// Dessiner les gadgets
	TGadgetWindow :: Paint ( dc, erase, rect ) ;
    }






/**************************************************************************/
/*                                                                        */
/*  TGadgetMenuWindow :: EvDrop -                                         */
/*      Crée une fenêtre popup avec ce menu.                              */
/*                                                                        */
/**************************************************************************/

LRESULT  TGadgetMenuWindow :: WmMenuDropped  ( WPARAM, LPARAM  lp )
   {
	EvDrop  ( * ( ( TPoint * ) lp ) ) ;
	return  ( 0L ) ;
    }



void	TGadgetMenuWindow :: EvDrop ( TPoint &  pos )
   {
	DroppedPosition = pos ;
	MenuDropped = TRUE ;
    }





/**************************************************************************/
/*                                                                        */
/*  TGadgetMenuWindow :: EvKeyDown -                                      */
/*      Gère les touches de déplacement.                                  */
/*                                                                        */
/**************************************************************************/

void	TGadgetMenuWindow :: EvKeyDown  ( UINT  key, UINT  repcnt, UINT  flags )
   {
	register TGadgetMenuItem *		gadget ;



	GotSelection = TRUE ;	// Si une touche provoque la fermeture de la
				// fenêtre, sans sélectionner de menu, ne pas
				// ignorer le prochain button up

	switch  ( key )
	   {

	// Escape : Détruire la fenêtre
		case	VK_ESCAPE :
		// Type de menu clône : ne rien faire
			if  ( MenuType  ==  Clone )
				break ;

		// DiesdFromEscape == TRUE : le dernier sous-menu a été fermé
		// avec ESC - ESC : il faut TOUT fermer
			if  ( DiedFromEscape )	
			   {
				if  (  ParentMenu )
				   {
					ParentMenu -> DiedFromEscapeTime = GetTickCount ( ) ;
					ParentMenu -> DiedFromEscape = TRUE ;        

					if  ( GetTickCount ( ) - DiedFromEscapeTime  <  DoubleClickTime )
						ParentMenu -> PostMessage ( WM_KEYDOWN, VK_ESCAPE ) ;
					else
						ParentMenu -> Invalidate ( ) ;
				    }
			    }
			else if  ( ParentMenu )	
			   {
				ParentMenu -> DiedFromEscapeTime = GetTickCount ( ) ;
				ParentMenu -> DiedFromEscape = TRUE ;
				ParentMenu -> Invalidate ( ) ;
			    }
				
			ShutDownWindow ( 0 ) ;
			break ;


	// Home : Sélectionner le premier élément
		case	VK_HOME :
			SelectItem ( GetFirstGadget ( ) ) ;
			break ;


	// End : sélectionner le dernier élément
		case	VK_END :
			SelectItem ( GetLastGadget ( ) ) ;
			break ;


	// Flèche vers le bas : Sélectionner l'élément suivant
		case	VK_DOWN :
			gadget = GetNextGadget ( CurrentGadget ) ;

			if  ( ! gadget )       		// Wrap around
				gadget = GetFirstGadget ( ) ;
			SelectItem ( gadget ) ;
			break ;


	// Flèche vers le haut : sélectionner le précédent
		case	VK_UP :
			gadget = GetPreviousGadget ( CurrentGadget ) ;

			if  ( gadget  ==  NULL )
				gadget = GetLastGadget ( ) ;
			SelectItem ( gadget ) ;
			break ;


	// Entrée : lancer l'option
		case	VK_RETURN :
			MenuSelect ( CurrentGadget ) ;
			break ;


	// Flèche droite : faire un EvSelect s'il s'agit d'un popup
		case	VK_RIGHT :
			if  ( MenuType  ==  Clone )
				break ;
			if  ( CurrentGadget  && 
				CurrentGadget -> GetType ( )  ==  TGadgetMenuItem :: Popup )
				MenuSelect ( CurrentGadget ) ;
			break ;


	// Flèche gauche : fermer ce menu s'il a un parent
		case	VK_LEFT :
			if  ( MenuType  ==  Clone )
				break ;
			if  ( ParentMenu )
			   {
				ParentMenu -> Invalidate ( ) ;
				ShutDownWindow ( 0 ) ;
			    }
			break ;


	// Autre : laisser faire gadget window
		default :
			TGadgetWindow :: EvKeyDown  ( key, repcnt, flags ) ;
	    }
     }

			



/**************************************************************************/
/*                                                                        */
/*  TGadgetMenuWindow :: EvChar -                                         */
/*      Gère les raccourcis pour les lettres soulignées (positionne la    */
/*      sélection sur l'élément ayant cette lettre).                      */
/*                                                                        */
/**************************************************************************/

void	TGadgetMenuWindow :: EvChar  ( uint  c, uint  repcnt, uint  flags )
   {
	if  ( CurrentGadget )
	   {
		if  ( c  ==  ' ' )	// Si checkmark, annuler
		   {
			TGadgetMenuItem :: TState  State =  CurrentGadget -> GetState ( ) ;


			if  ( State  &  TGadgetMenuItem :: Checked )
			   {
				State &= ~ TGadgetMenuItem :: Checked ;
				State |=   TGadgetMenuItem :: Unchecked ;
			    }
			else
			   {
				State &= ~ TGadgetMenuItem :: Unchecked ;
				State |=   TGadgetMenuItem :: Checked ;
			    }

			CurrentGadget -> SetState ( State ) ;
			return ;
		     }
			    

		register  TGadgetMenuItem *	item 	=  CurrentGadget ;

		do  
		   {
			register int  	letter = item -> GetShortcut ( ) ;


			if  ( toupper ( letter )  ==  toupper ( c ) )
			   {
				SelectItem ( item ) ;
				return ;
			    }

			item = GetValidItem ( item -> NextGadget ( ) ) ;
		     }
		while  ( item  &&  item  !=  CurrentGadget ) ;
	     }

	TGadgetWindow :: EvChar ( c, repcnt, flags ) ;
    }


		


/**************************************************************************/
/*                                                                        */
/*  TGadgetMenuWindow :: GetDesiredSize -                                 */
/*      Donne la taille demandée par les gadgets, plus ce qu'il faut      */
/*      pour peindre les effets de relief.                                */
/*      Calcule aussi l'origine de la fenêtre et la déplace si nécessaire.*/
/*      Remarque très importante :                                        */
/*      Le GetDesiredSize de TGadgetWindow ne sert pas à calculer la      */
/*      taille totale de la fenêtre, mais seulement sa taille initiale.   */
/*      Ce n'est qu'après l'avoir insérée dans une TDecoratedFrame que    */
/*      cette dernière, au moyen de TChildLayoutMetrics, ajuste la        */
/*      taille de la fenêtre gadget. EN clair, si on n'effectue pas       */
/*      d'opération d'insertion, la taille de la gadget window n'est      */
/*      jamais modifiée (sauf pour prendre sa taille initiale qui est     */
/*      celle du plus haut et du plus large gadget).                      */
/*      Pour notre TGadgetMenuWindow, il n'est pas question d'insertion.  */
/*      Sa fonction membre GetDesiredSize doit donc calculer la taille    */
/*      totale de la fenêtre (c'est à dire faire la somme des tailles     */
/*      de tous les gadgets).                                             */
/*      En fait, ce n'est pas cette possibilité d'insertion qui nous      */
/*      intéresse pour cette classe, mais la manière dont elle gère les   */
/*      gadgets.                                                          */
/*                                                                        */
/**************************************************************************/

void	TGadgetMenuWindow :: GetDesiredSize  ( TSize &  Size )
   {
// La taille de la fenêtre, c'est celle de tous les gadgets, plus la place
// occupée par les effets de relief et la bordure.
	Size. cx = 6 * GetSystemMetrics ( SM_CXBORDER ) ;
	Size. cy = 6 * GetSystemMetrics ( SM_CYBORDER ) ;

	
// Déterminer la taille de tous les gadgets
	register int		MaxWidth = 0,
				Height 	 = 0 ;
	TSize			GadgetSize ;
	register TGadget *	g 	 = FirstGadget ( ) ;


	while  ( g )
	   {
		g -> GetDesiredSize ( GadgetSize ) ;

		Height += GadgetSize. cy ;

		if  ( GadgetSize. cx  >  MaxWidth )
			MaxWidth = GadgetSize. cx ;

		g = g -> NextGadget ( ) ;
	    }


// Ajouter la taille trouvée
	Size. cx += MaxWidth ;
	Size. cy += Height ;

// Calculer la position de la fenêtre
	if  ( HWindow )
		ComputeChildPosition ( Size ) ;
    }

			

/**************************************************************************/
/*                                                                        */
/*  TGadgetMenuWindow :: GetInnerRect -                                   */
/*      Donne le rectangle des gadgets en tenant compte de la place prise */
/*      par les effets de relief.                                         */
/*                                                                        */
/**************************************************************************/

void	TGadgetMenuWindow :: GetInnerRect  ( TRect &  rect )
   {
	register int	cxBorder	=  GetSystemMetrics ( SM_CXBORDER ) * 3,
			cyBorder	=  GetSystemMetrics ( SM_CYBORDER ) * 3 ;
	TSize 		Size ;


	GetDesiredSize ( Size ) ;
	rect. left   = cxBorder ;
	rect. top    = cyBorder ;
	rect. right  = Size. cx - cxBorder ;
	rect. bottom = Size. cy - cyBorder ;
    }




/**************************************************************************/
/*                                                                        */
/*   TGadgetMenuWindow :: GetValidItem -                                  */
/*      Transtype un Tgadget en TGadgetMenuItem.                          */
/*                                                                        */
/**************************************************************************/

TGadgetMenuItem *  TGadgetMenuWindow :: GetValidItem  ( TGadget *  p )
   {
	register TGadget *		gadget  = p ;
	register TGadgetMenuItem *	item ;


	while  ( gadget )
	   {
		item = GetValidItemPointer ( gadget ) ;

		if  ( item ) 
			return  ( item ) ;

		gadget = gadget -> NextGadget ( ) ;

		if  ( gadget  ==  p )
			return  ( 0 ) ;
	     }

	return  ( 0 ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  TGadgetMenuWindow :: GetValidItemPointer -                            */
/*      Retourne un pointeur sur un TGadgetMenuItem valide (égal au       */
/*      gadget passé en paramètre puis transtype), ou 0.                  */
/*      Un gadget est valide si :                                         */
/*      . C'est un TGadgetMenuItem                                        */
/*      . Il est enable                                                   */
/*      . Ce n'est pas un séparateur.                                     */
/*                                                                        */
/**************************************************************************/

TGadgetMenuItem *	TGadgetMenuWindow :: GetValidItemPointer ( TGadget *  g )
   {
	TGadgetMenuItem *	item	=  TYPESAFE_DOWNCAST ( g, TGadgetMenuItem ) ;



// Retourner 0 si ce n'est pas un objet du bon type
	if  ( ! item )
		return  ( 0 ) ;


// De même, s'il s'agit d'un séparateur
	if  ( item -> GetType ( )  ==  TGadgetMenuItem :: Separator )
		return ( 0 ) ;

// S'il est disabled ou grayed, même topo
	if  ( item -> GetState ( )  &  ( TGadgetMenuItem :: Disabled |
					 TGadgetMenuItem :: Grayed ) )
		return  ( 0 ) ;

	return  ( item ) ;
    }
		



/**************************************************************************/
/*                                                                        */
/*  TgadgetMenuWindow :: TileGadgets                                      */
/*      Remplace le TileGadgets de TGadgetWindow. En effet, pour les      */
/*      éléments comme les séparateurs, nous avons besoin d'adapter le    */
/*      gadget en largeur dans une fenêtre verticale. Or, OWL ne le       */
/*      permet pas (OWL adapte les gadgets en hauteur si la fenêtre est   */ 
/*      verticale, et en largeur si elle est horizontale).                */
/*      Le code qui suit est inspiré copieusement d'OWL.                  */
/*                                                                        */
/**************************************************************************/

TRect	TGadgetMenuWindow :: TileGadgets ( void )
   {
	TRect  		innerRect ;
	TRect  		invalidRect ;
	int    		y ;



	GetInnerRect ( innerRect ) ;
	invalidRect. SetEmpty ( ) ;

  // Ajuster les gadgets
	y = innerRect. top ;

	for  ( TGadget *  gadget = Gadgets ; gadget ; gadget = gadget->NextGadget ( ) ) 
	   {
		TRect  		bounds = gadget -> GetBounds ( ) ;
		TRect  		originalBounds ( bounds ) ;
		TSize  		desiredSize ;



		gadget -> GetDesiredSize ( desiredSize ) ;

	// A la différence de TGadgetWindow, on donne la largeur max à tout le
	// monde
		bounds. top    = y ;
		bounds. bottom = bounds. top + desiredSize. cy ;
		bounds. left   = innerRect. left ;
		bounds. right  = innerRect. right ;

	// Si la taille du gadget a été changée, il faut le lui indiquer, et 
	// ajouter sa région aux régions à repeindre
		if  ( bounds  !=  originalBounds ) 
		   {
			gadget -> SetBounds ( bounds ) ;
			invalidRect |= bounds ;

			if  ( originalBounds. TopLeft ( )  !=  TPoint ( 0, 0 ) )
				invalidRect |= originalBounds ;
		    }

	// Comptabiliser la hauteur de ce gadget
		y += bounds. Height ( ) ;

    
		if  ( gadget -> NextGadget ( ) ) 
		   {
			TPoint  origin ( 0, y ) ;

			PositionGadget ( gadget, gadget -> NextGadget ( ), origin ) ;
			y = origin. y;
		     }                
	    }
  
	return ( invalidRect ) ;
    }






/**************************************************************************/
/*                                                                        */
/*  TGadgetWindow :: LayoutSession -                                      */
/*      Redessine les gadgets, ajoute ceux modifiés à la région d'update, */
/*      plus la bordure de la fenêtre (sans cela, le dessin de la         */
/*      bordure se fait aussi sur toute fenêtre en dessous, aux endroits  */
/*      où elle ne dessine pas.                                           */
/*                                                                        */
/**************************************************************************/

void	TGadgetMenuWindow :: LayoutSession  ( void )
   {
	if  ( ! HWindow )
		return ;
	TileGadgets ( ) ;
	Invalidate ( ) ;
     }




/**************************************************************************/
/*                                                                        */
/*  TGadgetMenuWindow :: EvMouseMove -                                    */
/*      Change la sélection de place si :                                 */
/*              . Le bouton gauche a été appuyé                           */
/*              . La position de la souris parvient à un autre gadget     */
/*                                                                        */
/**************************************************************************/

void	TGadgetMenuWindow :: EvMouseMove ( uint  mods, TPoint &  pos )
   {
	TGadgetWindow :: EvMouseMove  ( mods, pos ) ;


// TGadgetWindow, dans IdleAction, nous envoie gentiment un message, avec 
// TPoint ( -1, -1 ), pour nous dire que la souris a quitté la fenêtre.
// Pour éviter de merdoyer, il nous faut faire comme si le bouton n'avait
// jamais été appuyé.
	if  ( pos. x  ==  -1  &&  pos. y  ==  -1 )
	   {
		ButtonPressed = FALSE ;
		ButtonDownGadget = 0 ;
	    }
	
	
// Si aucun gadget n'a demandé la capture et que le bouton ghauche a été appuyé,
// il faut déplacer la sélection avec la souris.
	TGadget *		gadget	=  GadgetFromPoint ( pos ) ;
	TGadgetMenuItem *	item    =  0 ;


	if  ( gadget )
		item = TYPESAFE_DOWNCAST ( gadget, TGadgetMenuItem ) ;


	if  ( item )		// Ca ne concerne que les gadgetmenuitems
	   {
		if  ( ! Capture  &&  ButtonPressed )	// Déplacer la sélection
			SelectItem ( item ) ;
	     }


	if  ( EnteredGadget  !=  AtMouse )
		EnteredGadget = 0 ;
     }	   





/**************************************************************************/
/*                                                                        */
/*  TGadgetMenuWindow :: LButtonDown -                                    */
/*      Sélection de l'élément de menu correspondant.                     */
/*                                                                        */
/**************************************************************************/

void	TGadgetMenuWindow :: EvLButtonDown  ( uint  mods, TPoint &  pos )
   {
	TGadgetWindow :: EvLButtonDown  ( mods, pos ) ;
	ButtonPressed = TRUE ;

	TGadget *	gadget	=  GadgetFromPoint ( pos ) ;


	if  ( gadget )
	   {
		TGadgetMenuItem *	item = TYPESAFE_DOWNCAST ( gadget, TGadgetMenuItem ) ;


		if  ( item )
			SelectItem ( item ) ;
	    }

	ButtonDownGadget = CurrentGadget ;
     }	   




/**************************************************************************/
/*                                                                        */
/*  TGadgetMenuWindow :: LButtonUp -                                      */
/*      Sélection l'élément de menu correspondant.                        */
/*                                                                        */
/**************************************************************************/

void	TGadgetMenuWindow :: EvLButtonUp  ( uint  mods, TPoint &  pos )
   {
	TGadgetWindow :: EvLButtonUp  ( mods, pos ) ;
	ButtonPressed = FALSE ;
	GotSelection = FALSE ;

	if  ( IgnoreNextButtonUp )
	   {
		IgnoreNextButtonUp = FALSE ;	// Le focus vient d'un sous-menu
		IgnoreNextTimer    = TRUE ;	// Ne pas rouvrir de sous-menu trop vite
		MenuDropped	   = FALSE ;
	    }
	else
	   {
		TGadget *	gadget	=  GadgetFromPoint ( pos ) ;



		if  ( Receiver  &&  MenuDropped )
		   {
			MenuDropped = FALSE ;
			ButtonDownGadget = 0 ;
			CriticalSection = TRUE ;
			Receiver -> HandleMessage ( WM_MENUDROPPED, 0, ( LPARAM ) this ) ;
			CriticalSection = FALSE ;

			if ( Receiver  ||  GotKillFocus )
				EvKillFocus ( Receiver -> HWindow ) ;
		     }
		else if  ( gadget  &&  gadget  ==  ButtonDownGadget )
		   {
			TGadgetMenuItem *	item = TYPESAFE_DOWNCAST ( gadget, TGadgetMenuItem ) ;


			ButtonDownGadget = 0 ;

			if  ( item )
				MenuSelect ( item ) ;
		    }
	    }
     }	   






/**************************************************************************/
/*                                                                        */
/*  TGadgetMenuwindow :: MenuSelect -                                     */
/*      Envoie un message WM_MENUSELECTED à la fenêtre de destination,    */
/*      puis ferme le menu.                                               */
/*                                                                        */
/**************************************************************************/

void	TGadgetMenuWindow :: MenuSelect ( TGadgetMenuItem *  item )
   {
	TGadgetMenuItem :: TType	GadgetType ;


	if  ( item -> GetState ( )  &  ( TGadgetMenuItem :: Grayed |
					 TGadgetMenuItem :: Disabled ) )
		return ;

// Si le gadget est de type Checkmark, il ne faudra pas fermer la fenêtre
// (je trouve ça moins gonflant que les menus Windows qui se referment sans
// nous laisser vérifier qu'on a bien coché une option de menu)
	register BOOL			StateChanged	=  FALSE ;
	TGadgetMenuItem :: TState    	State ;


	if  ( item )
	   {
		State  =  item -> GetState ( ) ;

		if  ( State  &  TGadgetMenuItem :: Checked )
		   {
			State  &=  ~ TGadgetMenuItem :: Checked ;
			State  |=    TGadgetMenuItem :: Unchecked ;
			StateChanged = TRUE ;
		    }
		else if  ( State  &  TGadgetMenuItem :: Unchecked )
		   {
			State  &=  ~ TGadgetMenuItem :: Unchecked ;		
			State  |=    TGadgetMenuItem :: Checked ;
			StateChanged = TRUE ;
		    }


		if  ( StateChanged )
		   {
			item -> SetState ( State ) ;	// Ca suffit pour redessiner le gadget
			UpdateWindow ( ) ;
		     }
	     }


// Utiliser HandleMessage pour que le destinataire effectue son traitement
// avant de nous rendre la main (c'est important, car on doit faire un
// ShutDownWindow)
	GadgetType = item -> GetType ( ) ;


	if  ( Receiver  )
	   {
		GotSelection = ( item -> GetType ( )  ==  TGadgetMenuItem :: Item ) ;

	// Empêcher la fenêtre receiver d'effectuer des actions qui nous
	// feraient perdre le focus et nous détruiraient
		CriticalSection = TRUE ;
		Receiver -> HandleMessage ( WM_MENUSELECTED, 
			item -> GetId ( ), ( LPARAM ) this ) ;
		CriticalSection = FALSE ;
	     }                          

	if  ( ( GadgetType  !=  TGadgetMenuItem :: Popup  &&
			MenuType  !=  Clone  &&  Receiver ) )
		Receiver -> SetFocus ( ) ;
    }







/**************************************************************************/
/*                                                                        */
/*  TGadgetMenuWindow :: ShutDownWindow -                                 */
/*      Ferme la fenêtre.                                                 */
/*      Gère le cas du EvKillFocus qui lui-aussi ferme la fenêtre         */
/*      (empêche donc l'envoi de plusieurs Shutdown).                     */
/*                                                                        */
/**************************************************************************/

void	TGadgetMenuWindow :: ShutDownWindow  ( int  retval )
   {
// Lorsqu'on fait un shutdown, la fenêtre reçoit un killfocus...qui provoque
// le rappel de cette fonction. On fait donc en sorte de ne pas se faire 
// appeler deux fois
	if  ( ShutDownStarted  ||  CriticalSection )	
		return ;
	KillTimer ( ) ;
	ShutDownStarted = TRUE ;
        CanClose ( ) ;		// Envoyer le MENUCLOSED
	ShutDownWindow ( this, retval ) ;
     }




void	TGadgetMenuWindow :: ShutDownWindow  ( TWindow *  w, int  retval )
   {
	w -> Destroy ( retval ) ;
	delete  w ;	
    }







/**************************************************************************/
/*                                                                        */
/*  TGadgetMenuWindow :: EvSetFocus, EvKillFocus -                        */
/*      Gèrent le focus de manière à ce que :                             */
/*      . La fenêtre ne soit pas détruite deux fois (un CloseWindow       */
/*        entraînant un Kill focus avant que la fenêtre ne soit détruite) */
/*      . La fenêtre ne soit pas détruite lorsque le focus est donné à    */
/*        un sous-menu.                                                   */
/*                                                                        */
/**************************************************************************/

void	TGadgetMenuWindow :: EvKillFocus  	( HWND  gained )
   {
	register BOOL			ischild  =  FALSE ;
	register TGadgetMenuWindow *	menu     =  ChildMenu ;


	TGadgetWindow :: EvKillFocus ( gained ) ;
	GotKillFocus = TRUE ;

// Clône : ne rien faire
	if  ( MenuType  ==  Clone )
		return ;

// Shutdown déjà commencé : idem
	if  ( ShutDownStarted )
		return ;

// La fenêtre principale a provoqué une action qui nous fait perdre le
// focus (on est toujours dans menuselect) : ne rien faire pour que la
// fonction menuselect fasse elle-même le shutdown
	if  ( CriticalSection )
		return ;


// Supprimer le timer
	KillTimer ( ) ;
	EnteredGadget = 0 ;

// Parcourir la liste des enfants pour déterminer si le focus a été donné à
// un sous-menu (auquel cas, ce menu-là ne doit pas être fermé)
	if  ( gained  !=  NULL )
	   {
		while  ( menu )
		   {
			if  ( gained  ==  menu -> HWindow )
			   {
				ischild = TRUE ;
				GotKillFocus = FALSE ;
				Invalidate ( ) ;
				break ;
			    }

			menu = menu -> ChildMenu ;
		     }
	     }

	if  ( ! ischild )	
	   {

// Peut-être que le focus a été donné à une fenêtre parent : dans ce cas, il
// faut supprimer toutes les fenêtres jusqu'au parent non compris, et les détruire
		TGadgetMenuWindow *	parent ;

		menu = ParentMenu ;
		Show ( SW_HIDE ) ;	// Faire comme si ce menu se fermait en premier

		while  ( menu  &&  menu -> HWindow  !=  gained )
		   {
			parent = menu -> ParentMenu ;
			menu -> CloseWindow ( 0 ) ;
			delete  menu ;
			menu = parent ;
		     }
			

		if  ( menu )	// On a redonné le focus à un menu parent
		   {
		// Supprimer d'abord toute référence à un sous-menu de ce menu
			menu -> ChildMenu = 0 ;
			menu -> Invalidate ( ) ;
			menu -> UpdateWindow ( ) ;
			menu -> ChildDied ( this ) ;
			
		// Puis, comme il n'a pu être activé que par la souris, lui dire
		// d'ignorer le prochain LButtonUp, afin d'éviter de déclencher
		// une commande inutile (mais ne le faire que si le KillFocus
		// n'a pas été déclenché par une sélection d'un élément de ce
		// menu)
			if  ( ! GotSelection ) 
			   {
				menu -> IgnoreNextButtonUp = TRUE ;
				menu -> EvSetFocus ( HWindow ) ;
			    }
		     }

		ShutDownWindow ( ) ;
	     }
    }



void	TGadgetMenuWindow :: EvSetFocus  ( HWND  lost )
    {
	TGadgetWindow :: EvSetFocus ( lost ) ;
	EnteredGadget = 0 ;

	if  ( MenuType  ==  Clone )
		return ;

	ShutDownStarted = FALSE ;
	SetTimer ( ) ;
     }
				



/**************************************************************************/
/*                                                                        */
/*  TGadgetMenuWindow ::SetTimer, KillTimer, EvTimer -                    */
/*      Gestion du timer. Permettent l'ouverture automatique des menus    */
/*      popup lorsque la souris reste positionnée dessus un certain       */
/*      temps.                                                            */
/*                                                                        */
/**************************************************************************/

void	TGadgetMenuWindow :: KillTimer  ( void )
   {
	if  ( TimerId )
	   {
		TGadgetWindow :: KillTimer ( TimerId ) ;	
		TimerId = 0 ;
		EnteredGadget = 0 ;
	    }
    }





void	TGadgetMenuWindow :: SetTimer  ( void )
   {
	KillTimer ( ) ;

	if  ( TimeoutValue )
		TimerId = TGadgetWindow :: SetTimer ( TIMER_ID, TimeoutValue ) ;
     }




void	TGadgetMenuWindow :: EvTimer  ( uint  /* timerid */ )
   {
// Ne rien faire si nous n'avons pas le focus
	if  ( :: GetFocus ( )  !=  HWindow )
		return ;

// On vient de fermer un sous-menu : ne pas le rouvrir trop vite
	if  ( IgnoreNextTimer )
	   {
		IgnoreNextTimer = FALSE ;
		return ;
	    }

// Ne sélectionner l'élément que si la souris est restée positionnée dessus 
// depuis le dernier évènement WM_TIMER, et que c'est un menu popup
	if  ( EnteredGadget  &&  EnteredGadget  ==  AtMouse )
	   {
		TGadgetMenuItem *  item = TYPESAFE_DOWNCAST ( EnteredGadget, TGadgetMenuItem ) ;


		EnteredGadget = 0 ;

		if  ( item  &&  item -> GetType ( )  ==  TGadgetMenuItem :: Popup ) 
		   {
			SetTimer ( ) ;
			SelectItem ( item ) ;	// Déplacer la sélection
			MenuSelect ( item ) ;	// et lancer l'ouverture du popup
			EnteredGadget = 0 ;
		     }
	     }
	else
	   {
		TPoint		Pos ;


		GetCursorPos ( Pos ) ;
		ScreenToClient ( Pos ) ;
		EnteredGadget = GadgetFromPoint ( Pos ) ;
	     }

// Supprimer le timer si SetPopupTimeout a été appelé entre-temps, avec une
// valeur nulle
	if  ( ! TimeoutValue )
		KillTimer ( ) ;
     }






/**************************************************************************/
/*                                                                        */
/*  TGadgetMenuWindow :: ComputeChildPosition -                           */
/*      Calcule la position de cette fenêtre relativement au menu parent. */
/*                                                                        */
/**************************************************************************/

void	TGadgetMenuWindow :: ComputeChildPosition  ( TSize &  Size )
   {
// Clône : rien à faire
	if   ( MenuType  ==  Clone )
		return ;


// Menu principal : calculer la position par rapport au coin inférieur gauche
	if   ( MenuType  ==  MainMenu )
	   {

	// Changer la position de la fenêtre relativement au point où l'appelant veut
	// que le bas de cette fenêtre se trouve.
		register int	X = DesiredBottom. x,
				Y = DesiredBottom. y - Size. cy ;

		if  ( HWindow  &&  Attr. X  !=  X  &&  Attr. Y  !=  Y )
			SetWindowPos ( NULL, X, Y, 0, 0,
				SWP_NOZORDER | SWP_NOSIZE ) ;

	// Sauvegarder cette nouvelle position
		Attr. X = X ;
		Attr. Y = Y ;

		return ;
	     }


// Sous-menu : positionner le menu par rapport au parent
	TRect			ScreenRect ( 0, 0,
					GetSystemMetrics ( SM_CXSCREEN ),
					GetSystemMetrics ( SM_CYSCREEN ) ) ;
	TRect			WindowRect	=  ParentMenu -> GetWindowRect ( ) ;
	TGadget *		Gadget		=  ParentMenu -> GadgetWithId ( ParentMenuId ) ;


	if  ( ! Gadget  ||  ! HWindow  ||
		! :: IsWindow ( ParentMenu -> HWindow ) )
	   {
		//:: MessageBox ( NULL, "Parent Gadget = 0 !!!", "ComputeChildPosition", MB_OK ) ;
		return ;
	    }


	TRect			GadgetRect		=  Gadget -> GetBounds ( ) ;
	register int		W 			=  WindowRect. Width ( ) / 3 ;
	TRect			ChildRect ;
	BOOL			FoundDir		=  FALSE ;
	TPopupDirection 	PreferredDirection      =  CurrentDirection ;


	for  ( int  i = 0 ; i < ( int ) TotalMess ; i ++ )
	   {

	// Essayer déjà de placer ce menu en haut à droite relativement au papa
		if  ( CurrentDirection  ==  TopRight )
		   {
			ChildRect. bottom	=  WindowRect. top + GadgetRect. top ;
			ChildRect. left		=  WindowRect. left + ( 2 * W ) ;
			ChildRect. right	=  ChildRect. left + Size.cx ;
			ChildRect. top 		=  ChildRect. bottom - Size. cy ;

			if  ( ScreenRect. Contains ( ChildRect ) )
			   {
				FoundDir = TRUE ;
				break ;
			     }
		     }


	// Puis en haut à gauche
		if  ( CurrentDirection  ==  TopLeft )
		   {
			ChildRect. right  = WindowRect. left + W ;
			ChildRect. left   = ChildRect. right - Size. cx ;
			ChildRect. bottom = WindowRect. top + GadgetRect. top ;
			ChildRect. top    = ChildRect. bottom - Size. cy ;

			if  ( ScreenRect. Contains ( ChildRect ) )
			   {
				FoundDir = TRUE ;
				break ;
			     }
		     }


	// Puis en bas à gauche
		if  ( CurrentDirection  ==  BottomLeft )
		   {
			ChildRect. top     = WindowRect. top + GadgetRect. bottom ;
			ChildRect. bottom  = ChildRect. top + Size.cy ;
			ChildRect. right   = WindowRect. left + W ;
			ChildRect. left    = ChildRect. right - Size. cx ;

			if  ( ScreenRect. Contains ( ChildRect ) )
			   {
				FoundDir = TRUE ;
				break ;
			     }
		    }



	// Si ça marche pas, essayer en bas à droite
		if  ( CurrentDirection  ==  BottomRight )
		   {
			ChildRect. top 		=  WindowRect. top + GadgetRect. bottom ;
			ChildRect. bottom	=  ChildRect. top + Size. cy ;
			ChildRect. left 	=  WindowRect. left + ( 2 * W ) ;
			ChildRect. right	=  ChildRect. left + Size. cx ;

			if  ( ScreenRect. Contains ( ChildRect ) )
			   {
				FoundDir = TRUE ;
				break ;
			     }
		     }

		CurrentDirection  =  ( TPopupDirection ) (
			( ( int ) CurrentDirection + 1 ) % ( ( int ) TotalMess ) ) ;
	    }



// Foutoir total : on va faire ce qu'on peut
	if  ( ! FoundDir )
	   {
		register int	xDelta, yDelta ;

	// Passe no 2 : même chose que la première, sauf qu'on essaie de 
	// centrer le menu par rapport à son parent
	// Essayer déjà de placer ce menu en haut à droite relativement au papa
		if  ( PreferredDirection  ==  TopRight )
		   {
			ChildRect. bottom	=  WindowRect. top + GadgetRect. top ;
			ChildRect. left		=  WindowRect. left + ( 2 * W ) ;
			ChildRect. right	=  ChildRect. left + Size.cx ;
			ChildRect. top 		=  ChildRect. bottom - Size. cy ;

			xDelta = ScreenRect. right - ChildRect. right ;
			yDelta = ChildRect. top ;
		     }
	// Puis en haut à gauche
		else if  ( PreferredDirection  ==  TopLeft )
		   {
			ChildRect. right  = WindowRect. left + W ;
			ChildRect. left   = ChildRect. right - Size. cx ;
			ChildRect. bottom = WindowRect. top + GadgetRect. top ;
			ChildRect. top    = ChildRect. bottom - Size. cy ;

			xDelta = ChildRect. left ;
			yDelta = ChildRect. top ;
		     }
	// Puis en bas à gauche
		else if  ( PreferredDirection  ==  BottomLeft )
		   {
			ChildRect. top     = WindowRect. top + GadgetRect. bottom ;
			ChildRect. bottom  = ChildRect. top + Size.cy ;
			ChildRect. right   = WindowRect. left + W ;
			ChildRect. left    = ChildRect. right - Size. cx ;

			xDelta = ChildRect. left ;
			yDelta = ScreenRect. bottom - ChildRect. bottom ;
		    }
	// Si ça marche pas, essayer en bas à droite
		else if  ( PreferredDirection  ==  BottomRight )
		   {
			ChildRect. top 		=  WindowRect. top + GadgetRect. bottom ;
			ChildRect. bottom	=  ChildRect. top + Size. cy ;
			ChildRect. left 	=  WindowRect. left + ( 2 * W ) ;
			ChildRect. right	=  ChildRect. left + Size. cx ;

			xDelta = ScreenRect. right - ChildRect. right ;
			yDelta = ScreenRect. bottom - ChildRect. bottom ;
		     }
		else	// Là, c'est trop !
		   {
			ChildRect. left   =  ( ScreenRect. Width  ( ) - Size. cx ) / 2 ;
			ChildRect. right  =  ChildRect. left + Size. cx ;
			ChildRect. top    =  ( ScreenRect. Height ( ) - Size. cy ) / 2 ;
			ChildRect. bottom =  ChildRect. top + Size. cy ;

			xDelta = yDelta = 0 ;
		    }

		CurrentDirection = PreferredDirection ;

		if  ( xDelta  <  0 )
		   {
			ChildRect. left  -= xDelta ;
			ChildRect. right -= xDelta ;
		    }

		if  ( yDelta  <  0 ) 
		   {
			ChildRect. top    -= yDelta ;
			ChildRect. bottom -= yDelta ;
		     }
	    }


//  Déplacer la fenêtre
	SetWindowPos ( NULL, ChildRect. left, ChildRect. top, 0, 0,
		SWP_NOZORDER | SWP_NOSIZE ) ;
	Attr. X = ChildRect. left ;
	Attr. Y = ChildRect. right ;
     }





		
/**************************************************************************/
/*                                                                        */
/*  TGadgetMenuWindow :: GetClassName, GetWindowClass -                   */
/*      Permettent de créer une nouvelle classe pour cette fenêtre,       */
/*      où le hCursor est à NULL.                                         */
/*      Il faut savoir que lorsqu'on appelle SetCursor, Windows change    */
/*      bien le curseur, mais restaure celui de la classe au premier      */
/*      mouse move. Le seul moyen de conserver un curseur est donc de ne  */
/*      pas avoir de curseur de classe.                                   */
/*                                                                        */
/**************************************************************************/

char *  TGadgetMenuWindow :: GetClassName  ( void )
   {
	return  ( "TGadgetMenuWindow" ) ;
    }



void	TGadgetMenuWindow :: GetWindowClass ( WNDCLASS &  wc )
   {
	TGadgetWindow :: GetWindowClass ( wc ) ;
	wc. hCursor = 0 ;
    }
		


	

	