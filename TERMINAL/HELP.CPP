/**************************************************************************/
/*                                                                        */
/*  Help.cpp  -  Christian Vigh, 95/08/10.                                */
/*                                                                        */
/*  Implémentation de la classe THelp.                                    */
/*                                                                        */
/**************************************************************************/



# include	"AllTerm.h"
# pragma	hdrstop




# define	HELP_PROPERTY			"HelpReferenceCount"
# define	WNDPROC_PROPERTY		"HelpOriginalWndProc"



THelp *		THelp :: TheHelpObject			=  NULL ;




/**************************************************************************/
/*                                                                        */
/*  Constructeur.                                                         */
/*                                                                        */
/**************************************************************************/

THelp :: THelp   ( TWindow *	win )
   {
	register int		i ;


// Vérifier si un objet de ce type a déjà été déclaré
	if  ( TheHelpObject  !=  NULL )
		throw  xmsg ( string ( "Un objet de type THelp a été déclaré une seconde fois !" ) ) ;

	TheHelpObject = this ;


// Fenêtre parente de WinHelp
	MainWindow = win ;


// Initialisation des touches d'appel; on est sympa, on en fait une par défaut
	memset ( Hotkeys, 0, sizeof ( Hotkeys ) ) ;
	Hotkeys [0]. KeyCode 	=  VK_F1 ;
	Hotkeys [0]. Modifiers  =  None ;
	Hotkeys [0]. Type	=  Normal ;
	HotkeyCount = 1 ;

// Autres initialisations
	memset ( HelpTable, 0, sizeof ( HelpTable ) ) ;
	memset ( HelpFileNames, 0, sizeof ( HelpFileNames ) ) ;

	for  ( i = 0 ; i < MAX_HELP_ENTRIES ; i ++ )
		HelpTable [i]. HelpFileIndex = -1 ;

	HelpTableCount = 0 ;

	LastSelectedMenu	=  0 ;
	LastSelectedMenuType	=  0 ;
	LastSelectedMenuHandle 	=  0 ;


// Puis d'installer le hook qui va nous permettre de récupérer la même chose,
// mais pour les menus et les boîtes de dialogue
	MsgFilterHookId =  SetWindowsHookEx ( WH_MSGFILTER, 
				( HOOKPROC )  MsgFilterHookProc,
				* ( MainWindow -> GetModule ( ) ), 
					GetCurrentTask ( ) ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  Destructeur.                                                          */
/*                                                                        */
/**************************************************************************/

THelp :: ~THelp ( )
   {
	register int		i ;


// Remettre la bonne window proc
	for  ( i = 0 ; i < MAX_HELP_ENTRIES ; i ++ )
	   {
		if  ( HelpTable [i]. HelpFileIndex  ==  -1 )
			continue ;		// Entrée libre

		if  ( HelpTable [i]. AppliesTo  ==  ClassObject )
			continue ;		// Pas de window proc ici

		RemoveWindowProc ( i ) ;
		Quit ( HelpFileNames [ HelpTable [i]. HelpFileIndex ] ) ;
	     }					   


// Ainsi que le hook
	UnhookWindowsHookEx ( MsgFilterHookId ) ;


// La référence à l'objet THelp
	TheHelpObject = NULL ;
     }




/**************************************************************************/
/*                                                                        */
/*  THelp :: AddWindowProc                                                */
/*      Cette fonction installe la window proc de THelp pour attraper les */
/*      messages clavier d'une fenêtre donnée.                            */
/*      Cette window proc ne sera installée que si elle ne l'est pas      */
/*      déjà. On se sert pour cela d'une propriété que l'on ajoute à la   */
/*      fenêtre.                                                          */
/*                                                                        */
/**************************************************************************/

void	THelp :: AddWindowProc  ( int		index )
  {
	THelpDefinition &	e	=  HelpTable [index] ;
	register HANDLE		hProp	=  GetProp ( e. WindowHwnd, HELP_PROPERTY ) ;


/***
	 Premier appel pour ce HWND; on va créer deux propriétés de fenêtre
	. HELP_PROPERTY, pour indiquer le nombre de fois que cette fenêtre est
	  référencée dans une entrée de HelpTable (ce nombre de références est
	  utilisé ensuite par RemoveWindowProc, qui ne réinstallera la 
	  window proc d'origine que si ce nombre tombe à 1)
	. WNDPROC_PROPERTY, qui sert à stocker l'index de la 1e référence à ce
	  HWND. Les références ultérieures se contenteront de récupérer cet
	  index, afin de pouvoir donner une valeur à leur champ WindowProc
	  (en effet, s'il y avait plusieurs tables d'aide pour une même fenêtre,
	  un deuxièeme appel à GetWindowLong ( GWL_WNDPROC ) retournerait 
	  l'adresse de HelpWindowProc, ce qui donnerait une belle GPF).

	Note : on ajoute 1 à l'index, de manière à pouvoir identifier la
	valeur 0 comme l'absence de la propriété HELP_PROPERTY.
 ***/
	if  ( hProp  ==  NULL )		
	   {
		SetProp ( e. WindowHwnd, HELP_PROPERTY, ( HANDLE ) 1 ) ;
		e. WindowProc = ( WNDPROC ) GetWindowLong ( e. WindowHwnd, GWL_WNDPROC ) ;
		SetWindowLong ( e. WindowHwnd, GWL_WNDPROC, ( DWORD ) HelpWindowProc ) ;
		SetProp ( e. WindowHwnd, WNDPROC_PROPERTY, ( HANDLE ) ( index + 1 ) ) ;
	    }

/***
	La fenêtre était déjà référencée. On augmente son nombre de références et
	on récupère l'index, dans HelpTable, de sa première référence, afin de
	pouvoir initialiser le champ WindowProc.
 ***/
	else
	    {
		SetProp ( e. WindowHwnd, HELP_PROPERTY, ( HANDLE ) ( ( DWORD ) hProp + 1 ) ) ;

		hProp = GetProp ( e. WindowHwnd, WNDPROC_PROPERTY ) ;

		if  ( hProp )
			e. WindowProc = HelpTable [ ( int ) hProp - 1 ]. WindowProc ;
		else
			throw  xmsg ( string ( "La propriété WNDPROC n'a pas été trouvée !" ) ) ;
	     }
     }





/**************************************************************************/
/*                                                                        */
/*  THelp :: RemoveWindowProc                                             */
/*      Réinstalle la window proc d'origine, si le nombre de références   */
/*      à cette fenêtre est 1.                                            */
/*                                                                        */
/**************************************************************************/

void	THelp :: RemoveWindowProc  ( int  index )
   {
	THelpDefinition &	e	=  HelpTable [index] ;
	HANDLE			hProp	=  GetProp ( e. WindowHwnd, HELP_PROPERTY ) ;


	if  ( ( int ) hProp  ==  1 )  	// On peut remettre la bonne window proc
	   {
		RemoveProp ( e. WindowHwnd, HELP_PROPERTY ) ;           	
		RemoveProp ( e. WindowHwnd, WNDPROC_PROPERTY ) ;
		SetWindowLong ( e. WindowHwnd, GWL_WNDPROC, ( DWORD ) e. WindowProc ) ;
	    }
	else if  ( ( int ) hProp > 1 )  // sinon compter une référence de moins
		SetProp ( e. WindowHwnd, HELP_PROPERTY, ( HANDLE ) ( ( int ) hProp - 1 ) ) ;
     }
	    



/**************************************************************************/
/*                                                                        */
/*  Thelp :: GetHelpFileUsage                                             */
/*      Retourne le nombre de références à un fichier donné.              */
/*                                                                        */
/**************************************************************************/

int	THelp :: GetHelpFileUsage  ( int   index )
   {
	register int	i, count = 0 ;


	for  ( i = 0 ; i < MAX_HELP_ENTRIES ; i ++ )
	   {
		if  ( HelpTable [i]. HelpFileIndex  ==  index )
			count ++ ;
	    }

	return  ( count ) ;
     }



int	THelp :: GetHelpFileUsage  ( char *  name )
   {
	register int	i ;



	for  ( i = 0 ; i < MAX_HELP_ENTRIES ; i ++ )
	   {
		if  ( HelpFileNames [i]  !=  NULL  &&
				! stricmp ( HelpFileNames [i], name ) )
			return ( GetHelpFileUsage ( i ) ) ;
	    }

	return ( 0 ) ;
     }




/**************************************************************************/
/*                                                                        */
/*  THelp :: AddHelpFileName                                              */
/*      Ajoute un nom de fichier à la table, et retourne son index.       */
/*                                                                        */
/**************************************************************************/

int	THelp :: AddHelpFileName  ( char *  filename )
   {
	register int		i, first_free = -1 ;


	for  ( i = 0 ; i < MAX_HELP_ENTRIES ; i ++ )
	   {
		if   ( HelpFileNames [i]  ==  NULL )
		   {
			if  ( first_free  ==  -1 )
				first_free = i ;
		    }
		else if  ( ! stricmp ( HelpFileNames [i], filename ) )
			return ( i ) ;
	     }

	if  ( first_free  !=  -1 )
		HelpFileNames [ first_free ] = filename ;
	
	return ( first_free ) ;
     }






/**************************************************************************/
/*                                                                        */
/*  THelp :: AddHelp                                                      */
/*      Ajoute une entrée à l'aide en ligne.                              */
/*                                                                        */
/**************************************************************************/

int	THelp :: AddHelp  ( TWindow *		window,
			    char *		helpfile,
			    THelpEntry *	help )
   {
	register int	index = AddHelp ( WindowObject, ( void * ) window, helpfile, help ) ;


	if  ( index  !=  -1 )
		HelpTable [index]. FromResource = FALSE ;

	return ( index ) ;
    }


int	THelp :: AddHelp  ( char *		classname,
			    char *		helpfile,
			    THelpEntry *	help )
   {
	register int	index  =  AddHelp ( ClassObject, ( void * ) classname, helpfile, help ) ;


	if  ( index  !=  -1 )
		HelpTable [index]. FromResource = FALSE ;

	return ( index ) ;
    }




int	THelp :: AddHelp  ( THelpApply		appliesto,
			    void *		object,
			    char *		helpfile,
			    THelpEntry *	help )
    {
	register int		i, slot ;
	THelpEntry *		ep ;



// Rechercher une entrée libre
	for  ( i = 0, slot = -1 ; i < MAX_HELP_ENTRIES ; i ++ )
	   {
		if  ( HelpTable [i]. HelpFileIndex  ==  -1 )
		   {
			slot = i ;
			break ;
		    }
	    }

	if  ( slot  ==  -1 )
		return ( -1 ) ;


// Initialisation
	THelpDefinition &  e	=  HelpTable [slot] ;
	e. HelpFileIndex	=  AddHelpFileName ( helpfile ) ;
	e. AppliesTo		=  appliesto ;

	switch  ( e. AppliesTo )
	   {
		case	WindowObject :
			e. WindowHwnd = ( ( TWindow * ) object ) -> HWindow ;
			AddWindowProc ( slot ) ;
			break ;

		case	ClassObject :
			e. ClassName  = ( char * ) object ;
			break ;
	     }



// Recherche de l'aide pour la fenêtre
	ep  = help ;

	while  ( ep -> Type  !=  EndOfTable  &&  ep -> Type  !=  WindowStart )
		ep ++ ;

	if  ( ep -> Type  ==  WindowStart )
		e. WindowEntries = ep + 1 ;
	else
		e. WindowEntries = NULL ;


// Recherche de l'aide pour le menu
	ep  = help ;

	while  ( ep -> Type  !=  EndOfTable  &&  ep -> Type  !=  MenuStart )
		ep ++ ;

	if  ( ep -> Type  ==  MenuStart )
		e. MenuEntries = ep ;  	// MenuStart contient un contexte dont pas de +1
	else
		e. MenuEntries = NULL ;


// Recherche de l'aide pour les contrôles
	ep  = help ;

	while  ( ep -> Type  !=  EndOfTable  &&  ep -> Type  !=  ControlStart )
		ep ++ ;

	if  ( ep -> Type  ==  ControlStart )
		e. ControlEntries = ep + 1 ;
	else
		e. ControlEntries = NULL ;


// C'est bon
	HelpTableCount ++ ;
	return ( slot ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  THelp :: AddHelp                                                      */
/*      Autre version, pour charger les définitions à partir d'un fichier */
/*      de ressources.                                                    */
/*                                                                        */
/**************************************************************************/

# define	CHECKBOUND(p,end,needed)	if  ( p + needed  >=  end )  \
							goto  EndOfLoop
# define	REALLOC_INCREMENT		32



int	THelp :: AddHelp  ( TWindow *		window,
			    char *		helpfile,
			    TResId		id )
   {
	register int	index = AddHelp ( WindowObject, ( void * ) window, helpfile, id ) ;


	if  ( index  !=  -1 )
		HelpTable [index]. FromResource = TRUE ;

	return ( index ) ;
    }


int	THelp :: AddHelp  ( char *		classname,
			    char *		helpfile,
			    TResId		id )
   {
	register int	index  =  AddHelp ( ClassObject, ( void * ) classname, helpfile, id ) ;


	if  ( index  !=  -1 )
		HelpTable [index]. FromResource = TRUE ;

	return ( index ) ;
    }





int	THelp :: AddHelp  ( THelpApply		appliesto,
			    void *		object,
			    char *		helpfile,
			    TResId		id )
   {
	char 			Buffer [ MAX_PARAMETER_LENGTH ] ;
	HGLOBAL			hGlobal ;
	HRSRC			hResource ;
	char far *		p,
		 *		ResourceEnd ;
	char *			q ;
	THelpEntry *		Entries 	=  NULL ;
	THelpEntry		NewEntry ;
	int			EntryCount 	=  0 ;
	DWORD			ResourceSize ;
	BOOL			Break 		=  FALSE ;
	



// Accéder à la ressource
	hResource 	=  FindResource ( * :: Module, id, HELP_RESOURCE ) ;
	ResourceSize	=  SizeofResource ( * :: Module, hResource ) ;

	if  ( ! hResource )
		return ( -1 ) ;

	hGlobal		=  LoadResource ( * :: Module, hResource ) ;
	p		=  ( char far * ) LockResource ( hGlobal ) ;
	ResourceEnd	=  p + ( unsigned int ) ResourceSize ;


// Parcours de la resource
	while  ( p < ResourceEnd )
	   {
		switch ( * p )
		   {

		// DEFINE_WINDOW : 1 seul octet
			case	DEFWINTYPE - '0' :
				NewEntry. Id      	= 0 ;
				NewEntry. Type	        = WindowStart ;
				NewEntry. ContextParam	= 0L ;
				p ++ ;
				break ;

		// END_WINDOW : 1 seul octet
			case	ENDWINTYPE - '0' :
				NewEntry. Id      	= 0 ;
				NewEntry. Type	        = WindowEnd ;
				NewEntry. ContextParam	= 0L ;
				p ++ ;
				break ;

			
		// DEFINE_MENU : 1 octet + 1 chaîne
			case	DEFMENUTYPE - '0' :
				NewEntry. Id      	= 0 ;
				NewEntry. Type	        = MenuStart ;
				p ++ ;
				q = Buffer ;

				while  ( p < ResourceEnd  &&  
						q < Buffer + sizeof ( Buffer ) - 1  &&
							* p )
					* q ++ = * p ++ ;
				* q = 0 ;
				p ++ ;		// Sauter le zéro
                                                                 
				NewEntry. ContextParam  = ( DWORD ) strdup ( Buffer ) ;
				break ;

		// END_MENU : 1 octet
			case	ENDMENUTYPE - '0' :
				NewEntry. Id      	= 0 ;
				NewEntry. Type	        = MenuEnd ;
				NewEntry. ContextParam	= 0L ;
				p ++ ;
				break ;


		// DEFINE_CONTROL : 1 seul octet
			case	DEFCONTROLTYPE - '0' :
				NewEntry. Id      	= 0 ;
				NewEntry. Type	        = ControlStart ;
				NewEntry. ContextParam	= 0L ;
				p ++ ;
				break ;

		// END_CONTROL : 1 seul octet
			case	ENDCONTROLTYPE - '0' :
				NewEntry. Id      	= 0 ;
				NewEntry. Type	        = ControlEnd ;
				NewEntry. ContextParam	= 0L ;
				p ++ ;
				break ;


		// HELPID : 1 octet, id sur 16 bits, contexte sur 32
			case	BYIDTYPE - '0' :
				CHECKBOUND ( p, ResourceEnd, 
					1 + sizeof ( short int ) + sizeof ( long int ) ) ;
				p ++ ;
				NewEntry. Id		=  * ( ( short int * ) p ) ;
				p += sizeof ( short int ) ;

				NewEntry. ContextParam	=  * ( ( long int * ) p ) ;
				p += sizeof ( long int ) ;
				break ;


		// HELPKEY : 1 octet, id sur 16 bits, chaîne
			case	BYKEYTYPE - '0' :
				CHECKBOUND ( p, ResourceEnd, 1 + sizeof ( short int ) ) ;
				p ++ ;
				NewEntry. Id		=  * ( ( short int * ) p ) ;
				p += sizeof ( short int ) ;

				q = Buffer ;

				while  ( p < ResourceEnd  &&  
						q < Buffer + sizeof ( Buffer ) - 1  &&
							* p )
					* q ++ = * p ++ ;
				* q = 0 ;
				p ++ ;		// Sauter le zéro

				NewEntry. ContextParam  = ( DWORD ) strdup ( Buffer ) ;
				break ;


		// Zéro : c'est la fin de la ressource (normalement...)
			case	0 :
				NewEntry. Id		=  0 ;
				NewEntry. Type		=  EndOfTable ;
				NewEntry. ContextParam  =  0L ;
				Break 			=  TRUE ;
				break ;
				

		// Autre : y a une douille dans le pâté				
			default :
				p ++ ;
				continue ;
		   }

	// Le type de l'entrée a été déterminé : on l'ajoute à notre table
		if  ( ! EntryCount )
			Entries = ( THelpEntry * ) malloc ( REALLOC_INCREMENT * sizeof ( THelpEntry ) ) ;
		else if  ( ! ( ( EntryCount + 1 ) % REALLOC_INCREMENT ) )
			Entries = ( THelpEntry * ) realloc ( Entries,
				( EntryCount + 1 + REALLOC_INCREMENT ) *
					sizeof ( THelpEntry ) ) ;

		if  ( Entries  ==  NULL )
			throw  xmsg ( string ( "Erreur d'allocation mémoire durant la lecture"
					       " d'une ressource HELPTABLE" ) ) ;	

		Entries [ EntryCount ++ ]  =  NewEntry ;


	// On est tombé sur le zéro de fin...
		if ( Break )
			break ;
	   }


EndOfLoop :
	UnlockResource ( hGlobal ) ;
	FreeResource ( hGlobal ) ;

// On arrive ici, que la lecture se soit bien passée ou non
	if  ( ! EntryCount )	// Léger problème...
		return ( -1 ) ;
	else
		return ( AddHelp ( appliesto, object, helpfile, Entries ) ) ;
    }


				     


/**************************************************************************/
/*                                                                        */
/*  THelp :: RemoveHelp                                                   */
/*      Libère une entrée.                                                */
/*                                                                        */
/**************************************************************************/

BOOL	THelp :: RemoveHelp  ( int  index )
   {
	register int		i, hfindex ;


// Contrôles
	if  ( index < 0  ||  index  >=  MAX_HELP_ENTRIES  ||
			HelpTable [index]. HelpFileIndex  ==  -1 )
		return ( FALSE ) ;

	
// Supprimer le nom de fichier s'il n'est plus référencé
	hfindex = HelpTable [index]. HelpFileIndex ;
	HelpTable [index]. HelpFileIndex = -1 ;

	i = GetHelpFileUsage  ( hfindex ) ;

	if  ( ! i )
		HelpFileNames [ hfindex ] = NULL ;


// Si l'entrée correspond à une aide pour une fenêtre, il faut remettre la
// bonne window proc
	RemoveWindowProc ( index ) ;

// Libérer la mémoire si les tables proviennent d'un fichier de ressources
	if  ( HelpTable [index]. FromResource )
	   {
		register THelpEntry *	p  =  HelpTable [index]. ResourceEntries ;


		while  ( p  -> Type  !=  EndOfTable )
		   {
			if  ( p -> Type  ==  ByKey )
				delete []  ( char * ) p -> ContextParam ;
			p ++ ;
		    }

		delete []  p ;
	    }

	HelpTableCount -- ;
	return ( TRUE ) ;
    }
			    




/**************************************************************************/
/*                                                                        */
/*   THelp :: SetHotkeys                                                  */
/*      Définit les touches d'appel de l'aide en ligne.                   */
/*                                                                        */
/**************************************************************************/

void	THelp :: SetHotkeys  ( THotkey *	keys,
			       int		count )
   {
	register int		limit  =  ( count > MAX_HOTKEYS ) ?  MAX_HOTKEYS : count ;


	memcpy ( Hotkeys, keys, limit * sizeof ( THotkey ) ) ;
    }	






/**************************************************************************/
/*                                                                        */
/*  THelp :: Help                                                         */
/*      Fonctions d'appels de l'aide en ligne Windows.                    */
/*                                                                        */
/**************************************************************************/

int	THelp :: Help  ( char *  helpfile, DWORD   context_id )
    {
	return  ( WinHelp ( helpfile, HELP_CONTEXT, context_id ) ) ;
     }



int	THelp :: Help  ( char *  helpfile, char *  key )
    {
	return  ( WinHelp ( helpfile, HELP_KEY, ( DWORD ) key ) ) ;
     }




int	THelp :: Help  ( char *  helpfile )
    {
	return  ( WinHelp ( helpfile, HELP_INDEX, 0L ) ) ;
     }





int	THelp :: Help  ( char *  helpfile, MULTIKEYHELP *  keys )
    {
	return  ( WinHelp ( helpfile, HELP_MULTIKEY, ( DWORD ) keys ) ) ;
     }



int	THelp :: HelpOnHelp  ( void )
    {
	return  ( WinHelp ( 0, HELP_HELPONHELP, 0L ) ) ;
     }



int	THelp :: Quit  ( char *  filename )
   {
	return ( WinHelp ( filename, HELP_QUIT, 0L ) ) ;
    }



/**************************************************************************/
/*                                                                        */
/*   THelp :: WinHelp                                                     */
/*      Appel la vraie fonction WinHelp.                                  */
/*                                                                        */
/**************************************************************************/

int	THelp :: WinHelp  ( char *  filename, uint  command, DWORD  data ) 
   {
	return ( :: WinHelp ( MainWindow -> HWindow, filename, command, data ) ) ;
    }






/**************************************************************************/
/*                                                                        */
/*  THelp :: IsHotKey                                                     */
/*      Vérifie le message WM_KEYDOWN ou WM_CHAR passé en paramètre.      */
/*      Retourne :                                                        */
/*                                                                        */
/*      . THotkeyType::Unknown si cette touche n'est pas une hotkey.      */
/*      . THotkeyType::Normal s'il s'agit d'une hotkey "normale"          */
/*      . THotkeyType::Contextual s'il s'agit d'une hotkey destinée à     */
/*        appeler une aide contextuelle (relative à un seul contrôle de   */
/*        boîte de dialogue, par exemple).                                */
/*                                                                        */
/**************************************************************************/

# define	SAME(a,b)		( ( (a) && (b) )  ||  ! (a)  &&  ! (b) )


THelp :: THotkeyType  	THelp :: IsHotkey  ( HWND 	/* hwnd */,
					     uint	message,
					     WPARAM	wparam,
					     LPARAM	/* lparam */ )
   {
	register int		i ;
	uint			KeyCode ;
	TKeyModifiers		Modifiers ;
	THotkeyType		Type ;
	unsigned int		control, alt, shift ;



	if  ( message  !=   WM_KEYDOWN  &&  message  !=  WM_CHAR )
		return ( Unknown ) ;


	for  ( i = 0 ; i < MAX_HOTKEYS ; i ++ )
	   {
		if  ( ! Hotkeys [i]. KeyCode )
			continue ;

		
		KeyCode		=  Hotkeys [i]. KeyCode ;
		Modifiers	=  Hotkeys [i]. Modifiers ;
		Type		=  Hotkeys [i]. Type ;
		
		control 	=  GetAsyncKeyState ( VK_CONTROL )  &  0x8000 ;
		alt		=  GetAsyncKeyState ( VK_MENU    )  &  0x8000 ;
		shift		=  GetAsyncKeyState ( VK_SHIFT   )  &  0x8000 ;

		if  ( message  !=  WM_KEYDOWN )
			return  ( Unknown ) ;

		if  ( KeyCode  !=  wparam )
			return ( Unknown ) ;

		if  ( SAME ( Modifiers & Control, control )  &&
		      SAME ( Modifiers & Alt    , alt     )  &&
		      SAME ( Modifiers & Shift  , shift ) )
			return  ( Type ) ;
	     }


	return ( Unknown ) ;
     }
		






/**************************************************************************/
/*                                                                        */
/*  THelp :: NotFound                                                     */
/*      Cette fonction est appelée lorsqu'on n'a pas trouvé d'entrée      */
/*      dans la table des chaînes contextuelles, qui corresponde au       */
/*      destinataire du message. Elle ne fait rien ici, mais est plutôt   */
/*      destinée à une redéfinition dans une classe dérivée.              */
/*      NotFound doit renvoyer TRUE si elle a traité le message.          */
/*      Si window est à TRUE, on n'a pu trouver la fenêtre destinatrice.  */
/*      Dans le cas contraire, cela signifie qu'il n'y avait aucun        */
/*      contexte de défini.                                               */
/*                                                                        */
/**************************************************************************/

BOOL	THelp :: NotFound  ( BOOL, char *,
			     TOrigin, THotkeyType, HWND, uint, WPARAM, LPARAM )
   {
	return ( TRUE ) ;
    }











/**************************************************************************/
/*                                                                        */
/*  THelp :: ProcessHelpRequest                                           */
/*      Lance l'aide en ligne relative au contexte sélectionné.           */
/*      Il existe 4 cas de figure :                                       */
/*                                                                        */
/*      . L'origine est WindowHelp ou DialogHelp et l'aide demandée est   */
/*        contextuelle : on va cherche une entrée correspondant au        */
/*        contrôle actif dans la liste ControlEntries ; en cas d'échec,   */
/*        on prend WindowEntries                                          */
/*                                                                        */
/*      . L'origine est WindowHelp ou DialogHelp et l'aide demandée       */
/*        n'est pas contextuelle : on prend WindowEntries.                */
/*                                                                        */
/*      Les menus sont un cas à part. Lorsqu'on définit les contextes     */
/*      de menus, il faut respecter l'ordre suivant :                     */
/*              . La première entrée contient l'aide générale des         */
/*                menus (on doit arriver sur un écran d'aide qui          */
/*                indique la liste des menus figurant sur la barre de     */
/*                menu).                                                  */
/*     Viennent ensuite les contextes pour chacun des menus de la         */
/*     barre de menus :                                                   */
/*              . Le premier élément de cette liste doit être le contexte */
/*                relatif à l'aide pour ce menu popup.                    */
/*              . Les entrées suivantes correspondent aux éléments du     */
/*                menu.                                                   */
/*     Pour les deux contextes décrivant la barre de menu et les menus    */
/*     popup, le champ Id du contexte doit être à zéro.                   */
/*                                                                        */
/*     Bien entendu, avant de déterminer quel contexte utiliser pour      */
/*     appeler l'aide en ligne, il faut déjà déterminer si la fenêtre     */
/*     hwnd appartient bien à la liste gérée par l'objet THelp.           */
/*                                                                        */
/*     . Avant toute chose, le message WM_QUERYHELPCONTEXT est envoyé     */
/*       à la fenêtre. Si celle si désire spécifier son propre numéro de  */
/*       contexte, elle doit renseigner les champs Entry. ContextParam    */
/*       et Entry. Type (l'adresse de cette structure est passée dans     */
/*       lparam).                                                         */
/*                                                                        */
/*     . Si la fenêtre ne gère pas ce message, il reste deux              */
/*       possibilités :                                                   */
/*                                                                        */
/*       . Le hwnd correspond à un WindowHwnd de la table d'aide :        */
/*         on effectuera la recherche dans cette entrée.                  */
/*                                                                        */
/*       . Le hwnd ne correspond pas à un WindowHwnd de la table d'aide : */
/*         Si la fenêtre est une fenêtre OWL, on essaiera de déterminer   */
/*         si sa classe est définie dans la table d'aide; si oui, on      */
/*         effectuera la recherche, si non, on ne pourra rien faire.      */
/*                                                                        */
/*  Dans le cas où l'aide est relative à une classe OWL qui gère          */
/*  plusieurs boîtes de dialogues, il est possible d'avoir plusieurs      */
/*  WindowEntries, chacune ayant un champ Id différent.                   */
/*                                                                        */
/**************************************************************************/

void	THelp :: ProcessHelpRequest  ( TOrigin		Origin,
				       THotkeyType	Type,
				       HWND		hwnd,
				       uint		message,
				       WPARAM		wparam,
				       LPARAM		lparam )
    {
	THelpQueryContext		Context ;
	register int			i, depth, found, nesting ;
	register THelpDefinition *	HelpDef ;
	register THelpEntry *		Entry		=  NULL ,
			    *		ep,
			    *		sp ;
	HWND				ControlHwnd ;
	HMENU				hMenu ;
	register char *			p ;
	TWindow *			OwlWindow	=  GetWindowPtr ( hwnd ) ;




// La fenêtre veut-elle nous donner son contexte d'aide ?	
	Context. hwnd		=  hwnd ;
	Context. message	=  message ;
	Context. wparam		=  wparam ;
	Context. lparam		=  lparam ;
	Context. Origin		=  Origin ;
	Context. Type		=  Type ;
	memset ( & Context. Entry, 0, sizeof ( Context. Entry ) ) ;

	SendMessage ( hwnd, WM_QUERYHELPCONTEXT, 0, ( LPARAM ) & Context ) ;


	if  ( Context. Entry. ContextParam )
	   {
		Entry = & Context. Entry ;
		goto  GotHelpEntry ;
	    }


// Rechercher si cette fenêtre n'est pas déjà déclarée dans notre liste
	for  ( i = 0 ; i < MAX_HELP_ENTRIES ;  i ++ )
	   {
		if  ( HelpTable [i]. HelpFileIndex  ==  -1 )
			continue ;

		if  ( HelpTable [i]. AppliesTo  ==  WindowObject  &&
			HelpTable [i]. WindowHwnd  ==  hwnd )
		   {
			HelpDef = HelpTable + i ;
			goto  GotHelpDef ;
		    }
	     }


// Pas trouvée dans la table : s'agit-il d'une fenêtre OWL ?
// Si le focus est sur un contrôle, faudrait peut-être mieux prendre la
// boîte de dialogue parent...
	if  ( OwlWindow  ==  NULL  ||  OwlWindow -> GetDlgCtrlID ( ) )
		OwlWindow = GetWindowPtr ( GetParent ( hwnd ) ) ;

	if  ( OwlWindow  !=  NULL )
	   {
		const char *  zz = typeid ( * OwlWindow ). name ( ) ;


		for  ( i = 0 ; i < MAX_HELP_ENTRIES ; i ++ )
		    {
			if  ( HelpTable [i]. HelpFileIndex  ==  -1 )
				continue ;


			if  ( HelpTable [i]. AppliesTo  ==  ClassObject  &&
				! strcmp ( HelpTable [i]. ClassName, zz ) ) 
			   {
				HelpDef = HelpTable + i ;
				goto  GotHelpDef ;
			    }
		      }
	     }


// C'est la fin des haricots...
	NotFound ( TRUE, NULL, Origin, Type, hwnd, message, wparam, lparam ) ;
	return ;


GotHelpDef :
	// L'entrée dans la table d'aide a été trouvée ; il s'agit maintenant
	// de retrouver le contexte adéquat
	if  ( Origin  ==  WindowHelp  ||  Origin  ==  DialogBoxHelp )
	   {
		if  ( Type  ==  Contextual )
		   {
			ControlHwnd  =  GetFocus ( ) ;
			i = GetDlgCtrlID ( ControlHwnd ) ;

			for  ( ep = HelpDef -> ControlEntries ; 
					ep  !=  NULL  &&
					ep -> Type  !=  EndOfTable  &&
					ep -> Type  !=  WindowEnd ; ep ++ )
			   {
				if  ( i  ==  ep -> Id )
				   {
					Entry = ep ;
					break ;
				     }
			     }
		     }


	//  L'aide demandée n'est pas contextuelle OU 
	//  le contrôle qui a le focus n'appartient pas à cette fenêtre OU
	//  aucun contexte n'a été trouvé dans les contrôles pour cette
	// fenêtre
		if  ( Entry  ==  NULL )
		   {

		// Si la fenêtre est une boîte de dialogue, on va essayer de
		// voir s'il existe un contexte spécifique.
		// Note : cela ne sera possible que si l'aide est relative à une
		// classe OWL
			TWindow *	W = GetWindowPtr ( hwnd ) ;
			TDialog *	D = NULL ;


			if  ( W  !=  NULL )
				D = dynamic_cast <TDialog *> ( W ) ;


			if  ( D  ==  NULL )
			   {
				Entry = HelpDef -> WindowEntries ;
				goto  GotHelpEntry ;
			    }

		// Ca ne marche que si le nom de la ressource dialogue est
		// défini comme un nombre entier !!!
			i = ( int ) D -> Attr. Name ;				


		// Recherche de la WindowEntry ayant le bon numéro de boîte
		// ou, à défaut, de celle qui possède un id à 0.
			for  ( ep = HelpDef -> WindowEntries, sp = NULL ;
					ep  !=  NULL		&&	
					ep -> Type  !=  WindowEnd  &&
					ep -> Type  !=  EndOfTable ; ep ++ )
			  {
				if  ( ! ep -> Id  &&  sp  == NULL )
					sp = ep ; 	// 1er id à zéro
				else if  ( ep -> Id  ==  i )
				   {
					sp = ep ;
					break ;
				    }
			    }

			ep = ( sp  ==  NULL ) ?  HelpDef -> WindowEntries : sp ;
			goto  GotHelpDef ;
		    }
	      }	
		
	
	
// Cas des menus
	if  ( Origin  ==  MenuHelp )
	   {
		if  ( Type  ==  Normal )		// le cas le plus simple
		   {
			Entry = HelpDef -> MenuEntries ;
			goto  GotHelpEntry ;
		    }
			

	// Menu popup : on va  recherche son entrée dans la table en se basant
	// sur l'identificateur de son premier élément
		if  ( LastSelectedMenuType  & MF_SYSMENU )
		   {
			Entry = HelpDef -> MenuEntries ;
			goto  GotHelpEntry ;
		    }


		if  ( LastSelectedMenuType  & MF_POPUP )
		   {
			hMenu = LastSelectedMenuHandle ;
			depth = 0 ;
			i = -1 ;

			while  ( i  ==  -1 )
			   {
				i = GetMenuItemID ( hMenu, 0 ) ;
				hMenu = GetSubMenu ( hMenu, 0 ) ;
				depth ++ ;

				if  ( hMenu  ==  NULL )
					break ;
			     }
				

		// Aucun sous-élément n'a été trouvé !
			if  ( i  ==  -1 )
			   {
				Entry = HelpDef -> MenuEntries ;
				goto  GotHelpEntry ;
			    }

		// Un élément a été trouvé : on le cherche dans les entrées
			nesting = 0 ;

			for  ( sp = NULL, ep  =  HelpDef -> MenuEntries, found = 0 ; 
					ep  !=  NULL  &&
					ep -> Type !=  EndOfTable  &&  
					( ep  -> Type  != MenuEnd  ||  nesting ) ; ep ++ )
			   {
				if  ( ! ep -> Id )	
					sp = ep ;	// Dernier élément nul
				else if  ( ep -> Id  ==  i )
				   {
					found = 1 ;
					break ;
				    }

				if  ( ep -> Type  ==   MenuStart )
					nesting ++ ;
				else if  ( ep -> Type  !=  MenuEnd )
					nesting -- ;
			     }

		// Pas trouvé : on mettra l'aide en ligne sur le 1er élément
			if  ( ! found  ||  sp  ==  NULL )
			   {
				Entry = HelpDef -> MenuEntries ;
				goto  GotHelpEntry ;
			    }


		// Trouvé : dans le do...while, on a été susceptibles de descendre
		// plusieurs niveau de menu (si le 1er élément du menu est un
		// popup, dont le 1er élément est un popup, etc..)
		// On va essayer de remonter à partir de sp pour retrouver 
		// l'index du 1er popup
			while  ( depth  &&  sp  >=  HelpDef -> MenuEntries )
			   {
				depth -- ; 

				if  ( ! sp -> Id  &&  ! depth )
				   {
					Entry = sp ;
					goto   GotHelpEntry ;
				    }

				sp -- ;
			     }

			Entry = HelpDef -> MenuEntries ;
			goto  GotHelpEntry ;
		    }
		else
		   {
		// L'élément de menu n'est pas un popup
			nesting = 0 ;

			for  ( ep = HelpDef -> MenuEntries ; 
						ep  !=  NULL  &&
						ep -> Type  !=  EndOfTable  &&
						( ep -> Type  !=  MenuEnd || nesting ) ; ep ++ )
			   {
				if  ( ep -> Id  ==  LastSelectedMenu )
				   {
					Entry = ep ;
					goto  GotHelpEntry ;
				    }
				
				if  ( ep -> Type  ==  MenuStart )
					nesting ++ ;
				else if  ( ep -> Type  ==  MenuEnd )
					nesting -- ;
			    }
		     }
	    }



GotHelpEntry :

// On arrive ici avec Entry qui est peut-être nul; dans ce cas, on appelle l'index
	p = HelpFileNames [ HelpDef -> HelpFileIndex ] ;

// Si le pointeur est nul, on essaie de prendre par défaut l'aide relative à
// la fenêtre
	if  ( Entry  ==  NULL )
		Entry = HelpDef -> WindowEntries ;

	if  ( Entry  ==  NULL )
	   {
		NotFound ( FALSE, p, Origin, Type, hwnd, message, wparam, lparam ) ;
		Help ( p ) ;
	    }
	else
	   {
:: MessageBox ( NULL, "HELP", MB_OK, "ID = %d, KeyValue = [%s]",
Entry -> Id, Entry -> ContextParam ) ;

		if  ( Entry -> Type  ==  ByID )
			Help ( p, Entry -> ContextParam ) ;
		else
			Help ( p, ( char * ) Entry -> ContextParam ) ;
	    }

// Ouf, c'est fini ! j'en ferai pas tous les jours, des comme ça
     }





/**************************************************************************/
/*                                                                        */
/*  HelpWindowProc -                                                      */
/*      Récupère les messages WM_KEYDOWN et WM_CHAR pour voir s'il ne     */
/*      s'agit pas d'une hotkey.                                          */
/*      Dans le cas d'un message WM_MENUSELECT, sauvegarde les            */
/*      paramètres.                                                       */
/*                                                                        */
/**************************************************************************/

LRESULT CALLBACK  HelpWindowProc  ( HWND  	hwnd, 
				    uint  	message,
				    WPARAM	wparam,
				    LPARAM	lparam )
   {
	register THelp :: THotkeyType		Type ;
	WNDPROC					WindowProc ;
	HANDLE					WndProcProperty ;


	if  ( ! THelp :: TheHelpObject  ||
			! THelp :: TheHelpObject -> HelpTableCount )
		throw  xmsg ( string ( "HelpWindowProc a été appelée alors qu'il n'y a aucune aide disponible !!!" ) ) ;

	switch  ( message )
	   {

	// Messages qui sont peut-être des hotkeys
		case	WM_KEYDOWN :
		case	WM_CHAR :
			Type = THelp :: TheHelpObject -> IsHotkey ( hwnd, message, wparam, lparam ) ;
			
			if  ( Type  !=  THelp :: Unknown )
			   {
				THelp :: TheHelpObject -> ProcessHelpRequest ( THelp :: WindowHelp, 
						     Type,
						     hwnd, message, 
						     wparam, lparam ) ;
				return ( 0L ) ;
			    }
			break ;


	// C'est au MENUSELECT qu'on peut récupérer des infos sur l'élément de 
	// menu actuellement sélectionné.
	// Lorsque LOWORD ( lparam ) == 0xFFFF et HIWORD ( lparam ) est à zéro,
	// on est sur un menu popup dont le handle est donné par wparam
		case	WM_MENUSELECT :	
			if  ( LOWORD ( lparam )  !=  0xFFFF  &&  HIWORD ( lparam ) )
			   {
				THelp :: TheHelpObject -> LastSelectedMenuType    =  LOWORD ( lparam ) ;

				if  ( LOWORD ( lparam )  &  MF_POPUP )
				   {
					THelp :: TheHelpObject -> LastSelectedMenu = 0 ;
					THelp :: TheHelpObject -> LastSelectedMenuHandle =
							( HMENU ) wparam ;
				    }
				else
				   {
					THelp :: TheHelpObject -> LastSelectedMenu = wparam ;
					THelp :: TheHelpObject -> LastSelectedMenuHandle  =  
							( HMENU ) HIWORD ( lparam ) ;
				    }
			     }
			break ;
	      }


// Il s'agit maintenant d'appeler la bonne WindowProc. On récupère l'index de la
// première référence à cette fenêtre
	WndProcProperty = GetProp ( hwnd, WNDPROC_PROPERTY ) ;
	WindowProc = NULL ;

	if  ( WndProcProperty )
		WindowProc = THelp :: TheHelpObject ->
				HelpTable [ ( int ) WndProcProperty - 1 ]. WindowProc ;

	if  ( WindowProc  !=  NULL )
		return ( CallWindowProc ( WindowProc, hwnd, message, wparam, lparam ) ) ;
	else
		return ( 0L ) ;
     }





/**************************************************************************/
/*                                                                        */
/*   MsgFilterHookProc -                                                  */
/*      Récupère les message destinés à des menus ou des boîtes de        */
/*      dialogue.                                                         */
/*                                                                        */
/**************************************************************************/
				    
LRESULT CALLBACK  MsgFilterHookProc  ( int		code,
				       WPARAM		wparam,
				       LPARAM		lparam )
   {
	MSG *				Message	 =  ( MSG * ) lparam ;
	THelp :: TOrigin		Origin ;
	THelp :: THotkeyType		Type ;


	if  ( THelp :: TheHelpObject -> HelpTableCount  &&
		( code  ==  MSGF_DIALOGBOX  ||  code  ==  MSGF_MENU ) )
	   {
		Type = THelp :: TheHelpObject -> IsHotkey ( Message -> hwnd,
						   Message -> message,
						   Message -> wParam,
						   Message -> lParam ) ;

		if  ( Type  !=  THelp :: Unknown )
		   {
			if  ( code  ==  MSGF_MENU )
				Origin = THelp :: MenuHelp ;
			else
				Origin = THelp :: DialogBoxHelp ;


			THelp :: TheHelpObject -> ProcessHelpRequest ( Origin, Type,
					     Message -> hwnd, Message -> message, 
					     Message -> wParam, Message -> lParam ) ;
			return ( TRUE ) ;
		     }
			
	     }

	return  ( CallNextHookEx ( THelp :: TheHelpObject -> MsgFilterHookId, 
					code, wparam, lparam ) ) ;
     }




		
