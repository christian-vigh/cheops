/**************************************************************************/
/*                                                                        */
/*  Huge.cpp  -  Christian Vigh, 94/09/22.                                */
/*                                                                        */
/*  Implémentation des classes THugeEntry et THugeTable.                  */
/*                                                                        */
/**************************************************************************/


# include	"AllTerm.h"
# pragma	hdrstop





/**************************************************************************/
/*                                                                        */
/*   THugeEntry :: THugeEntry                                             */
/*      Initialise l'entrée avec une chaîne de caractère. La chaîne est   */
/*      copie dans la mémoire globale.                                    */
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
/*      Libère le bloc de mémoire globale.                                */
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
/*      Retourne un pointeur sur la chaîne de caractères allouée dans le  */
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
/*      Déverrouille le bloc.                                             */
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
/*      Copie dans 'buffer' les 'count' prochains caractères du bloc.     */
/*      Retourne le nombre de caractères effectivement copiés.            */
/*                                                                        */
/**************************************************************************/

int	THugeEntry :: Copy  ( char *  buffer, int  count )
   {
	register long int 	size  =  count ;


// On a tout copié ou il y a eu un problème
	if  ( NextByte  >=  DataSize  || Handle  ==  NULL )
		return ( 0 ) ;

// Ne pas copier au-delà des données
	if  ( NextByte + size  >=  DataSize )
		size = DataSize - NextByte ;

// Verrouiller le bloc, effectuer la copie, et le déverrouiller
	Locked ( ) ;

	if  ( LockedPointer  ==  NULL )
		return ( 0 ) ;

	memmove ( buffer, LockedPointer + ( size_t ) NextByte, ( size_t ) size ) ;
	Unlock ( ) ;

// Mettre à jour le pointeur sur le prochain caractère.
	NextByte += size ;

	return ( ( int ) size ) ;
    }






/**************************************************************************/
/*                                                                        */
/*   THugeTable :: Reset                                                  */
/*      Remet à zero toutes les entrées de la table.                      */
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
/*      Ajoute un bloc à la table.                                        */
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
/*      Copie, depuis le bloc courant, 'count' caractères dans 'buffer'.  */
/*      Si tous les caractères du bloc courant ont été copiés, le bloc    */
/*      est supprimé et le suivant devient le bloc courant.               */
/*      Retourne le nombre de caractères effectivement copiés.            */
/*      Ne gère pas les chevauchements.                                   */
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






