/**************************************************************************/
/*                                                                        */
/*  Choptree.cpp  -  Christian Vigh, 95/11.                               */
/*                                                                        */
/*  Implémentation de la classe  TCheopsMenuTree.                         */
/*                                                                        */
/**************************************************************************/



# include	"AllTerm.h"
# pragma	hdrstop


# include	"MnCommon.h"
# include	"UnixApp.h"
# include	"ChopCfg.h"
# include	"ChopJet.h"
# include	"ChopTree.h"
# include	"ChopMenu.h"
# include	"ChopUp.h"



# define	MENU_BUFFER_SIZE		8192
# define	MENU_GROW_SIZE			  16





/***
	Fonction pour lire une ligne sans le newline à la fin.
 ****/

char *	fgets ( FILE *  fp, char *  buffer, int  size )
   {
	register char *		p = fgets ( buffer, size, fp ) ;


	if  ( p )
	   {
		register char * 	q = buffer + strlen ( buffer ) - 1 ;


		if  ( * q  ==  '\n' )
			* q = 0 ;
	     }

	return  ( p ) ;
     }





/**************************************************************************/
/*                                                                        */
/*  Constructeur.                                                         */
/*      Les menus sont lus dans l'ordrer alphabetique.                    */
/*                                                                        */
/**************************************************************************/

TCheopsMenuTree :: TCheopsMenuTree  ( FILE *	fp ) 
   {
// Initialisations
	MenuFileFP  	= fp ;
	MenuCount   	= 0 ;
	Menus		= 0 ;
	MenuFileBuffer  = 0 ;


	if  ( fp  ==  NULL )
		return ;

		
//  Allouer le buffer pour les entrées-sorties		
	MenuFileBuffer = new  char [ MENU_BUFFER_SIZE ] ;
	setvbuf ( fp, MenuFileBuffer, _IOFBF, MENU_BUFFER_SIZE ) ;
	fseek ( fp, 0, 0 ) ;


// Lire le fichier
	char		Buffer [ MAX_PARAMETER_LENGTH ] ;
	long int	CurrentOffset 	=  0 ;
	

	while  ( fgets ( fp, Buffer, sizeof ( Buffer ) )  !=  NULL )
	   {

	// Ligne commençant par 'M' : c'est un menu
		if  ( * Buffer  ==  'M' )
		   {
			char			Name [ MAX_PARAMETER_LENGTH ] ;
			register char *		p = Buffer + 1,
				      *		q = Name ;

		// Récupérer le nom du menu
			while  ( * p  &&  * p  !=  ',' )
				* q ++ = * p ++ ;
			* q = 0 ;

				
		// Allouer une nouvelle entrée
			if  ( ! MenuCount )
				Menus = ( TMenuPosition * ) malloc ( MENU_GROW_SIZE * sizeof ( TMenuPosition ) ) ;
			else if  ( ! ( ( MenuCount + 1 ) % MENU_GROW_SIZE ) )
			   {
				int		newcount  = MenuCount + 1 + MENU_GROW_SIZE ;
				TMenuPosition * mp ;

				mp = ( TMenuPosition * ) realloc ( Menus, newcount * sizeof ( TMenuPosition ) ) ;

				if  ( ! mp )	// Ignorer silencieusement les erreurs d'alloc
					return ;
				else
					Menus = mp ;
			     }


		// Y mettre l'offset et le nom du menu
			Menus [ MenuCount ]. Offset = CurrentOffset + 1 ;
			Menus [ MenuCount ]. Name   = strdup ( Name ) ;
			MenuCount ++ ;
		     }


	// La position courante du fichier sera peut-être le début d'un nouveau
	// menu
		CurrentOffset = ftell ( fp ) ;
	    }
     }
			




/**************************************************************************/
/*                                                                        */
/*  Destructeur.                                                          */
/*                                                                        */
/**************************************************************************/

