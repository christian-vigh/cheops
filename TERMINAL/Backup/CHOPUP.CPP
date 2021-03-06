/**************************************************************************/
/*                                                                        */
/*  ChopUp.cpp  -  Christian Vigh, 95/10.                                 */
/*                                                                        */
/*  Impl�mentation de la classe  TCheopsGadgetMenuWindow.                 */
/*                                                                        */
/**************************************************************************/


# include	"AllTerm.h"
# pragma	hdrstop

# include	"UnixApp.h"
# include	"MNCommon.h"
# include	"ChopCfg.h"
# include	"ChopJet.h"
# include	"ChopTree.h"
# include	"ChopMenu.h"
# include	"ChopUp.h"

# if	defined ( LEBRAIN )
# include	"DdeObj.h"
# endif




/**************************************************************************/
/*                                                                        */
/*  Constructeur pour un cl�ne.                                           */
/*                                                                        */
/**************************************************************************/

TCheopsGadgetMenuWindow :: TCheopsGadgetMenuWindow  ( TCheopsGadgetMenuWindow &  w,
						      TFont *			 font )
	: TGadgetMenuWindow ( w, font )
   {
	Menu = TCheopsMenuTree :: CopyMenu ( w. Menu ) ;
	AdditionalItemCount = 0 ;
	LastChild = TRUE ;
	FloatingParent = 0 ;
	ModuleName = strdup ( w. ModuleName ) ;
	ThisMenuPath = new char [ MAX_PARAMETER_LENGTH ] ;
	strcpy ( ThisMenuPath, w. ModuleName ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  Destructeur.                                                          */
/*                                                                        */
/**************************************************************************/

TCheopsGadgetMenuWindow :: ~TCheopsGadgetMenuWindow  ( )
   {
	char		Buffer [ MAX_PARAMETER_LENGTH ] ;


	if  ( MenuType  !=  Clone  &&  CurrentGadget )
	   {
	// Ecrire la derni�re option s�lectionn�e
		register int		i =  CurrentGadget -> GetId ( ) ;


		CheopsConfiguration -> Get ( "Menu Positions", Menu -> MenuName,
			Buffer ) ;


		if  ( strcmp ( Buffer, Menu -> MenuItems [i]. ItemName ) )
			CheopsConfiguration -> Set ( "Menu Positions", Menu -> MenuName, 
				Menu -> MenuItems [i]. ItemName ) ;


	// Sauvegarder le chemin d'acc�s � ce menu, s'il s'agit du dernier
		if  ( LastChild )
			CheopsConfiguration -> Set ( "Opened Menus", ModuleName, 
				ThisMenuPath ) ;
	     }

	
	free ( ModuleName ) ;
	delete []  ThisMenuPath ;
	TCheopsMenuTree :: FreeMenu ( Menu ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TCheopsGadgetMenuWindow :: Create -                                   */
/*      Ins�re automatiquement les �l�ments de menu avant la cr�ation de  */
/*      la fen�tre.                                                       */
/*                                                                        */
/**************************************************************************/

BOOL	TCheopsGadgetMenuWindow :: Create  ( void )
   {
	register BOOL		Status ;
	char			Buffer [ MAX_PARAMETER_LENGTH ] ;
	register int		i ;


	InsertItems ( ) ;
	Status = TGadgetMenuWindow :: Create ( ) ;

	if  ( MenuType  !=  Clone )
	   {
		CheopsConfiguration -> Get ( "Menu Positions", Menu -> MenuName,
			Buffer ) ;

	// S�lectionner le dernier �l�ment choisi
		if  ( * Buffer )
		   {
			register int	i, found = -1 ;



		// Positionner sur le dernier �l�ment 
			for  ( i = 0 ; i < Menu -> ItemCount ; i ++ )
			   {
				if  ( Menu -> MenuItems [i]. ItemType  !=  SeparatorItem  &&
					 ! strcmp ( Buffer, Menu -> MenuItems [i]. ItemName ) )
				   {
					found = i ;
					break ;
				    }
			     }


			if  ( found  >=  0 )
			   {
				found += AdditionalItemCount ;
				SelectItem ( found ) ;
			    }
		      }


	// V�rifier si on ne fait pas partie d'une arborescence � ouvrir 
	// automatiquement
		CheopsConfiguration -> Get ( "Opened Menus", ModuleName, Buffer ) ;


		if  ( * Buffer )
		   {
			register char *		p     = Buffer ;
			register bool		found = false ;


			Buffer [ strlen ( Buffer ) + 1 ] = 0 ;	// Ajouter un 0 de plus

		// Remplacer le ':' s�parateur par un z�ro
			while  ( * p )
			   {
				if  ( * p  ==  ':' )
					* p = 0 ;
				p ++ ;
			    }



		// Chercher si ce menu fait partie de la liste de menus
		// ouverts qui a �t� sauvegard�e
			for  ( p = Buffer ; * p ; p += strlen ( p ) + 1 )
			   {
				if  ( ! strcmp ( p, Menu -> MenuName ) )
				   {
					found = true ;
					break ;
				    }
			     }



		// Trouv� : chercher s'il faut encore ouvrir un sous-menu
			if  ( found )
			   {
				p += strlen ( p ) + 1 ;



			// Ouais...
				if  ( * p )
				   {
					for  ( i = 0 ; i < Menu -> ItemCount ; i ++ )
					   {
						if  ( Menu -> MenuItems [i]. ItemType  ==  SeparatorItem )
							continue ;


					// Trouv� !
						if  ( ! strcmp ( p, Menu -> MenuItems [i]. ItemName )  &&
							TheCheopsMenuWindowObject -> MenuTree -> ExistMenu ( p ) )
						   {
							MenuSelect ( GadgetWithId ( i ) ) ;
							break ;
						    }
					     }
				     }
			      }
		     }
	      }
		
	return ( Status ) ;
    }






/**************************************************************************/
/*                                                                        */
/*  GetDesiredSize -                                                      */
/*      Prend en compte la largeur du titre dans le calcul de la largeur  */
/*      de fen�tre.                                                       */
/*                                                                        */
/**************************************************************************/

void	TCheopsGadgetMenuWindow :: GetDesiredSize  ( TSize &  Size )
   {
	TGadgetMenuWindow :: GetDesiredSize  ( Size ) ;


	if  ( FloatingParent  &&  FloatingParent -> GetCaptionFont ( ) )
	   {
		TSize		TitleSize ;
		TDC		DC ( 0 ) ;

		DC. SelectObject ( * FloatingParent -> GetCaptionFont ( ) ) ;
		TitleSize = DC. GetTextExtent ( Menu -> MenuText,
					strlen ( Menu -> MenuText ) ) ;

		if ( TitleSize. cx  >  Size. cx )
			Size. cx = TitleSize. cx ;
	     }
     }
	

	



/**************************************************************************/
/*                                                                        */
/*  TCheopsGadgetMenuWindow :: InsertItems -                              */
/*      Ins�re les �l�ments de menu.                                      */
/*                                                                        */
/**************************************************************************/

void	TCheopsGadgetMenuWindow :: InsertItems  ( void )
   {
	char	LastExecutedProgram [ MAX_PARAMETER_LENGTH ] ;



// R�cup�rer le nom du dernieer programme ex�cut� dans ce menu
	CheopsConfiguration -> Get ( "Last Execution", Menu -> MenuName,
			LastExecutedProgram ) ;

// Si nous ne somme pas cl�nes, il faut ins�rer le gadget qui permet de d�tacher
// le menu (plus un s�parateur pour faire joli)
	if  ( MenuType  !=  Clone )
	   {
		Insert ( * new  TMenuDetachGadget ( ) ) ;
		Insert ( * new  TGadgetMenuItem ( ) ) ;
	    }

// Dans tous les cas, on ins�re les �l�ments � partir de la description du menu
	for  ( int  i = 0 ; i < Menu -> ItemCount ; i ++ )
	   {
		TCheopsMenuItem &   item  =  Menu -> MenuItems [i] ;


		switch  ( item. ItemType )
		   {
			case	SeparatorItem :
				if  ( i + 1  <  Menu -> ItemCount )
					Insert ( * new  TGadgetMenuItem ( ) ) ;
				break ;


			case	ProgramItem :
			   {
				TGadgetMenuItem :: TState 	State  =  TGadgetMenuItem :: Enabled ;


				if  ( ! strcmp ( LastExecutedProgram, item. ItemName ) )
					State  |=  TGadgetMenuItem :: Checked ;

				Insert ( * new  TGadgetMenuItem ( i, 
					item. ItemText, TGadgetMenuItem :: Item,
						State ) ) ;
				break ;
			     }


			case	MenuItem :
				Insert ( * new  TGadgetMenuItem ( i, 
					item. ItemText, TGadgetMenuItem :: Popup ) ) ;
				
				break ;
		      }
	      }
      }

bool	TCheopsGadgetMenuWindow :: PreProcessMsg ( MSG &  msg )
   {
	if  ( ( msg. message  ==  WM_KEYDOWN ) )
		if  ( msg.wParam  ==  VK_F1 )
		   {
			TGadgetMenuItem *	pItem ;
			char *			pchar,
# if	defined ( LEBRAIN )
						la_page [ 80 ],
# endif
						Buffer [ 20 ] ;

			pItem = GetCurrentGadget ( ) ;
			strcpy ( Buffer, Menu -> MenuItems [ pItem -> GetId ( ) ]. ItemName ) ;
			for  ( pchar = Buffer + strlen ( Buffer ); pchar > Buffer; pchar -- )
				if  ( * pchar  ==  '/' )
				   {
					strcpy ( Buffer, pchar +1 ) ;
					break ;
				    }

# if	defined ( LEBRAIN )
			if  ( Menu -> MenuItems [ pItem -> GetId ( ) ]. ItemType  ==  MenuItem )
				sprintf ( la_page, "Ouvrir:%.7s$.html#%.7s$.htm", Menu -> MenuName,Buffer ) ;
			else
				sprintf ( la_page, "Ouvrir:%.7s$.html#%.8s.htm", Menu -> MenuName,Buffer ) ;
			DdeGlobal. Appel ( DDELINK_HYPERAIDE ) -> Connect ( ) ;
			DdeGlobal. Appel ( DDELINK_HYPERAIDE ) -> Execute ( la_page ) ;
			DdeGlobal. Appel ( DDELINK_HYPERAIDE ) -> Disconnect ( ) ;
# endif
		    }

	return TGadgetMenuWindow :: PreProcessMsg ( msg ) ;
      }







/**************************************************************************/
/*                                                                        */
/*  TCheopsFloatingMenu :: UpdateCheckmark -                              */
/*      Enl�ve les checkmarks des �l�ments du menu, et l'ajoute pour      */
/*      l'�l�ment sp�cifi�.                                               */
/*                                                                        */
/**************************************************************************/

void	TCheopsFloatingMenu :: UpdateCheckmark  ( int  gadgetid )
   {
	TGadgetMenuItem *	item ;
	TCheopsMenu &		Menu = MenuWindow -> GetMenu ( ) ;


	for  ( int  i = 0 ; i < Menu. ItemCount ; i ++ )
	   {
		TGadgetMenuItem :: TState 	State ;
	

		if  ( i  ==  gadgetid )
			continue ;

		item = MenuWindow -> GadgetWithId ( i ) ;

		if  ( ! item )
			continue ;

		State = item -> GetState ( ) ;

		if  ( State & ( TGadgetMenuItem :: Checked | TGadgetMenuItem :: Unchecked ) )
		   {
			State &= ~ ( TGadgetMenuItem :: Checked | TGadgetMenuItem :: Unchecked ) ;
			item -> SetState ( State ) ;
		    }
	     }


	item = MenuWindow -> GadgetWithId ( gadgetid ) ;	

	item -> SetState ( item -> GetState ( ) | TGadgetMenuItem :: Checked ) ;
    }




/**************************************************************************/
/**************************************************************************/
/********                                                          ********/
/********    Impl�mentation de la classe TCheopsFloatingMenu       ********/
/********                                                          ********/
/******** Il aurait �t� sympa de pouvoir faire comme pour les      ********/
/******** autres fen�tres d�riv�es de TFrameWindow, � savoir faire ********/
/******** d�river cette classe de TFloatingGadgetWindow ET de      ********/
/******** TMDIChild. Malheureusemen, cela n'est possible que si    ********/
/******** TFloatingFrame h�rite de TFrameWindow en "virtual", ce   ********/
/******** qui n'est pas le cas actuellement.                       ********/
/******** Donc, deux solutions : recompiler OWL pour ajouter ce    ********/
/******** mot-cl� virtual (et appeler explicitement le constructeur********/
/******** de TFrameWindow dans le constructeur de TFloatingFrame)  ********/
/******** OU :                                                     ********/
/******** Pomper le code de TMDIChild et l'ajouter dans cette      ********/
/******** classe.                                                  ********/
/******** Dont acte.                                               ********/
/********                                                          ********/
/**************************************************************************/
/**************************************************************************/

DEFINE_RESPONSE_TABLE1 ( TCheopsFloatingMenu, TFloatingGadgetWindow )
	EV_WM_MDIACTIVATE,
	EV_WM_NCACTIVATE,
        EV_WM_SETFOCUS,
END_RESPONSE_TABLE;


/**************************************************************************/
/*                                                                        */
/*  Constructeur pour le menu flottant.                                   */
/*                                                                        */
/**************************************************************************/

TCheopsFloatingMenu :: TCheopsFloatingMenu   ( TCheopsMenuWindowClient *  	parent,
					       TCheopsGadgetMenuWindow *      	client,
					       TPoint &				pos ) 
	: TFloatingGadgetWindow ( parent, client -> GetMenu ( ). MenuText,
		client )
   {
	MenuWindow = client ;
	MenuWindow -> SetFloatingParent ( this ) ;
	Attr. X = pos. x ;
	Attr. Y = pos. y ;
	Attr. Style  &=  ~WS_POPUP ;
	Attr. Style  |=  WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN ;
    }  





/**************************************************************************/
/*                                                                        */
/*  Destructeur.                                                          */
/*                                                                        */
/**************************************************************************/

TCheopsFloatingMenu :: ~TCheopsFloatingMenu  ( )
   {
// Supprimer la fen�tre gadget
	delete   MenuWindow ;

	if  ( TheCheopsMenuWindowObject )
		TheCheopsMenuWindowObject -> RemoveFloatingMenu ( this ) ;
    }



/**************************************************************************/
/*                                                                        */
/*  TCheopsFloatingMenu :: EvNCActivate -                                 */
/*      Ne pas permettre l'activation de la bordure si nous sommes        */
/*      temporairement cach�s par suite de maintenance des fen�tres.      */
/*                                                                        */
/**************************************************************************/

bool	TCheopsFloatingMenu::EvNCActivate(bool active)
   {
	return ( IsFlagSet ( wfUnHidden ) ? 
			false : TFloatingGadgetWindow :: EvNCActivate ( active ) ) ;
     }






/**************************************************************************/
/*                                                                        */
/*  TCheopsFloatingMenu :: ShowWindow -                                   */
/*      Effectue un traitement sp�cial pour �tre s�r que le client MDI    */
/*      g�re correctement sa liste de fen�tres.                           */
/*                                                                        */
/**************************************************************************/

bool	TCheopsFloatingMenu :: ShowWindow ( int cmdShow )
   {
	int  		retVal = TFrameWindow :: ShowWindow ( cmdShow ) ;     // 0 si cach�e



// Ne faire quelque chose si l'�tat (visible ou non) a chang�
	if  ( ( retVal  !=  0 )  !=  ( cmdShow  !=  SW_HIDE ) ) 
	   {
		if  ( ( HWND) Parent -> HandleMessage ( WM_MDIGETACTIVE )  ==  * this ) 
		   {
		      if  ( cmdShow  == SW_HIDE )
				Parent -> HandleMessage ( WM_MDINEXT, (uint) (HWND) * this ) ;
		      else
				HandleMessage ( WM_NCACTIVATE, true ) ; // R��mettre le message supprim�
		     }

# if 	defined ( BI_PLAT_WIN32 )
		Parent -> HandleMessage ( WM_MDIREFRESHMENU ) ;
# else
		Parent -> HandleMessage ( WM_MDISETMENU, true ) ;
# endif
	      }

	return ( retVal ) ;
     }





/**************************************************************************/
/*                                                                        */
/*  TCheopsFloatingMenu :: EnableWindow -                                 */
/*      cf ShowWindow...                                                  */
/*                                                                        */
/**************************************************************************/

bool  TCheopsFloatingMenu :: EnableWindow ( bool enable )
   {
	int 	retVal = TFrameWindow :: EnableWindow ( enable ) ;  // 0 si elle �tait enable


// Ne faire quelquechose que si l'�tat (visible ou non) a chang�
	if  ( ( retVal  !=  0)  !=  ( enable  == 0 ) ) 
	   {
		if  ( ! enable  &&  ( HWND ) Parent -> HandleMessage ( WM_MDIGETACTIVE )  ==  * this )
			Parent -> HandleMessage ( WM_MDINEXT, ( uint ) ( HWND ) * this ) ;

# if 	defined ( BI_PLAT_WIN32 )
		Parent -> HandleMessage ( WM_MDIREFRESHMENU ) ;
# else
		Parent -> HandleMessage ( WM_MDISETMENU, true ) ;
# endif
	     }

	return ( retVal ) ;
      }




/**************************************************************************/
/*                                                                        */
/*  TCheopsFloatingMenu :: PreProcessMsg -                                */
/*      Si la fen�tre MDI a demand� la navigation clavier, alors          */
/*      TFrameWindow :: PreProcessMsg va appeler :: IsDialogMessage, qui  */
/*      va "manger" l'�v�nement. La fen�tre client MDI n'aura alors       */
/*      aucune chance d'effectuer le traitement des acc�l�rateurs MDI.    */
/*      On le fait donc ici, ce traitement, tiens...                      */
/*                                                                        */
/**************************************************************************/

bool	TCheopsFloatingMenu :: PreProcessMsg ( MSG &  msg )
   {
	if  ( KeyboardHandling  &&  Parent -> PreProcessMsg ( msg ) )
		return ( true ) ;

	if  ( hAccel  && 
		:: TranslateAccelerator ( Parent -> Parent -> HWindow, hAccel, & msg ) )
		return ( true ) ;

	return TFloatingGadgetWindow :: PreProcessMsg ( msg ) ;
      }





/**************************************************************************/
/*                                                                        */
/*  TCheopsFloatingMenu :: Destroy -                                      */
/*      D�truit la fen�tre associ�e � l'objet.                            */
/*                                                                        */
/**************************************************************************/

void	TCheopsFloatingMenu :: Destroy ( int )
   {
	if  ( HWindow ) 
	   {
		if  ( Parent ) 		// Envoyer le message destroy au client MDI
		   { 
			Parent -> HandleMessage ( WM_MDIDESTROY, ( WPARAM ) HWindow ) ;
			HWindow = 0 ;  // On dira que �a a march�
		    }
		else 
		   {
			if  ( :: DestroyWindow ( HWindow ) )
				HWindow = 0 ;

			GetApplication ( ) -> ResumeThrow ( ) ;
		     }
	     }
      }




/**************************************************************************/
/*                                                                        */
/*  TCheopsFloatingMenu :: PerformCreate -                                */
/*      Une fen�tre MDI est cr��e en envoyant au client MDI une structure */
/*      MDICREATESTRUCT.                                                  */
/*                                                                        */
/**************************************************************************/

void	TCheopsFloatingMenu :: PerformCreate ( int )
   {
	MDICREATESTRUCT  	createStruct ;


	createStruct. szClass 	= GetClassName ( ) ;
	createStruct. szTitle 	= Title ;
	createStruct. hOwner 	= * GetModule ( ) ;
	createStruct. x 	= Attr. X ;
	createStruct. y 	= Attr. Y ;
	createStruct. cx 	= Attr. W ;
	createStruct. cy 	= Attr. H ;
	createStruct. style 	= Attr. Style ;
	createStruct. lParam 	= ( LPARAM ) Attr. Param ;

	HWindow = ( HWND ) Parent -> HandleMessage(
				WM_MDICREATE, 0, ( LPARAM ) & createStruct ) ;
     }




/**************************************************************************/
/*                                                                        */
/*  TCheopsFloatingMenu :: EvMDIActivate -                                */
/*      G�re l'activation de la fen�tre MDI.                              */
/*                                                                        */
/**************************************************************************/

void   TCheopsFloatingMenu :: EvMDIActivate ( HWND 	hWndActivated, 
					      HWND 	hWndDeactivated )
   {
	if  ( HWindow  ==  hWndActivated ) 
	   {

	// Une bug subtile dans la gestion du MDI sous Window fait que le premier
	// enfant MDI ne re�oit pas de WM_SETFOCUS. On va donc le simuler
		if  ( ! GetWindow ( GW_HWNDNEXT )  &&  GetFocus ( )  !=  HWindow )
			HandleMessage ( WM_SETFOCUS, WPARAM ( HWindow ) ) ;

	// Ajouter la barre de menu de cette fen�tre � la fen�tre principale
	// MDI s'il y a un MenuDescr
		if  ( GetMenuDescr ( ) ) 
		   {
			TFrameWindow * 	frame = TYPESAFE_DOWNCAST ( Parent -> Parent,TFrameWindow ) ;


			if  ( frame )
				frame -> MergeMenu ( * GetMenuDescr ( ) ) ;
		     }
	     }
	else 
	   {
	// Restaurer la barre de menu d'origine de la fen�tre principale s'il
	// n'y a pas d'autre enfant MDI � activer
		if  ( GetMenuDescr ( )  &&  ! hWndActivated ) 
		   {
			TFrameWindow * 	frame = TYPESAFE_DOWNCAST ( Parent -> Parent, TFrameWindow ) ;

			if  ( frame )
				frame -> RestoreMenu ( ) ;
		     }
	      }


  // Puis on passe tout �a au client s'il y en a un
	TWindow * 	 w = GetClientWindow ( ) ;


	if  ( w  &&  w -> IsWindow ( ) )

# if 	defined ( BI_PLAT_WIN32 )
	w -> HandleMessage ( WM_MDIACTIVATE, WPARAM ( hWndActivated ), 
				LPARAM ( hWndDeactivated ) ) ;
# else
	w -> HandleMessage ( WM_MDIACTIVATE, 0, 
				MAKELPARAM ( hWndActivated, hWndDeactivated ) ) ;
# endif
    }







/**************************************************************************/
/*                                                                        */
/*  TCheopsFloatingMenu :: DefWindowProc -                                */
/*      A surcharger pour utiliser DefMDIChildProc.                       */
/*                                                                        */
/**************************************************************************/

LRESULT	TCheopsFloatingMenu :: DefWindowProc ( uint 	msg, 
					       WPARAM 	wParam, 
					       LPARAM 	lParam )
    {
	if  ( IsFlagSet ( wfAlias ) )
		return ( TWindow :: DefWindowProc ( msg, wParam, lParam ) ) ;

	return ( :: DefMDIChildProc ( HWindow, msg, wParam, lParam ) ) ;
      }


