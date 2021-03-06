/**************************************************************************/
/*                                                                        */
/*  Huge.cpp  -  Christian Vigh, 94/09/22.                                */
/*                                                                        */
/*  Impl�mentation des classes THugeEntry et THugeTable.                  */
/*                                                                        */
/**************************************************************************/


# include	"AllTerm.h"
# pragma	hdrstop





/**************************************************************************/
/*                                                                        */
/*   THugeEntry :: THugeEntry                                             */
/*      Initialise l'entr�e avec une cha�ne de caract�re. La cha�ne est   */
/*      copie dans la m�moire globale.                                    */
/*                                                                        */
/**************************************************************************/

THugeEntry :: THugeEntry  ( char far *  str, long int  buffer_size )
   {
	LockedPointer	=  NULL ;
	NextByte	=  0 ;
	Handle		=  GlobalAlloc ( GMEM_MOVEABLE, buffer_size ) ;
	DataSize	=  buffer_size ;


	if  ( Locked ( ) !=  NULL )
	   {
		memmove ( LockedPointer, str, ( size_t ) buffer_size ) ;
		Unlock ( ) ;
	    }
    }




/**************************************************************************/
/*                                                                        */
/*   THugeEntry :: ~THugeEntry                                            */
/*      Lib�re le bloc de m�moire globale.                                */
/*                                                                        */
/**************************************************************************/

THugeEntry :: ~THugeEntry  ( )
   {
	if  ( LockedPointer  !=  NULL )
		Unlock ( ) ;

	if  ( Handle  !=  NULL )
		GlobalFree ( Handle ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  THugeEntry :: Lock                                                    */
/*      Retourne un pointeur sur la cha�ne de caract�res allou�e dans le  */
/*      bloc global.                                                      */
/*                                                                        */
/**************************************************************************/

char far *	THugeEntry :: Locked  ( void )
   {
	if  ( Handle )
	   {
		if  ( LockedPointer  ==  NULL )
			LockedPointer = ( char * ) GlobalLock ( Handle ) ;
	    }
	else
		LockedPointer = NULL ;

	return ( LockedPointer ) ;
    }





/**************************************************************************/
/*                                                                        */
/*   THugeEntry :: Unlock                                                 */
/*      D�verrouille le bloc.                                             */
/*                                                                        */
/**************************************************************************/

void	THugeEntry :: Unlock  ( void )
   {
	if  ( Handle  !=  NULL  &&  LockedPointer  !=  NULL )
	   {
		GlobalUnlock ( Handle ) ;
		LockedPointer = NULL ;
	    }
    }




/**************************************************************************/
/*                                                                        */
/*   THugeEntry :: Copy                                                   */
/*      Copie dans 'buffer' les 'count' prochains caract�res du bloc.     */
/*      Retourne le nombre de caract�res effectivement copi�s.            */
/*                                                                        */
/**************************************************************************/

int	THugeEntry :: Copy  ( char *  buffer, int  count )
   {
	register long int 	size  =  count ;


// On a tout copi� ou il y a eu un probl�me
	if  ( NextByte  >=  DataSize  || Handle  ==  NULL )
		return ( 0 ) ;

// Ne pas copier au-del� des donn�es
	if  ( NextByte + size  >=  DataSize )
		size = DataSize - NextByte ;

// Verrouiller le bloc, effectuer la copie, et le d�verrouiller
	Locked ( ) ;

	if  ( LockedPointer  ==  NULL )
		return ( 0 ) ;

	memmove ( buffer, LockedPointer + ( size_t ) NextByte, ( size_t ) size ) ;
	Unlock ( ) ;

// Mettre � jour le pointeur sur le prochain caract�re.
	NextByte += size ;

	return ( ( int ) size ) ;
    }






/**************************************************************************/
/*                                                                        */
/*   THugeTable :: Reset                                                  */
/*      Remet � zero toutes les entr�es de la table.                      */
/*                                                                        */
/**************************************************************************/

void	THugeTable :: Reset  ( void )
   {
	THugeEntry *	q ;


	for  ( THugeEntry * p	=  Head ; p != NULL ;  )
	    {
		q = p -> Next ;
		delete  p ;
		p = q ;
	     }

	Head = Tail = NULL ;
     }





/**************************************************************************/
/*                                                                        */
/*   THugeTable :: Add                                                    */
/*      Ajoute un bloc � la table.                                        */
/*                                                                        */
/**************************************************************************/

void   THugeTable :: Add  ( char *  buffer, long int  size )
   {
	if  ( Head  ==  NULL )
	   {
		Head = Tail  = new  THugeEntry ( buffer, size ) ;
		Head -> Next = NULL ;
	    }
	else
	   {
		Tail -> Next = new  THugeEntry ( buffer, size ) ;
		Tail         = Tail -> Next ;
		Tail -> Next = NULL ;
	    }
     }





/**************************************************************************/
/*                                                                        */
/*  THugeTable :: Copy                                                    */
/*      Copie, depuis le bloc courant, 'count' caract�res dans 'buffer'.  */
/*      Si tous les caract�res du bloc courant ont �t� copi�s, le bloc    */
/*      est supprim� et le suivant devient le bloc courant.               */
/*      Retourne le nombre de caract�res effectivement copi�s.            */
/*      Ne g�re pas les chevauchements.                                   */
/*                                                                        */
/**************************************************************************/

int	THugeTable :: Copy  ( char *  buffer, int  size )
    {
	register int		copied ;
	register THugeEntry *	p	=  Head ;


	while  ( p != NULL )
	   {
		copied = p -> Copy ( buffer, size ) ;

		if  ( copied )
			return ( copied ) ;

		p = p -> Next ;
		delete  Head ;
		Head = p ;
	    }

	return ( 0 ) ;
     }