TCheopsMenuTree :: ~TCheopsMenuTree  ( )
   {
	register int	i ;


	if  ( MenuCount )
	   {
		for  ( i = 0 ; i < MenuCount ; i ++ )
			free ( Menus [i]. Name ) ;
		free ( Menus ) ;
	    }

	if  ( MenuFileBuffer )
		delete []  MenuFileBuffer ;

	if  ( MenuFileFP )
		fclose ( MenuFileFP ) ;
     }




/**************************************************************************/
/*                                                                        */
/*  TCheopsMenuTree :: GetMenu -                                          */
/*      Retourne la définition d'un menu Cheops.                          */
/*      La recherche est dichotomique : il est donc important que les     */
/*      menus nous arrivent triés.                                        */
/*                                                                        */
/**************************************************************************/

TCheopsMenu *	TCheopsMenuTree :: GetMenu  ( char *  name )
   {
	register int	low	=  0,
			up	=  MenuCount - 1,
			current =  ( up - low ) / 2 ;
	register bool	found	=  false ;



// Recherche
	while  ( low  <=  up )
	   {
		register int	i  =  stricmp ( name, Menus [current]. Name ) ;



		if  ( i > 0 )
			low = current + 1 ;
		else if  ( i < 0 )
			up  = current - 1 ;
		else
		   {
			found = true ;
			break ;
		    }


		current = low + ( ( up - low ) / 2 ) ;
	      }

	if  ( ! found ) 
		return ( 0 ) ;


// Trouvé : commencer la lecture
	TCheopsMenu *		Menu	  =  new  TCheopsMenu ;	
	TCheopsMenuItem *	MenuItems =  0 ;
	int			ItemCount =  0 ;
	char			Buffer [ MAX_PARAMETER_LENGTH ] ;
	char			Item   [ MAX_PARAMETER_LENGTH ] ;
	register char *		p,
		     *		q ;


	* Buffer = 0 ;
	fseek ( MenuFileFP, Menus [current]. Offset, 0 ) ;



// D'abord, la définition du menu
	fgets ( MenuFileFP, Buffer, sizeof ( Buffer ) ) ;
	p = Buffer ;
	q = Item ;


// le nom du menu...
	while  ( * p  &&  * p  !=  ',' )
		* q ++ = * p ++ ;
	* q = 0 ;

	Menu -> MenuName  = strdup ( Item ) ;
	Beautify ( p + 1 ) ;
	Menu -> MenuText  = strdup ( p + 1 ) ;
 


// Ensuite, les éléments
	while  ( fgets ( MenuFileFP, Buffer, sizeof ( Buffer ) )  !=  NULL  &&
			* Buffer  !=  'M' )
	   {
		register int	more ;


	// I majuscule : il faut un séparateur âprès
		if  ( * Buffer  ==  'I' )
			more = 2 ;
		else 
			more = 1 ;		


	// Allouer la place nécessaire
		if  ( ! MenuItems )
			MenuItems = ( TCheopsMenuItem * ) malloc ( 
				sizeof ( TCheopsMenuItem ) * more ) ;
		else
			MenuItems = ( TCheopsMenuItem * ) realloc ( MenuItems,
				sizeof ( TCheopsMenuItem ) * ( ItemCount + more ) ) ;

	// L'erreur d'allocation, je vais te dire, on cherche même pas à gérer
		if  ( ! MenuItems )
		   {
			free ( Menu -> MenuName ) ;
			free ( Menu -> MenuText ) ;
			delete  Menu ;
			return ( 0 ) ;
		    }


	// Lire les paramètres de l'élément de menu
	// D'abord, le nom de l'élément
		p = Buffer + 1, q = Item ;
		
		
		while  ( * p  &&  * p  !=  ',' )
			* q ++ = * p ++ ;
		* q = 0 ;

		MenuItems [ItemCount]. ItemName = strdup ( Item ) ;

	// Puis le type de l'élément
		if  ( * p )
			p ++ ;
		q = Item ;

		while  ( * p  &&  * p  !=  ',' )
			* q ++ = * p ++ ;
		* q = 0 ;


		if  ( ! stricmp ( Item, "m" )  ||  ! stricmp ( Item, "r" ) )
			MenuItems [ ItemCount ]. ItemType = MenuItem ;
		else if  ( ! stricmp ( Item, "p" ) )
			MenuItems [ ItemCount ]. ItemType = ProgramItem ;
		else                                               
			MenuItems [ ItemCount ]. ItemType = ProgramItem ;


	// Puis le texte du menu
		if  ( * p )
		   {
			p ++ ;
			Beautify ( p ) ;
		    }

		MenuItems [ ItemCount ]. ItemText = strdup ( p ) ;
		ItemCount ++ ;

	// Ajouter un séparateur si besoin est
		if  ( more > 1 )
			MenuItems [ ItemCount ++ ]. ItemType = SeparatorItem ;
	     }


	Menu -> MenuItems = MenuItems ;
	Menu -> ItemCount = ItemCount ;
	return  ( Menu ) ;
    }







