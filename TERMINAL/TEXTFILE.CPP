/**************************************************************************/
/*                                                                        */
/*   TextFile.cpp  -  Christian Vigh, 95/09/28.                           */
/*                                                                        */
/*   Implémentation de la classe TextFile.                                */
/*                                                                        */
/**************************************************************************/

/**************************************************************************

   MODIFICATIONS -

   97/09/26 (CV) -
	Modification de la fonction TextFile :: Read. Cette dernière ne
	retournait la valeur -1 (pour indiquer une fin de fichier) que si
	aucun caractère n'avait été lu. Sous certaines conditions, l'objet
	TextFile "perdait les pédales" et ne parvenait pas à déterminer la
	fin réelle du fichier (on se retrouvait avec "Page 100/86" dans la
	fenetre d'édition).

 **************************************************************************/

# include	"AllTerm.h"
# pragma	hdrstop


# include	"TextFile.h"





/**************************************************************************/
/*                                                                        */
/*  Constructeur.                                                         */
/*      Initialise les données.                                           */
/*                                                                        */
/**************************************************************************/

TTextFile :: TTextFile  ( )
   {

	Lines		=  new  TCachedLines [ MAX_CACHED_LINES ] ;
	InputBuffer	=  new  char         [ TEXT_BUFFER_SIZE ] ;
	Reset ( ) ;
    }




/**************************************************************************/
/*                                                                        */
/*   Destructeur.                                                         */
/*      Libère la mémoire allouée.                                        */
/*                                                                        */
/**************************************************************************/

TTextFile :: ~TTextFile ( )	
   {
	if  ( Lines )
	   {
		for  ( int  i = 0 ; i < MAX_CACHED_LINES ; i ++ )
		   {
			if  ( Lines [i]. Line )	
				free ( Lines [i]. Line ) ;
		     }

		delete []  Lines ;
		Lines = 0 ;
	    }

	if  ( InputBuffer )
	   { 
		delete []  InputBuffer ;
		InputBuffer = 0 ;
	    }

	Free ( ) ;

    }




/**************************************************************************/
/*                                                                        */
/*  TTextFile :: Reset                                                    */
/*      Initialise les données à zéro.                                    */
/*                                                                        */
/**************************************************************************/


void	TTextFile :: Reset  ( void )
   {
	OffsetTable 	=  0 ;
	OffsetTableEnd	=  0 ;
	OffsetCount	=  0 ;
	FilePosition	=  0 ;
	TextFileFP	=  0 ;

	if  ( Lines )
		memset ( Lines, 0, sizeof ( TCachedLines ) * MAX_CACHED_LINES ) ;
     }






/**************************************************************************/
/*                                                                        */
/*  TTextFile :: Free -                                                   */
/*      Libère les données allouées.                                      */
/*                                                                        */
/**************************************************************************/

void	TTextFile :: Free  ( void )
   {
	TOffsetTable *		p ;


// Fermer le fichier s'il est ouvert
	if  ( TextFileFP  &&  WasOpenedHere )
		fclose ( TextFileFP ) ;

// Supprimer la table des offsets
	while  ( OffsetTable )
	   {
		p = OffsetTable -> Next ;
		delete  OffsetTable ;
		OffsetTable = p ;
	    }

// Réinitialiser	    
	Reset ( ) ;
      }






/**************************************************************************/
/*                                                                        */
/*  TTextFile :: Open                                                     */
/*      Ouvre le fichier en lecture seule.                                */
/*      Retourne FALE en cas d'échec.                                     */
/*                                                                        */
/**************************************************************************/

BOOL	TTextFile :: Open  ( char *	filename )
   {
	strcpy ( TextFileName, filename ) ;
	TextFileFP = fopen  ( TextFileName, "rb" ) ;
	WasOpenedHere = TRUE ;

	if  ( TextFileFP )
	   {
		setvbuf ( TextFileFP, InputBuffer, _IOFBF, TEXT_BUFFER_SIZE ) ;
		Update ( ) ;
		return ( TRUE ) ;
	    }
	else
		return ( FALSE ) ;
     }




