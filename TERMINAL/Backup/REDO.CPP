/**************************************************************************/
/*                                                                        */
/*  Redo.cpp  -  Christian Vigh, 94/09/25.                                */
/*                                                                        */
/*  Implémentation de la classe TRedoList.                                */
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
/*      Désalloue la mémoire allouée pour les lignes de commande.         */
/*                                                                        */
/**************************************************************************/

TRedoList :: ~TRedoList  ( )
   {
     }





/**************************************************************************/
/*                                                                        */
/*   TRedoList :: operator +=                                             */
/*      Ajoute un caractère à l'entrée courante dans la liste de redo.    */
/*      Si ce caractères est '\r' (carriage return), la ligne courante    */
/*      est considérée comme terminée.                                    */
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


// Eliminer les caractères de contrôle
	if  ( c > 0  &&  c < ' '  &&  c  !=  '\t'  &&  c !=  '\r'  &&  c !=  8 )
		return ( * this ) ;

// Remplacer les tabulations par des espaces
	if  ( c == '\t' )
		c = ' ' ;

// Un caractère de plus...
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
/*      Retourne un pointeur sur la commande dont l'indice est spécifié.  */
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
