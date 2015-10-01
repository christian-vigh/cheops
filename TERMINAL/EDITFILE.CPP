/**************************************************************************/
/*                                                                        */
/*  EditFile.cpp  -  Patrick Sauvé, mai-juin 95.                          */
/*                                                                        */
/*  Sources des classes définies dans editfile.h                          */
/*  Décompression de donbnées et visualisation dans une fenêtre.          */
/*                                                                        */
/*  Modifications :                                                       */
/*     27.09.95 (Christian Vigh)                                          */
/*        . Changement de la classe de base de TEditWindow, de TWindow en */
/*          TDecoratedFrame.                                              */
/*        . Découpage de TEditWindow en plusieurs classes :               */
/*              . TEditWindowClient : fenêtre cliente                     */
/*              . TDecompressor     : processus de décompression          */
/*              . TTextFile         : gestion des lignes du fichier texte */
/*                                                                        */
/*     14.09.98 (Christian Vigh)                                          */
/*        . Déplacement de l'appel à ApplicationTable -> RemoveObject     */
/*          du destructeur vers la méthode EvClose de l'objet TEditFile.  */
/*          Dans certains cas, cet appel provoquait une GPF.              */
/*                                                                        */
/**************************************************************************/



# include	"AllTerm.h"
# pragma	hdrstop

# include	<regexp.h>

# include	"UnixApp.h"
# include	"TextFile.h"
# include	"Print.h"
# include	"ChopCfg.h"
# include	"ChopJet.h"
# include	"MNCommon.h"
# include	"ChopTree.h"
# include	"ChopMenu.h"
# include	"ChopUp.h" 
# include	"EditFile.h"




# define	NextTab(pos)		( (pos) + 8 - ( (pos) % 8 ) )



/***
	ATTENTION  !!!!!!!
	La classe TEditWindowClient (a) se partage maintenant le caret avec la
	TTerminalWindowClient (b). Bien qu'en temps normal, avec gestion de la
	création et destruction du caret sur réception des messages KILLFOCUS
	et SETFOCUS, tout fonctionne bien, il existe deux cas où ça ne va pas:

	. Lorsque la fenêtre (a) est créee, la fenêtre (b) reçoit apparemment 
	  un WM_PAINT avant un KILLFOCUS. Or (a) a auparavant reçu le SETFOCUS.
	  (a) crée donc le caret, puis (b) se repeint et remet à jour la 
	  position du caret... dans (a) !

	. Si (b) se trouve en dessous de (a) et qu'on l'active en cliquant sur
	  sa barre de titre et en la déplaçant, c'est (a) qui met alors à jour 
	  la position du caret dans (b)... qui ne reçoit (apparemment aussi)
	  le killfocus qu'en dernier.

	MORALITE : dans (a) comme dans (b), et en dehors du traitement des
	messages SetFocus et KillFocus, il est IMPERATIF de vérifier si on
	possède le focus avant de toucher au caret.

	Concrètement, si une fonction de redessin de la fenêtre doit toucher 
	au caret, elle doit inclure un test de ce genre :

	if  ( :: GetFocus ( )  ==  HWindow ) 
		// alors on peut modifier la position du caret
 ***/







# if	COMPRESS_STATISTICS  &&  ! STANDALONE

/**************************************************************************/
/**************************************************************************/
/********                                                          ********/
/********    Implémentation de la classe TDictionaryStatistics     ********/
/********                                                          ********/
/**************************************************************************/
/**************************************************************************/

DEFINE_RESPONSE_TABLE1 ( TDictionaryStatistics, TDialog )
	EV_COMMAND  ( IDC_NEXT_BUTTON	 , CmNext     ),
	EV_COMMAND  ( IDC_PREVIOUS_BUTTON, CmPrevious ),
	EV_COMMAND  ( IDC_FIRST_BUTTON   , CmFirst    ),
	EV_COMMAND  ( IDC_LAST_BUTTON    , CmLast     ),
END_RESPONSE_TABLE ;




/**************************************************************************/
/*                                                                        */
/*  Constructeur et destructeur.                                          */
/*                                                                        */
/**************************************************************************/



TDictionaryStatistics :: TDictionaryStatistics  ( )
	: TDialog ( 0, IDD_DICTIONARY_STATISTICS ) 
   {
	Statistics 		=  0 ;
	StatisticCount  	=  0 ;
	CurrentStatistics       =  -1 ;
	BlockCount 		=  0 ;
    }


TDictionaryStatistics :: ~TDictionaryStatistics ( )
   {
	if  ( Statistics )
		delete []  Statistics ;
    }




/**************************************************************************/
/*                                                                        */
/*   TDictionaryStatistics :: Execute                                     */
/*      Calcule le total de toutes les statistiques.                      */
/*      N'appelle la boîte de dialogue que s'il y a eu des statistiques.  */
/*                                                                        */
/**************************************************************************/

int	TDictionaryStatistics :: Execute  ( void )
   {
	if  ( ! StatisticCount )
	   {
		:: MessageBox ( HWindow, "Statistiques du dictionnaire", 
			MB_OK | MB_ICONSTOP, "Aucune statistique n'a été collectée." ) ;
		return  ( IDOK ) ;
	     }


	register TStatistics *	p  =  Statistics ;


	for  ( register int  i = 0 ; i < StatisticCount ; i ++, p ++ )
	   {
		Global. HashTableHits		+=  p -> HashTableHits ;
		Global. LinkedListHits		+=  p -> LinkedListHits ;
		Global. LinkedListHitsSearches	+=  p -> LinkedListHitsSearches ;
		Global. LinkedListAdds		+=  p -> LinkedListAdds ;
		Global. LinkedListAddsSearches	+=  p -> LinkedListAddsSearches ;

		if  ( p -> DictionarySize  >  Global. DictionarySize )
			Global. DictionarySize = p -> DictionarySize ;

		if  ( p -> MaxCodeSize  >  Global. MaxCodeSize ) 
			Global. MaxCodeSize    =  p -> MaxCodeSize ;
	      }

	return  ( TDialog :: Execute ( ) ) ;
     }





/**************************************************************************/
/*                                                                        */
/*   TDictionaryStatistics :: NewDictionary -                             */
/*      Alloue une nouvelle entrée de statistiques.                       */
/*                                                                        */
/**************************************************************************/

void	TDictionaryStatistics :: NewDictionary ( void )
   {
	if  ( ! StatisticCount )
		Statistics = new  TStatistics [1] ;
	else
	   {
		register   TStatistics *	s = new  TStatistics [ StatisticCount + 1 ] ;


		for  ( int  i = 0 ; i < StatisticCount ; i ++ )
			s [i] = Statistics [i] ;

		delete []  Statistics ;
		Statistics = s ;
	     }

	StatisticCount ++ ;
     }




/**************************************************************************/
/*                                                                        */
/*   TDictionaryStatistics :: operator []                                 */
/*      Retourne une référence aux statistiques dont l'index est donné.   */
/*                                                                        */
/**************************************************************************/

TDictionaryStatistics :: TStatistics & 
		 TDictionaryStatistics :: operator []  ( int  index )
   {
	if  ( index  ==  -1 )	
		return  ( Statistics [ StatisticCount - 1 ] ) ;
	else
		return  ( Statistics [ index ] ) ;
     }





/**************************************************************************/
/*                                                                        */
/*  TDictionaryStatistics :: CmShow -                                     */
/*      Affiche les statisiques pour le dictionnaire spécifié.            */
/*                                                                        */
/**************************************************************************/

# define	SETTEXT(id,fmt,field)    		   	\
			wsprintf ( Buffer, fmt, field ) ;	\
			SetDlgItemText ( id, Buffer ) 



void	TDictionaryStatistics :: CmShow  ( int  index )
   {
	char			Buffer [ 128 ] ;
	TStatistics &		Stats =  ( index  ==  -1 ) ?
						Global : Statistics [index] ;
					


	:: LockWindowUpdate  ( HWindow ) ;

	if  ( index  ==  -1 )
	   {
		SetCaption ( "Dictionnaire de décompression - Statistiques globales" ) ;
	
		:: ShowWindow ( GetDlgItem ( IDC_GLOBAL_GROUPBOX ), SW_SHOW ) ;
		:: ShowWindow ( GetDlgItem ( IDC_DICTIONARY_COUNT ), SW_SHOW ) ;
		:: ShowWindow ( GetDlgItem ( IDC_DICTIONARY_COUNT_TEXT ), SW_SHOW ) ;

		:: ShowWindow ( GetDlgItem ( IDC_BLOCK_COUNT ), SW_SHOW ) ;
		:: ShowWindow ( GetDlgItem ( IDC_BLOCK_COUNT_TEXT ), SW_SHOW ) ;

		SETTEXT ( IDC_BLOCK_COUNT, "%u", BlockCount ) ;				      
		SETTEXT ( IDC_DICTIONARY_COUNT, "%u", StatisticCount ) ;
	    }
	else
	   {
		wsprintf ( Buffer, "Statistiques - Dictionnaire de décompression No %d",
			index + 1 ) ;
		SetCaption ( Buffer ) ;

		:: ShowWindow ( GetDlgItem ( IDC_GLOBAL_GROUPBOX ), SW_HIDE ) ;
		:: ShowWindow ( GetDlgItem ( IDC_DICTIONARY_COUNT ), SW_HIDE ) ;
		:: ShowWindow ( GetDlgItem ( IDC_DICTIONARY_COUNT_TEXT ), SW_HIDE ) ;
		:: ShowWindow ( GetDlgItem ( IDC_BLOCK_COUNT ), SW_HIDE ) ;
		:: ShowWindow ( GetDlgItem ( IDC_BLOCK_COUNT_TEXT ), SW_HIDE ) ;
	     }                                        


	SETTEXT ( IDC_HT_HITS	      , "%lu", Stats.   HashTableHits ) ;
	SETTEXT ( IDC_LL_HITS         , "%lu", Stats.   LinkedListHits ) ;
	SETTEXT ( IDC_LL_HITS_SEARCHES, "%lu", Stats.   LinkedListHitsSearches ) ;
	SETTEXT ( IDC_LL_ADDS         , "%lu", Stats.   LinkedListAdds ) ;
	SETTEXT ( IDC_LL_ADDS_SEARCHES, "%lu", Stats.   LinkedListAddsSearches ) ;
	SETTEXT ( IDC_MAX_CODE_SIZE   ,  "%u", Stats.   MaxCodeSize ) ;
	SETTEXT ( IDC_DICTIONARY_SIZE , "%lu", Stats.   DictionarySize ) ;						       

	:: LockWindowUpdate ( NULL ) ;

	CurrentStatistics = index ;
   }



# endif  	/*  COMPRESS_STATISTICS  */





/**************************************************************************/
/**************************************************************************/
/********                                                          ********/
/********          Implémentation de la classe TDecompressor       ********/
/********                                                          ********/
/**************************************************************************/
/**************************************************************************/

# define	DATA_BUFFER_SIZE		16384
# define	DICTIONARY_BUFFER_SIZE		8192


/***
	Le dictionnaire est sstocké sous forme de hash table.
	HASH_TABLE_SIZE représente la taille initiale de cette table.
	La taille initiale d'un code étant de 9 bits, celle de la table
	sera suffisante pour stocker 2^13 valeurs (4096) sans nécéssiter 
	d'allocation.
 ***/

# define	HASH_TABLE_SIZE			4097




/**************************************************************************/
/*                                                                        */
/*    TUncompressor :: CheckError - -                                     */
/*      Suppression du fichier dictionnaire en cas d'erreur de            */
/*      décompression.                                                    */
/*                                                                        */
/**************************************************************************/


void  TUncompressor :: CheckError ( int  erreur )
   {
	if  ( ( erreur  !=  ERR_RECEIVED )  &&  ( erreur  !=  ERR_CLOSED ) )
	   {
		if  ( DictionaryFileFP )
		   {
			fclose ( DictionaryFileFP ) ;
			remove ( DictionaryFileName ) ;
			DictionaryFileFP = 0 ;
		    }
	    }
    }




/**************************************************************************/
/*                                                                        */
/*  TUncompressor :: Initialize -                                         */
/*      Crée les fichiers dictionnaire & données, et alloue le début du   */
/*      dictionnaire.                                                     */
/*                                                                        */
/**************************************************************************/

int  TUncompressor :: Initialize   ( TUnixMessage *	Msg,
				     char **		pData,
				     int *		size )
   {
	UTEditFile *	UData = ( UTEditFile * ) Msg -> Data ;


// le traitement est different si l'on recoit le premier bloc ou
// si on a à faire à un bloc suivant
	if  ( UData -> nbBloc  ==  1 )      /*= Premier bloc =*/
	   {
	// on recupere le nom de la fenetre en tete des données compressees
		* size = Msg -> DataSize +1 - sizeof ( UTEditFile ) ;
		memset ( Title, 0, sizeof ( Title ) ) ;
		strncpy ( Title, UData -> data, sizeof ( Title ) - 1 ) ;
		int longueur = strlen ( UData -> data ) +1 ;
		* size -= longueur ;
		* pData = UData -> data + longueur ;
		Device -> HostToLocalCharSet ( Title ) ;
		Beautify ( Title ) ;

		return  OK ;
	    }
	else 		/*=  Blocs suivants  =*/
	   {
	// on verifie que le bloc correspond bien
		if  ( UData -> nbBloc  ==  BlockCount + 1 )
		   {
			* pData = UData -> data ;
			* size = Msg -> DataSize - sizeof ( UTEditFile ) + 1 ;
			return  OK ;
		    }
		else  if  ( ( UData -> nbBloc )  ==  BlockCount )
			return  ERR_RECEIVED ;
		else
			return ERR_BLOCK ;
	    }
    }




/**************************************************************************/
/*                                                                        */
/*  TUncompressor :: InitializeDictionary -                               */
/*      Initialise les variables pour le démarrage de la décompression.   */
/*                                                                        */
/**************************************************************************/

void  TUncompressor :: InitializeDictionary ( void )
   {
	NextCode = FIRST_CODE ;
	CurrentCodeBits = 9 ;
    }



/**************************************************************************/
/*                                                                        */
/*  TUncompressor :: AllocateDictionary -                                 */
/*      Initialise le dictionnaire.                                       */
/*                                                                        */
/**************************************************************************/

int  TUncompressor :: AllocateDictionary ( void ) 
   {
	if  ( ! DictionaryResident )
	   {
		Dictionary = new  TDictionary [ HASH_TABLE_SIZE ] ;
		DictionaryResident = TRUE ;
	     }

	return  OK ;
    }




/**************************************************************************/
/*                                                                        */
/*  TUncompressor :: FreeDictionary -                                     */
/*      Libère la mémoire allouée pour le dictionnaire.                   */
/*                                                                        */
/**************************************************************************/

void  TUncompressor :: FreeDictionary ( BOOL  reset_only )
   {
	if  ( DictionaryResident )
	   {
		for  ( int  i = 0; i < HASH_TABLE_SIZE ; i ++ ) 
		   {
			register TDictionary *		p  =  Dictionary [i]. next,
					     *		q ;


			while  ( p )
			   {
				q = p -> next ;
				delete  p ;
				p = q ;
			    }
		      }


		if  ( reset_only )
		   {
			for  ( i = 0 ; i < HASH_TABLE_SIZE ; i ++ )
			    {
				Dictionary [i]. index = -1 ;
				Dictionary [i]. next  = 0 ;
			     }
		     }
		else
		   {
			delete  []  Dictionary ;
			DictionaryResident = FALSE ;
		    }
	    }
    }






/**************************************************************************/
/*                                                                        */
/*  TUncompressor :: Uncompress -                                         */
/*      Décompresse un bloc  de données.                                  */
/*                                                                        */
/**************************************************************************/