BOOL	TTextFile :: Open  ( FILE *  fp )
   {
	WasOpenedHere = FALSE ;

	if  ( fp )
	   {
		TextFileFP = fp ;
		Update ( ) ;
		return ( TRUE ) ;
	    }
	else
		return ( FALSE ) ;
     }





/**************************************************************************/
/*                                                                        */
/*  TTextFile :: Close                                                    */
/*      Ferme le fichier.                                                 */
/*                                                                        */
/**************************************************************************/

void	TTextFile :: Close  ( void )
   {
	if  ( TextFileFP )
		Free ( ) ;
    }







/**************************************************************************/
/*                                                                        */
/*  TTextFile :: CheckEOL                                                 */
/*      Indique si le caractère spécifié est un caractère de fin de       */
/*      ligne. En sortie, IsEOL sera TRUE si c'est le cas, et Discard     */
/*      indiquera si le caractère doit être conservé ou pas.              */
/*                                                                        */
/**************************************************************************/

TTextFile :: TCharacterType  TTextFile :: CheckCharacter   ( int   c ) 
   {
	TCharacterType 		Type ;


	switch  ( c )
	   {
		case	'\n' :
			Type = EOL | Discard ;
			break ;

		default :
			Type = Regular ;
	     }


	return  ( Type ) ;
     }

	






/**************************************************************************/
/*                                                                        */
/*  TTextFile :: GetLine                                                  */
/*      Retourne dans buffer le contenu de la ligne spécifiée.            */
/*      Retourne FALSE si la ligne n'existe pas.                          */
/*                                                                        */
/**************************************************************************/

BOOL 	TTextFile :: GetLine  ( long int  line, char *  buffer )
   {
// Vérifier si le fichier est ouvert
	if  ( ! TextFileFP  ||  ! OffsetTable  ||  line  <  0 )
		return  ( FALSE ) ;


// Si la ligne est déjà dans le cache, on n'a rien à faire
	int  		CachedIndex = ( int ) ( line % MAX_CACHED_LINES ) ;
	TCachedLines *	CachedLine  = Lines + CachedIndex ;

	if  ( CachedLine ->  LineNumber - 1  ==  line  &&  CachedLine -> Line )
	   {
		strcpy ( buffer, CachedLine -> Line ) ;
		return  ( TRUE ) ;
	    }


// Sinon, on la recherche dans le fichier
	if  ( line  >=  OffsetCount )		// Le fichier a peut-être été mis à jour
	   {
		Update ( ) ;			// Lire le surplus rajouté

		if  ( line  >=  OffsetCount )	// Rien à faire...
			return ( FALSE ) ;
	     }



// Déterminer son emplacement dans OffsetTable
	register TOffsetTable *		p  	=  OffsetTable ;
	register int			cluster =  ( int ) ( line / MAX_OFFSETS ),
					element =  ( int ) ( line % MAX_OFFSETS ) ;
	long int			pos ;


	while  ( cluster -- )
		p = p -> Next ;

	Read ( p -> Offsets [element] & EOP_MASK, buffer, pos ) ;

	if  ( pos  ==  -1 )
		return ( FALSE ) ;


// Ajouter la ligne dans le cache 
	if  ( CachedLine -> Line )
		free ( CachedLine -> Line ) ;

	CachedLine -> Line = strdup ( buffer ) ;
	CachedLine -> LineNumber = line + 1 ;


	return ( TRUE ) ;
    }






/**************************************************************************/
/*                                                                        */
/*  TTextFile :: Update                                                   */
/*      Effectue une remise à jour de la table des offsets de lignes      */
/*      en lisant la portion de fichier qui a éventuellement été          */
/*      rajoutée.                                                         */
/*                                                                        */
/**************************************************************************/

