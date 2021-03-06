/**************************************************************************/
/*                                                                        */
/*  Redo.cpp  -  Christian Vigh, 94/09/25.                                */
/*                                                                        */
/*  Impl�mentation de la classe TRedoList.                                */
/*                                                                        */
/**************************************************************************/


# include	"AllTerm.h"
# pragma	hdrstop


# define	LINE_SIZE		256



/**************************************************************************/
/*                                                                        */
/*  TRedoList :: TRedoList                                                */
/*      Initialise le tableau de pointeurs sur les commandes.             */
/*                                                                        */
/**************************************************************************/

TRedoList :: TRedoList  ( )
   {
	LineCount	=  0 ;
	Origin		=  0 ;
	MinRedoSize	=  4 ;

	for  ( int  i = 0 ; i < MaxLines ; i ++ )
		Lines [i] [0] = 0 ;
      }



/**************************************************************************/
/*                                                                        */
/*  TRedoList :: ~TRedoList                                               */
/*      D�salloue la m�moire allou�e pour les lignes de commande.         */
/*                                                                        */
/**************************************************************************/

TRedoList :: ~TRedoList  ( )
   {
     }





/**************************************************************************/
/*                                                                        */
/*   TRedoList :: operator +=                                             */
/*      Ajoute un caract�re � l'entr�e courante dans la liste de redo.    */
/*      Si ce caract�res est '\r' (carriage return), la ligne courante    */
/*      est consid�r�e comme termin�e.                                    */
/*                                                                        */
/**************************************************************************/

TRedoList &   TRedoList :: operator +=  ( char *  s )
   {
	register int		index ;


	if  ( strlen ( Lines [ ( Origin + LineCount ) % MaxLines ] ) )
	   {
		if  ( LineCount + 1  >=  MaxLines )
			Origin = ( Origin + 1 ) % MaxLines ;
		else
			LineCount ++ ;
	    }

	index = ( Origin + LineCount ) % MaxLines ;
	strncpy ( Lines [ index ], s, LINE_SIZE - 1 ) ;
	Lines [ index ] [ LINE_SIZE - 1 ] = 0 ;

	if  ( LineCount + 1  >=  MaxLines )
		Origin = ( Origin + 1 ) % MaxLines ;
	else
		LineCount ++ ;

	return ( * this ) ;
     }


TRedoList &   TRedoList :: operator +=  ( char  c )
   {
	register int		index ;


// Eliminer les caract�res de contr�le
	if  ( c > 0  &&  c < ' '  &&  c  !=  '\t'  &&  c !=  '\r'  &&  c !=  8 )
		return ( * this ) ;

// Remplacer les tabulations par des espaces
	if  ( c == '\t' )
		c = ' ' ;

// Un caract�re de plus...
	index = ( Origin + LineCount ) % MaxLines ;

	if  ( c  !=  '\r' )
	   {
		register int	length = strlen ( Lines [ index ] ) ;

	// Ligne trop longue
		if  ( c  ==  8  &&  length )
			length -- ;
		else if  ( length + 2  >=  LINE_SIZE )
			return ( * this ) ;
		else
			Lines [ index ] [ length ++ ] = c ;

		Lines [ index ] [ length ]    = 0 ;
	     }
	else if  ( strlen ( Lines [ index ] )  >=  MinRedoSize )
	    {
		if  ( LineCount + 1  >=  MaxLines )
			Origin = ( Origin + 1 ) % MaxLines ;
		else
			LineCount ++ ;

		Lines [ ( Origin + LineCount ) % MaxLines ] [0] = 0 ;
	      }
	else
		Lines [ index ] [0] = 0 ;


	return ( * this ) ;
    }
    


/**************************************************************************/
/*                                                                        */
/*   TRedoList &  TRedoList operator  []                                  */
/*      Retourne un pointeur sur la commande dont l'indice est sp�cifi�.  */
/*                                                                        */
/**************************************************************************/

char *	TRedoList :: operator []  ( int  index )
   {
	if  ( index >= 0  &&  index < MaxLines )
	   {
		index = ( Origin + index ) % MaxLines ;
		return ( Lines [index] ) ;
	    }

	return ( "Au secours !" ) ;
     }
