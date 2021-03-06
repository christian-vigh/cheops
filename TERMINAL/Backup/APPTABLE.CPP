/**************************************************************************/
/*                                                                        */
/*  AppTable.cpp  - Christian Vigh, 94/10/03.                             */
/*                                                                        */
/*  Impl�mentation de la classe TUnixApplicationTable.                    */
/*                                                                        */
/**************************************************************************/



# include	"AllTerm.h"
# pragma	hdrstop

# include	"UnixApp.h"
# include	"Event.h"


TEventQueue *			EventQueue ;


/**************************************************************************/
/*                                                                        */
/*   TUnixApplicationTable :: TUnixApplicationTable                       */
/*      Constructeur de la classe.                                        */
/*                                                                        */
/**************************************************************************/

TUnixApplicationTable :: TUnixApplicationTable  ( int      table_size )
    {
	ApplicationCount        =  0 ;
	ApplicationTableSize	=  table_size ;
	ApplicationTable	=  new  TUnixApplication *  [ table_size ] ;
	LastMessage. Data	=  NULL ;
	LastMessage. DataSize   =  0 ;
	EventQueue		=  new  TEventQueue ( ) ;
	InSuperMessage		=  FALSE ;
	MessageOrigin		=  0 ;
	memset ( & LastMessage. Message, 0, sizeof ( LastMessage. Message ) ) ;


	for  ( int  i = 0 ; i < table_size ; i ++ )
		ApplicationTable [i] = NULL ;

// Les entr�es de la tables des fonctions doivent �tre soigneusement remplies.
// La fonction correspondant � l'action x doit �tre l'�l�ment num�ro x de
// cette table.
// La boucle suivante v�rifie que c'est bien le cas...
	for  ( i = 0 ; DispatchTable [i]. Handler  !=  NULL ; i ++ )
	    {
		if  ( DispatchTable [i]. FunctionId  !=  i )
			:: MessageBox ( TerminalWindow -> HWindow, "Erreur interne",
				MB_OK | MB_ICONHAND,
				   "L'entr�e %s de la table des fonctions n'a pas la bonne valeur : %ld",
				      i, DispatchTable [i]. FunctionId ) ;
	     }


	for  ( i = 0 ; i < MAX_COMPRESSED_TRANSFERS ; i ++ )
	   {
		TransferTable [i]. FileId 	= -1 ;
		TransferTable [i]. Uncompressor = 0 ;
	    }
      }





/**************************************************************************/
/*                                                                        */
/*   TUnixApplicationTable :: ~TUnixApplicationTable                      */
/*      Lib�re la m�moire allou�e pour g�rer les applications.            */
/*                                                                        */
/**************************************************************************/

TUnixApplicationTable :: ~TUnixApplicationTable  ( )
   {
	for  ( int  i = 0 ; i < ApplicationTableSize ; i ++ )
	   {
		if  ( ApplicationTable [i]  !=  NULL )
			delete  ApplicationTable [i] ;
	    }

	for  ( i = 0 ; i < MAX_COMPRESSED_TRANSFERS ; i ++ )
	   {
		if  ( TransferTable [i]. Uncompressor )
			delete  TransferTable [i]. Uncompressor ;
		if  ( TransferTable [i]. FileId  !=  -1 )
			close ( TransferTable [i]. FileId ) ;
	    }

	delete []  ApplicationTable ;
	delete     EventQueue ;

	if  ( LastMessage. Data  !=  NULL )
		delete []  LastMessage. Data ;
     }




/**************************************************************************/
/*                                                                        */
/*   TUnixApplicationTable :: Find                                        */
/*      Retourne un pointeur sur la classe TUnixApplication               */
/*      correspondant au num�ro de processus sp�cifi�.                    */
/*                                                                        */
/**************************************************************************/

int	TUnixApplicationTable :: Find  ( integer16  pid )
   {
	register int	index1	=  pid % ApplicationTableSize,
			index2 ;


	if  ( index1 < 0 )
		return ( -1 ) ;

// Recherche par hash-value
	if  ( ApplicationTable [index1]  !=  NULL  &&
			ApplicationTable [index1] -> ProcessId  ==  pid )
		return  ( index1 ) ;


	index2  =  ( index1 + 1 ) % ApplicationTableSize ;

	while  ( index2  !=  index1 )
	   {
		if  ( ApplicationTable [index2]  !=  NULL  &&
				ApplicationTable [index2] -> ProcessId  ==  pid )
			return  ( index2 ) ;
		index2 = ( index2 + 1 ) % ApplicationTableSize ;
	    }

	return ( -1 ) ;
    }




/**************************************************************************/
/*                                                                        */
/*  TApplicationTable :: HandleMessage                                    */
/*      Dispatche le message re�u d'Unix vers la fonction appropri�e.     */
/*                                                                        */
/**************************************************************************/