void	TTextFile :: Update  ( void )
   {
	char		Buffer [ TEXT_LINE_SIZE ] ;
	long int	Offset ;
	register BOOL	Flag ;
	


// Ca ne marchera pas si le fichier n'est pas ouvert...
	if  ( ! TextFileFP )
		return ;


// Reprendre la mise à jour à partir de la dernière ligne
// (si celle-ci n'était pas complète lors du dernier Update, on se sera arrêté
// en milieu de ligne; l'Update suivant comptabilisera cette demi-ligne comme
// une ligne à part entière...)
	if  ( OffsetCount )
	   {
		long int			line	=  OffsetCount - 1 ;
		register TOffsetTable *		p  	=  OffsetTable ;
		register int			cluster =  ( int ) ( line / MAX_OFFSETS ),
						element =  ( int ) ( line % MAX_OFFSETS ) ;


		while  ( cluster -- )
			p = p -> Next ;

		FilePosition = p -> Offsets [ element ] ;
		OffsetCount -- ;
	     }

	
// Tenter de lire les lignes
	while  ( 1 )
	   {

	// Lecture
		Flag = Read ( FilePosition, Buffer, Offset ) ;

		if  ( Offset  ==  -1 ) 		// Fin de fichier atteinte
			break ;

		if  ( Flag )
			FilePosition |= EOP_FLAG ;
		Add  ( FilePosition ) ;

		FilePosition = Offset ;		// Mettre à jour la position de fin de fichier
	     }
    }






/**************************************************************************/
/*                                                                        */
/*  TTextFile :: Add -                                                    */
/*      Ajoute l'offset spécifié dans la liste chaînée des offsets.       */
/*                                                                        */
/**************************************************************************/

void	TTextFile :: Add  ( long int  offset ) 
   {
	register int	element 	=  ( int ) ( OffsetCount % MAX_OFFSETS ) ;


	if  ( ! OffsetCount )			// Première entrée de la table
	   {
		OffsetTable = new  TOffsetTable ;
		OffsetTableEnd = OffsetTable ;
		OffsetTable -> Next = 0 ;
		memset ( OffsetTableEnd -> Offsets, 0, 
				sizeof ( OffsetTableEnd -> Offsets ) ) ;
	    }
	else if  ( ! element )			// Entrée actuelle pleine :
	   {					// il faut en ajouter une
		OffsetTableEnd -> Next = new  TOffsetTable ;
		OffsetTableEnd  =  OffsetTableEnd -> Next ;
		OffsetTableEnd -> Next = 0 ;
		memset ( OffsetTableEnd -> Offsets, 0, 
				sizeof ( OffsetTableEnd -> Offsets ) ) ;
	    }


// Ajouter l'offset de cette ligne
	OffsetTableEnd -> Offsets [ element ] = offset ;
	OffsetCount ++ ;
     }




/**************************************************************************/
/*                                                                        */
/*  TTextFile :: Read                                                     */
/*      Se charge de lire une ligne à partir de la position spécifiée.    */
/*      Retourne la nouvelle position du fichier.                         */
/*                                                                        */
/**************************************************************************/