/**************************************************************************/
/*                                                                        */
/*  TCheopsMenuTree :: FreeMenu -                                         */
/*      Libère la mémoire allouée pour un menu.                           */
/*                                                                        */
/**************************************************************************/

void	TCheopsMenuTree :: FreeMenu  ( TCheopsMenu *  Menu )
   {
	free ( Menu -> MenuName ) ;
	free ( Menu -> MenuText ) ;


	if  ( Menu -> ItemCount )
	   {
		for  ( int  i = 0 ; i < Menu -> ItemCount ; i ++ )
		   {
			if  ( Menu -> MenuItems [i]. ItemType  !=  SeparatorItem )
			   {
				free ( Menu -> MenuItems [i]. ItemName ) ;
				free ( Menu -> MenuItems [i]. ItemText ) ;
			     }
		    }

		free ( Menu -> MenuItems ) ;
	     }

	delete  Menu ;
      }


		

/**************************************************************************/
/*                                                                        */
/*  TCheopsMenuTree :: CopyMenu -                                         */
/*      Duplique le menu spécifié.                                        */
/*                                                                        */
/**************************************************************************/

TCheopsMenu *  TCheopsMenuTree :: CopyMenu  ( TCheopsMenu *  Menu )
   {
	register TCheopsMenu *	m = new  TCheopsMenu ;

	
	m -> MenuName  = strdup ( Menu -> MenuName ) ;
	m -> MenuText  = strdup ( Menu -> MenuText ) ;
	m -> ItemCount = Menu -> ItemCount ;

	if  ( Menu -> ItemCount )
	   {
		m -> MenuItems = ( TCheopsMenuItem * ) 
			malloc ( sizeof ( TCheopsMenuItem ) * m -> ItemCount ) ;

		for  ( int  i = 0 ; i < Menu -> ItemCount ; i ++ )
		   {
			m -> MenuItems [i]. ItemType = Menu -> MenuItems [i]. ItemType ;		

			if  ( Menu -> MenuItems [i]. ItemType  ==  SeparatorItem )
				continue ;

			m -> MenuItems [i]. ItemName = strdup ( Menu -> MenuItems [i]. ItemName ) ;
			m -> MenuItems [i]. ItemText = strdup ( Menu -> MenuItems [i]. ItemText ) ;
		    }

	     }
	else
		m -> MenuItems = 0 ;

	return ( m ) ;
      }






/**************************************************************************/
/*                                                                        */
/*  TCheopsMenuTree :: ExistMenu -                                        */
/*      Retourne TRUE si le menu spécifié existe.                         */
/*                                                                        */
/**************************************************************************/

BOOL	TCheopsMenuTree :: ExistMenu  ( char *  name )
   {
	for  ( int  i = 0 ; i < MenuCount ; i ++ )
	   {
		if  ( ! stricmp ( Menus [i]. Name, name ) )
			return  ( TRUE ) ;
	    }

	return ( FALSE ) ;
     }
