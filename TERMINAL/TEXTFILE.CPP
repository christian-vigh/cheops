/**************************************************************************/
/*                                                                        */
/*   TextFile.cpp  -  Christian Vigh, 95/09/28.                           */
/*                                                                        */
/*   Impl�mentation de la classe TextFile.                                */
/*                                                                        */
/**************************************************************************/

/**************************************************************************

   MODIFICATIONS -

   97/09/26 (CV) -
	Modification de la fonction TextFile :: Read. Cette derni�re ne
	retournait la valeur -1 (pour indiquer une fin de fichier) que si
	aucun caract�re n'avait �t� lu. Sous certaines conditions, l'objet
	TextFile "perdait les p�dales" et ne parvenait pas � d�terminer la
	fin r�elle du fichier (on se retrouvait avec "Page 100/86" dans la
	fenetre d'�dition).

 **************************************************************************/

# include	"AllTerm.h"
# pragma	hdrstop


# include	"TextFile.h"





/**************************************************************************/
/*                                                                        */
/*  Constructeur.                                                         */
/*      Initialise les donn�es.                                           */
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
/*      Lib�re la m�moire allou�e.                                        */
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
/*      Initialise les donn�es � z�ro.                                    */
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
/*      Lib�re les donn�es allou�es.                                      */
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

// R�initialiser	    
	Reset ( ) ;
      }






/**************************************************************************/
/*                                                                        */
/*  TTextFile :: Open                                                     */
/*      Ouvre le fichier en lecture seule.                                */
/*      Retourne FALE en cas d'�chec.                                     */
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
/*      Indique si le caract�re sp�cifi� est un caract�re de fin de       */
/*      ligne. En sortie, IsEOL sera TRUE si c'est le cas, et Discard     */
/*      indiquera si le caract�re doit �tre conserv� ou pas.              */
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
/*      Retourne dans buffer le contenu de la ligne sp�cifi�e.            */
/*      Retourne FALSE si la ligne n'existe pas.                          */
/*                                                                        */
/**************************************************************************/

BOOL 	TTextFile :: GetLine  ( long int  line, char *  buffer )
   {
// V�rifier si le fichier est ouvert
	if  ( ! TextFileFP  ||  ! OffsetTable  ||  line  <  0 )
		return  ( FALSE ) ;


// Si la ligne est d�j� dans le cache, on n'a rien � faire
	int  		CachedIndex = ( int ) ( line % MAX_CACHED_LINES ) ;
	TCachedLines *	CachedLine  = Lines + CachedIndex ;

	if  ( CachedLine ->  LineNumber - 1  ==  line  &&  CachedLine -> Line )
	   {
		strcpy ( buffer, CachedLine -> Line ) ;
		return  ( TRUE ) ;
	    }


// Sinon, on la recherche dans le fichier
	if  ( line  >=  OffsetCount )		// Le fichier a peut-�tre �t� mis � jour
	   {
		Update ( ) ;			// Lire le surplus rajout�

		if  ( line  >=  OffsetCount )	// Rien � faire...
			return ( FALSE ) ;
	     }



// D�terminer son emplacement dans OffsetTable
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
/*      Effectue une remise � jour de la table des offsets de lignes      */
/*      en lisant la portion de fichier qui a �ventuellement �t�          */
/*      rajout�e.                                                         */
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


// Reprendre la mise � jour � partir de la derni�re ligne
// (si celle-ci n'�tait pas compl�te lors du dernier Update, on se sera arr�t�
// en milieu de ligne; l'Update suivant comptabilisera cette demi-ligne comme
// une ligne � part enti�re...)
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

		FilePosition = Offset ;		// Mettre � jour la position de fin de fichier
	     }
    }






/**************************************************************************/
/*                                                                        */
/*  TTextFile :: Add -                                                    */
/*      Ajoute l'offset sp�cifi� dans la liste cha�n�e des offsets.       */
/*                                                                        */
/**************************************************************************/

void	TTextFile :: Add  ( long int  offset ) 
   {
	register int	element 	=  ( int ) ( OffsetCount % MAX_OFFSETS ) ;


	if  ( ! OffsetCount )			// Premi�re entr�e de la table
	   {
		OffsetTable = new  TOffsetTable ;
		OffsetTableEnd = OffsetTable ;
		OffsetTable -> Next = 0 ;
		memset ( OffsetTableEnd -> Offsets, 0, 
				sizeof ( OffsetTableEnd -> Offsets ) ) ;
	    }
	else if  ( ! element )			// Entr�e actuelle pleine :
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
/*      Se charge de lire une ligne � partir de la position sp�cifi�e.    */
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


//  Le fichier doit �tre ouvert...
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
			

	// Type Marker : le caract�re repr�sente une ligne � lui seul
		if  ( Type  ==  Marker   ||  Type  ==  EndOfPage )
		   {
		// Le caract�re pr�c�dent �tait une fin de ligne : on le comptabilise
		// comme une ligne � part enti�re
			if  ( p  ==  buffer )
				* p ++ = c ;
		// Sinon on le laisse de c�t� pour le prochain appel et on termine 
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


// V�rifier le r�sultat
// (CV 97/09/26) : le test d'origine �tait :
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
/*      sp�cifi�e.                                                        */
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
/*      Retourne le no de la ligne d�butant la page sp�cifi�e.            */
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