void	TUnixApplicationTable :: HandleMessage  ( TUnixMessage *  msg,
						  TConnection *   origin )
   {
	register unsigned int	func	=  ( int )  msg -> Function ;
	register int		index 	=  Find  ( msg -> ProcessId ) ;
	TUnixApplication *	app 	=  ( index  ==  -1 )  ?
						NULL : ApplicationTable [index] ;


	MessageOrigin = origin ;

// M�thode de compression utilis�e
	func     &=  ~COMPRESSION_MASK ;


// On v�rifie que l'application s'est bien connect�e
	if  ( app  ==  NULL  &&  func  !=  UWF_INITIALIZE_APPLICATION )
	   {
		msg -> Function = UWF_NULL_FUNCTION ;
		Answer ( NULL, msg, 0, 0 ) ;
		return ;
	    }

// On v�rifie aussi que le num�ro de fonction est bien d�fini...
	if  ( func  >=  DispatchTableSize )
	   {
		char		buffer [ 64 ] ;

		sprintf  ( buffer, "Fonction inconnue : 0x%.4X !", func ) ;
		TerminalWindow -> MessageBox ( buffer, "Erreur interne" ) ;
		return ;
	    }
	    
	void  ( TUnixApplicationTable :: * hnd ) (
			TUnixApplication *,
			TUnixMessage * )  = DispatchTable [func]. Handler ;

	( this ->* hnd ) ( app, msg ) ;
     }




/**************************************************************************/
/*                                                                        */
/*   TApplicationTable :: ApplicationListBox                              */
/*      Affiche une bo�te de dialogue permettant de choisir l'une des     */
/*      applications Unix d�clar�es.                                      */
/*                                                                        */
/**************************************************************************/

void	TUnixApplicationTable :: ApplicationListBox  ( void )
    {
	int		UserChoice ;
	BOOL		Kill ;


	if  ( TApplicationTableDialog ( TerminalWindow, ApplicationTable,
		ApplicationTableSize, & UserChoice, & Kill ). Execute ( )  ==  IDOK )
	   {
		if  ( Kill )
			:: MessageBox ( TerminalWindow -> HWindow, "Erreur",
				MB_OK | MB_ICONEXCLAMATION, "Cette fonction n'est pas encore impl�ment�e." ) ;
	    }
       }




/**************************************************************************/
/*                                                                        */
/*   TUnixApplicationTable :: Remove                                      */
/*      Supprime une connection.                                          */
/*                                                                        */
/**************************************************************************/

# pragma 	argsused

void	TUnixApplicationTable :: Remove  ( TUnixApplication *  app )
   {
	for  ( int  i = 0 ; i < ApplicationTableSize ; i ++ )
	   {
		if  ( ApplicationTable [i]  ==  app )
		   {
			EventQueue -> Kill ( ApplicationTable [i] -> ProcessId ) ;
			delete  ApplicationTable [i] ;
			ApplicationTable [i] = NULL ;
			break ;
		    }
	    }          

	ApplicationCount -- ;
    }







/**************************************************************************/
/*                                                                        */
/*   TUnixApplicationTable :: RemoveObject                                */
/*      Supprime un objet de la table de l'application.                   */
/*                                                                        */
/**************************************************************************/

# pragma 	argsused

void	TUnixApplicationTable :: RemoveObject  ( int		     appid,
						 TWindow *	     win )
   {
	if  ( appid  >= 0  &&  appid  <   MAX_UNIX_APPLICATIONS )
	   {
		TUnixApplication *   	App = ApplicationTable [ appid ] ;


		if  ( App )
			App -> DeleteObject ( win ) ;
	     }
    }





/**************************************************************************/
/*                                                                        */
/*  TUnixApplicationTable :: Add                                          */
/*      Ajoute une application � la liste et retourne un pointeur dessus. */
/*                                                                        */
/**************************************************************************/

TUnixApplication *  TUnixApplicationTable :: Add  ( TUnixMessage *  msg )
   {
	register int		index1	=  msg -> ProcessId % ApplicationTableSize,
				index2 ;
	TUnixApplication *	App ;


	if  ( ApplicationTable [ index1 ]  !=  NULL )
	   {
		index2 = ( index1 + 1 ) % ApplicationTableSize ;

		while  ( index2  !=  index1 )
		   {
			if  ( ApplicationTable [ index2 ]  ==  NULL )
			   {
				index1 = index2 ;
				goto  Success ;
			    }

			index2 = ( index2 + 1 ) % ApplicationTableSize ;
		    }

		return ( NULL ) ;	/* Trop de connexions... */
	     }

Success :
	UTInitializeApplication *	params = ( UTInitializeApplication * ) msg -> Data ;

	ApplicationCount ++ ;

	App = ApplicationTable [ index1 ] = new  TUnixApplication (
			params -> ApplicationName,
			index1,
			msg -> ProcessId,
			( MessageOrigin ) ?  MessageOrigin : :: Connection ) ;

// On ajoute syst�matiquement l'objet fen�tre TERMINAL � la liste des objets
// de cette application
	App -> AddObject ( TerminalWindow ) ;

        return ( App ) ;
     }