BOOL		TTextFile :: Read  ( long int  where, char *  buffer,
				     long int &  newpos )
   {
	register char *		p	=  buffer ;
	register int		c ;
	TCharacterType		Type ;
	BOOL			discard ;
	BOOL			GotEOP	=  FALSE ;
	register int		Count	=  0 ;


//  Le fichier doit être ouvert...
	if  ( ! TextFileFP )
		return ( -1 ) ;


// Se positionner au bon endroit
	where &= EOP_MASK ;
	fseek ( TextFileFP, where, 0 ) ;


// Lire la ligne
	while ( ( c = GetCharacter ( TextFileFP ) )  !=  EOF )
	   {
		Type = CheckCharacter ( c ) ;
		discard  =  Type & Discard ;
		Type = ( TCharacterType ) ( ( ( int ) Type )  &  ~ ( ( int ) Discard ) ) ;
			

	// Type Marker : le caractère représente une ligne à lui seul
		if  ( Type  ==  Marker   ||  Type  ==  EndOfPage )
		   {
		// Le caractère précédent était une fin de ligne : on le comptabilise
		// comme une ligne à part entière
			if  ( p  ==  buffer )
				* p ++ = c ;
		// Sinon on le laisse de côté pour le prochain appel et on termine 
		// cette ligne
			else
				fseek ( TextFileFP, -1, SEEK_CUR ) ;

			if  ( Type  ==  EndOfPage )
				GotEOP = TRUE ;

			break ;
		     }


		if  ( ! discard )
		   {
			Count ++ ;
			* p ++ = c ;
		     }

		if  ( Type  ==  EOL )
			break ;

		if  ( Count + 2  >=  TEXT_LINE_SIZE )
			break ;
	     }


// Vérifier le résultat
// (CV 97/09/26) : le test d'origine était :
//		if   ( c  ==  EOF  &&  p  ==  buffer )...
	if  ( c  ==  EOF ) 		// Fin de fichier...
		newpos = -1 ;
	else
		newpos = ftell ( TextFileFP ) ;

	* p = 0 ;				// Terminer la ligne
	return ( GotEOP ) ;
    }			






/**************************************************************************/
/*                                                                        */
/*   TTextFile :: GetPageFromLine -                                       */
/*      Retourne le no de page dans laquelle se trouve la ligne           */
/*      spécifiée.                                                        */
/*                                                                        */
/**************************************************************************/

long int	TTextFile :: GetPageFromLine  ( long int  line )
   {
	long int		count	=  1 ;
	TOffsetTable *          lc 		=  OffsetTable ;
	register int		line_element	=  ( int ) ( line % MAX_OFFSETS ),
				line_cluster	=  ( int ) ( line / MAX_OFFSETS ),
				i, max ;


	if  ( line  <  0 )
		return  ( -1 ) ;

	if  ( line  >=  OffsetCount )
		return  ( 0 ) ;


	do
	   {
		max = ( line_cluster ) ?  MAX_OFFSETS : line_element + 1 ;

		for  ( i = 0 ; i < max ; i ++ )
		   {
			if  ( lc -> Offsets [i]  &  EOP_FLAG )
				count ++ ;
		     }

		lc = lc -> Next ;
		line_cluster -- ;
	     }   while  ( line_cluster  >=  0 ) ;


	return  ( count ) ;
     }	





/**************************************************************************/
/*                                                                        */
/*   TTextFile :: GetLineFromPage -                                       */
/*      Retourne le no de la ligne débutant la page spécifiée.            */
/*                                                                        */
/**************************************************************************/

long int	TTextFile :: GetLineFromPage  ( long int  page )
   {
	long int		count	=  0 ;
	TOffsetTable *          lc 		=  OffsetTable ;
	register int		line_element	=  ( int ) ( OffsetCount % MAX_OFFSETS ),
				line_cluster	=  ( int ) ( OffsetCount / MAX_OFFSETS ),
				i, max ;

	if  ( page  <  0 )
		return  ( -1 ) ;

	if  ( page  ==  1 )
		return ( 0 ) ;

	page -- ;

	do
	   {
		max = ( line_cluster ) ?  MAX_OFFSETS : line_element + 1 ;

		for  ( i = 0 ; i < max ; i ++ )
		   {
			if  ( lc -> Offsets [i]  &  EOP_FLAG )
			   {
				page -- ;

				if  ( ! page )
					return (  count + i + 1 ) ;
			     }
		     }

		count += max ;
		lc = lc -> Next ;
		line_cluster -- ;
	     }   while  ( line_cluster  >=  0  &&  page ) ;


	return  ( -1 ) ;
     }	