int  TUncompressor :: Uncompress ( char *  data, int  size )
   {
	int 			Character ;
	register BOOL		FirstTime 	=  ! BlockCount ;

// on initialise la structure pour lire correctement les bits en entrée
	BitInput. rack = 0 ;
	BitInput. mask = 0x80 ;
	BitInput. data = data ;
	BitInput. size = size ;

// on alloue la mémoire pour le dictionnaire
	if  ( AllocateDictionary ( ) ) 
		return  ( ERR_ALLOC ) ;

// on se repositionne à la fin du fichier de données
	Seek ( 0L , SEEK_END ) ;

// si c'est la suite d'un fichier on charge l'ancien dictionnaire
	if  ( BlockCount )
	   {
		if  ( ! DictionaryResident  &&  LoadDictionary ( DictionaryFileFP ) )
			return  ( ERR_LOAD ) ;
	    }
	else		// sinon on initialise normalement
	   {
		InitializeDictionary ( ) ;
	   }


// Compter un bloc de plus
	if  ( ! SpanBlocks )
		BlockCount ++ ;

# if	COMPRESS_STATISTICS   &&  ! STANDALONE
	if  ( FirstTime )
		DictionaryStatistics. NewDictionary ( ) ;

	DictionaryStatistics. BlockCount ++ ;
	DictionaryStatistics [-1]. MaxCodeSize = CurrentCodeBits ;
# endif


// Allouer la pile de décodage
	if  ( ! DecodeStack )
	   {
		DecodeStack = new  char [ TABLE_SIZE ] ;

		if  ( ! DecodeStack )
			return  ( ERR_ALLOC ) ;
	     }

	DecodeStackIndex = TABLE_SIZE - 1 ;


	if  ( FirstTime  ||  ! SpanBlocks )
	   {
		ReturnValue = 0 ;
		Mask        = 1L  <<  ( CurrentCodeBits - 1 ) ;
	    }



//  Boucle de décodage
	while  ( 1 )
	   {
	// Lecture des chaînes de bits suivantes
		if  ( InputBits ( & BitInput, & NewCode )  ==  ERR_UNCOMPRESS )
		   {
			if  ( SpanBlocks )
			   {
				BlockCount ++ ;
				return  ( OK ) ;
			    }
			else
				return  ( ERR_UNCOMPRESS ) ;
		     }
		else
		    {
			ReturnValue = 0 ;
			Mask        = 1L  <<  ( CurrentCodeBits - 1 ) ;
		     }


		switch  ( NewCode )
		   {
		// Fin de fichier atteinte
			case	END_OF_STREAM :
				FreeDictionary ( ) ;
				Complete = TRUE ;
				Flush ( ) ;
				CheckFF ( ) ;
				return  ( ERR_EOF ) ;


		// Fin de bloc
			case	FIN_BLOC :
				Flush ( ) ;

				if  ( ! SpanBlocks )
					return  ( OK ) ;
				else
				   {
					BitInput. mask = 0x80 ;
					continue ;
				    }



		// Recréer le dictionnaire
			case	FLUSH_CODE :
# if	COMPRESS_STATISTICS  &&  ! STANDALONE
				DictionaryStatistics. NewDictionary ( ) ;
# endif
				InitializeDictionary ( ) ;
				FreeDictionary ( TRUE ) ;
				FirstTime = TRUE ;
				Mask        = 1L  <<  ( CurrentCodeBits - 1 ) ;
				ReturnValue = 0 ;
				continue ;       


		// Changement du nombre de bits pour le codage
			case	BUMP_CODE :
# if	COMPRESS_STATISTICS  &&  ! STANDALONE
				DictionaryStatistics [-1]. MaxCodeSize ++ ;
# endif
				CurrentCodeBits ++ ;
				Mask        = 1L  <<  ( CurrentCodeBits - 1 ) ;
				ReturnValue = 0 ;
				continue ;
		    }


	// Dans le cas du premier bloc, il faut juste sauvegarder le code
	// et écrire le premier caractère
		if  ( FirstTime )
		   {
			char  Buffer [1] ;


			Character = OldCode = NewCode ;
			* Buffer = ( char ) OldCode ;
	
			if  ( Write ( Buffer, 1 )  !=  1 )
				return  ( ERR_WRITE ) ;
			
			FirstTime = FALSE ;
			continue ;
		     }
			
			

	// Décodage d'une chaîne
		DecodeStackIndex = TABLE_SIZE - 1 ;

		if  ( NewCode  >=  NextCode )
		   {
			DecodeStack [ DecodeStackIndex -- ] = Character ;
			DecodeString ( OldCode ) ;
		    }
		else
			DecodeString ( NewCode ) ;
	      

	// Ecriture
		Character = DecodeStack [ DecodeStackIndex + 1 ] ;

		Write ( DecodeStack + DecodeStackIndex + 1,
			TABLE_SIZE - 1 - DecodeStackIndex ) ;


	// Ajout au dictionnaire
		TDictionary &	Entry = DICT ( NextCode ) ;


		Entry. parent_code = OldCode ;
		Entry. character   = ( char ) Character ;
		NextCode ++ ;
		OldCode = NewCode ;

# if	COMPRESS_STATISTICS  &&  ! STANDALONE
		DictionaryStatistics [-1]. DictionarySize ++ ;
# endif

	    }
    }





/**************************************************************************/
/*                                                                        */
/*  TUncompressor :: DecodeString -                                       */
/*      Reconstitue une chaîne à partir du code spécifié.                 */
/*                                                                        */
/**************************************************************************/


void   TUncompressor :: DecodeString ( unsigned int 	code )
   {
	while  ( code > 255 )
	   {
		TDictionary &	Entry =  DICT ( code ) ;


		DecodeStack [ DecodeStackIndex -- ] = Entry. character ;
		code = Entry. parent_code ;
	    }

	DecodeStack [ DecodeStackIndex -- ] = code ;
    }





/**************************************************************************/
/*                                                                        */
/*  TUncompressor :: InputBits -                                          */
/*      Lecture des données bit par bit.                                  */
/*                                                                        */
/**************************************************************************/

int  TUncompressor :: InputBits ( BIT_INPUT *		bit_file,
				  unsigned int *	val_ret )
   {
	unsigned char			bf_mask		=  bit_file -> mask ;
	register int			bf_size		=  bit_file -> size,
					bf_rack		=  bit_file -> rack ;
	register char *			bf_data		=  bit_file -> data ;
	int				status		=  OK ;
	
	


	while  ( Mask )
	   {
		if  ( bf_mask  ==  0x80 )
		   {
			if  ( bf_size -- )
				bf_rack = * bf_data ++ ;
			else  
			   {
				status = ERR_UNCOMPRESS ;
				goto  Return ;
			    }
		    }


		if  ( bf_rack & bf_mask )
			ReturnValue |= Mask ;

		Mask >>= 1 ;
		bf_mask >>= 1 ;

		if  ( ! bf_mask )
			bf_mask = 0x80 ;
	    }

Return :
	bit_file -> rack	=  bf_rack ;
	bit_file -> mask	=  bf_mask ;
	bit_file -> data	=  bf_data ;
	bit_file -> size	=  bf_size ;
	* val_ret = ( unsigned int ) ReturnValue ;
	return  ( status ) ;
    }





/**************************************************************************/
/*                                                                        */
/*   TUncompressor :: SaveDictionary -                                    */
/*      Sauve  le dictionnaire à partir du disque.                        */
/*      Les éléments liés par liste chaînée à un élément principal de la  */
/*      table sont sauvegardés à la suite de ce dernier.                  */
/*                                                                        */
/**************************************************************************/

int  TUncompressor :: SaveDictionary  ( FILE *  fp )
   {
	register TDictionary *		dict ;
	register int			i ;


	rewind ( fp ) ;


	for  ( i = 0, dict = Dictionary ; i  <  HASH_TABLE_SIZE ; i ++ )
	   {
		register  TDictionary *		p  =  dict -> next,
				      *		q ;


		if  ( fwrite ( dict, sizeof ( TDictionary ), 1, fp )  !=  1 )
			return  ( ERR_SAVE ) ;

		while  ( p )
		   {
			q = p -> next ;

			if  ( fwrite ( p, sizeof ( TDictionary ), 1, fp )  !=  1 )
				return  ( ERR_SAVE ) ;

			p = q ;
		    }
	     }


	FreeDictionary ( ) ;
	return  ( OK ) ;
    }






/**************************************************************************/
/*                                                                        */
/*  TUncompressor :: LoadDictionary -                                     */
/*      Recharge un dictionnaire à partir du disque.                      */
/*                                                                        */
/**************************************************************************/

int   TUncompressor :: LoadDictionary  ( FILE *  fp )
   {
	register TDictionary *		dict ;
	register int			i ;
	TDictionary			entry ;


	rewind ( fp ) ;
	AllocateDictionary ( ) ;


	for  ( i = 0, dict = Dictionary ; i  <  HASH_TABLE_SIZE ; i ++ )
	   {
		if  ( fread ( dict, sizeof ( TDictionary ), 1, fp )  !=  1 )
			return  ( ERR_LOAD ) ;


		register TDictionary *		p	=  dict ;


		while  ( p -> next )
		   {
			if  ( fread ( & entry, sizeof ( TDictionary ), 1, fp )  !=  1 )
				return  ( ERR_LOAD ) ;

			p -> next = new  TDictionary ;
			* ( p -> next )  =  entry ;
		     }
	     }

	return  ( OK ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  TUncompressor :: DICT -                                               */
/*      Retourne une référence à l'élément spécifié du dictionnaire, en   */
/*      le créant si nécessaire.                                          */
/*                                                                        */
/**************************************************************************/

TDictionary &  TUncompressor ::  DICT ( unsigned int  index )
   {
	register unsigned int	element  =  ( index % HASH_TABLE_SIZE ) ;
	TDictionary &		entry	 =  Dictionary [element] ;


// Cas simple : l'élément correspondant existe déjà dans la table principale
	if  ( entry. index  ==  index )
	   {
# if	COMPRESS_STATISTICS  &&  ! STANDALONE
		DictionaryStatistics [-1]. HashTableHits ++ ;
# endif
		return  ( entry ) ;
	    }

// Autre cas simple : l'élément correspondant n'est pas utilisé
	else if  ( entry. index  ==  0xFFFF )
	   {
		entry. index = index ;
		return  ( entry ) ;
	    }


// Si cet élément n'a pas de liste chaînée, on ne cherche pas plus loin et on
// ajoute l'élément
	if  ( ! entry. next )
	   {
# if	COMPRESS_STATISTICS   &&  ! STANDALONE
		DictionaryStatistics [-1]. LinkedListAdds ++ ;
# endif
		entry. next = new  TDictionary ;
		entry. next -> index = index ;
		return  ( * ( entry. next ) ) ;
	     }


// Autrement, il faut parcourir la liste chaînée à partir de l'élément trouvé
	register TDictionary *		p =  entry. next,
			     *		q =  Dictionary + element ;

# if	COMPRESS_STATISTICS  &&  ! STANDALONE
	register int	search_count	=  0 ;


	while  ( p )
	   {
		search_count ++ ;

		if  ( p -> index  ==  index )	// Trouvé dans la liste
		   {
			DictionaryStatistics [-1]. LinkedListHits ++ ;
			DictionaryStatistics [-1]. LinkedListHitsSearches += search_count ;
			return  ( * p ) ;
		     }
			
		q = p ;
		p = p -> next ;
	     }
# else 
	while  ( p )
	   {
		if  ( p -> index  ==  index )	// Trouvé dans la liste
			return  ( * p ) ;

		q = p ;
		p = p -> next ;
	     }
# endif


// La liste a été parcourue, mais aucun élément trouvbé : on l'ajoute à la fin

# if 	COMPRESS_STATISTICS   &&  ! STANDALONE
	DictionaryStatistics [-1]. LinkedListAdds ++ ;
	DictionaryStatistics [-1]. LinkedListAddsSearches += search_count ;
# endif

	q -> next = p = new TDictionary ;
	p -> index = index ;
	return ( * p ) ;
     }
		










/**************************************************************************/
/*                                                                        */
/*  Constructeur.                                                         */
/*  Lorsque le nom de fichier de données est la chaîne vide, aucun fichier*/
/*  n'est ouvert. Il est alors à la charge de la classe dérivée de        */
/*  surcharger la fonction Write afin d'écrire dans un fichier à elle.    */
/*                                                                        */
/**************************************************************************/

TUncompressor :: TUncompressor  ( char *  output, BOOL  keep_data )
   {
	DataFileFP     		= 0 ;
	DictionaryFileFP     	= 0 ;
	* DataFileName		= 0 ;
	* DictionaryFileName	= 0 ;
	Complete		= FALSE ;
	DataBuffer		= 0 ;
	DictionaryBuffer	= new  char [ DICTIONARY_BUFFER_SIZE ] ;
	DictionaryResident 	= FALSE ;
	BlockCount		= 0 ;
	DecodeStack		= 0 ;
	KeepDataFile		= keep_data ;
	SpanBlocks		= FALSE ;


	if  ( ! output )
		tmpnam ( DataFileName ) ;
	else 
		strcpy ( DataFileName, output ) ;

// on tente d'ouvrir ce fichier (si le nom est spécifié)
	if  ( * DataFileName )
	   {
		if  ( ( DataFileFP = fopen ( DataFileName, "w+b" ) )  ==  NULL )
			return ;

		DataBuffer = new  char [ DATA_BUFFER_SIZE ] ;
	     
		if  ( DataBuffer )
			setvbuf ( DataFileFP, DataBuffer, _IOFBF, DATA_BUFFER_SIZE ) ;
	    }

// on genere les noms de fichiers temporaires
	tmpnam ( DictionaryFileName ) ;

// on tente d'ouvrir ces fichiers
	if  ( ( DictionaryFileFP = fopen ( DictionaryFileName, "w+b" ) )  !=  NULL )
	   {
		if  ( DictionaryBuffer )
			setvbuf ( DictionaryFileFP, DictionaryBuffer, _IOFBF, 
				DICTIONARY_BUFFER_SIZE ) ;
	    }
    }



/**************************************************************************/
/*                                                                        */
/*  Destructeur.                                                          */
/*                                                                        */
/**************************************************************************/

TUncompressor :: ~TUncompressor ( )
   {
	if  ( DictionaryFileFP )
	   {
		fclose ( DictionaryFileFP ) ;
		remove ( DictionaryFileName ) ;
		DictionaryFileFP = 0 ;
	    }

	if  ( DataFileFP )
	   {
		fclose ( DataFileFP ) ;

		if  ( ! KeepDataFile )
			remove ( DataFileName ) ;
		DataFileFP = 0 ;
	    }

	if  ( DataBuffer )
		delete []  DataBuffer ;

	if  ( DictionaryBuffer )
		delete []  DictionaryBuffer ;

	if  ( DecodeStack )
		delete []  DecodeStack ;
      }

	




/**************************************************************************/
/**************************************************************************/
/********                                                          ********/
/********     Implémentation de la classe TFileUncompressor        ********/
/********                                                          ********/
/**************************************************************************/
/**************************************************************************/

int	TFileUncompressor :: Write  ( char *  data, int  size )
   {
	if  ( BytesWritten  !=  -1 )
	   {
		register int	count = size ;

	// Mode binaire : écrire simplement les données
		if  ( BinaryMode ) 
			count = write ( FileFd, data, size ) ;
	// Sinon...
		else
		   {
			static char	cr []  =  "\r\n" ;
			register int	status ;

 
		// Convertir dans le bon jeu de caractères
			Device -> HostToLocalCharSet ( data, size ) ;

		// Et remplacer chaque newline par carriage return/newline
			while  ( size -- )
			   {
				if  ( * data  ==  '\n' )
					status = write ( FileFd, cr, 2 ) ;
				else
					status = write ( FileFd, data, 1 ) ;

				if  ( status < 1 )
					count = -1 ;
				data ++ ;
			     }
		     }


		if  ( count  ==  -1 )
			BytesWritten = -1 ;
		else
			BytesWritten += count ;

		return  ( count ) ;
	    }
	else
		return  ( -1 ) ;
     }




/**************************************************************************/
/**************************************************************************/
/********                                                          ********/
/********     Implémentation de la classe TEditWindowPrintout      ********/
/********                                                          ********/
/**************************************************************************/
/**************************************************************************/


class	TEditWindowPrintout :  public  TPrintout
   {
	private :
		TEditWindow *		EditWindow ;		// Pointeurs sur l'objet
		TEditWindowClient *	EditWindowClient ;
		TCheopsTextFile *	TextFile ;

		BOOL			GotLastLine ;		// TRUE si la dernière ligne est lue
		long int		LastLine ;		// No de la dernière ligne lue
		int			LastPage ;		// Dernière page imprimée
		int			PageWidth,		// Taille de la page imprimante en mm
					PageHeight ;
		int			XMargin,		// Marges du texte
					YMargin ;
		TPoint			Size ;			// Taille de la page en caractères
		TFont *			Font ;			// Police en cours
		LOGFONT 		LogFont ;		// Infos sur la police en cours
		int			CharacterWidth,		// Taille d'un caractère
					CharacterHeight,
					ExternalLeading ;
		int			ListCPI,		// Densité actuelle
					ListLPI ;
		BOOL			Bold, 			// Attributs en cours
					Italic,
					Underline,
					IsSemiGraphic ;

		int			Orientation ;		// Gestion des documents
		BOOL			Reoriented ;
						  


	public :

	// Constructeur : calcule les paramètres d'édition et crée éventuellement
	// une nouvelle police en jeu de caractères OEM
		TEditWindowPrintout ( char *		title,
				      TEditWindow *	w,
				      int		orient )
			: TPrintout ( title ),
			Size ( 0, 0 ) 
		   {
			EditWindow 	 = w ;
			EditWindowClient = w -> EditWindowClient ;
			TextFile	 = w -> TextFile ;
			GotLastLine      = FALSE ;
			LastLine	 = 0 ;
			Bold		 = FALSE ;
			Italic		 = FALSE ;
			Underline	 = FALSE ;
			IsSemiGraphic 	 = FALSE ;
			ListCPI		 = LIST_10_CPI ;
			ListLPI		 = LIST_6_LPI ;
	

		// Gestion des orientations
			Orientation    = orient ;
			Reoriented     = FALSE ;
			LastPage       = 0 ;	

		// Jeu de caractères
			Font = 0 ;
			CharacterWidth  = EditWindowClient -> CharacterWidth ;
			CharacterHeight = EditWindowClient -> CharacterHeight ;
			ExternalLeading = EditWindowClient -> ExternalLeading ;


		// Paramètres de police
			EditWindowClient -> Font -> GetObject ( LogFont ) ;

		// Récupérer la résolution de l'imprimante
			PageWidth  =  0 ;
			PageHeight =  0 ;
			XMargin    =  CharacterWidth ;
			YMargin	   =  CharacterHeight + ExternalLeading ;

		    }


	// Destructeur : libère les ressources
		~TEditWindowPrintout  ( )
		   {
			if  ( Font )
				delete  Font ;
		    }



	// Indique la taille du document
		void	GetDialogInfo ( int &  minpage, int &  maxpage,
					int &  selfrom, int &  selto )
		   {
			minpage = LastPage + 1 ; 
			maxpage = ( int ) TextFile -> PageCount ;
			selfrom = 0 ;
			selto   = 0 ;
			Reoriented = FALSE ;
		    }


	// Retourne FALSE quand la dernière ligne a été lue
	// Modifie au passage l'orientation
		BOOL	HasPage  ( int  pagenum )
		   {
			register BOOL	Status ;


			if  ( CheckOrientationSwitch ( - pagenum ) )
			   {
				TPrintDialog :: TData &	Data = Printer -> GetSetup ( ) ;
				DEVMODE			Mode = * Data. GetDevMode ( ) ;


				Mode. dmOrientation  =  Orientation ;
				Mode. dmFields       =  DM_ORIENTATION ;
				Data. SetDevMode ( & Mode ) ;

				Reoriented = FALSE ;
			     }

			Status = ! GotLastLine  &&  ! Reoriented ;
			return ( Status ) ;
		    }



	// Retourne l'orientation en cours
		int	GetOrientation ( void )
		   {
			return ( Orientation ) ;
		    }

	// Fonctions d'impression
		void	Begin        	  ( void ) ;
		void    PrintPage  	  ( int  page, TRect &  rect, unsigned  flags ) ;
		void	End        	  ( void ) ;
		void	SwitchOrientation ( int  neworient ) ;
		BOOL	CheckOrientationSwitch  ( long int  line ) ;


	// Interprétation des séquences escape pour le mode semi-graphique Unix
		char *  ProcessEscape   ( char *  buffer ) ;

	// Change la police en fonction des attributs en cours
		void	CreateFont	( int	  cpi,
					  int	  lpi,
					  BOOL	  bold,
					  BOOL	  italic,
					  BOOL	  underline,
					  BOOL	  issemigraphic = FALSE ) ;

	// Change la taille des caractères en fonction de la densité en cours
		void	SetExt  	( int  	  width,
					  int	  height,
					  int	  linewidth,
					  int	  pageheight,
					  BOOL	  issemigraphic = FALSE ) ;


	// Réinitialise les variables pour l'éventuelle impression suivante
		void EndDocument ( void )
		   {
			Reoriented	=  FALSE ;
			GotLastLine	=  FALSE ;
		    }
       } ;






/**************************************************************************/
/*                                                                        */
/*   TEditWindowPrintout :: Begin -                                       */
/*      Récupère les paramètres sur la taille de la page,   redéfinit     */
/*      la taille des caractères et sélectionne la police en cours.       */
/*      Tout ce bigntz doit être effectué à chaque appel de PrintPage     */
/*      par l'objet TPrinter. Fallait le savoir...                        */
/*                                                                        */
/**************************************************************************/

void	TEditWindowPrintout :: Begin  ( void )
   {
	GotLastLine = FALSE ;                 

// Résolution de l'imprimante
	if  ( Orientation  ==  DMORIENT_PORTRAIT )
	   {
		PageWidth  =  DC -> GetDeviceCaps ( HORZRES ) ;
		PageHeight =  DC -> GetDeviceCaps ( VERTRES ) ;
	    }
	else
	   {
		PageWidth  =  DC -> GetDeviceCaps ( VERTRES ) ;
		PageHeight =  DC -> GetDeviceCaps ( HORZRES ) ;
	    }
	    

//  Système de coordonnées 
	DC -> SetBkMode ( TRANSPARENT ) ;
	DC -> SetMapMode ( MM_ISOTROPIC ) ;


	if  ( ! Font )
		CreateFont  ( 0, 0, Bold, Italic, Underline ) ;
	else
		CreateFont ( ListCPI, ListLPI, Bold, Italic, Underline,
			IsSemiGraphic ) ;
     }




/**************************************************************************/
/*                                                                        */
/*   TEditWindowPrintout :: End -                                         */
/*      Restaure le contexte d'imprimante dans l'état où on aurait aimé   */
/*      le trouver en rentrant.                                           */
/*      De toute façon, le SavedDC et le RestoreDC sont superflus.        */
/*                                                                        */
/**************************************************************************/

void	TEditWindowPrintout :: End  ( void )
   {
    }





/**************************************************************************/
/*                                                                        */
/*   TEditWindowPrintout :: CreateFont -                                  */
/*      Recrée une police différente si les attributs (gras, italique,    */
/*      souligné) sont modifiés.                                          */
/*      Au passage, redéfinit si besoin est la taille des caractères.     */
/*                                                                        */
/**************************************************************************/

void	TEditWindowPrintout :: CreateFont  ( int   cpi, int  lpi, 
					     BOOL  bold, 
					     BOOL  italic, 
					     BOOL  underline,
					     BOOL  issemigraphic )
   {
	register int	width		=  TextFile -> CPI,
			height		=  TextFile -> LPI ;
	int *		linewidth	=  0,
	    *		pageheight	=  & TextFile -> MaxPageLength ;


// Calculer la nouvelle taille de caractères
	switch  ( cpi )
	   {
		case	LIST_10_CPI :
			width = 10 ;
			linewidth = & TextFile -> MaxLineLength10CPI ;
			break ;
			
		case	LIST_12_CPI :
			width = 12 ;
			linewidth = & TextFile -> MaxLineLength12CPI ;
			break ;                           

		case	LIST_15_CPI :
			width = 15 ;
			linewidth = & TextFile -> MaxLineLength15CPI ;
			break ;                      
	     }


	if  ( ! linewidth )		// Le comptage pour cette densité est passé à l'as
		linewidth = & TextFile -> MaxLineLength ;



	switch  ( lpi )
	  {
		case	LIST_6_LPI :
			height = 6 ;
			pageheight = & TextFile -> MaxPageLength ;
			break ;

		case	LIST_8_LPI :
			height = 8 ;
			pageheight = & TextFile -> MaxPageLength ;
			break ;                       
	    }

	SetExt ( width, height, * linewidth, * pageheight, issemigraphic ) ;


// Créer la police 
	if  ( bold  !=  Bold  ||  underline  !=  Underline  ||  italic  !=  Italic  ||
		! Font )
	   {
	// Attention : si on n'initialise pas OutPrecision et CharSet, dans le
	// cas de la font MsLineDraw contenant les caractères semi-graphiques,
	// Windows sélectionne une police de chiotte!
		LogFont. lfWeight 	=  ( bold ) ?  FW_HEAVY : FW_NORMAL ;
		LogFont. lfItalic	=  italic ;
		LogFont. lfUnderline	=  underline ;
		LogFont. lfOutPrecision =  OUT_TT_PRECIS ;
		LogFont. lfCharSet	=  DEFAULT_CHARSET ;

		TFont *	NewFont = new  TFont ( & LogFont ) ;



	// Libérer l'éventuelle police précédente et sélectionner la nouvelle dans le
	// DC de l'imprimante
		DC -> SelectObject ( * NewFont ) ;

		if  ( Font )
			delete  Font ;

		Font = NewFont ;
	     }
	else
		DC -> SelectObject ( * Font ) ;

    }





/**************************************************************************/
/*                                                                        */
/*  TEditWindowPrintout :: SetExt -                                       */
/*      Appelée par CreateFont, redéfinit la taille des caractères, non   */
/*      pas en créant une nouvelle police, mais en changeant le rapport   */
/*      window extent / viewport extent.                                  */
/*                                                                        */
/**************************************************************************/

static void	Adjust  ( TPoint &  Size, TPoint &  PrinterSize )
   {
	double		x1	=  Size. x,
			y1 	=  Size. y,
			x2	=  PrinterSize. x,
			y2 	=  PrinterSize. y ;
	double		v1, v2 ;


	v1 = ( x2 / x1 ) ;
	v2 = ( y2 / y1 ) ;

	Size. x = Size. y = 100 ;
	PrinterSize. x = ( int ) ( v1 * 100.0 ) ;	
	PrinterSize. y = ( int ) ( v2 * 100.0 ) ;
	PrinterSize. x = PrinterSize. y = min ( PrinterSize. x, PrinterSize. y ) ;
     }
		



void	TEditWindowPrintout :: SetExt  ( int  	width,
					 int	height,
					 int	linewidth,
					 int	pageheight,
					 BOOL	issemigraphic )
   {		
	long int	H, W ;

/***
	Plutôt que de créer une police avec la nouvelle taille de caractères, on
	modifie juste le rapport Window/Viewport en conséquence
	Pour ce faire, il faut calculer, en fonction de la densité, combien on
	peut caser de caractères et de lignes sur la page.
	La formule est la suivante :

		W = ( D * S ) / 25.4

	où :
		. W est la largeur de la page en caractères
		. D est la densité voulue (10, 12 ou 15 cpi)
		. S est la taille de la page en millimètres
		. 25.4 est le nombre de millimètres que représente un pouce

	Afin d'éviter l'utilisation de flottants, cette formule a été un peu
	remaniée :

			      1
		W =  D.S * ------
			    25.4

			     10
		  =  D.S * ------
			    254

			     5
		  =  D.S * ------
			    127

	Ceci bien entendu dans le cas d'états comme ceux du compte administratif.
	Pour les autres, l'édition est simplement adaptée aux plus grandes 
	largeur et hauteur de page.
 ***/
	if  ( issemigraphic )
	   {
		long int	HSize, VSize ;


		if  ( Orientation  ==  DMORIENT_PORTRAIT )
		   {
			HSize	=  DC -> GetDeviceCaps ( HORZSIZE ) ;
			VSize	=  DC -> GetDeviceCaps ( VERTSIZE ) ;
		     }
		else
		   {
			HSize	=  DC -> GetDeviceCaps ( VERTSIZE ) ;
			VSize	=  DC -> GetDeviceCaps ( HORZSIZE ) ;
		     }
		    
		W	=  ( ( long int ) width  * HSize * 5 ) / 127,
		H	=  ( ( long int ) height * VSize * 5 ) / 127 ;

	// Certains programmes éditent en 15 cpi, mais la largeur de l'édition
	// est telle que ça ne passera pas -> il faut alors se baser sur la
	// largeur de l'édition et adapter à la page.
		if  ( W  <  linewidth )
			W = linewidth ;
		if  ( H  <  pageheight )
			H = pageheight ;
	     }
	else
	   {
		W	=  linewidth ;
		H	=  pageheight ;
	    }



// Ajustement à la hauteur de page
	if  ( H - TextFile -> MaxPageLength  >  1 )
		H = TextFile -> MaxPageLength ;
	 
	TPoint 		Size  ( CharacterWidth  * ( int ) W,
			      ( CharacterHeight + ExternalLeading ) * ( int ) H ) ;
	TPoint		PrinterSize ;


	if  ( Orientation  ==  DMORIENT_PORTRAIT )
	   {
		PrinterSize. x = PageWidth ;
		PrinterSize. y = PageHeight ;
	     }
	else
	   {
		PrinterSize. x = PageHeight ;
		PrinterSize. y = PageWidth ;
	    }

	Size. x += 2 * XMargin ;
	Size. y += 2 * YMargin ;

// Tenir compte de l'orientation pour la largeur et la hauteur
	Adjust ( Size, PrinterSize ) ;
	DC -> SetWindowExt ( Size ) ;
	DC -> SetViewportExt ( PrinterSize ) ;
    }





/**************************************************************************/
/*                                                                        */
/*   TEditWindowPrintout :: ProcessEscape -                               */
/*      Gère les séquences escapes se trouvant dans les états utilisant   */
/*      le driver imprimante semi-graphique Unix.                         */
/*      Sous Unix, il faut bien entendu une définition d'imprimante       */
/*      spécifique, contenue dans le fichier #DESCRIP/printer/graphic.cfg,*/
/*      ainsi qu'une ligne                                                */
/*              graphic = graphic                                         */
/*      dans le fichier #DESCRIP/site.cfg, à la section [Printers].       */
/*      Les séquences escape y sont définies par :                        */
/*              . Un préfixe (LIST_BEGIN, asci 0x01)                      */
/*              . Le code de la séquence sur un caractère (voir les       */
/*                constantes LIST_xxx)                                    */
/*              . Un suffixe (LIST_END, ascii 0x02)                       */
/*                                                                        */
/**************************************************************************/

char *	TEditWindowPrintout :: ProcessEscape ( char *  buffer )
   {
	int		Width		=  ListCPI,
			Height		=  ListLPI ;
	BOOL		bold_attr       =  Bold,
			italic_attr	=  Italic,
			underline_attr	=  Underline ;



	buffer ++ ;		// Sauter le LIST_BEGIN


	while  ( * buffer  &&  * buffer  !=  LIST_END )
	   {
		switch  ( * buffer ) 
		   {
			case	LIST_10_CPI :
			case	LIST_12_CPI :
			case    LIST_15_CPI :
				Width = * buffer ;
				break ;
			
			case	LIST_6_LPI :
			case	LIST_8_LPI :
				Height = * buffer ;
				break ;

			case	LIST_BOLD_ON :
				bold_attr = TRUE ;
				break ;

			case	LIST_BOLD_OFF :
				bold_attr = FALSE ;
				break ;

			case	LIST_UNDERLINE_ON :
				underline_attr = TRUE ;
				break ;

			case	LIST_UNDERLINE_OFF :
				underline_attr = FALSE ;
				break ;
				
			case	LIST_ITALIC_ON :
				italic_attr = TRUE ;
				break ;

			case	LIST_ITALIC_OFF :
				italic_attr = FALSE ;
				break ;
		      }

		buffer ++ ;
	     }

	if  ( * buffer )
		buffer ++ ;	// Sauter LIST_END 


	if  ( Width  !=  ListCPI  ||  Height  !=  ListLPI  ||
		bold_attr  !=   Bold  ||
		underline_attr  !=  Underline  ||
		italic_attr  !=  Italic )
	   {
		CreateFont  ( Width, Height, bold_attr, italic_attr, 
				underline_attr, TRUE ) ;

		ListCPI		=  Width ;
		ListLPI		=  Height ;
		Bold		=  bold_attr ;
		Underline	=  underline_attr ;
		Italic		=  italic_attr ;
		IsSemiGraphic	=  TRUE ;
	     }

	return  ( buffer ) ;
      }

			
				





/**************************************************************************/
/*                                                                        */
/*  TEditWindowPrintout :: PrintPage -                                    */
/*      Imprime la page suivante du document.                             */
/*                                                                        */
/**************************************************************************/

void	TEditWindowPrintout :: PrintPage  ( int  page, TRect &, unsigned int )
   {
	char			Buffer [ TEXT_LINE_SIZE ] ;
	register char *		p ;
	register BOOL		Continue 	=  TRUE ;
	register int		Count		=  0 ;
	TPoint			Point ( 0, YMargin ) ;



	if  ( GotLastLine )
		return ;



	Begin ( ) ;
	LastPage = page ;

	LastLine = TextFile -> GetLineFromPage ( page ) ;
	DC -> SetViewportOrg ( Point ) ;

	

//  Boucle d'écriture
	while  ( 1 )
	   {

	// Ligne suivante
		Continue = TextFile -> GetLine ( LastLine + Count ++, Buffer ) ;

	// C'était la dernière
		if  ( ! Continue )
			break ;


	// Ecriture
		register int	X  	 =  XMargin,
				Position =  0 ;

		EditWindow -> TextFile -> HostToLocalCharSet ( Buffer ) ;
		p = Buffer ;
		


	// Form feed : la page est terminée
		if  ( * p  ==  '\f' )
			break ;
		

		while  ( * p )
		   {
			if ( * p  ==  LIST_BEGIN )
			   {
				p = ProcessEscape ( p ) ;
				continue ;
			    }
			else if  ( * p  ==  LIST_PORTRAIT  || 
				   * p  ==  LIST_LANDSCAPE )
			   {
			// Si un changement d'orientation intervient, il DOIT
			// figurer sur la 1e ligne d'une page (le traitement est 
			// fait à la fin de la fonction)
			// En arrivant ici, il faut simplement l'ignorer.
				p ++ ;
				continue ;
			    }

		// Calcul de la largeur effective du caractère afin de pouvoir 
		// le placer correctement, dans le cas d'une police 
		// proportionnelle
			TSize	      sz  =  DC -> GetTextExtent ( p, 1 ) ;
			int	      w   =  sz. cx ;	


			if  ( * p  ==  '\r' )
				X = XMargin ;
			else if  ( * p  ==  '\t' )	
			  {
				Position = NextTab ( Position ) ;
				X = Position * CharacterWidth ;
			    }
			else
			   {
				w = ( CharacterWidth - w ) / 2 ;
				DC -> TextOut ( X + w, 0, p, 1 ) ;
				X += CharacterWidth ;
			    }

			p ++ ;
			Position ++ ;
		    }


	// Modifier l'origine du viewport en fonction de la hauteur de la ligne
		Point. x = 0 ;
		Point. y = CharacterHeight + ExternalLeading ;
		DC -> LPtoDP ( & Point ) ;
		DC -> SetViewportOrg ( Point ) ;


	// Cas du driver imprimante en semi-graphique sous Unix : l'attribut
	// italique est remis à zéro lorsqu'on attaque l'imprimante en PCL. 
	// Mais ça n'est pas le cas ici, il faut donc le faire manuellement
		if  ( Italic )
		   {
			char		Buffer [ 32 ] ;


			wsprintf ( Buffer, "%c%c%c", LIST_BEGIN,
				LIST_ITALIC_OFF, LIST_END ) ;
			ProcessEscape ( Buffer ) ;
		    }
	    }


// C'est fini : on indique si c'était la dernière ligne ou pas
	GotLastLine = ! Continue ;
	LastLine += Count ;
	End ( ) ;
    }






/**************************************************************************/
/*                                                                        */
/*  TEditWindowPrintout :: CheckOrientationSwitch -                       */
/*      Vérifie si la ligne spécifiée contient une séquence escape        */
/*      changeant l'orientation du papier.                                */
/*                                                                        */
/**************************************************************************/

BOOL	TEditWindowPrintout :: CheckOrientationSwitch  ( long int   line )
   {
	char			Buffer  [ TEXT_LINE_SIZE ] ;
	register BOOL		Status = FALSE ;


	if  ( line  <  0 )
		line = TextFile -> GetLineFromPage ( - line ) ;

	if  ( TextFile -> GetLine ( line, Buffer ) )
	   {
		if  ( strchr ( Buffer, LIST_PORTRAIT )  !=  NULL )
			
		   {
			if  ( Orientation  ==  DMORIENT_LANDSCAPE )
				SwitchOrientation ( DMORIENT_PORTRAIT ) ;

			Status = TRUE ;
		    }
		else if  ( strchr ( Buffer, LIST_LANDSCAPE )  !=  NULL )
		   {
			if  ( Orientation  ==  DMORIENT_PORTRAIT )
				SwitchOrientation ( DMORIENT_LANDSCAPE ) ;

			Status = TRUE ;
		    }
	      }

	return  ( Status ) ;
      }



/**************************************************************************/
/*                                                                        */
/*  TEditWindowPrintout :: SwitchOrientation -                            */
/*      Change l'orientation de la page.                                  */
/*                                                                        */
/**************************************************************************/

void	TEditWindowPrintout :: SwitchOrientation  ( int  neworient )
   {
	Orientation = neworient ;
	Reoriented = TRUE ;
     }

	
	






/**************************************************************************/
/**************************************************************************/
/********                                                          ********/
/********          Implémentation de la classe TEditWindow         ********/
/********                                                          ********/
/**************************************************************************/
/**************************************************************************/


DEFINE_RESPONSE_TABLE1  ( TEditWindow, TDecoratedFrame )
	EV_COMMAND_ENABLE ( IDC_EDIT_COPY	 	, CommandEnable     ),
	EV_COMMAND_ENABLE ( IDC_EDIT_SELECT_ALL	 	, CommandEnable     ),
	EV_COMMAND_ENABLE ( IDC_EDIT_CANCEL_SELECTION	, CommandEnable     ),
	EV_COMMAND_ENABLE ( IDC_FILE_PRINT		, CommandEnable	    ),
	EV_COMMAND_ENABLE ( IDC_FILE_PRINT_CONFIG	, CommandEnable	    ),
	EV_COMMAND	  ( IDC_FILE_SAVE	 	, CmFileSave 	    ),
	EV_COMMAND	  ( IDC_FILE_FONT	 	, CmFileFont        ),
	EV_COMMAND	  ( IDC_FILE_PRINT       	, CmFilePrint       ),
	EV_COMMAND	  ( IDC_FILE_PRINT_CONFIG	, CmFilePrintConfig ),
	EV_COMMAND        ( IDC_FILE_EXIT        	, CmFileExit        ),
	EV_COMMAND        ( IDC_EDIT_COPY        	, CmEditCopy        ),
	EV_COMMAND	  ( IDC_EDIT_SELECT_ALL  	, CmEditSelectAll   ),
	EV_COMMAND	  ( IDC_EDIT_GOTO_PAGE   	, CmEditGotoPage    ),
	EV_COMMAND	  ( IDC_EDIT_GOTO_LINE   	, CmEditGotoLine    ),
	EV_COMMAND	  ( IDC_EDIT_SEARCH	 	, CmEditSearch 	    ),
	EV_COMMAND	  ( IDC_EDIT_SEARCH_AGAIN	, CmEditSearchAgain ),
	EV_COMMAND 	  ( IDC_EDIT_CANCEL_SELECTION	, CmEditCancelSelection ),
	EV_WM_CLOSE,
	EV_WM_TIMER,
	EV_WM_SETFOCUS,
	EV_WM_SYSCOLORCHANGE,
	EV_WM_SYSCOMMAND,
	EV_REGISTERED	  ( FINDMSGSTRING               , EvFindMsg	    ),

# if	COMPRESS_STATISTICS  &&  ! STANDALONE
	EV_COMMAND	  ( IDC_DICTIONARY_STATISTICS   , CmDictionaryStatistics ),
# endif

END_RESPONSE_TABLE ;




/**************************************************************************/
/*                                                                        */
/*  Constructeur.                                                         */
/*                                                                        */
/**************************************************************************/

TEditWindow :: TEditWindow  ( TUnixApplication *  app,
				TWindow *  parent, long int  size ) 
	: TDecoratedFrame ( parent, "", 
		EditWindowClient = new  TEditWindowClient ( this ), TRUE ) 
   {
	Zero ( ) ;
	UnixFileSize = size ;
	FileName     = 0 ;
	Application  = app ;

	Uncompressor = new  TUncompressor ( ) ;
    }




TEditWindow :: TEditWindow  ( char *	filename )
	: TDecoratedFrame ( NULL, "", 
		EditWindowClient = new  TEditWindowClient ( this ), TRUE ) 
   {
	Zero ( ) ;
	UnixFileSize = 0 ;
	FileName     = strdup ( filename ) ;
	Application  = 0 ;
    }




void	TEditWindow :: Zero  ( void )
   {
	StatusBar 		=  0 ;
	ControlBar 		=  0 ;
	FontData		=  0 ;
	Uncompressor		=  0 ;
	TextFile		=  0 ;
	TimerId 		=  0 ;
	CaretPosition		=  TPoint ( 0, 0 ) ;
	CurrentPage		=  1 ;

	FindMatchLine		=  -1 ;
	FindDialog		=  0 ;
     }





/**************************************************************************/
/*                                                                        */
/*  Destructeur.                                                          */
/*                                                                        */
/**************************************************************************/

TEditWindow :: ~TEditWindow  ( )
   {
	Free ( ) ;

// Redonner le focus au menu ou au terminal
	TWindow *	Window = ( TheCheopsMenuWindowObject ) ?
				( TWindow * ) TheCheopsMenuWindowObject :
				( TWindow * ) TerminalWindow ;


	Window -> BringWindowToTop ( ) ;
	Window -> SetFocus ( ) ;

	RestoreFocusToChild ( Window -> HWindow, HWindow ) ;
    }



void	TEditWindow :: Free  ( void )
   {
	if  ( FontData )
	   {
		delete  FontData ;
		FontData = 0 ;
	    }


	if  ( Uncompressor )
	   {
		delete  Uncompressor ;
		Uncompressor = 0 ;
	    }

	if  ( FileName )
	   {
		free ( FileName ) ;
		FileName = 0 ;
	    }

	if  ( TextFile )
	   {
		delete  TextFile ;
		TextFile = 0 ;
	    }

	if  ( FindDialog )
	   {
		delete  FindDialog ;
		FindDialog = 0 ;
	    }

	if  ( StatusBar )
	   {
		delete  StatusBar ;
		StatusBar = 0 ;
	    }

	if  ( ControlBar )
	   {
		delete  ControlBar ;
		ControlBar = 0 ;
	    }

	if  ( FilletBar )
	   {
		delete  FilletBar ;
		FilletBar = 0 ;
	    }
     }




/**************************************************************************/
/*                                                                        */
/*  TEditWindow :: EvClose -                                              */
/*      Libère les ressources allouées, s'enlève de la table des objets   */
/*      de l'application.                                                 */
/*      Ramène soit le menu soit le terminal au 1er plan (les fenêtres    */
/*      EditWindow étant popup, le focus est susceptible d'être redonné   */
/*      à une fenêtre n'appartenant pas à l'application).                 */
/*                                                                        */
/**************************************************************************/

void	TEditWindow :: EvClose  	( void )  
   {
	if  ( TimerId )
		KillTimer ( TimerId ) ;

	ApplicationTable -> RemoveObject ( Application -> ApplicationId, this ) ;
	TDecoratedFrame :: EvClose ( ) ;
     } 





/**************************************************************************/
/*                                                                        */
/*   TEditWindow :: GetLine                                               */
/*      Appelé par la fenêtre cliente lors de la réception d'un message   */
/*      WM_PAINT, retourne la ligne demandée.                             */
/*                                                                        */
/**************************************************************************/

BOOL	TEditWindow :: GetLine  ( long int  index, char *  buffer )
   {
	if  ( TextFile )
		return  ( TextFile -> GetLine ( index, buffer ) ) ;
	else
		return  ( FALSE ) ;
    }





/**************************************************************************/
/*                                                                        */
/*   TEditWindow :: Create                                                */
/*      Crée la fenêtre.                                                  */
/*                                                                        */
/**************************************************************************/

BOOL	TEditWindow :: Create  ( void )
   {
	register int		Status ;



// Infos sur la police
	FontData = new  TChooseFontDialog :: TData ( ) ;
	memset ( FontData, 0, sizeof ( TChooseFontDialog :: TData ) ) ;

	FontData -> Flags  |=  CF_INITTOLOGFONTSTRUCT | CF_FORCEFONTEXIST |
			       CF_SCREENFONTS ;
	strcpy ( FontData -> LogFont. lfFaceName, Configuration -> Session ( ). FontName ) ;
	FontData -> LogFont. lfHeight = Configuration -> Session ( ). FontHeight ;
	FontData -> LogFont. lfWidth  = Configuration -> Session ( ). FontWidth ;
	FontData -> LogFont. lfPitchAndFamily = FIXED_PITCH | FF_DONTCARE ;

	if  ( ! TextFile -> IsAnsi ( ) )
	   {
		FontData -> LogFont. lfCharSet = OEM_CHARSET ;
		strcpy ( FontData -> LogFont. lfFaceName, "MS LineDraw" ) ;
	    }
	else
		FontData -> LogFont. lfCharSet = ANSI_CHARSET ;


	EditWindowClient -> Font  =  new TFont ( & FontData -> LogFont ) ;
	FontData -> FontType = SCREEN_FONTTYPE ;

	EditWindowClient -> GetFontMetrics ( ) ;


// Barre d'icônes
	long int	HintColor ;
	CheopsConfiguration -> GetClassItem ( this, "HintColor", HintColor, 
					GetSysColor ( COLOR_BTNFACE ) ) ;


	StatusBar	= new  TColoredStatusBar ( this, HintColor, 
					TGadget :: Recessed, 0 ) ;

  
	StatusBar -> Insert ( * new  TTextGadget ( 0, TGadget :: Recessed,
					TTextGadget :: Center, 12 ) ) ;
	StatusBar -> Insert ( * new  TTextGadget ( 0, TGadget :: Recessed,
							TTextGadget :: Center, 7 ) ) ;

	Insert ( * StatusBar, TDecoratedFrame :: Bottom ) ;


// Barre d'état
	ControlBar	= new  TControlBar ( this ) ;

// Commandes du menu fichier
	ControlBar -> Insert ( * new  TButtonGadget ( IDC_FILE_SAVE, 
						IDC_FILE_SAVE,
						TButtonGadget :: Command, TRUE ) ) ;

	ControlBar -> Insert ( * new  TButtonGadget ( IDC_FILE_PRINT,
						IDC_FILE_PRINT,
						TButtonGadget :: Command, TRUE ) ) ;

	ControlBar -> Insert ( * new TSeparatorGadget ( 15 ) ) ;



// Presse-purée, euh, papiers
	ControlBar -> Insert ( * new TButtonGadget ( IDC_EDIT_COPY,
						IDC_EDIT_COPY ) ) ;

	ControlBar -> Insert ( * new TSeparatorGadget ( 15 ) ) ;


// Recherche
	ControlBar -> Insert ( * new  TButtonGadget ( IDC_EDIT_SEARCH,
						IDC_EDIT_SEARCH,
						TButtonGadget :: Command, TRUE ) ) ;

	ControlBar -> Insert ( * new  TButtonGadget ( IDC_EDIT_SEARCH_AGAIN,
						IDC_EDIT_SEARCH_AGAIN,
						TButtonGadget :: Command, TRUE ) ) ;

	ControlBar -> Insert ( * new TSeparatorGadget ( 15 ) ) ;


// Positionnement
	ControlBar -> Insert ( * new  TButtonGadget ( IDC_EDIT_GOTO_PAGE,
						IDC_EDIT_GOTO_PAGE,
						TButtonGadget :: Command, TRUE ) ) ;

	ControlBar -> Insert ( * new  TButtonGadget ( IDC_EDIT_GOTO_LINE,
						IDC_EDIT_GOTO_LINE,
						TButtonGadget :: Command, TRUE ) ) ;

	ControlBar -> SetHintMode ( TControlBar :: EnterHints ) ;
	Insert ( * ControlBar, TDecoratedFrame :: Top ) ;


// Liseré 
	COLORREF	Color = ( TheCheopsMenuWindowObject ) ?
					TheCheopsMenuWindowObject -> GetLastModuleColor ( ) :
					GetSysColor ( COLOR_BTNFACE ) ;


	FilletBar = new  TFilletBar ( this, TFilletBar :: Horizontal, Color ) ;
	Insert ( * FilletBar, TDecoratedFrame :: Top ) ;


// Créer la fenêtre
	Status = TDecoratedFrame :: Create ( ) ;

	SetCaption ( ( Uncompressor ) ? Uncompressor -> GetTitle ( ) : FileName ) ;


// Ajouter le menu et les accélérateurs
	AssignMenu ( "EditWindow" ) ;
	hAccel = GetModule ( ) -> LoadAccelerators ( "EditWindow" ) ;


// Mettre en place le timer
	if  ( Uncompressor )
	   {
		EvTimer ( 0 ) ;
		SetTimer ( EDITWINDOW_TIMER_ID, 1000 ) ;
		TimerId = EDITWINDOW_TIMER_ID ;
	     }


// Créer le scroller
	EditWindowClient -> CreateScroller ( ) ;

	return ( Status ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  TEditWindow :: UpdatePosition -                                       */
/*      Met à jour l'affichage de la position courante dans la barre      */
/*      d'état.                                                           */
/*                                                                        */
/**************************************************************************/

void	TEditWindow :: UpdatePosition  ( void )
   {
	char		buffer [ 32 ] ;


// Page courante
	wsprintf ( buffer, "Page %ld/%ld", CurrentPage,
			TextFile -> PageCount ) ;
	( ( TTextGadget * ) ( * StatusBar ) [1] ) -> SetText ( buffer ) ;

// Position courante
	register int	Delta = 0 ;


	if  ( EditWindowClient )
	   {
		if  ( EditWindowClient -> Scroller )
			Delta = ( int ) EditWindowClient -> Scroller -> XPos ;
	     }

	wsprintf ( buffer, "%ld:%d", 
		( long int ) CaretPosition. y + EditWindowClient -> TopLine + 1,
		CaretPosition. x + 1 + Delta ) ;
	( ( TTextGadget * ) ( * StatusBar ) [2] ) -> SetText ( buffer ) ;

// Caret
	if  ( :: GetFocus ( )   ==  EditWindowClient -> HWindow ) 
		EditWindowClient -> SetCaretPos ( CaretPosition. x, 
						  CaretPosition. y ) ;
     }





/**************************************************************************/
/*                                                                        */
/*   TTextFile :: UpdateText                                              */
/*      Appelée après chaque réception et decompression de bloc.          */
/*                                                                        */
/**************************************************************************/

void	TEditWindow :: UpdateText ( void )
   {

// Premier appel : il faut ouvrir le fichier
	if  ( ! TextFile )
	   {
		TextFile = new  TCheopsTextFile ( ) ;

		if  ( Uncompressor )
			TextFile -> Open ( * Uncompressor ) ;
		else
		   {
			TextFile -> Open ( FileName ) ;
			TextFile -> AnsiCharSet = FALSE ;
		    }
	    }

	TextFile -> Update ( ) ;	// sinon lire ce qui a été ajouté
	EditWindowClient -> UpdateScroller ( ) ;
     }






/**************************************************************************/
/*                                                                        */
/*   TEditWindow :: CommandEnable                                         */
/*      Gère la disponibilité de certaines commandes du menu.             */
/*                                                                        */
/**************************************************************************/

void	TEditWindow :: CommandEnable  ( TCommandEnabler &  ce )
   {
	switch  ( ce. Id )
	   {
		case	IDC_EDIT_COPY :
			ce. Enable ( EditWindowClient -> HasSelection ( ) ) ;
			break ;

		case	IDC_EDIT_CANCEL_SELECTION :
			ce. Enable ( EditWindowClient -> HasSelection ( ) ) ;
			break ;

		case	IDC_FILE_PRINT :
			if  ( ! TextFile -> GetLineCount ( ) )
			   {
				ce. Enable ( FALSE ) ;
				break ;
			    }

		case	IDC_FILE_PRINT_CONFIG :
			TPrintDialog :: TData & 	Data = Printer -> GetSetup ( ) ;
			ce. Enable ( Data. GetDevMode ( )  !=  NULL ) ;
			break ;
	     }
     }







/**************************************************************************/
/*                                                                        */
/*  TEditWindow :: CmFileSave -                                           */
/*      Sauvegarde l'édition dans un fichier.                             */
/*                                                                        */
/**************************************************************************/

void	TEditWindow :: CmFileSave ( void )
   {
// Initialisation des données pour la boiboite
	TOpenSaveDialog :: TData	FilenameData ( OFN_PATHMUSTEXIST |
						       OFN_HIDEREADONLY  |
						       OFN_OVERWRITEPROMPT,
						       "Fichiers texte (*.txt)|*.TXT|Tous les fichiers (*.*)|*.*|",
						       0, "", "txt" ) ;


// Et zou
ShootAgain :
	if  ( TFileSaveDialog ( this, FilenameData, 0, "Enregistrement d'une édition" ).
			Execute ( )  ==  IDOK )
	   {
		register FILE *		in	=  * Uncompressor ;
		register FILE *		out 	=  fopen ( FilenameData. FileName, "wt" ) ;
		char			buffer [ BUFSIZ ] ;



		if  ( out  ==  NULL )
		   {
			:: MessageBox ( HWindow, "Erreur de création", MB_OK | MB_ICONEXCLAMATION,
				"Le fichier %s n'a pu être créé.", 
					FilenameData. FileName ) ;
			goto  ShootAgain ;
		     }


		fseek ( in, 0, 0 ) ;

		while  ( fgets ( buffer, sizeof ( buffer ), in )  !=  NULL )
		   {
			TextFile -> HostToLocalCharSet ( buffer ) ;
			fputs ( buffer, out ) ;
		    }

		fclose ( out ) ;
	     }
      }




/**************************************************************************/
/*                                                                        */
/*   TEditWindow :: CmFileFont                                            */
/*      Permet de choisir une autre police de caractères.                 */
/*                                                                        */
/**************************************************************************/

void	TEditWindow :: CmFileFont  ( void ) 
   {
	TChooseFontDialog	ChooseFont ( this, * FontData ) ;
	register int		Status = ChooseFont. Execute ( ) ;


	if  ( Status  ==  IDOK )
	   {

	// Supprimer la police précédente
		delete  EditWindowClient -> Font ;

	// Créer une nouvelle police
		EditWindowClient -> Font = new TFont ( & FontData -> LogFont ) ;
		EditWindowClient -> GetFontMetrics ( ) ;

	// Forcer la fenêtre à se redessiner
		EditWindowClient -> ManageCaret ( FALSE ) ;
		EditWindowClient -> ManageCaret ( TRUE ) ;
		EditWindowClient -> UpdateScroller ( ) ;
		EditWindowClient -> Invalidate ( ) ;
	    }
      }







			
					
			

/**************************************************************************/
/*                                                                        */
/*  TEditWindow :: CmFilePrint                                            */
/*      Imprime le fichier.                                               */
/*      La classe printout n'est définie qu'ici parceque zut.             */
/*                                                                        */
/**************************************************************************/


void	TEditWindow :: CmFilePrint  ( void )
   {
// avant toute chose, on va vérifier que l'impression du document avec l'orientation
// actuelle ne va pas être à chier
	TPrintDialog :: TData &		PrintData   =  Printer -> GetSetup ( ) ;
	DEVMODE				DevMode	    =  * PrintData. GetDevMode ( ) ;
	TPrintDC *			PrintDC     =  new  TPrintDC ( 
							   PrintData. GetDriverName ( ),
							   PrintData. GetDeviceName ( ),
							   PrintData. GetOutputName ( ),
							   PrintData. GetDevMode ( ) ) ;
	int				PaperWidth  =  PrintDC -> GetDeviceCaps ( HORZRES ),
					PaperHeight =  PrintDC -> GetDeviceCaps ( VERTRES ) ;
	int				OldOrient   =  -1 ;


	delete  PrintDC ;



	if  ( ! TextFile -> HasOrientationSet ( ) )
	   {
	// On calcule combien de points correspond la largeur et la 
	// hauteur d'un caractère
		int		xmm	=  PaperWidth  / TextFile -> MaxLineLength,
				ymm	=  PaperHeight / TextFile -> MaxPageLength ;


	// On décide (un peu arbitrairement) que si le rapport de ces deux valeurs
	// est strictement supérieur à deux, le résultat va être à chier
		if  ( DevMode. dmOrientation  ==  DMORIENT_PORTRAIT  &&
			ymm / xmm > 2 )
		   {
			register int	Status = :: MessageBox ( HWindow, "Réglage de l'impression",
							MB_YESNOCANCEL,
							"L'imprimante est actuellement réglée pour imprimer en mode portrait ; "
							"Or, la largeur de l'édition est telle qu'il faudrait plutôt l'imprimer"
							" à l'italienne.\n"
							"Désirez-vous changer l'orientation du papier conformément à ces informations ?" ) ;

			if  ( Status  ==  IDCANCEL )
				return ;

			if  ( Status  ==  IDYES )
			   {
				OldOrient = DMORIENT_PORTRAIT ;
				DevMode. dmOrientation = DMORIENT_LANDSCAPE ;
				DevMode. dmFields      = DM_ORIENTATION ;
				PrintData. SetDevMode ( & DevMode ) ;
			    }
		      }
		else if  ( DevMode. dmOrientation  ==  DMORIENT_LANDSCAPE  &&
				ymm / xmm > 2 )
		   {
			register int	Status = :: MessageBox ( HWindow, "Réglage de l'impression",
							MB_YESNOCANCEL,
							"L'imprimante est actuellement réglée pour imprimer à l'italienne ; "
							"Or, la hauteur de page de l'édition est telle qu'il faudrait plutôt l'imprimer"
							" en mode portrait.\n"
							"Désirez-vous changer l'orientation du papier conformément à ces informations ?" ) ;

			if  ( Status  ==  IDCANCEL )
				return ;

			if  ( Status  ==  IDYES )
			   {
				OldOrient = DMORIENT_LANDSCAPE ;
				DevMode. dmOrientation = DMORIENT_PORTRAIT ;
				DevMode. dmFields      = DM_ORIENTATION ;
				PrintData. SetDevMode ( & DevMode ) ;
			    }
		     }
	    }
		


// Impression du document.
	TEditWindowPrintout *	Printout = new  TEditWindowPrintout ( ( Uncompressor ) ? 
						Uncompressor -> GetTitle ( ) :
						FileName,
					   this, DevMode. dmOrientation ) ;
	Printer -> Print ( this, * Printout, TRUE ) ;


// restaurer l'orientation si elle a changé
	if  ( OldOrient  !=  -1 )
	   {
		DevMode. dmOrientation = OldOrient ;
		DevMode. dmFields      = DM_ORIENTATION ;
		PrintData. SetDevMode ( & DevMode ) ;
	    }

	delete  Printout ;
    }





/**************************************************************************/
/*                                                                        */
/*   TEditWindow :: CmFilePrintConfig                                     */
/*      Ouvre la boîte de dialogue de configuration de l'imprimante.      */
/*                                                                        */
/**************************************************************************/

void	TEditWindow :: CmFilePrintConfig  ( void )
   {
	Printer -> Setup ( this ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  TEditWindow :: CmFileExit                                             */
/*      Ferme la fenêtre                                                  */
/*                                                                        */
/**************************************************************************/

void	TEditWindow :: CmFileExit ( void )
   {
// On simule ALT-F4. A la place du SendMessage, il y avait auparavant un
// CloseWindow qui provoquait un problème : lorsqu'on choisissait
// Fichier/Quitter alors qu'un transfert était en cours, l'objet n'était pas
// supprimé vraiment (destructeur non encore appelé) et la réception du bloc
// suivant provoquait un plantage.
	SendMessage ( WM_SYSCOMMAND, SC_CLOSE ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  TEditWindow :: CmEditCopy                                             */
/*      Copie le texte sélectionné vers le presse-papiers.                */
/*                                                                        */
/**************************************************************************/

void	TEditWindow :: CmEditCopy ( void )
  {
	register long int	size ;
	long int		SX1, SX2, SY1, SY2 ;
	int			l ;


// Calcul de la taille de la sélection ( +2, c'est pour le zéro final de chaque 
// ligne)
	EditWindowClient -> GetNormalizedSelection ( SX1, SY1, SX2, SY2 ) ;

	size = ( SX2 - SX1 + 3 ) * ( SY2 - SY1 + 1 ) ;

	
// Ouverture du presse-papiers
	if  ( :: OpenClipboard ( HWindow ) )
	   {
		char			buffer [ TEXT_LINE_SIZE ] ;
		register char far *	memstr ;
		HANDLE			hmem       =  GlobalAlloc ( GMEM_DDESHARE,
								size + 1 ) ;

		if  ( ! hmem )
		   {
			:: MessageBox ( HWindow, "Erreur", MB_OK | MB_ICONEXCLAMATION,
					"Pas assez de mémoire pour copier la sélection dans le presse-papiers." ) ;
			return ;
		    }


		memstr = ( char * ) GlobalLock ( hmem ) ;

	// Copie de la sélection dans le HGLOBAL
		for  ( long int  i = SY1 ; i <= SY2 ; i ++ )
		   {
			TextFile -> GetLine ( i, buffer ) ;

			buffer [ ( int ) ( SX2 + 1 ) ] = 0 ;
			l = strlen ( buffer ) ;
	
			if  ( buffer [ l - 1 ]  ==  '\n' )
				buffer [ l - 1 ] = 0 ;
			strcat ( buffer, "\r\n" ) ;
			
			strcpy ( memstr, buffer + ( int ) SX1 ) ;
			memstr  +=  strlen ( memstr ) ;
		     }

		* memstr = 0 ;			// Ne pas oubiler le zéro de fin


	// Vider le presse-purée
		:: EmptyClipboard ( ) ;

		GlobalUnlock ( hmem ) ;

	// et lui assigner notre texte
		:: SetClipboardData ( CF_OEMTEXT, hmem ) ;
		:: CloseClipboard ( ) ;
	     }

	EditWindowClient -> CancelSelection ( ) ;
   }




/**************************************************************************/
/*                                                                        */
/*  TEditWindow :: CmEditSelectAll                                        */
/*      Sélectionne tout le texte.                                        */
/*                                                                        */
/**************************************************************************/

void	TEditWindow :: CmEditSelectAll ( void )
   {
	EditWindowClient -> SelectionX1 = EditWindowClient -> SelectionY1 = 0 ;
	EditWindowClient -> SelectionX2 = TextFile -> MaxLineLength ;
	EditWindowClient -> SelectionY2 = TextFile -> GetLineCount ( ) ;
	EditWindowClient -> DrawSelection ( ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TEditWindow :: CmEditCancelSelection -                                */
/*      Annule la sélection en cours.                                     */
/*                                                                        */
/**************************************************************************/

void	TEditWindow :: CmEditCancelSelection  ( void )
   {
	EditWindowClient -> CancelSelection ( ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TEditWindow :: CmGotoPage -                                           */
/*      Positionne à la page demandée.                                    */
/*                                                                        */
/**************************************************************************/

//
// Classe pour éviter que TRangeValidator ne nous balance une message box
// fille de la fenêtre Terminal
//
class 	TJunkRangeValidator 	:  public  TRangeValidator
   {
	private :
		TWindow *  	Parent ;
		char *		Message ;


	public :
		TJunkRangeValidator  ( TWindow *  parent, 
				       char *	  message,
				       long int   min,
				       long int   max )
			: TRangeValidator ( min, max ),
			  Parent ( parent ),
			  Message  ( message )
		   { }


		void	Error  ( void )
		   {
			:: MessageBox ( Parent -> GetLastActivePopup ( ), "Erreur",
				MB_OK | MB_ICONEXCLAMATION, Message, Min, Max ) ;
		    }
      } ;

		



void	TEditWindow :: CmEditGotoPage  ( void ) 
   {
	char		Buffer [ MAX_PARAMETER_LENGTH ] ;
	register int	Status ;


	* Buffer = 0 ;

	Status = TInputDialog ( this, "Aller à la page...", "Numéro de page :",
			Buffer, sizeof ( Buffer ), 0,
			new  TJunkRangeValidator ( this,
				"Le numéro de page doit être compris entre %ld et %ld.",
					1, TextFile -> PageCount ) ). Execute  ( ) ;


	if  ( Status  ==  IDOK )
	   {
		long int	page = atol ( Buffer ) ;


		if  ( page > 0  &&  page  <=  TextFile -> PageCount )
		   {
			long int	line  =  TextFile -> GetLineFromPage ( page ) ;


			EditWindowClient -> Scroller -> VScroll ( SB_THUMBPOSITION,
				( int ) min ( line, 32767L ) ) ;
			UpdatePosition ( ) ;
		     }
	     }
    }




/**************************************************************************/
/*                                                                        */
/*  TEditWindow :: CmGotoLine -                                           */
/*      Positionne le curseur à la ligne demandée.                        */
/*                                                                        */
/**************************************************************************/

void	TEditWindow :: CmEditGotoLine  ( void )
   {
	char		Buffer [ MAX_PARAMETER_LENGTH ] ;
	register int	Status ;


	* Buffer = 0 ;

	Status = TInputDialog ( this, "Aller à la ligne...", "Numéro de ligne :",
			Buffer, sizeof ( Buffer ), 0,
			new  TJunkRangeValidator ( this,
				"Le numéro de ligne doit être compris entre %ld et %ld.",
					1, TextFile -> GetLineCount ( ) ) ). Execute  ( ) ;

	if  ( Status  ==  IDOK )
	   {
		long int	line = atol ( Buffer ) ;


		if  ( line > 0  &&  line  <=  TextFile -> GetLineCount ( ) )
		   {
			long int	top ;


			line -- ;
			EditWindowClient -> Scroller -> VScroll ( SB_THUMBPOSITION,
				( int ) min ( line, 32767L ) ) ;

			top = EditWindowClient -> TopLine ;

			if  ( line  >=  top  &&  line  <  top +  EditWindowClient -> LineCount )
			   {
				CaretPosition. y = ( int ) ( line - top ) ;
				UpdatePosition ( ) ;
			    }
		     }
	     }
    }





/**************************************************************************/
/*                                                                        */
/*   TEditWindow :: CmSearch -                                            */
/*      Ouvre une boîte "Recherche" puis démarre la recherche d'un        */
/*      texte.                                                            */
/*                                                                        */
/**************************************************************************/

void	TEditWindow :: CmEditSearch ( void )
   {
	if  ( GetKeyState ( VK_SHIFT )  &  0x8000 )
		FindData. Flags  &=  ~FR_DOWN ;
	else
		FindData. Flags  |=   FR_DOWN ;

	FindDialog = new  TFindDialog ( this, FindData ) ;
	FindDialog -> Create ( ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TEditWindow :: CmSearchAgain -                                        */
/*      Recherche la dernière chaîne spécifiée.                           */
/*                                                                        */
/**************************************************************************/

void	TEditWindow :: CmEditSearchAgain  ( void )
   {
// Gerber l'utilisateur s'il n'a rien saisi 
	if  ( ! FindData. FindWhat  ||  ! * FindData. FindWhat )
	   {
		:: MessageBox  ( HWindow, "Poursuivre la recherche...", 
			MB_OK | MB_ICONEXCLAMATION, "Vous n'avez saisi aucun texte à rechercher." ) ;
		return ;
	    }


// Au boulot !
	char		buffer [ TEXT_LINE_SIZE ] ;
	long int	start_line	=  EditWindowClient -> TopLine +
						CaretPosition. y ;
	int		start_col 	=  0 ;
	size_t 		length ;
	int		start ;


	if  ( FindMatchLine  ==  start_line )
	   {
		if  ( FindData. Flags & FR_DOWN ) 
			start_col = CaretPosition. x + FindMatchLength ;
		else
			start_line -- ;	
            }
	

	EditWindowClient -> DrawMatch ( 0, TRUE ) ; 	// Annuler le précédent
	FindMatchLine   = -1 ;

	while  ( GetLine ( start_line, buffer ) )
	   {
		Find ( buffer, FindData. FindWhat, FindData. Flags,
			start, length, start_col ) ;

		if  ( start  >=  0 )	// trouvé !
		   {
			long int	top  =  EditWindowClient -> TopLine ;

			FindMatchLine   = start_line ;	// Sauvegarder la position
			FindMatchColumn = start ;	// du texte
			FindMatchLength = length ;


		// Mettre à jour la position du scroller
			if  ( start_line  <   top  ||
			      start_line  >=  top + EditWindowClient -> LineCount )
				EditWindowClient -> Scroller -> VScroll ( SB_THUMBPOSITION,
					( int ) min ( start_line, 32767L ) ) ;

		// et du caret
			if  ( start_line  >=  top  &&  
					start_line  <  top +  EditWindowClient -> LineCount )
			   {
				CaretPosition. y = ( int ) ( start_line - top ) ;
				CaretPosition. x = start ;
				UpdatePosition ( ) ;
			    }

			EditWindowClient -> DrawMatch ( ) ;
			return ;
		      }


		start_col	=  0 ;

		if  ( FindData. Flags  &  FR_DOWN )
			start_line ++ ;
		else
			start_line -- ;
	     }

	:: MessageBox ( ( FindDialog ) ?  FindDialog -> HWindow : HWindow, 
		"Recherche de texte", MB_OK, 
			"Le texte n'a pas été trouvé" ) ;
	EditWindowClient -> DrawMatch ( ) ;
    }






/**************************************************************************/
/*                                                                        */
/*  TEditWindow :: Find -                                                 */
/*      Effectue la recherche de texte sur une ligne en fonction des      */
/*      paramètres.                                                       */
/*                                                                        */
/**************************************************************************/

void	TEditWindow :: Find  ( char *		buffer,
			       char *		rx,
			       uint32		flags,
			       int &		start,
			       size_t &		length,
			       int		start_col )
   {		
	char		regex [ MAX_PARAMETER_LENGTH ] ;


// Le set_case_sensitive de string ne marche pas avec les regexp...
	strcpy ( regex, rx ) ;

	if  ( ! ( flags  &  FR_MATCHCASE ) )
	   {
		strlwr ( buffer ) ;	// Aucune importance si buffer est modifié
		strlwr ( regex ) ;	// par contre il faut une copie de l'expression
					// car l'originale est utilisée par FindDialog
	     }
	

	string	s ( buffer ) ;
	start = s. find ( TRegexp ( regex ),
				& length, start_col ) ;

// Vérifier s'il fallait ne trouver que les mots entiers
// (avec une méthode beubeu...)
	if  ( start >= 0  )
	   {
		if   ( flags  &  FR_WHOLEWORD )
		   {
			register char *		p  =  buffer + start + length,
				      * 	q  =  buffer + start - 1 ;


			if  ( ( q >= buffer  &&  ! isalnum ( * q ) )  &&
				! isalnum ( * p ) )
				return ;
		      }
		else
			return ;
	     }

	start = -1 ;
	length = 0 ;
     }
		



/**************************************************************************/
/*                                                                        */
/*  TEditWindow :: EvSysCommand -                                         */
/*      Demande au menu de créer une fenêtre iconique MDI.                */
/*                                                                        */
/**************************************************************************/

void	TEditWindow :: EvSysCommand  ( uint  type, TPoint &  pos )
   {
	if  ( type  ==  SC_MINIMIZE  &&  TheCheopsMenuWindowObject )
		TheCheopsMenuWindowObject -> IconifyMe ( this, 
			TResId ( ICON_LISTE ) ) ;
	else             
	   {
		TDecoratedFrame :: EvSysCommand ( type, pos ) ;
	     }
     }






/**************************************************************************/
/*                                                                        */
/*   TEditWindow :: EvTimer                                               */
/*      Met à jour le texte de la barre d'état selon l'état de la         */
/*      transmission.                                                     */
/*                                                                        */
/**************************************************************************/

void	TEditWindow :: EvTimer  ( UINT  /* timer_id */ )
   {
	long int	size    = TextFile -> FileSize ( ) ;
	int		percent = ( UnixFileSize ) ? 
					( int )  ( ( size * 100 ) / UnixFileSize ) :
					100 ;
	char		Buffer [ MAX_PARAMETER_LENGTH ] ;


	if  ( Uncompressor -> IsComplete ( ) )
	   {
		wsprintf ( Buffer, "Réception complète  (%ldk,   %ld lignes).",
			( size + 1023 ) / 1024, TextFile -> GetLineCount ( ) ) ;
		StatusBar -> SetText ( Buffer ) ;
		UpdatePosition ( ) ;
	    }
	else if  ( Uncompressor -> IsProcessing ( ) ) 
	   {
		wsprintf ( Buffer, "Réception en cours  (%d%%)...", percent ) ;
		StatusBar -> SetText ( Buffer ) ;
		UpdatePosition ( ) ;
		return ;                          
	    }
	else				// Erreur de transmission
	   {
		wsprintf ( Buffer, "Fichier incomplet  (%d%%) : erreur de transmission",
			percent ) ;
		UpdatePosition ( ) ;
		StatusBar -> SetText ( Buffer ) ; 
	    }

	KillTimer ( TimerId ) ;
	TimerId = 0 ;
     }









/**************************************************************************/
/**************************************************************************/
/********                                                          ********/
/********     Implémentation de la classe TEditWindowClient        ********/
/********                                                          ********/
/**************************************************************************/
/**************************************************************************/


DEFINE_RESPONSE_TABLE1  ( TEditWindowClient, TWindow )
	EV_WM_SIZE,
	EV_WM_KEYDOWN,
	EV_WM_SETFOCUS,
	EV_WM_KILLFOCUS,
	EV_WM_LBUTTONDOWN,
	EV_WM_LBUTTONUP,
	EV_WM_MOUSEMOVE,
	EV_WM_CHAR,
END_RESPONSE_TABLE ;




/**************************************************************************/
/*                                                                        */
/*  Constructeur.                                                         */
/*                                                                        */
/**************************************************************************/

TEditWindowClient :: TEditWindowClient  ( TEditWindow *   ed )
	: TWindow ( 0, "EditWindowClient" )
   {
	TopLine		=  -1 ;
	EditWindow 	=  ed ;

	memset ( Display, 0, sizeof ( Display ) ) ;

	Attr. Style    |=  WS_VSCROLL | WS_HSCROLL | WS_POPUP ;
	Scroller	=  0 ;
	LineCount	=  0 ;
	Font		=  0 ;

	LButtonDown 	=  FALSE ;
	SelectionX1     = -1 ;
	SelectionX2 	= -1 ;
	SelectionY1 	= 0 ;
	SelectionY2 	= 0 ;

	LastSelectionX1 = -1 ;
	LastSelectionX2	= -1 ;
	LastSelectionY1 =  0 ;
	LastSelectionY2 =  0 ;
     }                        




/**************************************************************************/
/*                                                                        */
/*  Destructeur.                                                          */
/*                                                                        */
/**************************************************************************/

TEditWindowClient :: ~TEditWindowClient ( )
   {
	Free ( ) ;
    }



void  TEditWindowClient :: Free ( void )
   {
	for  ( int  i = 0 ; i < MAX_EDITWINDOW_LINES ; i ++ )
	   {
		if  ( Display [i] )
		   {
			free ( Display [i] ) ;
			Display [i] = 0 ;
		    }
	    }

	if  ( Font )
		delete  Font ;
     }




/**************************************************************************/
/*                                                                        */
/*   TEditWindowClient :: Create                                          */
/*      Crée l'objet TScroller                                            */
/*                                                                        */
/**************************************************************************/

BOOL	TEditWindowClient :: Create ( void )
   {
	register BOOL		Status ;


	Status = TWindow :: Create ( ) ;
	


	return ( Status ) ;
     }





/**************************************************************************/
/*                                                                        */
/*  TEditWindowClient :: GetFontMetrics                                   */
/*      Récupère des informations sur la police de caractères             */
/*      sélectionnée.                                                     */
/*                                                                        */
/**************************************************************************/

void	TEditWindowClient :: GetFontMetrics  ( void ) 
   {
	TClientDC	dc ( 0 ) ;
	TEXTMETRIC	tm ;


	if  ( ! dc )
		return ;

	dc. SelectObject ( * Font ) ;
	dc. GetTextMetrics ( tm ) ;
	
	CharacterHeight       = tm. tmHeight ;
	CharacterWidth        = tm. tmMaxCharWidth ;
	AverageCharacterWidth = tm. tmAveCharWidth ;
	ExternalLeading	      = tm. tmExternalLeading ;
     }


/**************************************************************************/
/*                                                                        */
/*   TEditWindowClient :: CreateScroller                                  */
/*      Crée l'objet TScroller                                            */
/*                                                                        */
/**************************************************************************/

void	TEditWindowClient :: CreateScroller ( void ) 
   {
	Scroller = new  TScroller ( this, CharacterWidth,
					  CharacterHeight,
					  EditWindow -> TextFile -> MaxLineLength,
					  EditWindow -> TextFile -> GetLineCount ( ) ) ;
	Scroller -> AutoOrg = FALSE ;
	UpdateScroller ( ) ;
	Invalidate ( ) ;
     }          






/**************************************************************************/
/*                                                                        */
/*  TEditWindowClient :: EvSize, UpdateScroller                           */
/*      Mise à jour des paramètres du TScroller en fonction de la taille. */
/*                                                                        */
/**************************************************************************/

void	TEditWindowClient :: EvSize  ( UINT  i, TSize &  Size )
   {
	TWindow :: EvSize ( i, Size ) ;

	if  ( Scroller )
	   {
		UpdateScroller ( Size ) ;
		Invalidate ( ) ;
	    }
     }



void	TEditWindowClient :: UpdateScroller  ( TSize &  Size )
   {
	if  ( Scroller ) 
	   {
		if  ( ! Size. cx  ||  ! Size. cy )
		   {
			TRect		Rect 	=  GetClientRect ( ) ;


			Size. cx = Rect. Width ( ) ;
			Size. cy = Rect. Height ( ) ;
		    }


		Size. cx /= CharacterWidth ;
		Size. cy /= CharacterHeight ;

		Scroller -> SetUnits ( CharacterWidth, CharacterHeight ) ;

		Scroller -> SetRange ( EditWindow -> TextFile -> MaxLineLength - Size. cx,
				( int ) min ( EditWindow -> TextFile -> GetLineCount ( ) - Size. cy,
					32767L ) ) ;
	     }              
    }	





/**************************************************************************/
/*                                                                        */
/*   TEditWindowClient :: EvKeyDown, EvChar                               */
/*      Gère les défilements au clavier.                                  */
/*      EvChar n'est là que pour annuler la sélection s'il y en a une.    */
/*                                                                        */
/**************************************************************************/

# define	IsUp(k)			( GetKeyState ( k )  &  0x8000 )



void	TEditWindowClient :: EvChar  ( UINT  key, UINT  repcnt, UINT  flags )
   {
	TWindow :: EvChar  ( key, repcnt, flags ) ;

	if  ( HasSelection ( ) )
		CancelSelection ( ) ;
    }



void	TEditWindowClient :: EvKeyDown  ( UINT  key, UINT  rep, UINT  flags )
   {
	TRect		Rect		=  GetClientRect ( ) ;
	int		Width 		=  Rect. Width  ( ) / CharacterWidth,
			Height  	=  Rect. Height ( ) / CharacterHeight ;
	TPoint &	Pos		=  EditWindow -> CaretPosition ;
	TPoint		SavedPos        =  Pos ;
	BOOL		Repaint		=  FALSE ;
	long int	SavedXPos	=  Scroller -> XPos,
			SavedTop	=  TopLine ;
	BOOL		StartSel	=  FALSE ;


	TWindow :: EvKeyDown ( key, rep, flags ) ;

	Width --, Height -- ;

	if (  EditWindow -> FindMatchLine  >=  0 )
		Repaint = TRUE ;
	

	switch ( key )
	   {
	// Home		:  Début de ligne
	// Shift-Home	:  Haut de l'écran
	// Ctrl-Home	:  Début du fichier
		case	VK_HOME :
			if  ( IsUp ( VK_CONTROL ) )
			   {
SBTop :
				Scroller -> VScroll ( SB_TOP, 0 ) ;
				Pos. x = Pos. y = 0 ;
			    }
			else if   ( IsUp ( VK_SHIFT ) )
				Pos. x  =  Pos. y = 0 ;
			else
			   {
				Scroller -> HScroll ( SB_TOP, 0 ) ;
				Pos. x  =  0 ;
			    }			
			break ;


	// La même chose avec End
		case	VK_END :
			if  ( IsUp ( VK_CONTROL ) )
			   {
SBBottom :
				Scroller -> VScroll ( SB_BOTTOM, 0 ) ;
				Pos. y = Height ;
			    }
			else if  ( IsUp ( VK_SHIFT ) )
				Pos. y = Height ;
			else
			   {
				Scroller -> HScroll ( SB_BOTTOM, 0 ) ;
				Pos. x = Width ;
			    }
			break ;


	// PageUp : Défiler d'une page vers l'arrière
		case	VK_PRIOR :
			if  ( IsUp ( VK_CONTROL ) )
				goto  SBTop ;
			else if  ( IsUp ( VK_SHIFT )  &&
					EditWindow -> CurrentPage  >  1 )
			   {
				long int  	l = EditWindow -> TextFile -> GetLineFromPage (
							EditWindow -> CurrentPage - 1 ) ;

				Scroller -> VScroll ( SB_THUMBPOSITION, ( int ) min ( l, 32767L ) ) ;
			    }
			else
				Scroller -> VScroll ( SB_PAGEUP, 0 ) ;
			break ;


	// PageDown : vers l'avant
		case	VK_NEXT :
			if  ( IsUp ( VK_CONTROL ) )
				goto  SBBottom ;
			else if  ( IsUp ( VK_SHIFT )  &&
				EditWindow -> CurrentPage < EditWindow -> TextFile -> PageCount )
			   {
				long int  	l = EditWindow -> TextFile -> GetLineFromPage (
							EditWindow -> CurrentPage - 1 ) ;

				Scroller -> VScroll ( SB_THUMBPOSITION, ( int ) min ( l, 32767L ) ) ;
			    }
			else
				Scroller -> VScroll ( SB_PAGEDOWN, 0 ) ;
			break ;


	// Un caractère à gauche
		case 	VK_LEFT :
			if  ( Pos. x )
				Pos. x -- ;
			else
				Scroller -> HScroll ( SB_LINELEFT, 0 ) ;
			StartSel = IsUp ( VK_SHIFT ) ;
			break ;


	// Et un à droite
		case	VK_RIGHT :
			if  ( Pos. x  <  Width )
				Pos. x ++ ;
			else
				Scroller -> HScroll ( SB_LINERIGHT, 0 ) ;
			StartSel = IsUp ( VK_SHIFT ) ;
			break ;                       


	// Une ligne vers le haut
		case	VK_UP :
			if  ( Pos. y )
				Pos. y -- ;
			else
				Scroller -> VScroll ( SB_LINEUP, 0 ) ;
			StartSel = IsUp ( VK_SHIFT ) ;
			break ;                       


	// et vers le bas
		case	VK_DOWN :
			if  ( Pos. y  <  Height )
				Pos. y ++ ;
			else
				Scroller -> VScroll ( SB_LINEDOWN, 0 ) ;
			StartSel = IsUp ( VK_SHIFT ) ;
			break ;                       


	// Escape : Annuler la sélection
		case	VK_ESCAPE :
			if  ( HasSelection ( ) )
				CancelSelection ( ) ;


		default :
			return ;
	     }


	if  ( Pos  !=  SavedPos  ||  
			Scroller -> XPos  !=  SavedXPos  ||
			TopLine  !=  SavedTop )
	   {
		EditWindow -> CurrentPage = EditWindow -> TextFile -> GetPageFromLine ( 
			TopLine + Pos. y ) ;

		if  ( HasSelection ( ) )
		   {
			SelectionX2 = Pos. x + Scroller -> XPos ;
			SelectionY2 = Pos. y + TopLine ;
			DrawSelection ( ) ;
		    }
		else if  ( StartSel )
		   {
			SelectionX1 = SavedPos. x + SavedXPos ;
			SelectionY1 = SavedPos. y + SavedTop ;
			SelectionX2 = Pos. x + Scroller -> XPos ;
			SelectionY2 = Pos. y + TopLine ;
			DrawSelection ( ) ;
		    }                   

		EditWindow -> UpdatePosition ( ) ;
	     }

	if  ( Repaint )
	   {
		DrawMatch ( 0, TRUE ) ;
		EditWindow -> FindMatchLine = -1 ;
	    }
     }





/**************************************************************************/
/*                                                                        */
/*   TEditWindowClient :: Paint                                           */
/*      Réactualise l'affichage                                           */
/*                                                                        */
/**************************************************************************/

void	TEditWindowClient :: Paint  ( TDC &  dc, BOOL, TRect & )
   {
	TRect			ClientRect	=  GetClientRect ( ) ;
	TCheopsTextFile *	TextFile	=  EditWindow -> TextFile ;
	int			OldCount ;



	Scroller -> BeginView ( dc, ClientRect ) ;


// Calculer les lignes de début et de fin
// On fait +1 sur la dernière ligne afin d'en écrire une partie en bas de la
// fenêtre; cette partie sera ensuite scrollée par l'appui de flèche vers le
// bas, et la partie découverte par le scroll représente en fait le reste de 
// cette dernière ligne.
	long int	FirstLine	=  Scroller -> YPos,
			LastLine	=  FirstLine + ( ClientRect. Height ( ) / 
						CharacterHeight ) + 1 ;


	if  ( LastLine >= TextFile -> GetLineCount ( ) )
		EditWindow -> UpdateText ( ) ;

	OldCount  = LineCount ;
	LineCount = ( int ) ( LastLine - FirstLine + 1 ) ;
	

// Récupérer les lignes si elles ne sont pas présentes dans le tableau Display
	if  ( TopLine  ==  -1  ||  TopLine  !=  FirstLine  ||  OldCount  !=  LineCount )
		Adjust ( FirstLine, LastLine, OldCount ) ;

	EditWindow -> CurrentPage = TextFile -> GetPageFromLine ( 
			FirstLine + EditWindow -> CaretPosition. y ) ;


// Calcul des coordonnées initiales pour l'affichage
	int		X	=  - ( int ) ( Scroller -> XPos * Scroller -> XUnit ),
			Y	=  0 ;

// Sélectionner la police courante et les couleurs de Windows
	COLORREF	BackgroundColor	= GetSysColor  ( COLOR_WINDOW ) ;

	dc. SelectObject ( * Font ) ;
	dc. SetTextColor ( GetSysColor ( COLOR_WINDOWTEXT ) ) ;
	dc. SetBkColor   ( BackgroundColor ) ;
	dc. SetBkMode ( TRANSPARENT ) ;


// Remplir la fenêtre avec la couleur de fond
	TBrush 		Brush ( BackgroundColor ) ;

	dc. FillRect ( ClientRect, Brush ) ;


// Afficher
	int		Width	=  CharacterWidth,
			Height 	=  CharacterHeight ;


// Afficher le texte caractère par caractère
	for  ( int  i = 0 ; i < LineCount ; i ++ )
	   {

	// Comme on veut toujours écrire une ligne de plus (celle qui
	// peut apparaître à moitié en bas de la fenêtre), il faut 
	// prendre en compte le cas où la dernière ligne complète 
	// affichée est la dernière ligne du fichier; dans ce cas,
	// Display [i] est la dernière ligne du fichier plus un et 
	// n'est donc pas définie
		if  ( Display [i]  ==  NULL )
			continue ;

		if  ( IsSpecial ( * Display [i] )  )
			DisplaySpecial ( Display [i],
				dc, X, Y, ClientRect. Width ( ), Height ) ;
		else
		   {
			register char *		p 	 =  Display [i] ;
			register int		Xpos     =  X,
						Position =  0 ;


			while  ( * p )
			   {
				Position ++ ;

				if  ( * p  ==  '\r' )		  // Retour chariot -> Début de ligne
				   {
					Xpos = X ;
					p ++ ;
				    }
				else if  ( * p  ==  LIST_PORTRAIT  ||  
					   * p  ==  LIST_LANDSCAPE ) 
					p ++ ;
				else if  ( * p  ==  LIST_BEGIN )  // Sauter les escapes
				   {
					do
					   {
						p ++ ;
					    } while  ( * p  &&  * p  !=  LIST_END ) ;

					if  ( * p )
						p ++ ;  // Sauter le LIST_END
				    }
				else if  ( * p  ==  '\t' )
				   {
					Position = NextTab ( Position ) ;
					Xpos = Position * Width ;
					p ++ ;
				    }
				else
				    {
					dc. TextOut ( Xpos, Y, p, 1 ) ;
					Xpos += Width ;
					p ++ ;
				     }
			     }

		     }

		Y += Height ;
	      }


// Mettre à jour la barre de défilement
	DrawMatch ( & dc ) ;

	if  ( HasSelection ( ) )
		DrawSelection ( & dc ) ;
	Scroller -> EndView ( ) ;
	EditWindow -> UpdatePosition ( ) ;
    }







/**************************************************************************/
/*                                                                        */
/*  TEditWindowClient :: DrawMatch -                                      */
/*      Affiche le texte qui a été recherché (Edition/Chercher) s'il y en */
/*      a un.                                                             */
/*                                                                        */
/**************************************************************************/

void	TEditWindowClient :: DrawMatch ( TDC *   dc, BOOL  cancel )
   {
	register TDC *		DC ;



// Ne rien faire si le match est en dehors de la zeone d'affichage
	if  ( EditWindow -> FindMatchLine  <  TopLine  ||
	      EditWindow -> FindMatchLine  >= TopLine + LineCount ) 
		return ;

	if  ( dc )
		DC = dc ;
	else
	   {
		DC = new  TClientDC ( HWindow ) ;
		DC -> SelectObject ( * Font ) ;
		DC -> SetBkMode ( TRANSPARENT ) ;
	    }

	
// Définir la couleur pour ce texte
	register int	i      =  ( int ) ( EditWindow -> FindMatchLine - TopLine ) ;
	COLORREF	OldBg  =  DC -> GetBkColor ( ),
			OldFg  =  DC -> GetTextColor ( ) ;
	COLORREF 	NewBg  =  GetSysColor ( COLOR_HIGHLIGHT ),
			NewFg  =  GetSysColor ( COLOR_HIGHLIGHTTEXT ) ;


	if  ( ! cancel ) 
	   {
		DC -> SetTextColor ( NewFg ) ;
		DC -> SetBkColor   ( NewBg ) ;
	    }



// Calculer déjà la position initiale du machin en tenant compte des tabs
	int		j  	=  0 ;
	int		x 	=  0 ;


	while  ( j  <  EditWindow -> FindMatchColumn )
	   {
		switch  ( Display [i] [j] )
		   {
			case	'\r' :
				j = EditWindow -> FindMatchColumn ;
				break ;

			case	LIST_PORTRAIT :
			case	LIST_LANDSCAPE :
				break ;

			case	LIST_BEGIN :
				while  ( j < EditWindow -> FindMatchColumn  &&
					 Display [i] [j]  !=  LIST_END )
					j ++ ;
				break ;

			case	'\t' :
				x = NextTab ( x ) ;
				break ;

			default :
				x ++ ;
		      }

		j ++ ;
	      }

	x *= CharacterWidth ;
			
			


// Remplir le rectangle correspondant avec la couleur de fond                      
	int 		y	=  i * CharacterHeight ;
	TRect		MatchRect ( x, y, 
				    x + ( EditWindow -> FindMatchLength * CharacterWidth ),
				    y + CharacterHeight ) ;
	TBrush		MB ( ( cancel ) ? OldBg : NewBg ) ;

				
	DC -> FillRect ( MatchRect, MB ) ;

// Ecrire le texte
	DC -> TextOut ( x, y, Display [i] + EditWindow -> FindMatchColumn,
			EditWindow -> FindMatchLength ) ;
				

// Restaurer les couleurs
	if  ( ! cancel )
	   {
		DC -> SetBkColor  (  OldBg ) ;
		DC -> SetTextColor ( OldFg ) ;
	    }

	if  ( ! dc )
		delete  DC ;
      }
	    





/**************************************************************************/
/*                                                                        */
/*  TEditWindowClient :: DisplayFormFeed                                  */
/*      Affiche le marqueur de saut de page.                              */
/*                                                                        */
/**************************************************************************/

void	TEditWindowClient :: DisplaySpecial  (  char *	line,
						TDC &  	dc,
						int	x,
						int	y,
						int	cx,
						int	cy )
   {
	char 		text [ 128 ] ;
	char *		p ;
	int		newy		=  y + ( ( cy - CharacterHeight ) / 2 ),
			newx,
			length ;
	COLORREF	BrushColor 	=  GetSysColor ( COLOR_GRAYTEXT ) ;


// Construire le message approprié
	switch  ( * line )
	   {
		case	'\f' :
			p = "Saut de page" ;
			break ;

		default :
			return ;
	     }



// Calculer la longueur du texte pour le centrer
	strcpy ( text, p ) ;

	if  ( ! EditWindow -> TextFile -> IsAnsi ( ) )
		AnsiToOem ( text, text ) ;

	length =  ( dc. GetTextExtent ( text, strlen ( text ) - 1 ) ). cx ;
	newx   = x + ( ( cx - length ) / 2 ) ;

// Affichage
	TBrush 		Brush  ( BrushColor ) ;
	TPen		Pen    ( GetSysColor ( COLOR_WINDOWFRAME ) ) ;


	dc. SaveDC ( ) ;

	dc. SetTextColor ( 0L ) ;
	dc. SelectObject ( Pen ) ;
	dc. SelectObject ( Brush ) ;
	dc. Rectangle ( x, y, x + cx, y + cy ) ;

	dc. TextOut ( newx, newy, text ) ;

	dc. RestoreDC ( ) ;
     }



	



/**************************************************************************/
/*                                                                        */
/*   TEditWindowClient :: Adjust                                          */
/*      Lit les lignes de texte no First à Last si elles ne sont pas      */
/*      présentes dans le tableau Display.                                */
/*                                                                        */
/**************************************************************************/

void	TEditWindowClient :: Adjust ( long int  first, long int  last,
				      int  oldcount ) 
   {
	char			Buffer [ TEXT_LINE_SIZE ] ;
	register BOOL		Continue 	=  TRUE ;
	register int		a, b, i, 
				count		=  LineCount, 
				index 		=  0 ;
	long int		read_first 	=  first ;


// 1er appel : il faut absolument lire quelque chose
	if  ( TopLine  ==  - 1 )
	   {
		read_first = first = 0 ;
		goto  PerformRead ;
	    }
// Changement de taille : on relit tout
	else if  ( oldcount  !=  LineCount ) 
	   {
		read_first = first ;
		goto  PerformRead ;
	    }


// Vérifier si rien n'a bougé
	if  ( first  ==  TopLine  &&  last  ==  TopLine + LineCount - 1 )
		return ;


//  On parcourt les lignes vers l'arrière : on décale pour ne lire que la
//  partie découverte par le scroll
	if  ( first >=  TopLine  &&   first  <  TopLine + LineCount )
	   {
		a =  ( int ) ( first - TopLine ) ;
		b =  LineCount - a ;
		

	// Calcul :
	//	. De la 1e ligne à lire effectivement (read_first)
	//	. Du nombre de lignes à lire effectivement (count)
	//	. De l'indice dans le tableau Display où la 1e ligne lue
	//	  doit être stockée.
		read_first = first + b ;
		count      = LineCount - b ;
		index 	   = LineCount - a ;

	// Décalage des lignes
		for  ( i = a ; i < LineCount ; i ++ )
		   {
			if  ( Display [i - a] )
				free ( Display [i - a] ) ;
			Display [i - a] = Display [i] ;
			Display [i] = 0 ;
		    }
	     }
//  Parcours des lignes vers l'avant : la 1e ligne à lire se retrouve avant
//  la 1e affichée
	else if  ( first  <  TopLine  &&   last  >=  TopLine  
				      &&   last  <   TopLine + LineCount )
	   {
		b = ( int ) ( TopLine - first ) ;
		a = LineCount - b ;


	// Calcul du bazar
		read_first  =  first ;
		count	    =  b ;
		index	    =  0 ;	


	// Décalage des lignes vers le haut du tableau
		for  ( i = LineCount - 1 ; i >= b ; i -- )
		   {
			if  ( Display [i] )
				free ( Display [i] ) ;
			Display [i] = Display [i - b] ;
			Display [i - b] = 0 ;
		     }

	      }
		
			

	
PerformRead :
	LineCount -= count ;

	for  ( i = 0 ; Continue  &&  i < count ; i ++, index ++ )
	   {
		if  ( Display [index] )
		   {
			free ( Display [index] ) ;
			Display [index] = 0 ;
		    }

		Continue = EditWindow -> GetLine ( read_first + i, Buffer ) ;

		if  ( Continue )
		   {
			EditWindow -> TextFile -> HostToLocalCharSet ( Buffer ) ;
			Display [index] = strdup ( Buffer ) ;
			LineCount ++ ;
		    }
	    }


// Mettre à jour les variables internes
	TopLine = first ;
     }








/**************************************************************************/
/*                                                                        */
/*  TEditWindowClient :: ManageCaret                                      */
/*      Gestion du caret.                                                 */
/*                                                                        */
/**************************************************************************/

void	TEditWindowClient :: ManageCaret  ( BOOL  on )
   {
	if  ( on ) 
	   {
		CreateCaret ( FALSE, CharacterWidth, CharacterHeight ) ;

		SetCaretPos ( EditWindow -> CaretPosition. x * CharacterWidth,
			      EditWindow -> CaretPosition. y * CharacterHeight ) ;
		ShowCaret ( ) ;
	    }

	else 
	   {
		HideCaret ( ) ;
		DestroyCaret ( ) ;
	    }
     }




/**************************************************************************/
/*                                                                        */
/*  TEditWindow :: EvSetFocus, EvKillFocus -                              */
/*      Gestion du caret.                                                 */
/*                                                                        */
/**************************************************************************/


void	TEditWindowClient :: EvSetFocus  ( HWND  /* lost */ )
   {
	ManageCaret ( TRUE ) ;
    }


void	TEditWindowClient :: EvKillFocus  ( HWND  /* gained */ )
   {
	ManageCaret ( FALSE ) ;
    }






/**************************************************************************/
/*                                                                        */
/*  TEditWindowClient :: EvLbuttonUp, Down, EvMouseMove -                 */
/*      Mise à jour de la sélection.                                      */
/*                                                                        */
/**************************************************************************/

void	TEditWindowClient :: EvLButtonDown  ( UINT  modkeys, TPoint &  pos )
   {
	if  ( HasSelection ( ) )
		CancelSelection ( ) ;
	   
	GetCursorPos ( pos ) ;
	PointToPos ( pos. x, pos. y, SelectionX1, SelectionY1 ) ;
	
	SelectionX2 = SelectionX1 ;
	SelectionY2 = SelectionY1 ;
	LButtonDown = TRUE ;
	SetCapture ( ) ;
	TWindow :: EvLButtonDown ( modkeys, pos ) ;
     }



void	TEditWindowClient :: EvLButtonUp  ( UINT  modkeys, TPoint &  pos )
   {
	GetCursorPos ( pos ) ;
	PointToPos ( pos. x, pos. y, SelectionX2, SelectionY2 ) ;
	LButtonDown = FALSE ;
	ReleaseCapture ( ) ;
	EditWindow -> UpdatePosition ( ) ;
	TWindow :: EvLButtonUp  ( modkeys, pos ) ;
     }


void	TEditWindowClient :: EvMouseMove  ( UINT  modkeys, TPoint &  pos )
   {
	GetCursorPos ( pos ) ;

	if  ( LButtonDown )
	   {
		PointToPos ( pos. x, pos. y, SelectionX2, SelectionY2 ) ;
		DrawSelection ( ) ;
	    }

	TWindow :: EvMouseMove ( modkeys, pos ) ;
    }





/**************************************************************************/
/*                                                                        */
/*  TEditWindowClient :: PointToPos -                                     */
/*      Convertir une coordonnée souris en coordonnée fichier.            */
/*                                                                        */
/**************************************************************************/

void	TEditWindowClient :: PointToPos ( long int    oldx, long int    oldy,
					  long int &  newx, long int &  newy )
  {
// Convertir déjà les coordonnées écran en coordonnées fenêtre
	TPoint		newone = TPoint ( ( int ) oldx, ( int ) oldy ) ;
	ScreenToClient ( newone ) ;

// Vérifier qu'elles ne sortent pas de l'écran
	TRect	Rect = GetClientRect ( ) ;

	if  ( newone. x  < Rect. left )
		newone. x = Rect. left ;
	else if  ( newone. x  >  Rect. right )
		newone. x = Rect. right ;

	if  ( newone. y  <  Rect. top )
		newone. y = Rect. top ;
	else if  ( newone. y  >  Rect. bottom )
		newone. y = Rect. bottom ;

// Puis les coordonnées fenêtre en coordonnées caractères
	newone. x /= CharacterWidth ;
	newone. y /= CharacterHeight ;
	
	EditWindow -> CaretPosition = newone ;

// Et enfin, convertir en position dans le fichier
	newx = newone. x + Scroller -> XPos ;
	newy = newone. y + TopLine ;	
     }







/**************************************************************************/
/*                                                                        */
/*   TEditWindowClient :: DrawSelection -                                 */
/*      Dessine la sélection.                                             */
/*                                                                        */
/**************************************************************************/

void	TEditWindowClient :: DrawSelection ( TDC *	dc )
   {
	register BOOL		needs_release = FALSE,
				clipped	      = FALSE ;
	long int		X1, X2, Y1, Y2 ;
	int			i ;
	TRegion			ClipRegion ;


// Calculer déjà les coordonnées d'origine et de fin
	GetNormalizedSelection ( X1, Y1, X2, Y2 ) ;
	

// NOTE : Lorsque le scroller ne se déplace pas d'une page écran, il fait un
// ScrollWindow, qui n'invalide pas le contenu de la fenêtre, mais ajoute 
// plutôt la partie découverte à la région d'update . Aucun WM_PAINT n'est donc
// envoyé dans ce ca, d'où problème lorsque cette fonction est appelée depuis
// le EvMouseMove : elle s'attend en effet à effacer une sélection qui n'est 
// peut-être plus là du fait du ScrollWindow...
// Pour parer à ce problème, lorsqu'on dessine depuis le MouseMove, il faut
// ne le faire que dans la région correspondant à l'intersection de :
//	. La région d'update
//      . La région où il faut redessiner la sélection.
// On obtient la région d'update en appelant GetUpdateRgn; pour établir 
// l'intersection, il suffit juste de clipper le DC.
//
// NOTE 2 : Lorsqu'on fait un SelectClipRgn sur un DC, avant de relâcher le DC,
// il faut faire un SelectClipRgn ( 0 ), car la région de clipping reste 
// apparemment valide...
	if  ( ! dc )
	   {
		dc = new  TClientDC ( HWindow ) ;
		i  = GetUpdateRgn ( ClipRegion ) ;
		clipped = ( i != NULLREGION )  &&  ( i != ERROR ) ;
				
		if  ( clipped )
			dc -> SelectClipRgn ( ClipRegion ) ;
		
		needs_release = TRUE ;
	    }

	
// Faire l'intersection entre le rectangle de sélection et le rectangle
// de la fenêtre
	TRect		SelectionRect ( ( int ) X1, 
					( int ) Y1,
					( int ) X2 + 1, 
					( int ) Y2 + 1 ),
			LastSelRect   ( ( int ) LastSelectionX1, 
					( int ) LastSelectionY1,
					( int ) LastSelectionX2 + 1, 
					( int ) LastSelectionY2 + 1 ),
			WindowRect    ( ( int ) Scroller -> XPos,
					( int ) Scroller -> YPos,
					( int ) EditWindow -> TextFile -> MaxLineLength,
					( int ) Scroller -> YPos + LineCount ) ;
	TRect		OldIntersection,
			NewIntersection ;

	

// Calculer la taille de l'ancienne sélection dans la zone actuellement affichée
	IntersectRect ( & OldIntersection, & LastSelRect, & WindowRect ) ;
	OldIntersection -= TSize ( ( int ) Scroller -> XPos, ( int ) Scroller -> YPos ) ;
	OldIntersection. left   *=  CharacterWidth ;
	OldIntersection. right  *=  CharacterWidth ;
	OldIntersection. top    *=  CharacterHeight ;
	OldIntersection. bottom *=  CharacterHeight ;


// Recommencer avec la nouvelle sélection
	IntersectRect ( & NewIntersection, & SelectionRect, & WindowRect ) ;
	NewIntersection -= TSize ( ( int ) Scroller -> XPos, ( int ) Scroller -> YPos ) ;
	LastSelRect = NewIntersection ;		// La nouvelle devient l'ancienne au prochain appel

	NewIntersection. left   *=  CharacterWidth ;
	NewIntersection. right  *=  CharacterWidth ;
	NewIntersection. top    *=  CharacterHeight ;
	NewIntersection. bottom *=  CharacterHeight ;


// Créer les régions correspondantes aux deux sélections
	TRegion		OldRegion ( OldIntersection ),
			NewRegion ( NewIntersection ) ;
	TRegion		OldInvert = OldRegion,
			NewInvert = NewRegion ;


// N'inverser la sélection précédente que si :
//	. il y en avait une
//	. DrawSelection n'a pas été appelé depuis Paint (car dans ce cas, la
//	  sélection a déjà été effacée par le Paint)
// Dans l'autre cas (DrawSelection a été appelé par Paint), la région entière
// de la sélection est redessinée
	if  ( LastSelectionX1  >=  0  &&  needs_release )
	   {
		OldInvert -= NewRegion ;
		dc -> InvertRgn ( OldInvert ) ;
		NewInvert -= OldRegion ;
	    }

	if  ( SelectionX1  >=  0 )
		dc -> InvertRgn ( NewInvert ) ;


// Libérer le DC s'il avait été acquis
	if  ( needs_release )
	   {
		if  ( clipped )
			:: SelectClipRgn ( * dc, 0 ) ;
		delete  dc ;	
	    }

// Sauvegarder le rectangle contenant la dernière sélection affichée
	LastSelectionX1	=  X1 ;
	LastSelectionY1	=  Y1 ;
	LastSelectionX2	=  X2 ;
	LastSelectionY2	=  Y2 ;
    }





/**************************************************************************/
/*                                                                        */
/*   TCheopsTextFile :: Read -                                            */
/*      Collecte au fur et à mesure de la lecture des infos sur :         */
/*      . La longueur max d'une ligne                                     */
/*      . La longueur max d'une page                                      */
/*      . Le nombre de pages                                              */
/*                                                                        */
/**************************************************************************/

BOOL	TCheopsTextFile :: Read  ( long int		offset,
				   char *		buffer,
				   long int &		newpos )
   {
	register BOOL	IsEOP	=  TTextFile :: Read ( offset, buffer, newpos ) ;
	register int *	length  =  NULL ;


// Si la lecture s'est bien déroulée et que la nouvelle position du fichier
// est supérieure à la dernière qu'on ait traité, alors on met à jour les
// statistiques sur le texte
	if  ( newpos  !=  -1  &&  newpos > LastReadOffset )
	   {

		if  ( IsEOP )		// Form feed : Mettre à jour nombre de pages
		   {
			PageCount ++ ;

			if  ( CurrentPageLength > MaxPageLength )
				MaxPageLength = CurrentPageLength ;
			CurrentPageLength = 0 ;
		    }
		else			// Longueur max d'une ligne
		   {
			register char *		p		=  buffer ;
			register BOOL		in_sequence     =  FALSE ;
			register int		size		=  0 ;
			


			while  ( * p  &&  * p  !=  '\r' )
			   {
				if  ( * p  ==  LIST_BEGIN )	// Marquer le début 
					in_sequence = TRUE ;
				else if  ( * p  ==  LIST_END )	// et la fin des escapes
					in_sequence = FALSE ;
				else if  ( in_sequence  && ! size )	// Sinon, sauvegarder la densité
				   {
					switch  ( * p )
					   {
						case	LIST_10_CPI :
							CPI = 10 ;
							break ;

						case	LIST_12_CPI :
							CPI = 12 ;
							break ;         

						case	LIST_15_CPI :
							CPI = 15 ;
							break ;         

						case	LIST_6_LPI :
							LPI = 6 ;
							break ;

						case	LIST_8_LPI :
							LPI = 8 ;
					      }
				      }
				else if  ( * p  ==  '\t' )
					size += 8 ;	// Tabulations
				else
					size ++ ;

				p ++ ;
			     }


			switch  ( CPI )
			   {
				case	10 :
					length = & MaxLineLength10CPI ;
					break ;

				case	12 :
					length = & MaxLineLength12CPI ;
					break ;

				case	15 :
					length = & MaxLineLength15CPI ;
					break ;
			      }

			if  ( size > * length )
				* length = size ;

			if  ( size > MaxLineLength )
				MaxLineLength = size ;
			CurrentPageLength ++ ;
		      }

		LastReadOffset = newpos ;
	      }
	else if  ( newpos  ==  -1  &&  ! MaxPageLength )	// Cas d'un fichier d'une page
		MaxPageLength = CurrentPageLength ;
	

	return  ( IsEOP ) ;
    }
